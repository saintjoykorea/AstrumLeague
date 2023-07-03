//
//  CSWebView.h
//  CDK
//
//  Created by ChangSun on 19. 7. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSVideoView__
#define __CDK__CSVideoView__

#include "CSLayer.h"

class CSVideoView : public CSLayer {
public:
	CSHandler<CSVideoView*> OnPlayFinish;
private:
    void* _handle;
public:
    CSVideoView();
protected:
    virtual ~CSVideoView();
public:
    static inline CSVideoView* videoView() {
        return autorelease(new CSVideoView());
    }
    
    void playStart(const char* path);

    void onPlayFinish();
    
private:
    void applyFrame();
    
protected:
    virtual void onStateChanged() override;
    virtual void onFrameChanged() override;
    virtual void onProjectionChanged() override;
};

#endif /* defined(__CDK__CSVideoView__) */
