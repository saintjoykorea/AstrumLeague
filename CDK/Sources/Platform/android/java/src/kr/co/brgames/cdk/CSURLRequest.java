package kr.co.brgames.cdk;

import android.util.Log;

import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;
import java.io.IOException;
import java.net.URL;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;

public class CSURLRequest {
    private float _timeoutInterval;
    private boolean _usingCache;
    private URL _url;
    private String _method;
    private final Map<String, String> _headers;
    private byte[] _body;
    
    private static final String TAG = "CSURLRequest";

    private CSURLRequest(String url, String method, boolean usingCache, float timeoutInterval) throws MalformedURLException {
        _url = new URL(url);
        _method = method;
        _usingCache = usingCache;
        _timeoutInterval = timeoutInterval;
        _headers = new HashMap<>();
    }
    
    public static CSURLRequest create(String url, String method, boolean usingCache, float timeout) {
        try {
            return new CSURLRequest(url, method, usingCache, timeout);
        }
        catch (MalformedURLException e) {
            Log.e(TAG, e.getMessage(), e);
            return null;
        }
    }

    public void setURL(String url) {
        try {
            _url = new URL(url);
        }
        catch (MalformedURLException e) {
            Log.e(TAG, e.getMessage(), e);
        }
    }

    public String URL() {
        return _url.toString();
    }

    public void setMethod(String method) {
        _method = method;
    }

    public String method() {
        return _method;
    }

    public void setHeaderField(String name, String value) {
        _headers.put(name, value);
    }

    public String[] headerFields() {
        String[] headers = new String[_headers.size() * 2];

        int i = 0;
        for (Map.Entry<String, String> header : _headers.entrySet()) {
            headers[i++] = header.getKey();
            headers[i++] = header.getValue();
        }
        return headers;
    }

    public Map<String, String> headers() {
        return _headers;
    }

    public void setBody(byte[] body) {
        _body = body;
    }

    public byte[] body() {
        return _body;
    }

    public void setUsingCache(boolean usingCache) {
        _usingCache = usingCache;
    }

    public boolean usingCache() {
        return _usingCache;
    }

    public void setTimeoutInterval(float timeoutInterval) {
        _timeoutInterval = timeoutInterval;
    }

    public float timeoutInterval() {
        return _timeoutInterval;
    }

    public HttpURLConnection openConnection() throws IOException {
        HttpURLConnection httpConn = (HttpURLConnection)_url.openConnection();

        httpConn.setUseCaches(_usingCache);
        int timeout = (int)(_timeoutInterval * 1000);
        httpConn.setConnectTimeout(timeout);
        httpConn.setReadTimeout(timeout);
        httpConn.setRequestMethod(_method);
        for (Map.Entry<String, String> header : _headers.entrySet()) {
            httpConn.setRequestProperty(header.getKey(), header.getValue());
        }
        if (_body != null) {
            httpConn.setDoOutput(true);
            httpConn.setFixedLengthStreamingMode(_body.length);
            try (OutputStream out = httpConn.getOutputStream()) {
                out.write(_body);
            }
        }
        return httpConn;
    }
}
