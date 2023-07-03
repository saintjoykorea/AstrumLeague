package kr.co.brgames.cdk;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.BatteryManager;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.telephony.TelephonyManager;
import android.util.Log;

import java.util.Locale;
import java.util.UUID;

public class CSDevice {
    private static IntentFilter _intentBatteryFilter;

    private static final String TAG = "CSDevice";

    public static String brand() {
        return Build.BRAND;
    }

    public static String model() {
        return Build.MODEL;
    }

    public static String systemVersion() {
        return Build.VERSION.RELEASE;
    }

    public static String locale() {
        Locale locale = CSActivity.sharedActivity().getResources().getConfiguration().locale;

        String code = locale.getLanguage();

        switch(code) {
            case "zh":      //add country code for another language if necessary (ex:en-US...)
                code += "-" + locale.getCountry();
                break;
        }

        return code;
    }

    public static String uuid() {
        return UUID.randomUUID().toString();
    }

    public static String countryCode() {
        try {
            TelephonyManager tm = (TelephonyManager)CSActivity.sharedActivity().getSystemService(Context.TELEPHONY_SERVICE);
            String simCountry = tm.getSimCountryIso();
            if (simCountry != null && simCountry.length() == 2) { // SIM country code is available
                simCountry = simCountry.toUpperCase();
                return simCountry;
            }
            else if (tm.getPhoneType() != TelephonyManager.PHONE_TYPE_CDMA) { // device is not 3G (would be unreliable)
                String networkCountry = tm.getNetworkCountryIso();
                if (networkCountry != null && networkCountry.length() == 2) { // network country code is available
                    networkCountry = networkCountry.toUpperCase();
                    return networkCountry;
                }
            }
        }
        catch (Exception e) {
            Log.e(TAG, "countryCode error", e);
        }

        return CSActivity.sharedActivity().getResources().getConfiguration().locale.getCountry();
    }

    public static void memoryUsage(long[] out) {
        MemoryInfo mi = new MemoryInfo();
        ActivityManager am = (ActivityManager)CSActivity.sharedActivity().getSystemService(Context.ACTIVITY_SERVICE);
        am.getMemoryInfo(mi);
        out[0] = mi.totalMem;
        out[1] = mi.availMem;
        out[2] = mi.threshold;
    }

    public static int battery() {
        if (_intentBatteryFilter == null) {
            _intentBatteryFilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        }

        Intent batteryStatus = CSActivity.sharedActivity().registerReceiver(null, _intentBatteryFilter);

        int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
        int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, 100);

        return level * 100 / scale;
    }
}
