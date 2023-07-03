package kr.co.brgames.cdk;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import android.util.Log;

public class CSFileLog {
    private final SimpleDateFormat _format;
    private final String _dirpath;
    private String _path;
    private PrintStream _ps;

    private static final String dateFormat = "yyyyMMdd";
    private static final String timeFormat = "HH:mm:ss|";

    private static final String TAG ="CSFileLog";

    public CSFileLog(String dirpath) throws IOException {
        dirpath = CSActivity.sharedActivity().storagePath() + File.separator + dirpath;
        File dir = new File(dirpath);
        if (!dir.exists()) {
            dir.mkdirs();
        }

        _dirpath = dirpath;

        _format = new SimpleDateFormat();
        _path = getFilePath(Calendar.getInstance().getTime());
        _ps = new PrintStream(new FileOutputStream(_path, true));
    }

    public void write(String str) {
        write(Calendar.getInstance().getTime(), str);
    }

    private synchronized void write(Date date, String str) {
        Log.i(TAG, str);

        String path = getFilePath(date);

        try {
            if (!path.equals(this._path)) {
                if (_ps != null) {
                    _ps.close();
                }
                _ps = new PrintStream(new FileOutputStream(path, true));
                this._path = path;
            }
            _format.applyPattern(timeFormat);
            _ps.print(_format.format(date));
            _ps.println(str);
        } catch (IOException e) {
            Log.e(TAG, e.getMessage(), e);
        }
    }

    public void write(Throwable error) {
        write(Calendar.getInstance().getTime(), error);
    }

    private synchronized void write(Date date, Throwable error) {
        Log.i(TAG, "error", error);

        String path = getFilePath(date);

        try {
            if (!path.equals(this._path)) {
                if (_ps != null) {
                    _ps.close();
                }
                _ps = new PrintStream(new FileOutputStream(path, true));
                this._path = path;
            }
            _format.applyPattern(timeFormat);
            _ps.print(_format.format(date));
            error.printStackTrace(_ps);
        } catch (IOException e) {
            Log.e(TAG, e.getMessage(), e);
        }
    }

    private String getFilePath(Date date) {
        _format.applyPattern(dateFormat);
        return _dirpath + _format.format(date) + ".txt";
    }

    public void close() throws IOException {
        _ps.close();
    }
}

