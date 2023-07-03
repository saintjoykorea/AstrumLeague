//
//  CSAndroidView.h
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#if defined(CDK_ANDROID) && defined(CDK_IMPL)

#ifndef __CDK__CSAndroidView__
#define __CDK__CSAndroidView__

#include "CSView.h"

class CSAndroidView {
private:
	CSView* _view;
	CSGraphics* _graphics;
	uint _width;
	uint _height;
	uint _horizontalEdge;
	uint _verticalEdge;
public:
	CSAndroidView(uint width, uint height, uint bufferWidth, uint bufferHeight, uint horizontalEdge, uint verticalEdge);
	~CSAndroidView();

	inline CSView* view() {
		return _view;
	}
    
    inline CSGraphics* graphics() {
        return _graphics;
    }

    inline uint width() const {
		return _width;
	}

	inline uint height() const {
		return _height;
	}

	inline uint horizontalEdge() const {
		return _horizontalEdge;
	}

	inline uint verticalEdge() const {
		return _verticalEdge;
	}

	void touchesBegan(int num, int ids[], float xs[], float ys[]);
	void touchesMoved(int num, int ids[], float xs[], float ys[]);
	void touchesCancelled(int num, int ids[], float xs[], float ys[]);
	void touchesEnded(int num, int ids[], float xs[], float ys[]);
	void backKey();
    void setKeyboardHeight(float height);

    inline uint bufferWidth() const {
        return _graphics->target()->width();
    }
    inline uint bufferHeight() const {
        return _graphics->target()->height();
    }
    void resizeBuffer(uint bufferWidth, uint bufferHeight);
    void reload();
    void render(bool refresh);
};

#endif /* defined(__CDK__CSAndroidView__) */

#endif
