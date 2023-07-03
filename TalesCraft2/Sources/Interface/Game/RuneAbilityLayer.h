//
//  RuneAbilityLayer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 6. 13..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef RuneAbilityLayer_h
#define RuneAbilityLayer_h

#include "Rune.h"

class RuneAbilityLayer : public CSLayer {
public:
	CSHandler<CSLayer*, bool> OnConfirm;
private:
	CSScrollPane* _pane;
	const Rune* _rune;
	const CSArray<AbilityDescription>* _values;
	const CSArray<AbilityDescription>* _descriptions;
	CSVector2 _position;
	CSButton* _trainConfirmButton;
	CSButton* _trainCancelButton;
	bool _train;
public:
	RuneAbilityLayer(const Rune* rune, const CSVector2& position, bool train);
private:
	~RuneAbilityLayer();
public:
	static inline RuneAbilityLayer* layer(const Rune* rune, const CSVector2& position, bool train) {
		return autorelease(new RuneAbilityLayer(rune, position, train));
	}
private:
	void resetFrame();

	void onDraw(CSGraphics* graphics) override;
	void onTouchesEnded() override;
	void onTouchesCancelled() override;
	void onTimeout() override;

	void onDrawPaneContent(CSLayer* layer, CSGraphics* graphics);

	void onDrawTrainConfirm(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedTrainConfirm(CSLayer* layer);

	void onDrawTrainCancel(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedTrainCancel(CSLayer* layer);
};

#endif /* RuneAbilityLayer_h */
