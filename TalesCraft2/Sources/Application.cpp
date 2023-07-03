//
//  Application.cpp
//  TalesCraft2
//
//  Created by 김찬 on 12. 7. 31..
//  Copyright (c) 2012brgames. All rights reserved.
//
#define AssetImpl

#include "Account.h"

#include "PVPGateway.h"

#include "TitleLayer.h"
#include "LobbyLayer.h"

float ProjectionWidth;
float ProjectionHeight;
float ProjectionBaseX;
float ProjectionBaseY;

void onStart(CSView* rootView) {
    CSApplication::sharedApplication()->setVersion(ApplicationVersion);
    
    CSRect bounds = rootView->bounds();
    float rate = CSMath::max(BaseProjectionWidth / bounds.size.width, BaseProjectionHeight / bounds.size.height);
    ProjectionWidth = bounds.size.width * rate;
    ProjectionHeight = bounds.size.height * rate;
    ProjectionBaseX = (ProjectionWidth - BaseProjectionWidth) / 2;
    ProjectionBaseY = (ProjectionHeight - BaseProjectionHeight) / 2;
    CSLog("projection:%.2f, %.2f", ProjectionWidth, ProjectionHeight);
    
    rootView->setTouchSensitivity(TouchSensitivity);
    rootView->setProjection(0, ProjectionWidth, ProjectionHeight, 1, 10000);
    rootView->setPopupDarkness(0.4f);
    rootView->setPopupBlur(1.0f);

    Asset::load();
    PVPGateway::load();
    Account::load();
    
    rootView->addLayer(TitleLayer::layer());
    //rootView->addLayer(LobbyLayer::layer());
}

void onPause(CSView* rootView) {

}

void onResume(CSView* rootView) {

}

void onReload(CSView* rootView) {
    Asset::sharedAsset()->reload();
}

void onMemoryWarning(CSView* rootView, CSMemoryWarningLevel level) {
    Asset::sharedAsset()->memoryWarning(level);
}

void onDestroy() {
	PVPGateway::destroy();
	Account::destroy();
	Asset::destroy();
}

void onReceiveQuery(CSView* rootView, const char* url, const CSDictionary<CSString, CSString>* queries) {
    
}
