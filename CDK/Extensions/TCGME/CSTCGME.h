//
//  CSTCGME.h
//  CDK
//
//  Created by Kim Chan on 2019. 7. 3..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef CSTCGME_h
#define CSTCGME_h

#include "CSTypes.h"

enum CSTCGMESoundQuality {
    CSTCGMESoundQualityLow,
    CSTCGMESoundQualityNormal,
    CSTCGMESoundQualityHigh
};

#define CSTCGMEDefaultSpeakerVolume	0.5f
#define CSTCGMEDefaultMicVolume		0.5f

class CSTCGMEDelegate {
public:
    inline virtual void onTCGMEEnterRoom(bool success) {}
    inline virtual void onTCGMEExitRoom(bool intensive) {}
    inline virtual void onTCGMENotifyEnterRoom(int64 userId) {}
    inline virtual void onTCGMENotifyExitRoom(int64 userId) {}
    inline virtual void onTCGMETalk(int64 userId, bool on) {}
    inline virtual void onTCGMEReconnect(bool success) {}
};

class CSTCGME {
public:
	static bool isEnabled();
    static void initialize();
    static void finalize();
	
    static void connect(int64 userId);
    static void disconnect();
    static void setDelegate(CSTCGMEDelegate* delegate);
    static void enterRoom(const char* roomId, CSTCGMESoundQuality quality);
    static void exitRoom();
    static bool isRoomEntered();
    static void setMicEnabled(bool enabled);
    static bool isMicEnabled();
    static void setSpeakerEnabled(bool enabled);
    static bool isSpeakerEnabled();
    static void setMicVolume(float volume);
    static float micVolume();
    static void setSpeakerVolume(float volume);
    static float speakerVolume();
    static void pause();
    static void resume();
};

#endif /* CSTCGME_h */
