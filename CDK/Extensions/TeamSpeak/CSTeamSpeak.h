//
//  CSTeamSpeak.h
//  CDK
//
//  Created by Kim Chan on 2018. 11. 23..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSTeamSpeak_h
#define CSTeamSpeak_h

#include "CSString.h"
#include "CSArray.h"

#define CSTeamSpeakCustomErrorCreateChannelFail      0x9000
#define CSTeamSpeakCustomErrorMoveChannelTimeout     0x9001

enum CSTeamSpeakStatus {
    CSTeamSpeakStatusDisconnected,
    CSTeamSpeakStatusConnecting,
    CSTeamSpeakStatusConnected,
    CSTeamSpeakStatusConnectionEstablishing,
    CSTeamSpeakStatusConnectionEstablished
};
enum CSTeamSpeakTalkStatus {
    CSTeamSpeakTalkStatusNone,
    CSTeamSpeakTalkStatusTalking,
    CSTeamSpeakTalkStatusTalkingWhileDisabled
};

class CSTeamSpeakDelegate {
public:
    inline virtual void onTeamSpeakCheckPermission(bool granted) {}
    inline virtual void onTeamSpeakConnect() {}
    inline virtual void onTeamSpeakMoveChannel(const char* name, bool intensive) {}
    inline virtual void onTeamSpeakTalk(ushort clientId, CSTeamSpeakTalkStatus status) {}
    inline virtual void onTeamSpeakError(uint error) {}
};

class CSTeamSpeak {
private:
    CSTeamSpeakDelegate* _delegate;
    char* _identity;
    uint64 _serverConnectionHandlerId;
    int _status;
    ushort _clientId;
    bool _permissionGranted;
    bool _libraryInitialized;
    bool _playbackDeviceOpened;
    bool _captureDeviceOpened;
    char* _channelName;
    
    static CSTeamSpeak* _sharedTeamSpeak;
    
    CSTeamSpeak();
    ~CSTeamSpeak();
    
public:
    static void initialize();
    static void finalize();
    
    static inline CSTeamSpeak* sharedTeamSpeak() {
        return _sharedTeamSpeak;
    }
    inline CSTeamSpeakDelegate* delegate() const {
        return _delegate;
    }
    inline void setDelegate(CSTeamSpeakDelegate* delegate) {
        _delegate = delegate;
    }
    inline ushort clientId() const {
        return _clientId;
    }
    inline CSTeamSpeakStatus status() const {
        return (CSTeamSpeakStatus)_status;
    }
    inline bool isPlaybackDeviceOpened() const {
        return _playbackDeviceOpened;
    }
    inline bool isCaptureDeviceOpened() const {
        return _captureDeviceOpened;
    }

    uint64 channelId(ushort clientId) const;
    CSString* channelName(uint64 channelId) const;
    CSArray<uint64>* channelIds() const;
    CSArray<ushort>* clientIds(uint64 channelId) const;
    CSString* clientName(ushort clientId) const;
    
    inline uint64 channelId() const {
        return channelId(_clientId);
    }
    inline CSString* channelName() const {
        return channelName(channelId());
    }
    inline CSArray<ushort>* clientIds() const {
        return clientIds(channelId());
    }
    inline CSString* clientName() const {
        return clientName(_clientId);
    }
    
    CSTeamSpeakTalkStatus talkStatus(ushort clientId) const;
    bool isMuted(ushort clientId) const;

#ifdef CDK_IMPL
public:
    bool initializeLibrary(const char* nativeLibraryDir);
#endif
private:
    //==================================================================
#if defined(CDK_ANDROID)
    bool initializeLibraryAndroid();
    void checkPermissionAndroid(bool request);
#ifdef CDK_IMPL
public:
#endif
    void checkPermissionResultAndroid(bool granted);
private:
    void prepareAudioAndroid();
    void restoreAudioAndroid();
#elif defined(CDK_IOS)
    void prepareAudioIOS();
    void restoreAudioIOS(bool byPause);
#endif
    //==================================================================
    bool initializeLibrary();
    bool spawnServerConnectionHandler();
    bool createIdentity();
public:
    void checkPermission(bool request);
    bool connect(const char* host, int port, const char* name, const char* channelName, const char* serverPassword);
    void disconnect(bool byPause = false);
    bool openPlaybackDevice();
    bool openCaptureDevice();
    void closePlaybackDevice();
    void closeCaptureDevice();
    static float playbackDefaultVolume();
    static float captureDefaultVolume();
    float playbackVolume() const;
    float captureVolume() const;
    bool setPlaybackVolume(float volume);
    bool setCaptureVolume(float volume);
private:
    void moveChannel();
    bool createChannel();
public:
    void moveChannel(const char* channelName);
    void mute(ushort clientId);
    void unmute(ushort clientId);
#ifdef CDK_IMPL
    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerId, int status, uint error);
    void onNewChannelCreatedEvent(uint64 serverConnectionHandlerId, uint64 channelId, ushort invokerId);
    void onClientMoveEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 channelId);
    void onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 channelId);
    void onClientMoveTimeoutEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 channelId);
    void onTalkStatusChangeEvent(uint64 serverConnectionHandlerId, int status, ushort clientId);
    void onServerErrorEvent(uint64 serverConnectionHandlerId, uint error);
#endif
};

#endif /* CSTeamSpeak_h */
