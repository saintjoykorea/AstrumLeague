//
//  CSTeamSpeak+iOS.mm
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 5. 9..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSTeamSpeak.h"

#import "CSAudioBridge.h"

#import "teamspeak/clientlib.h"

void CSTeamSpeak::prepareAudioIOS() {
    CSLog("prepareAudioIOS");

    AVAudioSession* audioSession = [AVAudioSession sharedInstance];

    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord mode:AVAudioSessionModeVoiceChat options:19 error:nil];
    [audioSession setActive:YES error:nil];
}

void CSTeamSpeak::restoreAudioIOS(bool byPause) {
    CSLog("restoreAudioIOS:%d", byPause);

    if (byPause) {
        if (_identity) {
            free(_identity);
            _identity = NULL;
        }
        if (_serverConnectionHandlerId) {
            ts3client_destroyServerConnectionHandler(_serverConnectionHandlerId);
            _serverConnectionHandlerId = 0;
        }
        if (_libraryInitialized) {
            ts3client_destroyClientLib();
            _libraryInitialized = false;
        }
    }
    else {
        CSAudioBridge::restoreCategory();
    }
}

#endif
