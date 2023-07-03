//
//  FadeMessageLayer.h
//  TalesCraft2
//
//  Created by ChangSun on 2016. 6. 21..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef FadeMessageLayer_h
#define FadeMessageLayer_h

#include "Application.h"

class FadeMessageLayer : public CSLayer {
private:
	const CSString* _message;

	static FadeMessageLayer* __layer;

	FadeMessageLayer(const CSString* message);
	~FadeMessageLayer();

	void onTimeout() override;
	void onDraw(CSGraphics* graphics) override;
public:
	static void show(CSView* view, const CSString* message);
};

#endif /* FadeMessageLayer_h */
