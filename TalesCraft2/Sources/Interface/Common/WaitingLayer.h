//
//  WaitingLayer.h
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 22..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef WaitingLayer_h
#define WaitingLayer_h

#include "GatewayDelegate.h"
#include "Animation.h"

class WaitingLayer : public CSLayer {
private:
    Animation* _animation;
    CSArray<CSLayer>* _owners;
    CSButton* _expireButton;
	bool _expire;
	float _alpha;

    WaitingLayer();
    ~WaitingLayer();
    
    static WaitingLayer* __layer;
public:
    static void show(CSLayer* owner, bool expire = true);
    static bool hide(CSLayer* owner);
    static void hideAll();
    static bool contains(CSLayer* owner);
private:
    void onDraw(CSGraphics* graphics) override;
    void onTimeout() override;

    void onDrawExpire(CSLayer* layer, CSGraphics* graphics);
    void onTouchesEndedExpire(CSLayer* layer);
};

#endif /* WaitingLayer_h */
