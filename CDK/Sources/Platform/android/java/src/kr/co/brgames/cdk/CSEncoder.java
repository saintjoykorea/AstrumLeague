package kr.co.brgames.cdk;

import android.util.Log;

import java.io.UnsupportedEncodingException;

public class CSEncoder {
    private static final String TAG = "CSEncoder";
    
    public static String decodeString(byte[] bytes, String encoding) {
        try {
            return new String(bytes, encoding);
        }
        catch (UnsupportedEncodingException e) {
            Log.e(TAG, e.getMessage(), e);
            return null;
        }
    }

    public static byte[] encodeString(String str, String encoding) {
        try {
            return str.getBytes(encoding);
        }
        catch (UnsupportedEncodingException e) {
            Log.e(TAG, e.getMessage(), e);
            return null;
        }
    }
}
