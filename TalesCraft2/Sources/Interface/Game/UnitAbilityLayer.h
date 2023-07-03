//
//  UnitAbilityLayer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 6. 13..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef UnitAbilityLayer_h
#define UnitAbilityLayer_h

#include "UnitBase.h"

class UnitAbilityLayer : public CSLayer {
private:
	CSScrollPane* _pane;
	const CSObject* _tag;
    const UnitBase* _unit;
    const CSArray<AbilityDescription>* _values;
    const CSArray<AbilityDescription>* _descriptions;
    CSVector2 _position;
public:
    UnitAbilityLayer(const CSObject* tag, const UnitBase* unit, const CSVector2& position);
private:
    ~UnitAbilityLayer();
public:
    static inline UnitAbilityLayer* layer(const CSObject* tag, const UnitBase* unit, const CSVector2& position) {
        return autorelease(new UnitAbilityLayer(tag, unit, position));
    }
private:
    void resetFrame();
    
	void onDraw(CSGraphics* graphics) override;
	void onTouchesEnded() override;
    void onTouchesCancelled() override;
    void onTimeout() override;

	void onDrawPaneContent(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedPane(CSLayer* layer);

	void onDrawAttackType(CSLayer* layer, CSGraphics* graphics);
	void onDrawArmorType(CSLayer* layer, CSGraphics* graphics);
};

#endif /* UnitAbilityLayer_h */
