package kr.co.brgames.cdk;

import android.util.Log;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;

public class CSExceptionHandler implements Thread.UncaughtExceptionHandler {
    private final Thread.UncaughtExceptionHandler defaultExceptionHandler;

    private static final String TAG = "CSExceptionHandler";

    public static void start() {
        Thread.setDefaultUncaughtExceptionHandler(new CSExceptionHandler());
    }

    private CSExceptionHandler() {
        defaultExceptionHandler = Thread.getDefaultUncaughtExceptionHandler();
    }

    public void uncaughtException(@NonNull Thread thread, Throwable ex) {
		String path = CSActivity.sharedActivity().storagePath() + File.separator + "signal.txt";
		
        try {
            PrintWriter out = new PrintWriter(new OutputStreamWriter(new FileOutputStream(path), StandardCharsets.UTF_8));
            out.println("platform:android");
            out.println("device:" + CSDevice.brand() + " " + CSDevice.model() + " " + CSDevice.systemVersion());
            out.println("app version:" + CSActivity.sharedActivity().version());
            out.println("exception " + ex.toString() + "(" + ex.getMessage() + ")");
            ex.printStackTrace(out);
            out.close();

            Log.e(TAG, ex.getMessage(), ex);
        }
        catch (IOException e) {
            Log.e(TAG, e.getMessage(), e);
        }
        if (defaultExceptionHandler != null) {
            defaultExceptionHandler.uncaughtException(thread, ex);
        }
        System.exit(-1);
    }
}
