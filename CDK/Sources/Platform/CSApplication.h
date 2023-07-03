//
//  lication.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013brgames. All rights reserved.
//

#ifndef __CDK__CSApplication__
#define __CDK__CSApplication__

#include "CSHandler.h"
#include "CSDictionary.h"
#include "CSString.h"

class CSView;

enum CSMemoryWarningLevel {
    CSMemoryWarningLevelNormal,
    CSMemoryWarningLevelLow,
    CSMemoryWarningLevelCritical
};

enum CSResolution : byte {
    CSResolution720,
    CSResolution1080,
    CSResolutionFit
};

class CSApplication {
private:
    uint _glContextIdentifier;
    CSString* _version;
public:
	CSHandler<> OnPause;
	CSHandler<> OnResume;
	CSHandler<> OnDestroy;
	CSHandler<> OnReload;
	CSHandler<CSMemoryWarningLevel> OnMemoryWarning;
	CSHandler<const char*, const CSDictionary<CSString, CSString>*> OnReceiveQuery;
private:
	static CSApplication __application;
#ifdef CDK_IMPL
public:
#else
private:
#endif
    CSApplication();
    ~CSApplication();
public:
    static inline CSApplication* sharedApplication() {
        return &__application;
    }
#ifdef CDK_IMPL
    inline void expireGLContextIdentifier() {
        _glContextIdentifier++;
    }
#endif
    inline uint glContextIdentifier() const {
        return _glContextIdentifier;
    }
    void setVersion(const char* version);
    inline const CSString* version() const {
        return _version;
    }
    void setResolution(CSResolution resolotion);
    CSResolution resolution() const;
    void openURL(const char* url);
    const CSString* clipboard();
    void setClipboard(const char* text);
	void shareUrl(const char* title, const char* message, const char* url);
	void finish();
};

extern void onStart(CSView* rootView);
extern void onPause(CSView* rootView);
extern void onResume(CSView* rootView);
extern void onReload(CSView* rootView);
extern void onMemoryWarning(CSView* rootView, CSMemoryWarningLevel level);
extern void onDestroy();
extern void onReceiveQuery(CSView* rootView, const char* url, const CSDictionary<CSString, CSString>* queries);

#endif
