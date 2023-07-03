//
//  CSColor.cpp
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSColor.h"

#include "CSBuffer.h"

const CSColor CSColor::White(1.0f, 1.0f, 1.0f, 1.0f);
const CSColor CSColor::Black(0.0f, 0.0f, 0.0f, 1.0f);
const CSColor CSColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
const CSColor CSColor::Gray(0.5f, 0.5f, 0.5f, 1.0f);
const CSColor CSColor::DarkGray(0.25f, 0.25f, 0.25f, 1.0f);
const CSColor CSColor::LightGray(0.75f, 0.75f, 0.75f, 1.0f);
const CSColor CSColor::Red(1.0f, 0.0f, 0.0f, 1.0f);
const CSColor CSColor::DarkRed(0.5f, 0.0f, 0.0f, 1.0f);
const CSColor CSColor::LightRed(1.0f, 0.5f, 0.5f, 1.0f);
const CSColor CSColor::Green(0.0f, 1.0f, 0.0f, 1.0f);
const CSColor CSColor::DarkGreen(0.0f, 0.5f, 0.0f, 1.0f);
const CSColor CSColor::LightGreen(0.5f, 1.0f, 0.5f, 1.0f);
const CSColor CSColor::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const CSColor CSColor::DarkBlue(0.0f, 0.0f, 0.5f, 1.0f);
const CSColor CSColor::LightBlue(0.0f, 1.0f, 1.0f, 1.0f);
const CSColor CSColor::Orange(1.0f, 0.5f, 0.0f, 1.0f);
const CSColor CSColor::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const CSColor CSColor::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
const CSColor CSColor::Brown(0.282f, 0.227f, 0.176f, 1.0f);
const CSColor CSColor::TranslucentBlack(0.0f, 0.0f, 0.0f, 0.5f);
const CSColor CSColor::TranslucentWhite(1.0f, 1.0f, 1.0f, 0.5f);

CSColor::CSColor(CSBuffer* buffer, bool normalized) {
    if (normalized) {
        r = (buffer->readByte() & 0xff) / 255.0f;
        g = (buffer->readByte() & 0xff) / 255.0f;
        b = (buffer->readByte() & 0xff) / 255.0f;
        a = (buffer->readByte() & 0xff) / 255.0f;
    }
    else {
        r = buffer->readFloat();
        g = buffer->readFloat();
        b = buffer->readFloat();
        a = buffer->readFloat();
    }
}

CSColor::CSColor(const byte*& raw, bool normalized) {
    if (normalized) {
        r = (readByte(raw) & 0xff) / 255.0f;
        g = (readByte(raw) & 0xff) / 255.0f;
        b = (readByte(raw) & 0xff) / 255.0f;
        a = (readByte(raw) & 0xff) / 255.0f;
    }
    else {
        r = readFloat(raw);
        g = readFloat(raw);
        b = readFloat(raw);
        a = readFloat(raw);
    }
}
