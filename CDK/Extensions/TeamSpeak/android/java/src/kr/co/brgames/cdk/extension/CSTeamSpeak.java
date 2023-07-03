package kr.co.brgames.cdk.extension;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioDeviceInfo;
import android.media.AudioManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

import kr.co.brgames.cdk.CSActivity;

public class CSTeamSpeak {
    private static final int REQUEST_CODE = 1001;

    public static boolean initializeLibrary() {
        Context applicationContext = CSActivity.sharedActivity().getApplicationContext();

        String nativeLibraryDir = applicationContext.getApplicationInfo().nativeLibraryDir;

        return nativeInitializeLibrary(applicationContext, nativeLibraryDir);
    }

    public static void checkPermission(boolean request) {
        if (Build.VERSION.SDK_INT >= 23) {
            CSActivity.sharedActivity().queueEvent(() -> {
				int permssion = ContextCompat.checkSelfPermission(CSActivity.sharedActivity(), Manifest.permission.RECORD_AUDIO);
				if (permssion == PackageManager.PERMISSION_GRANTED) {
					CSActivity.sharedActivity().view().queueEvent(() -> nativeCheckPermissionResult(true));
				} else if (request) {
					CSActivity.sharedActivity().addRequestPermissionsResultListener(new CSActivity.RequestPermissionsResultListener() {
						public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
							if (requestCode == REQUEST_CODE) {
								boolean flag = true;
								for (int grantResult : grantResults) {
									if (grantResult != PackageManager.PERMISSION_GRANTED) {
										flag = false;
										break;
									}
								}
								final boolean granted = flag;

								CSActivity.sharedActivity().view().queueEvent(() -> nativeCheckPermissionResult(granted));
								CSActivity.sharedActivity().removeRequestPermissionsResultListener(this);
							}
						}
					});

					ActivityCompat.requestPermissions(CSActivity.sharedActivity(), new String[]{Manifest.permission.RECORD_AUDIO, Manifest.permission.MODIFY_AUDIO_SETTINGS}, REQUEST_CODE);
				}
				else {
					CSActivity.sharedActivity().view().queueEvent(() -> nativeCheckPermissionResult(false));
				}
            });
        }
        else {
            nativeCheckPermissionResult(true);
        }
    }

    private static boolean speakerphoneOn;

    private static Runnable updateAudioRunnable = new Runnable() {
        @Override
        public void run() {
            AudioManager audioManager = (AudioManager) CSActivity.sharedActivity().getSystemService(Context.AUDIO_SERVICE);

            boolean on;
            if (Build.VERSION.SDK_INT >= 23) {
                AudioDeviceInfo[] audioDevices = audioManager.getDevices(AudioManager.GET_DEVICES_ALL);

                on = true;
                for (AudioDeviceInfo deviceInfo : audioDevices) {
                    if (deviceInfo.getType() == AudioDeviceInfo.TYPE_WIRED_HEADPHONES || deviceInfo.getType() == AudioDeviceInfo.TYPE_WIRED_HEADSET) {
                        on = false;
                        break;
                    }
                }
            }
            else {
                on = false;
            }

            if (speakerphoneOn != on) {
                audioManager.setSpeakerphoneOn(on);
                speakerphoneOn = on;
            }
        }
    };

    private static Runnable restoreAudioRunnable = new Runnable() {
        @Override
        public void run() {
            if (speakerphoneOn) {
                AudioManager audioManager = (AudioManager) CSActivity.sharedActivity().getSystemService(Context.AUDIO_SERVICE);
                audioManager.setSpeakerphoneOn(false);
                speakerphoneOn = false;
            }
        }
    };

    public static boolean updateAudioEnabled() {
        return Build.VERSION.SDK_INT >= 23;
    }

    public static void updateAudio() {
        CSActivity.sharedActivity().queueEvent(updateAudioRunnable);
    }

    public static void restoreAudio() {
        CSActivity.sharedActivity().queueEvent(restoreAudioRunnable);
    }

    private static native void nativeCheckPermissionResult(boolean granted);
    private static native boolean nativeInitializeLibrary(Context context, String nativeLibraryDir);
}