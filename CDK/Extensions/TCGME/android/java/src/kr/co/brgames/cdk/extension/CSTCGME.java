package kr.co.brgames.cdk.extension;

import java.util.Set;
import java.util.HashSet;

import android.content.Intent;
import android.os.Bundle;
import android.os.Build;
import android.util.Log;

import com.tencent.TMG.ITMGContext;
import com.tencent.av.sdk.AVError;
import com.tencent.av.sig.AuthBuffer;

import kr.co.brgames.cdk.CSActivity;

class CSTCGMEImpl extends ITMGContext.ITMGDelegate implements Runnable {
    private String _appId;
    private String _appKey;
    private String _userId;
    private Set<String> _talkings;
    private boolean _paused;
	
    private static final String TAG = "CSTCGME";

    private static final int POLL_INTERVAL = 200;

    public CSTCGMEImpl() {
        _appId = CSActivity.sharedActivity().resourceString("tcgme_app_id");
        _appKey = CSActivity.sharedActivity().resourceString("tcgme_app_secret");
        
		if (_appId == null || _appKey == null) {
			Log.e(TAG, "not ready");
		}
		
        _talkings = new HashSet<String>();

        ITMGContext.GetInstance(CSActivity.sharedActivity()).SetLogLevel(ITMGContext.ITMG_LOG_LEVEL_NONE, ITMGContext.ITMG_LOG_LEVEL_NONE);
    }
    
    public void connect(long userId) {
        ITMGContext context = ITMGContext.GetInstance(CSActivity.sharedActivity());
        
        String str = Long.toString(userId);
        
        if (_userId != null) {
            if (_userId.equals(str)) {
                return;
            }
            context.Uninit();
            _userId = null;
        }
        _userId = str;
        context.SetTMGDelegate(this);
        context.Init(_appId, _userId);

        CSActivity.sharedActivity().queueEvent(this, POLL_INTERVAL);
    }
    
    public void disconnect() {
        _talkings.clear();
        
        if (_userId != null) {
            ITMGContext context = ITMGContext.GetInstance(CSActivity.sharedActivity());
            context.SetTMGDelegate(null);
            context.Uninit();
            
            _userId = null;
        }
    }
    
    public void enterRoom(String roomId, int quality) {
        byte[] authBuffer = AuthBuffer.getInstance().genAuthBuffer(Integer.parseInt(_appId), roomId, _userId, _appKey);
        
        ITMGContext.GetInstance(CSActivity.sharedActivity()).EnterRoom(roomId, quality, authBuffer);
    }
    
    public void exitRoom() {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).ExitRoom();
    }
    
    public boolean isRoomEntered() {
        return ITMGContext.GetInstance(CSActivity.sharedActivity()).IsRoomEntered();
    }
    
    public void setMicEnabled(boolean enabled) {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().EnableMic(enabled);
    }
    
    public boolean isMicEnabled() {
        return ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().GetMicState() != 0;
    }
    
    public void setSpeakerEnabled(boolean enabled) {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().EnableSpeaker(enabled);
    }
    
    public boolean isSpeakerEnabled() {
        return ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().GetSpeakerState() != 0;
    }
    
    public void setMicVolume(float volume) {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().SetMicVolume((int)(volume * 200));
    }
    
    public float micVolume() {
        return ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().GetMicVolume() / 200.0f;
    }
    
    public void setSpeakerVolume(float volume) {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().SetSpeakerVolume((int)(volume * 200));
    }
    
    public float speakerVolume() {
        return ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().GetSpeakerVolume() / 200.0f;
    }
    
    public void pause() {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).Pause();
        _paused = true;
    }
    
    public void resume() {
        ITMGContext.GetInstance(CSActivity.sharedActivity()).Resume();
        _paused = false;
    }
    
    @Override
    public void run() {
        if (_userId != null) {
            if (!_paused) {
                ITMGContext.GetInstance(CSActivity.sharedActivity()).Poll();
            }
            CSActivity.sharedActivity().queueEvent(this, POLL_INTERVAL);
        }
    }
    
    @Override
    public void OnEvent(ITMGContext.ITMG_MAIN_EVENT_TYPE type, Intent data) {
        Log.i(CSTCGME.TAG, String.format("OnEvent:%s", type.name()));

        switch (type) {
            case ITMG_MAIN_EVENT_TYPE_ENTER_ROOM:
                {
                    int errorCode = data.getIntExtra("result", -1);
                    //String errorMsg = data.getStringExtra("error_info");

                    final boolean success = errorCode == AVError.AV_OK;
                    
                    if (success) {
                        ITMGContext.GetInstance(CSActivity.sharedActivity()).GetAudioCtrl().TrackingVolume(0.5f);
                    }

                    CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeEnterRoom(success));
                }
                break;
            case ITMG_MAIN_EVENT_TYPE_EXIT_ROOM:
                _talkings.clear();
                
                CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeExitRoom(true));
                break;
            case ITMG_MAIN_EVENT_TYPE_ROOM_DISCONNECT:
                _talkings.clear();
                
                CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeExitRoom(false));
                break;
            case ITMG_MAIN_EVNET_TYPE_USER_UPDATE:
                {
                    final int eventId = data.getIntExtra("event_id", 0);
                    final String[] userIds = data.getStringArrayExtra("user_list");

                    CSActivity.sharedActivity().view().queueEvent(() -> {
						switch (eventId) {
							case ITMGContext.ITMG_EVENT_ID_USER_ENTER:
								for (String userId : userIds) {
									CSTCGME.nativeNotifyEnterRoom(Long.parseLong(userId));
								}
								break;
							case ITMGContext.ITMG_EVENT_ID_USER_EXIT:
								for (String userId : userIds) {
									CSTCGME.nativeNotifyExitRoom(Long.parseLong(userId));
								}
								break;
								/*
							case ITMGContext.ITMG_EVENT_ID_USER_HAS_AUDIO:
								for (String userId : userIds) {
									CSTCGME.nativeTalk(Long.parseLong(userId), true);
								}
								break;
							case ITMGContext.ITMG_EVENT_ID_USER_NO_AUDIO:
								for (String userId : userIds) {
									CSTCGME.nativeTalk(Long.parseLong(userId), false);
								}
								break;
								 */
								//ITMG_EVENT_ID_USER_HAS_AUDIO, ITMG_EVENT_ID_USER_NO_AUDIO 잘 동작하지 않음
						}
                    });
                }
                break;
            case ITMG_MAIN_EVNET_TYPE_USER_VOLUMES:
                {
                    Bundle bundle = data.getExtras();
                    if (bundle != null) {
                        for (String userId : bundle.keySet()) {
                            int volume = ((Integer)bundle.get(userId)).intValue();
                    
                            if (volume > 0) {
                                if (_talkings.add(userId)) {
                                    final long nuserId = Long.parseLong(userId);
                                    
                                    CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeTalk(nuserId, true));
                                }
                            }
                            else {
                                if (_talkings.remove(userId)) {
                                    final long nuserId = Long.parseLong(userId);
                                    
                                    CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeTalk(nuserId, false));
                                }
                            }
                        }
                    }
                }
                break;
            case ITMG_MAIN_EVENT_TYPE_RECONNECT_START:
                CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeReconnect(false));
                break;
            case ITMG_MAIN_EVENT_TYPE_RECONNECT_SUCCESS:
                CSActivity.sharedActivity().view().queueEvent(() -> CSTCGME.nativeReconnect(true));
                break;
        }
    }
}

public class CSTCGME {
    private static CSTCGMEImpl _impl;
    
    private static final String[] unsupportedDevices = {
        "Xiaomi", "Redmi Note 4X"
    };
    
    public static boolean isEnabled() {
        for (int i = 0; i < unsupportedDevices.length; i += 2) {
            if (Build.BRAND.equalsIgnoreCase(unsupportedDevices[i]) && Build.MODEL.equalsIgnoreCase(unsupportedDevices[i + 1])) {
                return false;
            }
        }
        return true;
    }
    
    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSTCGMEImpl());
    }
    
    public static void dispose() {
		if (_impl != null) {
			CSActivity.sharedActivity().queueEvent(() -> {
				_impl.disconnect();
				_impl = null;
			});
		}
    }
    
    public static void connect(final long userId) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.connect(userId));
    }
    
    public static void disconnect() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.disconnect());
    }
    
    public static void enterRoom(final String roomId, final int quality) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.enterRoom(roomId, quality));
    }
    
    public static void exitRoom() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.exitRoom());
    }

    private static boolean isRoomEnteredReturn;
    public static boolean isRoomEntered() {
        Runnable r = new Runnable() {
            @Override
            public void run() {
                isRoomEnteredReturn = _impl.isRoomEntered();

                synchronized (CSTCGME.class) {
                    CSTCGME.class.notify();
                }
            }
        };
        synchronized (CSTCGME.class) {
            CSActivity.sharedActivity().queueEvent(r);
            try {
                CSTCGME.class.wait();
            } catch (InterruptedException e) {}
        }
        return isRoomEnteredReturn;
    }
    
    public static void setMicEnabled(final boolean enabled) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.setMicEnabled(enabled));
    }

    private static boolean isMicEnabledReturn;
    public static boolean isMicEnabled() {
        Runnable r = new Runnable() {
            @Override
            public void run() {
                isMicEnabledReturn = _impl.isMicEnabled();

                synchronized (CSTCGME.class) {
                    CSTCGME.class.notify();
                }
            }
        };
        synchronized (CSTCGME.class) {
            CSActivity.sharedActivity().queueEvent(r);
            try {
                CSTCGME.class.wait();
            } catch (InterruptedException e) {}
        }
        return isMicEnabledReturn;
    }
    
    public static void setSpeakerEnabled(final boolean enabled) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.setSpeakerEnabled(enabled));
    }

    private static boolean isSpeakerEnabledReturn;
    public static boolean isSpeakerEnabled() {
        Runnable r = new Runnable() {
            @Override
            public void run() {
                isSpeakerEnabledReturn = _impl.isSpeakerEnabled();

                synchronized (CSTCGME.class) {
                    CSTCGME.class.notify();
                }
            }
        };
        synchronized (CSTCGME.class) {
            CSActivity.sharedActivity().queueEvent(r);
            try {
                CSTCGME.class.wait();
            } catch (InterruptedException e) {}
        }
        return isSpeakerEnabledReturn;
    }
    
    public static void setMicVolume(final float volume) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.setMicVolume(volume));
    }

    private static float micVolumeReturn;
    public static float micVolume() {
        Runnable r = new Runnable() {
            @Override
            public void run() {
                micVolumeReturn = _impl.micVolume();

                synchronized (CSTCGME.class) {
                    CSTCGME.class.notify();
                }
            }
        };
        synchronized (CSTCGME.class) {
            CSActivity.sharedActivity().queueEvent(r);
            try {
                CSTCGME.class.wait();
            } catch (InterruptedException e) {}
        }
        return micVolumeReturn;
    }
    
    public static void setSpeakerVolume(final float volume) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.setSpeakerVolume(volume));
    }

    private static float speakerVolumeReturn;
    public static float speakerVolume() {
        Runnable r = new Runnable() {
            @Override
            public void run() {
                speakerVolumeReturn = _impl.speakerVolume();
                synchronized (CSTCGME.class) {
                    CSTCGME.class.notify();
                }
            }
        };
        synchronized (CSTCGME.class) {
            CSActivity.sharedActivity().queueEvent(r);
            try {
                CSTCGME.class.wait();
            } catch (InterruptedException e) {}
        }
        return speakerVolumeReturn;
    }
    
    public static void pause() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.pause());
    }
    
    public static void resume() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.resume());
    }
    
    public static native void nativeEnterRoom(boolean success);
    public static native void nativeExitRoom(boolean intensive);
    public static native void nativeNotifyEnterRoom(long userId);
    public static native void nativeNotifyExitRoom(long userId);
    public static native void nativeTalk(long userId, boolean on);
    public static native void nativeReconnect(boolean success);
}
