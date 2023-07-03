//
//  CSLayerContainer.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSLayerContainer__
#define __CDK__CSLayerContainer__

#include "CSVector2.h"

#include "CSObject.h"

class CSLayer;
class CSView;

class CSLayerContainer : public CSObject {
public:
    virtual void refresh() = 0;
    virtual CSView* view() const = 0;
    virtual CSLayerContainer* parent() const = 0;
    virtual CSLayer* findParent(int key, bool mask = false) = 0;
    virtual void convertToLocalSpace(CSVector2* point) const = 0;
    virtual void convertToParentSpace(CSVector2* point, const CSLayerContainer* parent) const = 0;
    virtual void convertToParentSpace(CSVector2* point, int key, bool mask = false) const = 0;
    virtual void convertToViewSpace(CSVector2* point) const = 0;
    virtual void submitLayout() = 0;
    virtual uint transition() const = 0;
    virtual float transitionProgress() const = 0;
    
	virtual void focusLayer(CSLayer* layer, float duration = 0) {}

    bool contains(const CSLayerContainer* layer) const;
};

#endif /* defined(__CDK__CSLayerContainer__) */
