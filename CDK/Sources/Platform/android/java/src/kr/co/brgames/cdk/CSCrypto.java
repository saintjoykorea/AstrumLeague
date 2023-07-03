package kr.co.brgames.cdk;

import android.util.Log;

import java.security.MessageDigest;
import java.security.spec.AlgorithmParameterSpec;
import java.util.Arrays;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class CSCrypto {
    private static final String TAG = "CSCrypto";
    
    public static byte[] encrypt(byte[] data, byte[] key) {
        try {
            SecretKeySpec newKey = new SecretKeySpec(key, "AES");
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
            byte[] ivBytes = new byte[16];
            System.arraycopy(key, 0, ivBytes, 0, 16);
            AlgorithmParameterSpec ivSpec = new IvParameterSpec(ivBytes);
            cipher.init(Cipher.ENCRYPT_MODE, newKey, ivSpec);
            return cipher.doFinal(data);
        }
        catch (Exception e) {
            Log.e(TAG, e.getMessage(), e);
            return null;
        }
        finally {
            Arrays.fill(key, (byte) 0);
        }
    }

    public static byte[] decrypt(byte[] data, byte[] key) {
        try {
            SecretKeySpec newKey = new SecretKeySpec(key, "AES");
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
            byte[] ivBytes = new byte[16];
            System.arraycopy(key, 0, ivBytes, 0, 16);
            AlgorithmParameterSpec ivSpec = new IvParameterSpec(ivBytes);
            cipher.init(Cipher.DECRYPT_MODE, newKey, ivSpec);
            return cipher.doFinal(data);
        }
        catch (Exception e) {
            Log.e(TAG, e.getMessage(), e);
            return null;
        }
        finally {
            Arrays.fill(key, (byte) 0);
        }
    }

    public static byte[] sha1(byte[] data) {
        try {
            MessageDigest md = MessageDigest.getInstance("SHA-1");
            return md.digest(data);
        }
        catch(Exception e) {
            Log.e(TAG, e.getMessage(), e);
            return null;
        }
    }
}
