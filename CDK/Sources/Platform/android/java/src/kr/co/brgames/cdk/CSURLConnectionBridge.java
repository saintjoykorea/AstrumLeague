package kr.co.brgames.cdk;

import java.util.LinkedList;
import java.util.ArrayList;
import java.io.InputStream;
import java.net.HttpURLConnection;

import android.util.Log;

class CSURLConnection {
    private final CSURLRequest _request;
    private final long _target;
    private boolean _active;

    public CSURLConnection(CSURLRequest request, long target) {
        _request = request;
        _target = target;
    }

    public CSURLRequest request() {
        return _request;
    }

    public long target() {
        return _target;
    }

    public boolean start() {
        if (!_active) {
            _active = true;
            return true;
        }
        return false;
    }

    public boolean cancel() {
        if (_active) {
            _active = false;
            return true;
        }
        return false;
    }

    public boolean isActive() {
        return _active;
    }
}


class CSURLConnectionThread extends Thread {
    private final byte[] _buffer;
    private boolean _active;
    private boolean _alive;

    private static final int MAX_IDLE_TIME = 60000;

    public CSURLConnectionThread() {
        _buffer = new byte[4096];
        _alive = true;
        _active = true;

        setDaemon(true);
    }

    public synchronized boolean wakeup() {
        if (_active || !_alive) {
            return false;
        }
        _active = true;
        notify();
        return true;
    }

    public synchronized void dispose() {
        _alive = false;
        notify();
    }

    private static class ResponseRunnable implements Runnable {
        private final CSURLConnection connection;
        private final int statusCode;

        ResponseRunnable(CSURLConnection connection, int statusCode) {
            this.connection = connection;
            this.statusCode = statusCode;
        }

        @Override
        public void run() {
            if (connection.isActive()) {
                CSURLConnectionBridge.nativeReceiveResponse(connection.target(), statusCode);
            }
        }
    }
    private static class DataRunnable implements Runnable {
        private final CSURLConnection connection;
        private final byte[] data;

        DataRunnable(CSURLConnection connection, byte[] data) {
            this.connection = connection;
            this.data = data;
        }

        @Override
        public void run() {
            if (connection.isActive()) {
                CSURLConnectionBridge.nativeReceiveData(connection.target(), data);
            }
        }
    }
    private static class FinishRunnable implements Runnable {
        private final CSURLConnection connection;

        FinishRunnable(CSURLConnection connection) {
            this.connection = connection;
        }

        @Override
        public void run() {
            if (connection.isActive()) {
                CSURLConnectionBridge.nativeFinishLoading(connection.target());

                connection.cancel();
            }
        }
    }
    private static class ErrorRunnable implements Runnable {
        private final CSURLConnection connection;

        ErrorRunnable(CSURLConnection connection) {
            this.connection = connection;
        }

        @Override
        public void run() {
            if (connection.isActive()) {
                CSURLConnectionBridge.nativeError(connection.target());

                connection.cancel();
            }
        }
    }

    @Override
    public void run() {
        for (; ; ) {
            CSURLConnection connection;
            for (; ; ) {
                if (!_alive) {
                    return;
                }
                if (!_active) {
                    CSURLConnectionBridge.sharedImpl().release(this);
                    return;
                }
                connection = CSURLConnectionBridge.sharedImpl().poll();

                synchronized (this) {
                    if (connection != null) {
                        _active = true;
                        break;
                    }
                    else {
                        _active = false;
                        try {
                            wait(MAX_IDLE_TIME);
                        }
                        catch (InterruptedException e) {
                            Log.e(CSURLConnectionBridge.TAG, e.getMessage(), e);
                        }
                    }
                }
            }

            HttpURLConnection httpConn = null;

            flow:
            {
                try {
                    httpConn = connection.request().openConnection();

                    int responseCode = httpConn.getResponseCode();

                    if (_alive) {
                        CSActivity.sharedActivity().view().queueEvent(new ResponseRunnable(connection, responseCode));
                        if (!connection.isActive()) break flow;
                    }
                    else break flow;

                    int contentLength = httpConn.getContentLength();
                    int receiveLength = 0;

                    try (InputStream in = responseCode == HttpURLConnection.HTTP_OK ? httpConn.getInputStream() : httpConn.getErrorStream()) {
                        for (int n; (n = in.read(_buffer)) != -1; ) {
                            if (n > 0) {
                                byte[] data = new byte[n];
                                System.arraycopy(_buffer, 0, data, 0, n);
                                receiveLength += n;

                                if (_alive) {
                                    CSActivity.sharedActivity().view().queueEvent(new DataRunnable(connection, data));
                                    if (!connection.isActive()) break flow;
                                }
                                else break flow;
                            }
                        }
                    }

                    if (contentLength == -1 || receiveLength == contentLength) {
                        CSActivity.sharedActivity().view().queueEvent(new FinishRunnable(connection));
                    }
                    else {
                        CSActivity.sharedActivity().view().queueEvent(new ErrorRunnable(connection));
                    }
                }
                catch (Throwable e) {
                    Log.e(CSURLConnectionBridge.TAG, e.getMessage(), e);

                    if (_alive) {
                        CSActivity.sharedActivity().view().queueEvent(new ErrorRunnable(connection));
                    }
                }
            }
            if (httpConn != null) httpConn.disconnect();
        }
    }
}

class CSURLConnectionBridgeImpl {
    private final LinkedList<CSURLConnection> _queue;
    private final ArrayList<CSURLConnectionThread> _threads;

    private static final int MAX_THREAD = 20;

    public CSURLConnectionBridgeImpl() {
        _queue = new LinkedList<>();
        _threads = new ArrayList<>(MAX_THREAD);
    }

    public void dispose() {
        synchronized (_queue) {
            _queue.clear();
        }
        synchronized(_threads) {
            for (CSURLConnectionThread thread : _threads) {
                thread.dispose();
            }
            _threads.clear();
        }
    }

    public void release(CSURLConnectionThread thread) {
        synchronized(_threads) {
            _threads.remove(thread);
            
            Log.i(CSURLConnectionBridge.TAG, "thread released:" + _threads.size());
        }
    }
    
    public CSURLConnection poll() {
        synchronized (_queue) {
            if (_queue.isEmpty()) {
                return null;
            }
            return _queue.removeLast();         //나중을 먼저 처리, 여러개가 한꺼번에 들어올 경우 나중 것을 먼저 처리하는 것이 유저 인지가 좋다.
        }
    }

    public CSURLConnection createHandle(CSURLRequest request, long target) {
        return new CSURLConnection(request, target);
    }

    public void destroyHandle(CSURLConnection connection) {
        if (connection.cancel()) {
            synchronized (_queue) {
                _queue.remove(connection);
            }
        }
    }

    public void cancel(CSURLConnection connection) {
        if (connection.cancel()) {
            synchronized (_queue) {
                _queue.remove(connection);
            }
        }
    }

    public void start(CSURLConnection connection) {
        if (connection.start()) {
            synchronized (_queue) {
                _queue.addLast(connection);
            }
            synchronized(_threads) {
                boolean isNew = true;
                for (CSURLConnectionThread thread : _threads) {
                    if (thread.wakeup()) {
                        isNew = false;
                        break;
                    }
                }
                if (isNew && _threads.size() < MAX_THREAD) {
                    CSURLConnectionThread thread = new CSURLConnectionThread();
                    
                    _threads.add(thread);
                    
                    thread.start();
                    
                    Log.i(CSURLConnectionBridge.TAG, "thread created:" + _threads.size());
                }
            }
        }
    }
    
    public byte[] sendSynchronousRequest(CSURLRequest request, int[] status) {
        byte[] bytes = null;
        
        HttpURLConnection httpConn = null;
        
        try {
            httpConn = request.openConnection();

            int responseCode = httpConn.getResponseCode();

            status[0] = responseCode;

            int contentLength = httpConn.getContentLength();
            int receiveLength = 0;

            try (InputStream in = responseCode == HttpURLConnection.HTTP_OK ? httpConn.getInputStream() : httpConn.getErrorStream()) {
                byte[] b = new byte[4096];
                for (int len; (len = in.read(b)) != -1; ) {
                    if (len > 0) {
                        if (bytes == null) {
                            bytes = new byte[len];
                        } else if (bytes.length < receiveLength + len) {
                            byte[] newbytes = new byte[receiveLength + len];
                            System.arraycopy(bytes, 0, newbytes, 0, receiveLength);
                            bytes = newbytes;
                        }
                        System.arraycopy(b, 0, bytes, receiveLength, len);
                        receiveLength += len;
                    }
                }
            }
            if (contentLength != -1 && receiveLength != contentLength) {
                bytes = null;
            }
        }
        catch (Throwable e) {
            Log.e(CSURLConnectionBridge.TAG, e.getMessage(), e);
        }
        if (httpConn != null) httpConn.disconnect();

        return bytes;
    }
}

public class CSURLConnectionBridge {
    public static final String TAG = "CSURLConnectionBridge";

    private static CSURLConnectionBridgeImpl _impl = new CSURLConnectionBridgeImpl();

    public static CSURLConnectionBridgeImpl sharedImpl() {
        return _impl;
    }
    
    public static void dispose() {
        _impl.dispose();
    }

    public static CSURLConnection createHandle(CSURLRequest request, long target) {
        return _impl.createHandle(request, target);
    }

    public static void destroyHandle(CSURLConnection connection) {
        _impl.destroyHandle(connection);
    }

    public static void cancel(CSURLConnection connection) {
        _impl.cancel(connection);
    }

    public static void start(CSURLConnection connection) {
        _impl.start(connection);
    }

    public static byte[] sendSynchronousRequest(CSURLRequest request, int[] status) {
        return _impl.sendSynchronousRequest(request, status);
    }

    public static native void nativeReceiveResponse(long target, int statusCode);
    public static native void nativeReceiveData(long target, byte[] data);
    public static native void nativeFinishLoading(long target);
    public static native void nativeError(long target);
}
