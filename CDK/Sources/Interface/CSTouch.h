//
//  CSTouch.h
//  CDK
//
//  Created by 김찬 on 12. 8. 10..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSTouch__
#define __CDK__CSTouch__

#include "CSLayerContainer.h"

enum CSTouchButton : byte {
	CSTouchButtonMouseLeft,		//same as GLFW_MOUSE_BUTTON
	CSTouchButtonMouseRight,
	CSTouchButtonMouseMiddle,
	CSTouchButtonMouse4,
	CSTouchButtonMouse5,
	CSTouchButtonMouse6,
	CSTouchButtonMouse7,
	CSTouchButtonMouse8,
	CSTouchButtonFinger
};

enum CSTouchState : byte {
    CSTouchStateStopped,
    CSTouchStateMoved,
    CSTouchStateEnded
};

class CSTouch : public CSObject {
private:
    uint _key;
	CSTouchButton _button;
    CSVector2 _firstPoint;
    CSVector2 _prevPoint;
    CSVector2 _point;
    CSTouchState _state;
    double _timestamp;
public:
    CSTouch(uint key, CSTouchButton button, const CSVector2& point);
private:
    inline ~CSTouch() {
    
    }
public:
    static inline CSTouch* touch(uint key, CSTouchButton button, const CSVector2& point) {
        return autorelease(new CSTouch(key, button, point));
    }
    
    void setPoint(const CSVector2& point);
    
    inline uint key() const {
        return _key;
    }

	inline CSTouchButton button() const {
		return _button;
	}
    
    inline CSTouchState state() const {
        return _state;
    }
    
    inline void end() {
        _state = CSTouchStateEnded;
    }
    
    inline bool isMoved() const {
        return _prevPoint != _point;
    }
    
    inline CSVector2 convertToLayerSpace(const CSLayerContainer* layer, CSVector2 point) const {
        layer->convertToLocalSpace(&point);
        return point;
    }
    
    inline const CSVector2& firstPoint() const {
        return _firstPoint;
    }
    
    inline const CSVector2& prevPoint() const {
        return _prevPoint;
    }
    
    inline const CSVector2& point() const {
        return _point;
    }
    
    inline CSVector2 firstPoint(const CSLayerContainer* layer) const {
        return convertToLayerSpace(layer, _firstPoint);
    }
    
    inline CSVector2 prevPoint(const CSLayerContainer* layer) const {
        return convertToLayerSpace(layer, _prevPoint);
    }
    
    inline CSVector2 point(const CSLayerContainer* layer) const {
        return convertToLayerSpace(layer, _point);
    }
    
    float time() const;

	inline bool isEqualToTouch(const CSTouch* touch) const {
		return _key == touch->key();
	}

	inline uint hash() const override {
		return _key;
	}

	inline bool isEqual(const CSObject* object) const override {
		const CSTouch* touch = dynamic_cast<const CSTouch*>(object);

		return touch && isEqualToTouch(touch);
	}
};

#endif /* defined(__CDK__CSTouch__) */
