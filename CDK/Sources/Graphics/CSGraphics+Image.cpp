//
//  CSGraphics+Image.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

void CSGraphics::drawImageImpl(const CSImage* image, const CSZRect& destRect, const CSRect& frame, const CSColor* colors) {
    begin();
    
    const CSSize& textureSize = image->textureSize();
    
    float lu = frame.left() / textureSize.width;
    float ru = frame.right() / textureSize.width;
    float tv = frame.top() / textureSize.height;
    float bv = frame.bottom() / textureSize.height;
    
    float lx = destRect.left();
    float rx = destRect.right();
    float ty = destRect.top();
    float by = destRect.bottom();
    
    if (colors) {
        addVertex(CSVertex(CSVector3(lx, ty, destRect.origin.z), colors[0], CSVector2(lu, tv)));
        addVertex(CSVertex(CSVector3(rx, ty, destRect.origin.z), colors[1], CSVector2(ru, tv)));
        addVertex(CSVertex(CSVector3(lx, by, destRect.origin.z), colors[2], CSVector2(lu, bv)));
        addVertex(CSVertex(CSVector3(rx, by, destRect.origin.z), colors[3], CSVector2(ru, bv)));
    }
    else {
        addVertex(CSVertex(CSVector3(lx, ty, destRect.origin.z), CSVector2(lu, tv)));
        addVertex(CSVertex(CSVector3(rx, ty, destRect.origin.z), CSVector2(ru, tv)));
        addVertex(CSVertex(CSVector3(lx, by, destRect.origin.z), CSVector2(lu, bv)));
        addVertex(CSVertex(CSVector3(rx, by, destRect.origin.z), CSVector2(ru, bv)));
    }
    addIndex(0);
    addIndex(1);
    addIndex(2);
    addIndex(1);
    addIndex(3);
    addIndex(2);
    
    end(image->root(), GL_TRIANGLES);
}

void CSGraphics::drawImage(const CSImage* image, const CSZRect& destRect, const CSRect& srcRect) {
    if (!image) {
        return;
    }
    CSRect subframe = srcRect;
    const CSRect& frame = image->frame();
    subframe.origin += frame.origin;
    subframe.intersect(frame);
    
    drawImageImpl(image, destRect, subframe, NULL);
}

void CSGraphics::drawImage(const CSImage* image, const CSZRect& destRect, bool stretch) {
    if (!image) {
        return;
    }
    if (stretch) {
        drawImageImpl(image, destRect, image->frame(), NULL);
    }
    else {
        const CSRect& frame = image->frame();
        float rate = CSMath::min(destRect.size.width / frame.size.width, destRect.size.height / frame.size.height);
        float x = destRect.center();
        float y = destRect.middle();
        float w = frame.size.width * rate;
        float h = frame.size.height * rate;
        drawImageImpl(image, CSZRect(x - w * 0.5f, y - h * 0.5f, destRect.origin.z, w, h), frame, NULL);
    }
}

void CSGraphics::drawImage(const CSImage* image, CSVector3 point, CSAlign align) {
    if (!image) {
        return;
    }
    const CSSize& size = image->size();
    
    applyAlign(point, size, align);
    
    drawImageImpl(image, CSZRect(point, size), image->frame(), NULL);
}

void CSGraphics::drawImage(const CSImage* image, const CSVector3& point) {
    if (!image) {
        return;
    }
    drawImageImpl(image, CSZRect(point, image->size()), image->frame(), NULL);
}

void CSGraphics::drawImageScaled(const CSImage* image, CSVector3 point, float scale, CSAlign align) {
    if (!image) {
        return;
    }
    CSSize size = image->size();
    size *= scale;
    
    applyAlign(point, size, align);
    
    drawImageImpl(image, CSZRect(point, size), image->frame(), NULL);
}

void CSGraphics::drawImageScaled(const CSImage* image, const CSVector3& point, float scale) {
    if (!image) {
        return;
    }
    drawImageImpl(image, CSZRect(point, image->size() * scale), image->frame(), NULL);
}

void CSGraphics::drawLineImage(const CSImage* image, float scroll, const CSVector3& src, const CSVector3& dest) {
    if (!image || src == dest) {
        return;
    }
    
    CSVector3 diff = dest - src;
    float len = diff.length();
    float angle = CSMath::atan2(diff.y, diff.x);
    
    push();
    translate(CSVector3(src.x, src.y, 0));
    rotateZ(angle);
    
    const CSSize& textureSize = image->textureSize();
    const CSRect& frame = image->frame();
    
    float tv = frame.top();
    float bv = frame.bottom();
    tv /= textureSize.height;
    bv /= textureSize.height;
    
    float hh = frame.size.height * 0.5f;
    
    scroll -= (int)(scroll / frame.size.width) * frame.size.width;
    
    if (scroll > 0) {
        scroll -= frame.size.width;
    }
    
    begin();
    
    int i = 0;
    do {
        float lu = 0;
        float ru = frame.size.width;
        float lx = scroll;
        float rx = scroll + frame.size.width;
        if (scroll < 0) {
            lu = -scroll;
            lx = 0;
        }
        if (scroll + frame.size.width > len) {
            ru = len - scroll;
            rx = len;
        }
        lu += frame.origin.x;
        ru += frame.origin.x;
        lu /= textureSize.width;
        ru /= textureSize.width;
        
        float lz = CSMath::lerp(src.z, dest.z, lx / len);
        float rz = CSMath::lerp(src.z, dest.z, rx / len);
        addVertex(CSVertex(CSVector3(lx, -hh, lz), CSVector2(lu, tv)));
        addVertex(CSVertex(CSVector3(rx, -hh, rz), CSVector2(ru, tv)));
        addVertex(CSVertex(CSVector3(lx, hh, lz), CSVector2(lu, bv)));
        addVertex(CSVertex(CSVector3(rx, hh, rz), CSVector2(ru, bv)));
        
        addIndex(i + 0);
        addIndex(i + 1);
        addIndex(i + 2);
        addIndex(i + 1);
        addIndex(i + 3);
        addIndex(i + 2);
        
        scroll += frame.size.width;
        i += 4;
    } while (scroll < len);
    
    end(image->root(), GL_TRIANGLES);
    
    pop();
}

void CSGraphics::drawClockImage(const CSImage* image, float progress, CSVector3 point, CSAlign align) {
    if (!image) {
        return;
    }
    CSSize size = image->size();
    
    applyAlign(point, size, align);
    
    drawClockImage(image, progress, CSZRect(point, size));
}
void CSGraphics::drawClockImageScaled(const CSImage* image, float progress, CSVector3 point, float scale, CSAlign align) {
    if (!image) {
        return;
    }
    CSSize size = image->size();
    size *= scale;
    
    applyAlign(point, size, align);
    
    drawClockImage(image, progress, CSZRect(point, size));
}
void CSGraphics::drawClockImage(const CSImage* image, float progress, const CSZRect& destRect) {
    if (!image) {
        return;
    }
    if (progress >= 1.0f) {
        drawImage(image, destRect);
        return;
    }
    float a = progress * FloatTwoPi;
    
    static const float Angle0 = FloatPiOverFour;
    static const float Angle1 = FloatPiOverFour * 3;
    static const float Angle2 = FloatPiOverFour * 5;
    static const float Angle3 = FloatPiOverFour * 7;
    
    begin();
    
    const CSRect& frame = image->frame();
    const CSSize& textureSize = image->textureSize();
    
    addVertex(CSVertex(CSVector3(destRect.center(), destRect.middle(), destRect.origin.z),
                       CSVector2(frame.center() / textureSize.width, frame.middle() / textureSize.height)));
    
    addVertex(CSVertex(CSVector3(destRect.center(), destRect.top(), destRect.origin.z),
                       CSVector2(frame.center() / textureSize.width, frame.origin.y / textureSize.height)));
    
    if (a <= Angle0) {
        float t = CSMath::tan(a);
        
        addVertex(CSVertex(CSVector3(destRect.center() + destRect.size.width * 0.5f * t, destRect.top(), destRect.origin.z),
                           CSVector2((frame.center() + frame.size.width * 0.5f * t) / textureSize.width, frame.top() / textureSize.height)));
    }
    else {
        addVertex(CSVertex(CSVector3(destRect.right(), destRect.top(), destRect.origin.z),
                           CSVector2(frame.right() / textureSize.width, frame.top() / textureSize.height)));
        
        if (a <= Angle1) {
            float t = CSMath::tan(FloatPiOverTwo - a);
            
            addVertex(CSVertex(CSVector3(destRect.right(), destRect.middle() - destRect.size.height * 0.5f * t, destRect.origin.z),
                               CSVector2(frame.right() / textureSize.width, (frame.middle() - frame.size.height * 0.5f * t) / textureSize.height)));
        }
        else {
            addVertex(CSVertex(CSVector3(destRect.right(), destRect.bottom(), destRect.origin.z),
                               CSVector2(frame.right() / textureSize.width, frame.bottom() / textureSize.height)));
            
            if (a <= Angle2) {
                float t = CSMath::tan(a - FloatPi);
                
                addVertex(CSVertex(CSVector3(destRect.center() - destRect.size.width * 0.5f * t, destRect.bottom(), destRect.origin.z),
                                   CSVector2((frame.center() - frame.size.width * 0.5f * t) / textureSize.width, frame.bottom() / textureSize.height)));
                
            }
            else {
                addVertex(CSVertex(CSVector3(destRect.left(), destRect.bottom(), destRect.origin.z),
                                   CSVector2(frame.left() / textureSize.width, frame.bottom() / textureSize.height)));
                
                if (a <= Angle3) {
                    float t = CSMath::tan(FloatPi * 1.5f - a);
                    
                    addVertex(CSVertex(CSVector3(destRect.left(), destRect.middle() + destRect.size.height * 0.5f * t, destRect.origin.z),
                                       CSVector2(frame.left() / textureSize.width, (frame.middle() + frame.size.height * 0.5f * t) / textureSize.height)
                                       ));
                    
                }
                else {
                    addVertex(CSVertex(CSVector3(destRect.left(), destRect.top(), destRect.origin.z),
                                       CSVector2(frame.left() / textureSize.width, frame.top() / textureSize.height)
                                       ));
                    
                    float t = CSMath::tan(a - FloatTwoPi);
                    
                    addVertex(CSVertex(CSVector3(destRect.center() + destRect.size.width * 0.5f * t, destRect.top(), destRect.origin.z),
                                       CSVector2((frame.center() + frame.size.width * 0.5f * t) / textureSize.width, frame.top() / textureSize.height)
                                       ));
                    
                }
            }
        }
    }
    for (int i = 0; i < vertexCount() - 1; i++) {
        addIndex(0);
        addIndex(i);
        addIndex(i + 1);
    }
    end(image->root(), GL_TRIANGLES);
    
}

void CSGraphics::drawStretchImage(const CSImage *image, const CSZRect &destRect, float horizontal, float vertical) {
    if (!image) {
        return;
    }
    const CSRect& frame = image->frame();
    float tw = image->textureWidth();
    float th = image->textureHeight();
    
    float u0 = frame.left() / tw;
    float u1 = (frame.left() + horizontal) / tw;
    float u2 = (frame.right() - horizontal) / tw;
    float u3 = frame.right() / tw;
    float v0 = frame.top() / th;
    float v1 = (frame.top() + vertical) / th;
    float v2 = (frame.bottom() - vertical) / th;
    float v3 = frame.bottom() / th;
    
    float hw = CSMath::min(horizontal, destRect.size.width * 0.5f);
    float hh = CSMath::min(vertical, destRect.size.height * 0.5f);
    
    float x0 = destRect.left();
    float x1 = x0 + hw;
    float x3 = destRect.right();
    float x2 = x3 - hw;
    
    float y0 = destRect.top();
    float y1 = y0 + hh;
    float y3 = destRect.bottom();
    float y2 = y3 - hh;
    
    begin();
    addVertex(CSVertex(CSVector3(x0, y0, destRect.origin.z), CSVector2(u0, v0)));
    addVertex(CSVertex(CSVector3(x1, y0, destRect.origin.z), CSVector2(u1, v0)));
    addVertex(CSVertex(CSVector3(x2, y0, destRect.origin.z), CSVector2(u2, v0)));
    addVertex(CSVertex(CSVector3(x3, y0, destRect.origin.z), CSVector2(u3, v0)));
    addVertex(CSVertex(CSVector3(x0, y1, destRect.origin.z), CSVector2(u0, v1)));
    addVertex(CSVertex(CSVector3(x1, y1, destRect.origin.z), CSVector2(u1, v1)));
    addVertex(CSVertex(CSVector3(x2, y1, destRect.origin.z), CSVector2(u2, v1)));
    addVertex(CSVertex(CSVector3(x3, y1, destRect.origin.z), CSVector2(u3, v1)));
    addVertex(CSVertex(CSVector3(x0, y2, destRect.origin.z), CSVector2(u0, v2)));
    addVertex(CSVertex(CSVector3(x1, y2, destRect.origin.z), CSVector2(u1, v2)));
    addVertex(CSVertex(CSVector3(x2, y2, destRect.origin.z), CSVector2(u2, v2)));
    addVertex(CSVertex(CSVector3(x3, y2, destRect.origin.z), CSVector2(u3, v2)));
    addVertex(CSVertex(CSVector3(x0, y3, destRect.origin.z), CSVector2(u0, v3)));
    addVertex(CSVertex(CSVector3(x1, y3, destRect.origin.z), CSVector2(u1, v3)));
    addVertex(CSVertex(CSVector3(x2, y3, destRect.origin.z), CSVector2(u2, v3)));
    addVertex(CSVertex(CSVector3(x3, y3, destRect.origin.z), CSVector2(u3, v3)));
    
    for (int i = 0; i < 12; i += 4) {
        for (int j = 0; j < 3; j++) {
            int k = i + j;
            addIndex(k);
            addIndex(k + 1);
            addIndex(k + 4);
            addIndex(k + 1);
            addIndex(k + 5);
            addIndex(k + 4);
        }
    }
    end(image->root(), GL_TRIANGLES);
}

void CSGraphics::drawStretchImage(const CSImage* image, const CSZRect& destRect) {
    if (!image) {
        return;
    }
    drawStretchImage(image, destRect, image->width() * 0.5f, image->height() * 0.5f);
}

void CSGraphics::drawShadowFlatImageImpl(const CSImage* image, const CSZRect& destRect, const CSRect& frame, float left, float right, bool xflip, bool yflip) {
    const CSLight& light = _state->geometry.var.sub.light;
    
    float lz = CSMath::abs(light.direction.z);
    if (lz == 0 || lz == 1) return;
    
    CSVector2 ld = -(CSVector2)light.direction;
    float ll = ld.length();
    if (ll == 0) return;
    ld /= ll;
    
    left *= destRect.size.width;
    right *= destRect.size.width;
    float length = ll / lz;
    
    CSVector2 cameraRight = CSVector2::normalize((CSVector2)_state->geometry.camera.right());
    CSVector2 cameraDown(-cameraRight.y, cameraRight.x);
    CSVector2 bottomLeft = cameraRight * (xflip ? -destRect.right() : destRect.left()) + cameraDown * left;
    CSVector2 bottomRight = cameraRight * (xflip ? -destRect.left() : destRect.right()) + cameraDown * right;
    float bottom = yflip ? destRect.top() : -destRect.bottom();
    if (bottom > 0) bottom *= length;
    bottom += destRect.origin.z * length;
    CSVector2 dir = ld * bottom;
    bottomLeft += dir;
    bottomRight += dir;
    
    CSVector2 lr(ld.y, -ld.x);
    float sin = CSVector2::dot(lr, CSVector2::normalize(bottomRight - bottomLeft));
    if (sin < 0) {
        lr = -lr;
        sin = -sin;
        yflip = false;
    }
    
    static const float ThicknessMin = 0.75f;
    static const float ThicknessSinMax = CSMath::sin(CSMath::atan(ThicknessMin));
    
    if (sin < ThicknessSinMax) {
        float tan = sin / CSMath::sqrt(1 - sin * sin);
        CSVector2 ar = lr * ((ThicknessMin - tan) * destRect.size.width * 0.5f);
        bottomLeft -= ar;
        bottomRight += ar;
    }
    CSVector2 topLeft = bottomLeft;
    CSVector2 topRight = bottomRight;
    dir = ld * (-bottom + ((yflip ? destRect.bottom() : -destRect.top()) + destRect.origin.z) * length);
    topLeft += dir;
    topRight += dir;
    
    float lu, ru, tv, lbv, rbv;
    if (xflip) {
        lu = frame.right();
        ru = frame.left();
    }
    else {
        lu = frame.left();
        ru = frame.right();
    }
    if (yflip) {
        tv = frame.bottom();
        lbv = frame.top() - CSMath::min(left, 0.0f);
        rbv = frame.top() - CSMath::min(right, 0.0f);
    }
    else {
        tv = frame.top();
        lbv = frame.bottom() + CSMath::min(left, 0.0f);
        rbv = frame.bottom() + CSMath::min(right, 0.0f);
    }
    const CSSize& textureSize = image->textureSize();
    lu /= textureSize.width;
    ru /= textureSize.width;
    tv /= textureSize.height;
    lbv /= textureSize.height;
    rbv /= textureSize.height;
    
    begin();
    
    addVertex(CSVertex(topLeft, CSColor::Black, CSVector2(lu, tv)));
    addVertex(CSVertex(topRight, CSColor::Black, CSVector2(ru, tv)));
    addVertex(CSVertex(bottomLeft, CSColor::Black, CSVector2(lu, lbv)));
    addVertex(CSVertex(bottomRight, CSColor::Black, CSVector2(ru, rbv)));

    addIndex(0);
    addIndex(1);
    addIndex(2);
    addIndex(1);
    addIndex(3);
    addIndex(2);
    
    end(image->root(), GL_TRIANGLES);
}

void CSGraphics::drawShadowFlatImage(const CSImage* image, const CSZRect& destRect, const CSRect& srcRect, float left, float right, bool xflip, bool yflip) {
    if (!image) {
        return;
    }
    CSRect subframe = srcRect;
    const CSRect& frame = image->frame();
    subframe.origin += frame.origin;
    subframe.intersect(frame);
    
    drawShadowFlatImageImpl(image, destRect, subframe, left, right, xflip, yflip);
}

void CSGraphics::drawShadowFlatImage(const CSImage* image, const CSZRect& destRect, float left, float right, bool xflip, bool yflip) {
    if (!image) {
        return;
    }
    drawShadowFlatImageImpl(image, destRect, image->frame(), left, right, xflip, yflip);
}

void CSGraphics::drawShadowFlatImage(const CSImage* image, CSVector3 point, CSAlign align, float left, float right, bool xflip, bool yflip) {
    if (!image) {
        return;
    }
    const CSSize& size = image->size();
    
    applyAlign(point, size, align);
    
    drawShadowFlatImageImpl(image, CSZRect(point, size), image->frame(), left, right, xflip, yflip);
}

void CSGraphics::drawShadowFlatImage(const CSImage* image, const CSVector3& point, float left, float right, bool xflip, bool yflip) {
    if (!image) {
        return;
    }
    drawShadowFlatImageImpl(image, CSZRect(point, image->size()), image->frame(), left, right, xflip, yflip);
}


void CSGraphics::drawShadowRotateImageImpl(const CSImage* image, const CSZRect& destRect, const CSRect& frame, const CSVector2& offset, float flatness) {
    const CSLight& light = _state->geometry.var.sub.light;
    
    float lz = CSMath::abs(light.direction.z);
    if (lz == 0 || lz == 1) return;
    
    CSVector2 ld = -(CSVector2)light.direction;
    float ll = ld.length();
    if (ll == 0) return;
    ld /= ll;
    
    float length = ll / lz;
    
    CSVector2 cameraRight = CSVector2::normalize((CSVector2)_state->geometry.camera.right());
    CSVector2 cameraDown(-cameraRight.y, cameraRight.x);
    CSVector2 lr(ld.y, -ld.x);
    float sin = CSVector2::dot(lr, cameraRight);
    if (sin < 0) lr = -lr;
    
    CSVector2 bottomLeft = lr * destRect.left();
    CSVector2 bottomRight = lr * destRect.right();
    CSVector2::lerp(bottomLeft, cameraRight * destRect.left(), flatness, bottomLeft);
    CSVector2::lerp(bottomRight, cameraRight * destRect.right(), flatness, bottomRight);
    float bottom = -destRect.bottom();
    if (bottom > 0) bottom *= length;
    bottom += destRect.origin.z * length;
    CSVector2 dir = ld * bottom + cameraRight * offset.x + cameraDown * offset.y;
    bottomLeft += dir;
    bottomRight += dir;
    
    CSVector2 topLeft = bottomLeft;
    CSVector2 topRight = bottomRight;
    dir = ld * (-bottom + (-destRect.top() + destRect.origin.z) * length);
    topLeft += dir;
    topRight += dir;
    
    const CSSize& textureSize = image->textureSize();
    float lu = frame.left() / textureSize.width;
    float ru = frame.right() / textureSize.width;
    float tv = frame.top() / textureSize.height;
    float bv = frame.bottom() / textureSize.height;
    
    begin();
    
    addVertex(CSVertex(topLeft, CSColor::Black, CSVector2(lu, tv)));
    addVertex(CSVertex(topRight, CSColor::Black, CSVector2(ru, tv)));
    addVertex(CSVertex(bottomLeft, CSColor::Black, CSVector2(lu, bv)));
    addVertex(CSVertex(bottomRight, CSColor::Black, CSVector2(ru, bv)));
    
    addIndex(0);
    addIndex(1);
    addIndex(2);
    addIndex(1);
    addIndex(3);
    addIndex(2);
    
    end(image->root(), GL_TRIANGLES);
}

void CSGraphics::drawShadowRotateImage(const CSImage* image, const CSZRect& destRect, const CSRect& srcRect, const CSVector2& offset, float flatness) {
    if (!image) {
        return;
    }
    CSRect subframe = srcRect;
    const CSRect& frame = image->frame();
    subframe.origin += frame.origin;
    subframe.intersect(frame);
    
    drawShadowRotateImageImpl(image, destRect, subframe, offset, flatness);
}

void CSGraphics::drawShadowRotateImage(const CSImage* image, const CSZRect& destRect, const CSVector2& offset, float flatness) {
    if (!image) {
        return;
    }
    drawShadowRotateImageImpl(image, destRect, image->frame(), offset, flatness);
}

void CSGraphics::drawShadowRotateImage(const CSImage* image, CSVector3 point, CSAlign align, const CSVector2& offset, float flatness) {
    if (!image) {
        return;
    }
    const CSSize& size = image->size();
    
    applyAlign(point, size, align);
    
    drawShadowRotateImageImpl(image, CSZRect(point, size), image->frame(), offset, flatness);
}

void CSGraphics::drawShadowRotateImage(const CSImage* image, const CSVector3& point, const CSVector2& offset, float flatness) {
    if (!image) {
        return;
    }
    drawShadowRotateImageImpl(image, CSZRect(point, image->size()), image->frame(), offset, flatness);
}
