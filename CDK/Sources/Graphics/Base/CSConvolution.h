//
//  CSConvolution.h
//  CDK
//
//  Created by 김찬 on 12. 8. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSConvolution__
#define __CDK__CSConvolution__

#include "CSTypes.h"

#include "CSLog.h"

#define CSConvolutionMaxWidth   2

struct CSConvolution {
private:
    float* _kernel;
    int _width;
public:
    CSConvolution();
    CSConvolution(uint width, const float* kernel, float factor);
    CSConvolution(const CSConvolution& other);
    ~CSConvolution();
    
    inline operator bool() const {
        return _width != 0;
    }
    inline bool operator !() const {
        return _width == 0;
    }
    
    bool operator ==(const CSConvolution& other) const;
    
    inline bool operator !=(const CSConvolution& other) const {
        return !(*this == other);
    }
    
    CSConvolution& operator *=(const CSConvolution& other);
    CSConvolution operator *(const CSConvolution& other) const;
    CSConvolution& operator *=(float weight);
    CSConvolution operator *(float weight) const;
    
    inline operator float* () {
        return _kernel;
    }
    inline operator const float* () const {
        return _kernel;
    }
    inline float operator[] (uint index) const {
        CSAssert(index < length(), "out of range");
        return _kernel[index];
    }
    CSConvolution& operator =(const CSConvolution& other);
    
    inline uint width() const {
        return _width;
    }
    
    static inline uint length(uint width) {
        return (width * 2 + 1) * (width * 2 + 1);
    }
    
    inline uint length() const {
        return length(_width);
    }
    
    static inline uint toIndex(int x, int y, uint width) {
        return (y + width) * (2 * width + 1) + (x + width);
    }
    
    inline uint toIndex(int x, int y) const {
        return toIndex(x, y, _width);
    }
    static const float KernelMean_1[];
    static const float KernelMean_2[];
    static const float KernelSmoothing_1[];
    static const float KernelSmoothing_2[];
    static const float KernelSharpen_1[];
    static const float KernelSharpen_2[];
    
    static const CSConvolution None;
    
    static const CSConvolution Mean_1;
    static const CSConvolution Mean_2;
    static const CSConvolution Smoothing_1;
    static const CSConvolution Smoothing_2;
    static const CSConvolution Sharpen_1;
    static const CSConvolution Sharpen_2;
};


#endif
