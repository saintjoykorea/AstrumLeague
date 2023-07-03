//
//  Vision.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 13..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Vision_h
#define Vision_h

#include "Terrain.h"

#define VisionLuminanceHidden     110
#define VisionLuminanceFog        150

class Vision {
private:
    CSTVertexArray<CSVector3>* _vertices;
    CSScratch* _diffuse;
public:
    Vision(const Terrain* terrain);
    ~Vision();
private:
    Vision(const Vision& other);
    Vision& operator =(const Vision& other);
public:
    inline uint pixel(uint x, uint y) const {
        return _diffuse->pixel(x, y);
    }
	inline void update(uint x, uint y, uint luminance) {
		_diffuse->update(x, y, luminance);
	}
	inline void transfer() {
		_diffuse->transfer();
	}
    inline const CSRootImage* image() const {
        return _diffuse->image();
    }
    void draw(CSGraphics* graphics);
};

#endif /* Vision_h */
