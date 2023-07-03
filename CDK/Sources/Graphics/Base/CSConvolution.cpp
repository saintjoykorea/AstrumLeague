//
//  CSConvolution.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSConvolution.h"

#include "CSMath.h"

#include "CSMemory.h"

CSConvolution::CSConvolution() : _width(0), _kernel(NULL) {

}

CSConvolution::CSConvolution(uint width, const float* kernel, float factor) : _width(width) {
    if (_width) {
        uint len = length();
        _kernel = (float*)fcalloc(len, sizeof(float));
        if (factor != 1.0f) {
            for (uint i = 0; i < len; i++) {
                _kernel[i] *= factor;
            }
        }
    }
    else {
        _kernel = NULL;
    }
}

CSConvolution::CSConvolution(const CSConvolution& other) : _width(other._width) {
    if (_width) {
        uint len = length();
        _kernel = (float*)fmalloc(len * sizeof(float));
        memcpy(_kernel, other._kernel, len * sizeof(float));
    }
    else {
        _kernel = NULL;
    }
}
CSConvolution::~CSConvolution() {
    if (_kernel) {
        free(_kernel);
    }
}

bool CSConvolution::operator==(const CSConvolution& other) const {
    return _width == other._width && (!_width || memcmp(_kernel, other._kernel, length() * sizeof(float)) == 0);
}

CSConvolution& CSConvolution::operator *=(const CSConvolution& other) {
    if (other._width) {
        if (other._width > _width) {
            uint len = other.length();
            float* kernel = (float*)fcalloc(len, sizeof(float));
            if (!_kernel) {
                memcpy(kernel, other._kernel, len * sizeof(float));
                _kernel = kernel;
                _width = other._width;
                return *this;
            }
            
            for (int sy = -_width; sy <= _width; sy++) {
                for (int sx = -_width; sx <= _width; sx++) {
                    kernel[toIndex(sx, sy, other._width)] = _kernel[toIndex(sx, sy, _width)];
                }
            }
            free(_kernel);
            _kernel = kernel;
            _width = other._width;
        }
        for (int sy = -_width; sy <= _width; sy++) {
            for (int sx = -_width; sx <= _width; sx++) {
                float sum = 0;
                for (int dy = -other._width; dy <= other._width; dy++) {
                    for (int dx = -other._width; dx <= other._width; dx++) {
                        int cx = sx + dx;
                        int cy = sy + dy;
                        if (cx >= -_width && cx <= _width && cy >= -_width && cy <= _width) {
                            sum += _kernel[toIndex(cx, cy)] * other._kernel[other.toIndex(dx, dy)];
                        }
                    }
                }
                _kernel[toIndex(sx, sy)] = sum;
            }
        }
    }
    return *this;
}

CSConvolution CSConvolution::operator *(const CSConvolution& other) const {
    CSConvolution conv(*this);
    conv *= other;
    return conv;
}

CSConvolution& CSConvolution::operator*=(float weight) {
    if (weight < 0) weight = 0;
    if (_width && weight <= 1) {
        uint len = length();
        for (int i = 0; i < len; i++) {
            _kernel[i] *= weight;
        }
        _kernel[len / 2] += 1 - weight;
    }
    return *this;
}

CSConvolution CSConvolution::operator*(float weight) const {
    CSConvolution convolution(*this);
    convolution *= weight;
    return convolution;
}

CSConvolution& CSConvolution::operator =(const CSConvolution& other) {
    if (_kernel) {
        free(_kernel);
    }
    _width = other._width;
    if (_width) {
        uint len = length();
        _kernel = (float*)fmalloc(len * sizeof(float));
        memcpy(_kernel, other._kernel, len * sizeof(float));
    }
    else {
        _kernel = NULL;
    }
    return *this;
}

const float CSConvolution::KernelMean_1[] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1
};

const float CSConvolution::KernelMean_2[] = {
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1
};

const float CSConvolution::KernelSmoothing_1[] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1
};

const float CSConvolution::KernelSmoothing_2[] = {
    1, 4,  7,  4,  1,
    4, 16, 26, 16, 4,
    7, 26, 41, 26, 7,
    4, 16, 26, 16, 4,
    1, 4,  7,  4,  1
};

const float CSConvolution::KernelSharpen_1[] = {
    -1, -1, -1,
    -1, 9, -1,
    -1, -1, -1
};

const float CSConvolution::KernelSharpen_2[] = {
    0, -1, -1, -1, 0,
    -1, 2, 2,  2,  -1,
    -1, 2, 8,  2,  -1,
    -1, 2, 2,  2,  -1,
    0, -1, -1, -1, 0
};

const CSConvolution CSConvolution::None(0, NULL, 1);

const CSConvolution CSConvolution::Mean_1(1, CSConvolution::KernelMean_1, 1.0f / 9.0f);
const CSConvolution CSConvolution::Mean_2(2, CSConvolution::KernelMean_2, 1.0f / 25.0f);
const CSConvolution CSConvolution::Smoothing_1(1, CSConvolution::KernelSmoothing_1, 1.0f / 16.0f);
const CSConvolution CSConvolution::Smoothing_2(2, CSConvolution::KernelSmoothing_2, 1.0f / 273.0f);
const CSConvolution CSConvolution::Sharpen_1(1, CSConvolution::KernelSharpen_1, 1.0f);
const CSConvolution CSConvolution::Sharpen_2(2, CSConvolution::KernelSharpen_2, 1.0f);
