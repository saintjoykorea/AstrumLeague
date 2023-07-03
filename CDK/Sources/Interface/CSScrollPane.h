//
//  CSScrollPane.h
//  CDK
//
//  Created by 김찬 on 12. 8. 20..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSScrollPane__
#define __CDK__CSScrollPane__

#include "CSLayer.h"

#include "CSScroll.h"

class CSScrollPane : public CSLayer, public CSScrollParent {
public:
	DrawHandler OnDrawContent;
    CSScroll scroll;
protected:
    bool _focusScroll;
    bool _lockScroll;
public:
    CSScrollPane();
protected:
    inline virtual ~CSScrollPane() {
    
    }
    
public:
    static inline CSScrollPane* scrollPane() {
        return autorelease(new CSScrollPane());
    }
    
    inline CSSize scrollContentSize() const override {
        return contentSize();
    }

    inline CSSize scrollClipSize() const override {
        return _frame.size;
    }
    
    inline void setFocusScroll(bool focusScroll) {
        _focusScroll = focusScroll;
    }
    inline bool focusScroll() const {
        return _focusScroll;
    }
    
    inline void setLockScroll(bool lockScroll) {
        _lockScroll = lockScroll;
    }
    inline bool lockScroll() const {
        return _lockScroll;
    }
    
    void focusLayer(CSLayer* layer, float duration = 0) override;

public:
    virtual void draw(CSGraphics* graphics) override;
	virtual CSLayerTouchReturn layersOnTouch(const CSTouch* touch, CSArray<CSLayer>* targetLayers, bool began) override;
protected:
	bool touchContains(const CSVector2& p) const override;

	virtual void onDrawContent(CSGraphics* graphics);

	virtual void onTimeout() override;
    virtual void onTouchesMoved() override;
public:
    virtual void convertToLocalSpace(CSVector2* point) const override;
    virtual void convertToParentSpace(CSVector2* point, const CSLayerContainer* parent) const override;
    virtual void convertToParentSpace(CSVector2* point, int key, bool masking = false) const override;
    virtual void convertToViewSpace(CSVector2* point) const override;
};

#endif /* defined(__CDK__CSScrollPane__) */
