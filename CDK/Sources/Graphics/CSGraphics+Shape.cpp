//
//  CSGraphics+Shape.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

void CSGraphics::drawPoint(const CSVector3& point) {
    begin();
    
    addVertex(CSVertex(point));
    
    addIndex(0);
    
    end(NULL, GL_POINTS);
}

void CSGraphics::drawLine(const CSVector3& point1, const CSVector3& point2) {
    begin();
    
    addVertex(CSVertex(point1));
    addVertex(CSVertex(point2));
    
    addIndex(0);
    addIndex(1);
    
    end(NULL, GL_LINES);
}

void CSGraphics::drawGradientLine(const CSVector3& point1, const CSColor& color1, const CSVector3& point2, const CSColor& color2) {
    begin();
    
    addVertex(CSVertex(point1, color1));
    addVertex(CSVertex(point2, color2));
    
    addIndex(0);
    addIndex(1);
    
    end(NULL, GL_LINES);
}

void CSGraphics::drawRect(const CSZRect& rect, bool fill) {
    begin();
    
    addVertex(CSVertex(CSVector3(rect.left(), rect.top(), rect.origin.z)));
    addVertex(CSVertex(CSVector3(rect.right(), rect.top(), rect.origin.z)));
    addVertex(CSVertex(CSVector3(rect.left(), rect.bottom(), rect.origin.z)));
    addVertex(CSVertex(CSVector3(rect.right(), rect.bottom(), rect.origin.z)));
    
    if (fill) {
        addIndex(0);
        addIndex(1);
        addIndex(2);
        addIndex(1);
        addIndex(3);
        addIndex(2);
        
        end(NULL, GL_TRIANGLES);
    }
    else {
        addIndex(0);
        addIndex(1);
        addIndex(1);
        addIndex(3);
        addIndex(3);
        addIndex(2);
        addIndex(2);
        addIndex(0);
        
        end(NULL, GL_LINES);
    }
}

void CSGraphics::drawGradientRect(const CSZRect& rect, const CSColor& leftTopColor, const CSColor& rightTopColor, const CSColor& leftBottomColor, const CSColor& rightBottomColor, bool fill) {
    begin();
    
    addVertex(CSVertex(CSVector3(rect.left(), rect.top(), rect.origin.z), leftTopColor));
    addVertex(CSVertex(CSVector3(rect.right(), rect.top(), rect.origin.z), rightTopColor));
    addVertex(CSVertex(CSVector3(rect.left(), rect.bottom(), rect.origin.z), leftBottomColor));
    addVertex(CSVertex(CSVector3(rect.right(), rect.bottom(), rect.origin.z), rightBottomColor));
    
    if (fill) {
        addIndex(0);
        addIndex(1);
        addIndex(2);
        addIndex(1);
        addIndex(3);
        addIndex(2);
        
        end(NULL, GL_TRIANGLES);
    }
    else {
        addIndex(0);
        addIndex(1);
        addIndex(1);
        addIndex(3);
        addIndex(3);
        addIndex(2);
        addIndex(2);
        addIndex(0);
        
        end(NULL, GL_LINES);
    }
}

static float radianDistance(float radius) {
    return radius > 1 ? CSMath::acos((radius - 1) / radius) : FloatPiOverTwo;
}

void CSGraphics::drawRoundRect(const CSZRect& rect, float radius, bool fill, CSCorner corner) {
    if (rect.size.width < radius * 2 || rect.size.height < radius * 2) {
        return;
    }
    
    begin();
    
    float d = radianDistance(radius);
    
    float s, e;
    
    if (corner & CSCornerLeftTop) {
        s = FloatPi;
        e = FloatPi * 1.5f;
        for (float t = s; t < e; t += d) {
            float sx = rect.left() + radius + CSMath::cos(t) * radius;
            float sy = rect.top() + radius + CSMath::sin(t) * radius;
            addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z)));
        }
        addVertex(CSVertex(CSVector3(rect.left() + radius, rect.top(), rect.origin.z)));
    }
    else {
        addVertex(CSVertex(CSVector3(rect.left(), rect.top(), rect.origin.z)));
    }
    
    if (corner & CSCornerRightTop) {
        s = FloatPi * 1.5f;
        e = FloatTwoPi;
        for (float t = s; t < e; t += d) {
            float sx = rect.right() - radius + CSMath::cos(t) * radius;
            float sy = rect.top() + radius + CSMath::sin(t) * radius;
            addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z)));
        }
        addVertex(CSVertex(CSVector3(rect.right(), rect.top() + radius, rect.origin.z)));
    }
    else {
        addVertex(CSVertex(CSVector3(rect.right(), rect.top(), rect.origin.z)));
    }
    
    if (corner & CSCornerRightBottom) {
        s = 0.0f;
        e = FloatPiOverTwo;
        for (float t = s; t < e; t += d) {
            float sx = rect.right() - radius + CSMath::cos(t) * radius;
            float sy = rect.bottom() - radius + CSMath::sin(t) * radius;
            addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z)));
        }
        addVertex(CSVertex(CSVector3(rect.right() - radius, rect.bottom(), rect.origin.z)));
    }
    else {
        addVertex(CSVertex(CSVector3(rect.right(), rect.bottom(), rect.origin.z)));
    }
    
    if (corner & CSCornerLeftBottom) {
        s = FloatPiOverTwo;
        e = FloatPi;
        for (float t = s; t < e; t += d) {
            float sx = rect.left() + radius + CSMath::cos(t) * radius;
            float sy = rect.bottom() - radius + CSMath::sin(t) * radius;
            addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z)));
        }
        addVertex(CSVertex(CSVector3(rect.left(), rect.bottom() - radius, rect.origin.z)));
    }
    else {
        addVertex(CSVertex(CSVector3(rect.left(), rect.bottom(), rect.origin.z)));
    }
    
    if (fill) {
        for (uint i = 0; i < vertexCount() - 2; i++) {
            addIndex(0);
            addIndex(i + 1);
            addIndex(i + 2);
        }
        end(NULL, GL_TRIANGLES);
    }
    else {
        for (uint i = 0; i < vertexCount() - 1; i++) {
            addIndex(i);
            addIndex(i + 1);
        }
        addIndex(vertexCount() - 1);
        addIndex(0);
        
        end(NULL, GL_LINES);
    }
}

void CSGraphics::drawGradientRoundRect(const CSZRect& rect, float radius, const CSColor& leftTopColor, const CSColor& rightTopColor, const CSColor& leftBottomColor, const CSColor& rightBottomColor, bool fill) {
    if (rect.size.width < radius * 2 || rect.size.height < radius * 2) {
        return;
    }
    
    begin();
    
    float d = radianDistance(radius);
    
    if (fill) {
        addVertex(CSVertex(CSVector3(rect.center(), rect.middle(), rect.origin.z), (leftTopColor + rightTopColor + leftBottomColor + rightBottomColor) * 0.25f));
    }
    float s = FloatPi;
    float e = FloatPi * 1.5f;
    for (float t = s; t < e; t += d) {
        float sx = rect.left() + radius + CSMath::cos(t) * radius;
        float sy = rect.top() + radius + CSMath::sin(t) * radius;
        float xr = (sx - rect.left()) / rect.size.width;
        float yr = (sy - rect.top()) / rect.size.height;
        CSColor color = CSColor::lerp(CSColor::lerp(leftTopColor, rightTopColor, xr), CSColor::lerp(leftBottomColor, rightBottomColor, xr), yr);
        addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z), color));
    }
    {
        float r = radius / rect.size.width;
        CSColor color = CSColor::lerp(leftTopColor, rightTopColor, r);
        addVertex(CSVertex(CSVector3(rect.left() + radius, rect.top(), rect.origin.z), color));
    }
    
    s = FloatPi * 1.5f;
    e = FloatTwoPi;
    for (float t = s; t < e; t += d) {
        float sx = rect.right() - radius + CSMath::cos(t) * radius;
        float sy = rect.top() + radius + CSMath::sin(t) * radius;
        float xr = (sx - rect.left()) / rect.size.width;
        float yr = (sy - rect.top()) / rect.size.height;
        CSColor color = CSColor::lerp(CSColor::lerp(leftTopColor, rightTopColor, xr), CSColor::lerp(leftBottomColor, rightBottomColor, xr), yr);
        addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z), color));
    }
    {
        float r = radius / rect.size.height;
        CSColor color = CSColor::lerp(rightTopColor, rightBottomColor, r);
        addVertex(CSVertex(CSVector3(rect.right(), rect.top() + radius, rect.origin.z), color));
    }
    
    s = 0.0f;
    e = FloatPiOverTwo;
    for (float t = s; t < e; t += d) {
        float sx = rect.right() - radius + CSMath::cos(t) * radius;
        float sy = rect.bottom() - radius + CSMath::sin(t) * radius;
        float xr = (sx - rect.left()) / rect.size.width;
        float yr = (sy - rect.top()) / rect.size.height;
        CSColor color = CSColor::lerp(CSColor::lerp(leftTopColor, rightTopColor, xr), CSColor::lerp(leftBottomColor, rightBottomColor, xr), yr);
        addVertex(CSVertex(CSVector3(sx, sy, rect.origin.z), color));
    }
    {
        float r = (rect.size.width - radius) / rect.size.width;
        CSColor color = CSColor::lerp(leftBottomColor, rightBottomColor, r);
        addVertex(CSVertex(CSVector3(rect.right() - radius, rect.bottom(), rect.origin.z), color));
    }
    s = FloatPiOverTwo;
    e = FloatPi;
    for (float t = s; t < e; t += d) {
        float sx = rect.left() + radius + CSMath::cos(t) * radius;
        float sy = rect.bottom() - radius + CSMath::sin(t) * radius;
        float xr = (sx - rect.left()) / rect.size.width;
        float yr = (sy - rect.top()) / rect.size.height;
        CSColor color = CSColor::lerp(CSColor::lerp(leftTopColor, rightTopColor, xr), CSColor::lerp(leftBottomColor, rightBottomColor, xr), yr);
        addVertex(CSVertex(CSVector3(sx, sy, 0), color));
    }
    {
        float r = (rect.size.height - radius) / rect.size.height;
        CSColor color = CSColor::lerp(leftTopColor, leftBottomColor, r);
        addVertex(CSVertex(CSVector3(rect.left(), rect.bottom() - radius, rect.origin.z), color));
    }
    
    if (fill) {
        for (uint i = 0; i < vertexCount() - 2; i++) {
            addIndex(0);
            addIndex(i + 1);
            addIndex(i + 2);
        }
        addIndex(0);
        addIndex(vertexCount() - 1);
        addIndex(1);
        
        end(NULL, GL_TRIANGLES);
    }
    else {
        for (uint i = 0; i < vertexCount() - 1; i++) {
            addIndex(i);
            addIndex(i + 1);
        }
        addIndex(vertexCount() - 1);
        addIndex(0);
        
        end(NULL, GL_LINES);
    }
}

void CSGraphics::drawArc(const CSZRect& rect, float angle1, float angle2, bool fill) {
    if (angle1 < angle2 && rect.size.width && rect.size.height) {
        begin();
        
        float d = radianDistance(CSMath::max(rect.size.width, rect.size.height) * 0.5f);
        
        float hw = rect.size.width * 0.5f;
        float hh = rect.size.height * 0.5f;
        float cx = rect.origin.x + hw;
        float cy = rect.origin.y + hh;
        
        if (fill) {
            addVertex(CSVertex(CSVector3(cx, cy, rect.origin.z)));
        }
        float x;
        float y;
        
        for (float a = angle1; a < angle2; a += d) {
            x = cx + hw * CSMath::cos(a);
            y = cy + hh * CSMath::sin(a);
            addVertex(CSVertex(CSVector3(x, y, rect.origin.z)));
        }
        x = cx + hw * CSMath::cos(angle2);
        y = cy + hh * CSMath::sin(angle2);
        addVertex(CSVertex(CSVector3(x, y, rect.origin.z)));
        if (fill) {
            for (int i = 0; i < vertexCount() - 2; i++) {
                addIndex(0);
                addIndex(i + 1);
                addIndex(i + 2);
            }
            end(NULL, GL_TRIANGLES);
        }
        else {
            for (int i = 0; i < vertexCount() - 1; i++) {
                addIndex(i);
                addIndex(i + 1);
            }
            end(NULL, GL_LINES);
        }
    }
}

void CSGraphics::drawGradientArc(const CSZRect& rect, float angle1, float angle2, const CSColor& centerColor, const CSColor& surroundColor) {
    if (angle1 < angle2 && rect.size.width && rect.size.height) {
        begin();
        
        float d = radianDistance(CSMath::max(rect.size.width, rect.size.height) * 0.5f);
        
        float hw = rect.size.width * 0.5f;
        float hh = rect.size.height * 0.5f;
        float cx = rect.origin.x + hw;
        float cy = rect.origin.y + hh;
        
        addVertex(CSVertex(CSVector3(cx, cy, rect.origin.z), centerColor));
        
        float x;
        float y;
        for (float a = angle1; a < angle2; a += d) {
            x = cx + hw * CSMath::cos(a);
            y = cy + hh * CSMath::sin(a);
            addVertex(CSVertex(CSVector3(x, y, rect.origin.z), surroundColor));
        }
        x = cx + hw * CSMath::cos(angle2);
        y = cy + hh * CSMath::sin(angle2);
        addVertex(CSVertex(CSVector3(x, y, rect.origin.z), surroundColor));
        
        for (int i = 0; i < vertexCount() - 2; i++) {
            addIndex(0);
            addIndex(i + 1);
            addIndex(i + 2);
        }
        end(NULL, GL_TRIANGLES);
    }
}
