//
//  CSTeamSpeak.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 11. 23..
//  Copyright © 2018년 brgames. All rights reserved.
//

#define CDK_IMPL

#include "CSMemory.h"
#include "CSLog.h"
#include "CSMath.h"

#include "CSTeamSpeak+callback.h"
#include "CSTeamSpeak.h"

#ifdef CDK_IOS
static const char* Device = "iOS";
#elif defined(CDK_ANDROID)
static const char* Device = "";
#endif

#define PlaybackMaxVolume       15.0f
#define PlaybackMinVolume       -40.0f
#define PlaybackDefaultVolume   0.0f

#define CaptureMinVolume        0.0f
#define CaptureMaxVolume        16000.0f
#define CaptureDefaultVolume    16000.0f

CSTeamSpeak* CSTeamSpeak::_sharedTeamSpeak = NULL;

CSTeamSpeak::CSTeamSpeak() :
    _delegate(NULL),
    _identity(NULL),
    _serverConnectionHandlerId(0),
    _status(STATUS_DISCONNECTED),
    _clientId(0),
    _permissionGranted(false),
    _libraryInitialized(false),
    _playbackDeviceOpened(false),
    _captureDeviceOpened(false),
    _channelName(NULL)
{
}

CSTeamSpeak::~CSTeamSpeak() {
    disconnect();
    
    if (_identity) {
        free(_identity);
    }
    if (_serverConnectionHandlerId) {
        ts3client_destroyServerConnectionHandler(_serverConnectionHandlerId);
    }
    if (_libraryInitialized) {
        ts3client_destroyClientLib();
    }
}

void CSTeamSpeak::initialize() {
    if (!_sharedTeamSpeak) {
        _sharedTeamSpeak = new CSTeamSpeak();
    }
}

void CSTeamSpeak::finalize() {
    if (_sharedTeamSpeak) {
        delete _sharedTeamSpeak;
        _sharedTeamSpeak = NULL;
    }
}

bool CSTeamSpeak::initializeLibrary() {
    if (_libraryInitialized) {
#ifdef CDK_IOS
        prepareAudioIOS();
#endif
        return true;
    }
#ifdef CDK_ANDROID
    _libraryInitialized = initializeLibraryAndroid();
#else
    _libraryInitialized = initializeLibrary(NULL);
#endif
    return _libraryInitialized;
}

bool CSTeamSpeak::initializeLibrary(const char* nativeLibraryDir) {
    struct ClientUIFunctions funcs;
    
    memset(&funcs, 0, sizeof(struct ClientUIFunctions));
    
    funcs.onConnectStatusChangeEvent    = ::onConnectStatusChangeEvent;
    funcs.onNewChannelCreatedEvent      = ::onNewChannelCreatedEvent;
    funcs.onClientMoveEvent             = ::onClientMoveEvent;
    funcs.onClientMoveSubscriptionEvent = ::onClientMoveSubscriptionEvent;
    funcs.onClientMoveTimeoutEvent      = ::onClientMoveTimeoutEvent;
    funcs.onTalkStatusChangeEvent       = ::onTalkStatusChangeEvent;
    funcs.onServerErrorEvent            = ::onServerErrorEvent;
    
#ifdef CDK_CONSOLE_DEBUG
#define LogType LogType_CONSOLE
#else
#define LogType LogType_NONE
#endif
    
    uint error = ts3client_initClientLib(&funcs, 0, LogType, NULL, nativeLibraryDir);
#ifdef CDK_CONSOLE_DEBUG
    ts3client_setLogVerbosity(LogLevel_DEVEL);
#endif
    if (error == ERROR_ok) {
        CSLog("initializeLibrary");
        return true;
    }
    else {
        CSErrorLog("initializeLibrary error:%d", error);
        return false;
    }
}

bool CSTeamSpeak::spawnServerConnectionHandler() {
    if (_serverConnectionHandlerId) {
        return true;
    }
    uint error = ts3client_spawnNewServerConnectionHandler(0, &_serverConnectionHandlerId);
    if (error == ERROR_ok) {
        CSLog("spawnServerConnectionHandler:%" PRIu64, _serverConnectionHandlerId);
        return true;
    }
    else {
        CSErrorLog("spawnNewServerConnectionHandler error:%d", error);
        return false;
    }
}

bool CSTeamSpeak::createIdentity() {
    if (_identity) {
        return true;
    }
    char* identitiy;
    
    uint error = ts3client_createIdentity(&identitiy);
    
    if (error == ERROR_ok) {
        _identity = strdup(identitiy);
        ts3client_freeMemory(identitiy);
        
        CSLog("createIdentity:%s", _identity);
        
        return true;
    }
    else {
        CSErrorLog("createIdentity error:%d", error);
        return false;
    }
}

void CSTeamSpeak::checkPermission(bool request) {
#ifdef CDK_ANDROID
    checkPermissionAndroid(request);
#else
    _permissionGranted = true;
    
    if (_delegate) _delegate->onTeamSpeakCheckPermission(true);
#endif
}

bool CSTeamSpeak::connect(const char* host, int port, const char* name, const char* channelName, const char* serverPassword) {
    if (!_permissionGranted) {
        CSErrorLog("permission not granted");
        return false;
    }
    if (!initializeLibrary()) {
        return false;
    }
    if (!spawnServerConnectionHandler()) {
        return false;
    }
    if (!createIdentity()) {
        return false;
    }
    if (!serverPassword) {
        serverPassword = "";
    }
#ifdef CDK_ANDROID
    prepareAudioAndroid();
#endif
    uint error;
    
    if (_channelName) {
        free(_channelName);
    }
    
    CSLog("connect:%s:%d:%s", host, port, serverPassword);
    
    if (channelName) {
        const char* channelArray[] = {
            channelName, ""
        };
        _channelName = strdup(channelName);
        
        error = ts3client_startConnection(_serverConnectionHandlerId, _identity, host, port, name, channelArray, "", serverPassword);
    }
    else {
        _channelName = NULL;
        
        error = ts3client_startConnection(_serverConnectionHandlerId, _identity, host, port, name, NULL, "", serverPassword);
    }
    if (error == ERROR_ok) {
        CSLog("connect start");
        return true;
    }
    else {
        CSErrorLog("connect error:%d", error);
        return false;
    }
}

void CSTeamSpeak::disconnect(bool byPause) {
    closePlaybackDevice();
    closeCaptureDevice();
    
    ts3client_stopConnection(_serverConnectionHandlerId, "leaving");
    
    _clientId = 0;
    if (_channelName) {
        free(_channelName);
        _channelName = NULL;
    }
    
#ifdef CDK_IOS
    restoreAudioIOS(byPause);
#elif defined(CDK_ANDROID)
    restoreAudioAndroid();
#endif
    
    CSLog("disconnect");
}

bool CSTeamSpeak::openPlaybackDevice() {
    if (!_playbackDeviceOpened && _status == STATUS_CONNECTION_ESTABLISHED) {
        uint error = ts3client_openPlaybackDevice(_serverConnectionHandlerId, "", Device);
        if (error == ERROR_ok) {
            CSLog("openPlaybackDevice");
            _playbackDeviceOpened = true;
        }
        else {
            CSErrorLog("openPlaybackDevice error:%d", error);
        }
    }
    return _playbackDeviceOpened;
}

bool CSTeamSpeak::openCaptureDevice() {
    if (!_captureDeviceOpened && _status == STATUS_CONNECTION_ESTABLISHED) {
        uint error = ts3client_openCaptureDevice(_serverConnectionHandlerId, "", Device);
        if (error == ERROR_ok) {
            CSLog("openCaptureDevice");
            _captureDeviceOpened = true;
        }
        else {
            CSErrorLog("openCaptureDevice error:%d", error);
        }
    }
    return _captureDeviceOpened;
}

void CSTeamSpeak::closePlaybackDevice() {
    if (_playbackDeviceOpened) {
        uint error = ts3client_closePlaybackDevice(_serverConnectionHandlerId);
        if (error == ERROR_ok) {
            CSLog("closePlaybackDevice");
        }
        else {
            CSErrorLog("closePlaybackDevice error:%d", error);
        }
        _playbackDeviceOpened = false;
    }
}

void CSTeamSpeak::closeCaptureDevice() {
    if (_captureDeviceOpened) {
        uint error = ts3client_closeCaptureDevice(_serverConnectionHandlerId);
        if (error == ERROR_ok) {
            CSLog("closeCaptureDevice");
        }
        else {
            CSErrorLog("closeCaptureDevice error:%d", error);
        }
        _captureDeviceOpened = false;
    }
}

static float playbackVolumeNormal(float volume) {
    return (volume - PlaybackMinVolume) / (PlaybackMaxVolume - PlaybackMinVolume);
}

static float playbackVolumeDenormal(float volume) {
    return PlaybackMinVolume + volume * (PlaybackMaxVolume - PlaybackMinVolume);
}

float CSTeamSpeak::playbackDefaultVolume() {
    static float defaultVolume = playbackVolumeNormal(PlaybackDefaultVolume);
    
    return defaultVolume;
}

float CSTeamSpeak::playbackVolume() const {
    if (!_playbackDeviceOpened) {
        return 0;
    }
    float volume;
    uint error = ts3client_getPlaybackConfigValueAsFloat(_serverConnectionHandlerId, "volume_modifier", &volume);
    if (error == ERROR_ok) {
        return playbackVolumeNormal(volume);
    }
    else {
        CSErrorLog("getPlaybackConfigValueAsFloat error:%d", error);
        return 0;
    }
}

bool CSTeamSpeak::setPlaybackVolume(float volume) {
    if (!_playbackDeviceOpened) {
        CSErrorLog("setPlaybackVolume device not open");
        return false;
    }
    volume = playbackVolumeDenormal(volume);
    uint error = ts3client_setPlaybackConfigValue(_serverConnectionHandlerId, "volume_modifier", CSString::cstringWithFormat("%.2f", volume));
    if (error == ERROR_ok) {
        CSLog("setPlaybackVolume:%.2f", volume);
        return true;
    }
    else {
        CSErrorLog("ts3client_setPlaybackConfigValue error:%d", error);
        return false;
    }
}

static float captureVolumeNormal(float volume) {
    return (volume - CaptureMinVolume) / (CaptureMaxVolume - CaptureMinVolume);
}

static float captureVolumeDenormal(float volume) {
    return CaptureMinVolume + volume * (CaptureMaxVolume - CaptureMinVolume);
}

float CSTeamSpeak::captureDefaultVolume() {
    static float defaultVolume = captureVolumeNormal(CaptureDefaultVolume);
    
    return defaultVolume;
}

float CSTeamSpeak::captureVolume() const {
    if (!_captureDeviceOpened) {
        return 0;
    }
    char* result;
    uint error = ts3client_getPreProcessorConfigValue(_serverConnectionHandlerId, "agc_level", &result);
    if (error == ERROR_ok) {
        float volume = CSString::floatValue(result);
        ts3client_freeMemory(result);
        
        return captureVolumeNormal(volume);
    }
    else {
        CSErrorLog("ts3client_getPreProcessorConfigValue error:%d", error);
        return 0;
    }
}

bool CSTeamSpeak::setCaptureVolume(float volume) {
    if (!_captureDeviceOpened) {
        CSErrorLog("setCaptureVolume device not open");
        return false;
    }
    
    volume = captureVolumeDenormal(volume);
    uint error = ts3client_setPreProcessorConfigValue(_serverConnectionHandlerId, "agc_level", CSString::cstringWithFormat("%d", (int)volume));
    if (error == ERROR_ok) {
        CSLog("setCaptureVolume:%.2f", volume);
        return true;
    }
    else {
        CSErrorLog("ts3client_setPreProcessorConfigValue error:%d", error);
        return false;
    }
}

bool CSTeamSpeak::createChannel() {
    uint error;
    
    error = ts3client_setChannelVariableAsString(_serverConnectionHandlerId, 0, CHANNEL_NAME, _channelName);
    if (error) {
        CSErrorLog("setChannelVariableAsString error:%d", error);
        return false;
    }
    error = ts3client_flushChannelCreation(_serverConnectionHandlerId, 0, NULL);
    if (error) {
        CSErrorLog("flushChannelCreation error:%d", error);
        return false;
    }
    return true;
}

void CSTeamSpeak::moveChannel() {
    CSAssert(_channelName, "invalid operation");
    char empty[1] = { 0 };
    char* channelArray[2] = { _channelName, empty };
    uint64 destChannelId;
    
    uint error = ts3client_getChannelIDFromChannelNames(_serverConnectionHandlerId, channelArray, &destChannelId);
    
    if (error == ERROR_ok) {
        if (destChannelId == 0) {
            if (!createChannel()) {
                if (_delegate) _delegate->onTeamSpeakError(CSTeamSpeakCustomErrorCreateChannelFail);
            }
        }
        else {
            uint64 srcChannelId;
            
            error = ts3client_getChannelOfClient(_serverConnectionHandlerId, _clientId, &srcChannelId);
            
            if (error == ERROR_ok) {
                if (srcChannelId != destChannelId) {
                    error = ts3client_requestClientMove(_serverConnectionHandlerId, _clientId, destChannelId, "", NULL);
                    if (error != ERROR_ok) {
                        CSErrorLog("requestClientMove error:%d", error);
                        if (_delegate) _delegate->onTeamSpeakError(error);
                    }
                }
                else {
                    if (_delegate) _delegate->onTeamSpeakMoveChannel(_channelName, true);
                }
            }
            else {
                CSErrorLog("getChannelOfClient error:%d", error);
                if (_delegate) _delegate->onTeamSpeakError(error);
            }
        }
    }
    else {
        CSErrorLog("getChannelIDFromChannelNames error:%d", error);
        if (_delegate) _delegate->onTeamSpeakError(error);
    }
}

void CSTeamSpeak::moveChannel(const char* channelName) {
    if (_channelName) {
        free(_channelName);
    }
    _channelName = strdup(channelName);
    moveChannel();
}

uint64 CSTeamSpeak::channelId(ushort clientId) const {
    uint64 channelId = 0;
    uint error = ts3client_getChannelOfClient(_serverConnectionHandlerId, clientId, &channelId);
    if (error != ERROR_ok) {
        CSErrorLog("getChannelOfClient error:%d", error);
    }
    return channelId;
}

CSString* CSTeamSpeak::channelName(uint64 channelId) const {
    char* name;
    uint error = ts3client_getChannelVariableAsString(_serverConnectionHandlerId, channelId, CHANNEL_NAME, &name);
    if (error == ERROR_ok) {
        CSString* str = CSString::string(name);
        ts3client_freeMemory(name);
        return str;
    }
    else {
        CSErrorLog("getChannelVariableAsString error:%d", error);
        return NULL;
    }
}

CSArray<uint64>* CSTeamSpeak::channelIds() const {
    uint64* ids;
    uint error = ts3client_getChannelList(_serverConnectionHandlerId, &ids);
    if (error == ERROR_ok) {
        CSArray<uint64>* arr = CSArray<uint64>::array();
        int i = 0;
        while (ids[i] != 0) {
            arr->addObject(ids[i]);
            i++;
        }
        return arr;
    }
    else {
        CSErrorLog("getChannelList error:%d", error);
        return NULL;
    }
}

CSArray<ushort>* CSTeamSpeak::clientIds(uint64 channelId) const {
    ushort* ids;
    uint error = ts3client_getChannelClientList(_serverConnectionHandlerId, channelId, &ids);
    if (error == ERROR_ok) {
        CSArray<ushort>* arr = CSArray<ushort>::array();
        int i = 0;
        while (ids[i] != 0) {
            arr->addObject(ids[i]);
            i++;
        }
        return arr;
    }
    else {
        CSErrorLog("getChannelClientList error:%d", error);
        return NULL;
    }
}

CSString* CSTeamSpeak::clientName(ushort clientId) const {
    char* name;
    uint error = ts3client_getClientVariableAsString(_serverConnectionHandlerId, clientId, CLIENT_NICKNAME, &name);
    if (error == ERROR_ok) {
        CSString* str = CSString::string(name);
        ts3client_freeMemory(name);
        return str;
    }
    else {
        CSErrorLog("getClientVariableAsString error:%d", error);
        return NULL;
    }
}

CSTeamSpeakTalkStatus CSTeamSpeak::talkStatus(ushort clientId) const {
    int status = CSTeamSpeakTalkStatusNone;
    uint error = ts3client_getClientVariableAsInt(_serverConnectionHandlerId, clientId, CLIENT_FLAG_TALKING, &status);
    if (error != ERROR_ok) {
        CSErrorLog("getClientVariableAsInt error:%d", error);
    }
    return (CSTeamSpeakTalkStatus)status;
}

bool CSTeamSpeak::isMuted(ushort clientId) const {
    int mute = 0;
    uint error = ts3client_getClientVariableAsInt(_serverConnectionHandlerId, clientId, CLIENT_IS_MUTED, &mute);
    if (error != ERROR_ok) {
        CSErrorLog("getClientVariableAsInt error:%d", error);
    }
    return mute;
}

void CSTeamSpeak::onConnectStatusChangeEvent(uint64 serverConnectionHandlerId, int status, uint error) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId) {
        _status = status;
        
        if (error == ERROR_ok) {
            if (status == STATUS_CONNECTION_ESTABLISHED) {
                uint error = ts3client_getClientID(_serverConnectionHandlerId, &_clientId);
                if (error != ERROR_ok) {
                    CSErrorLog("getClientID error:%d", error);
                    if (_delegate) _delegate->onTeamSpeakError(error);
                }
                else {
                    if (_delegate) _delegate->onTeamSpeakConnect();
                    
                    if (_channelName) {
                        moveChannel();
                    }
                }
            }
        }
        else {
            if (_delegate) _delegate->onTeamSpeakError(error);
        }
    }
}

void CSTeamSpeak::onNewChannelCreatedEvent(uint64 serverConnectionHandlerId, uint64 channelId, ushort invokerId) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId && _clientId == invokerId) {
#ifdef CS_ASSERT_DEBUG
        CSAssert(_channelName, "invalid operation");
        char* name;
        uint error = ts3client_getChannelVariableAsString(_serverConnectionHandlerId, channelId, CHANNEL_NAME, &name);
        if (error == ERROR_ok) {
            CSAssert(strcmp(_channelName, name) == 0, "invalid operation");
            ts3client_freeMemory(name);
        }
#endif
        if (_delegate) _delegate->onTeamSpeakMoveChannel(_channelName, true);
    }
}

void CSTeamSpeak::onClientMoveEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 channelId) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId && _clientId == clientId) {
#ifdef CS_ASSERT_DEBUG
        CSAssert(_channelName, "invalid operation");
        char* name;
        uint error = ts3client_getChannelVariableAsString(_serverConnectionHandlerId, channelId, CHANNEL_NAME, &name);
        if (error == ERROR_ok) {
            CSAssert(strcmp(_channelName, name) == 0, "invalid operation");
            ts3client_freeMemory(name);
        }
#endif
        if (_delegate) _delegate->onTeamSpeakMoveChannel(_channelName, true);
    }
}

void CSTeamSpeak::onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 channelId) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId && _clientId == clientId) {
        char* name;
        uint error = ts3client_getChannelVariableAsString(_serverConnectionHandlerId, channelId, CHANNEL_NAME, &name);
        if (error == ERROR_ok) {
            if (_delegate) _delegate->onTeamSpeakMoveChannel(name, false);
            ts3client_freeMemory(name);
        }
        else {
            CSErrorLog("getChannelVariableAsString error:%d", error);
            if (_delegate) _delegate->onTeamSpeakError(error);
        }
    }
}

void CSTeamSpeak::onClientMoveTimeoutEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 channelId) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId && _clientId == clientId) {
        if (_delegate) _delegate->onTeamSpeakError(CSTeamSpeakCustomErrorMoveChannelTimeout);
    }
}

void CSTeamSpeak::onTalkStatusChangeEvent(uint64 serverConnectionHandlerId, int status, ushort clientId) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId) {
        if (_delegate) _delegate->onTeamSpeakTalk(clientId, (CSTeamSpeakTalkStatus)status);
    }
}

void CSTeamSpeak::onServerErrorEvent(uint64 serverConnectionHandlerId, uint error) {
    if (serverConnectionHandlerId == _serverConnectionHandlerId) {
        if (error == ERROR_channel_name_inuse && _channelName) {
            moveChannel();
        }
        else {
            if (_delegate) _delegate->onTeamSpeakError(error);
        }
    }
}
