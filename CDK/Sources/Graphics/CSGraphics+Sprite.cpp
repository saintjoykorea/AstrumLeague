//
//  CSGraphics+Sprite.cpp
//  CDK
//
//  Created by 김찬 on 2015. 10. 15..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

#include "CSBytes.h"

enum CSSpriteOp {
    CSSpriteOpColor,
    CSSpriteOpLightness,
    CSSpriteOpBrightness,
    CSSpriteOpContrast,
    CSSpriteOpSaturation,
    CSSpriteOpBlendMode,
    CSSpriteOpMaskingMode,
    CSSpriteOpConvolution,
    CSSpriteOpStroke,
    CSSpriteOpFont,
    CSSpriteOpFontColor,
    CSSpriteOpMaterial,
    CSSpriteOpTranslate,
    CSSpriteOpScale,
    CSSpriteOpRotate,
    CSSpriteOpHInvert,
    CSSpriteOpVInvert,
    CSSpriteOpBillboard,
    CSSpriteOpPush,
    CSSpriteOpPop,
    CSSpriteOpReset,
    CSSpriteOpImage,
    CSSpriteOpLine,
    CSSpriteOpRect,
    CSSpriteOpArc,
    CSSpriteOpGradientLine,
    CSSpriteOpGradientRect,
    CSSpriteOpGradientArc,
    CSSpriteOpString,
    CSSpriteOpExternElement,
    CSSpriteOpShockwave,
    CSSpriteOpLens,
    CSSpriteOpSwirl,
    CSSpriteOpBlur
};

void CSGraphics::sprColor(const byte*& cursor) {
    CSColor color(cursor, true);
    setColor(color);
}

void CSGraphics::sprLightness(const byte*& cursor) {
    float lightness = readFloat(cursor);
    setLightness(lightness);
}

void CSGraphics::sprBrightness(const byte*& cursor) {
    float brightness = readFloat(cursor);
    setBrightness(brightness);
}

void CSGraphics::sprContrast(const byte*& cursor) {
    float contrast = readFloat(cursor);
    setContrast(contrast);
}

void CSGraphics::sprSaturation(const byte*& cursor) {
    float saturation = readFloat(cursor);
    setSaturation(saturation);
}

void CSGraphics::sprBlendMode(const byte*& cursor) {
    CSBlendMode blendMode = (CSBlendMode)readByte(cursor);
    setBlendMode(blendMode);
}

void CSGraphics::sprMaskingMode(const byte*& cursor) {
    CSMaskingMode maskingMode = (CSMaskingMode)readByte(cursor);
    setMaskingMode(maskingMode);
}

void CSGraphics::sprConvolution(const byte*& cursor) {
    int width = readByte(cursor);
    int length = CSConvolution::length(width);
    float* kernel = (float*)alloca(length * sizeof(float));
    memcpy(kernel, cursor, length * sizeof(float));
    cursor += length * sizeof(float);
    CSConvolution conv(width, kernel, 1.0f);
    setConvolution(conv);
}

void CSGraphics::sprStroke(const byte*& cursor) {
    CSColor color(cursor, true);
    CSStrokeMode mode = (CSStrokeMode)readByte(cursor);
    int width = readByte(cursor);
    
    setStrokeColor(color);
    setStrokeMode(mode);
    setStrokeWidth(width);
}

void CSGraphics::sprFont(const byte*& cursor) {
    const CSString* name = readString(cursor);
    float size = readFloat(cursor);
    int style = readByte(cursor);
    
    if (!size) {
        size = _state->font->size();
    }
    if (style < 0) {
        style = _state->font->style();
    }
    setFont(name ? CSFont::font(*name, size, (CSFontStyle)style) : _state->font->derivedFont(size, (CSFontStyle)style));
}

void CSGraphics::sprFontColor(const byte *&cursor) {
    CSColor color1(cursor, true);
    CSColor color2(cursor, true);
    if (readBoolean(cursor)) {
        setFontColorV(color1, color2);
    }
    else {
        setFontColorH(color1, color2);
    }
}

void CSGraphics::sprMaterial(const byte *&cursor) {
    CSMaterial material(cursor);
    
    setMaterial(material);
}

void CSGraphics::sprTranslate(const byte*& cursor) {
    CSVector3 translation(cursor);
    
    translate(translation);
}

void CSGraphics::sprScale(const byte*& cursor) {
    CSVector3 scaling(cursor);
    
    scale(scaling);
}

void CSGraphics::sprRotate(const byte*& cursor) {
    float yaw = readFloat(cursor);
    float pitch = readFloat(cursor);
    float roll = readFloat(cursor);
    
    rotateYawPitchRoll(yaw, pitch, roll);
}

void CSGraphics::sprBillboard(const byte*& cursor) {
    int type = readByte(cursor);
    
    switch (type) {
        case 0:
            _state->world = _state->geometry.camera.view().billboard(&_state->world) * _state->world;
            break;
        case 1:
            _state->world = _state->geometry.camera.view().horizontalBillboard(&_state->world) * _state->world;
            break;
        case 2:
            _state->world = _state->geometry.camera.view().verticalBillboard(&_state->world) * _state->world;
            break;
    }
}

void CSGraphics::sprImage(const byte*& cursor, bool shadow, CSResourceDelegate* resourceDelegate) {
    const CSImage* image = NULL;
    
    int indexCount = readByte(cursor);
    if (indexCount) {
        ushort* indices = (ushort*)alloca(indexCount * sizeof(ushort));
        memcpy(indices, cursor, indexCount * sizeof(ushort));
        image = static_cast<const CSImage*>((*resourceDelegate)(CSResourceTypeImage, indices, indexCount));
        cursor += indexCount * sizeof(ushort);
    }
    else {
        image = static_cast<const CSImage*>((*resourceDelegate)(CSResourceTypeImage, NULL, 0));
    }
    
    if (image) {
        CSZRect destRect;
        CSRect* srcRect = NULL;
        
        int type = readByte(cursor);
        
        switch (type) {
            case 0:
                destRect.origin = CSVector3(cursor);
                destRect.size = image->size();
                applyAlign(destRect.origin, destRect.size, (CSAlign)readByte(cursor));
                break;
            case 1:
                destRect = CSZRect(cursor);
                break;
            case 2:
                destRect = CSZRect(cursor);
                srcRect = new CSRect();
				srcRect->origin.x = readShort(cursor);
				srcRect->origin.y = readShort(cursor);
				srcRect->size.width = readShort(cursor);
				srcRect->size.height = readShort(cursor);
                break;
        }
        if (!shadow) {
            if (srcRect) drawImage(image, destRect, *srcRect);
            else drawImage(image, destRect);
        }
        else {
            int shadowType = readByte(cursor);
            
            if (shadowType) {
                CSMatrix prev = world();
                
                float scale;
                CSVector3 translation;
                CSQuaternion rotation;
                
                if (prev.decomposeUniformScale(scale, rotation, translation)) {
					float shadowDistance = readFloat(cursor);

                    destRect.origin.z += translation.z + shadowDistance;
                    
                    rotation.x = 0;
                    rotation.y = 0;
                    rotation.z = -rotation.z;
                    rotation.normalize();
                    
                    translation.z = 0;
                    
                    setWorld(CSMatrix::affineTransformation(scale, rotation, translation));
                    
                    switch (shadowType) {
                        case 1:
                            {
                                float shadowFlatLeft = readFloat(cursor);
                                float shadowFlatRight = readFloat(cursor);
                                bool shadowFlatXFlip = readBoolean(cursor);
                                bool shadowFlatYFlip = readBoolean(cursor);
                                if (srcRect) drawShadowFlatImage(image, destRect, *srcRect, shadowFlatLeft, shadowFlatRight, shadowFlatXFlip, shadowFlatYFlip);
                                else drawShadowFlatImage(image, destRect, shadowFlatLeft, shadowFlatRight, shadowFlatXFlip, shadowFlatYFlip);
                            }
                            break;
                        case 2:
                            {
                                CSVector2 shadowRotateOffset(cursor);
                                float shadowRotateFlatness = readFloat(cursor);
                                if (srcRect) drawShadowRotateImage(image, destRect, *srcRect, shadowRotateOffset, shadowRotateFlatness);
                                else drawShadowRotateImage(image, destRect, shadowRotateOffset, shadowRotateFlatness);
                            }
                            break;
                    }
                    
                    setWorld(prev);
                }
            }
        }
        if (srcRect) delete srcRect;
    }
}

void CSGraphics::sprLine(const byte*& cursor) {
    CSVector3 point1(cursor);
    CSVector3 point2(cursor);
    drawLine(point1, point2);
}

void CSGraphics::sprRect(const byte*& cursor) {
    CSZRect rect(cursor);
    bool fill = readBoolean(cursor);
    
    drawRect(rect, fill);
}

void CSGraphics::sprArc(const byte*& cursor) {
    CSZRect rect(cursor);
    float s = readFloat(cursor);
    float e = readFloat(cursor);
    bool fill = readBoolean(cursor);
    drawArc(rect, s, e, fill);
}

void CSGraphics::sprGradientLine(const byte*& cursor) {
    CSVector3 point1(cursor);
    CSVector3 point2(cursor);
    CSColor color1(cursor, true);
    CSColor color2(cursor, true);
    drawGradientLine(point1, color1, point2, color2);
}

void CSGraphics::sprGradientRect(const byte*& cursor) {
    CSZRect rect(cursor);
    CSColor color1(cursor, true);
    CSColor color2(cursor, true);
    bool fill = readBoolean(cursor);
    
    if (readBoolean(cursor)) {
        drawGradientRectV(rect, color1, color2, fill);
    }
    else {
        drawGradientRectH(rect, color1, color2, fill);
    }
}

void CSGraphics::sprGradientArc(const byte*& cursor) {
    CSZRect rect(cursor);
    float s = readFloat(cursor);
    float e = readFloat(cursor);
    CSColor centerColor(cursor, true);
    CSColor surroundColor(cursor, true);
    drawGradientArc(rect, s, e, centerColor, surroundColor);
}

void CSGraphics::sprString(const byte*& cursor, CSResourceDelegate* resourceDelegate) {
	const CSString* str;
	if (readBoolean(cursor)) {
		str = readBoolean(cursor) ? readLocaleString(cursor)->value() : readString(cursor);
	}
	else {
		int indexCount = readByte(cursor);
		if (indexCount) {
			ushort* indices = (ushort*)alloca(indexCount * sizeof(ushort));
			memcpy(indices, cursor, indexCount * sizeof(ushort));
			str = static_cast<const CSString*>((*resourceDelegate)(CSResourceTypeString, indices, indexCount));
			cursor += indexCount * sizeof(ushort);
		}
		else {
			str = static_cast<const CSString*>((*resourceDelegate)(CSResourceTypeString, NULL, 0));
		}
	}
	int seq = readInt(cursor);
	CSVector3 pos(cursor);
	CSAlign align = (CSAlign)readByte(cursor);
	int usingSize = readByte(cursor);

	switch (usingSize) {
		case 0:
            drawString(StringParam(str, 0, seq), pos, align);
			break;
		case 1:
			{
				CSSize size(cursor);
				drawStringScaled(StringParam(str, 0, seq), CSZRect(pos, size), align);
			}
			break;
		case 2:
			{
				float width = readFloat(cursor);
				drawStringScaled(StringParam(str, 0, seq), pos, align, width);
			}
			break;
	}
}

void CSGraphics::sprShockwave(const byte*& cursor) {
    CSVector3 position(cursor);
    float range = readFloat(cursor);
    float thickness = readFloat(cursor);
    float progress = readFloat(cursor);
    
    shockwave(position, range, thickness, progress);
}

void CSGraphics::sprLens(const byte*& cursor) {
    CSVector3 position(cursor);
    float range = readFloat(cursor);
    bool convex = readBoolean(cursor);
    
    lens(position, range, convex);
}

void CSGraphics::sprSwirl(const byte*& cursor) {
    CSVector3 position(cursor);
    float range = readFloat(cursor);
    float angle = readFloat(cursor);
    
    swirl(position, range, angle);
}

void CSGraphics::sprBlur(const byte*& cursor) {
    CSZRect rect(cursor);

	switch (readByte(cursor)) {
		case 0:
			{
				float weight = readFloat(cursor);
				blur(rect, weight);
			}
			break;
		case 1:
			{
				CSVector2 dir(cursor);
				blur(rect, dir);
			}
			break;
		case 2:
			{
				CSVector3 center(cursor);
				float range = readFloat(cursor);
				blur(rect, center, range);
			}
			break;
	}
}

void CSGraphics::drawSprite(const CSArray<byte>* sprite, bool shadow, CSResourceDelegate* resourceDelegate, CSSpriteDelegate* spriteDelegate) {
    const byte* current = sprite->pointer();
    const byte* end = current + sprite->count();
    
    push(true);
    
    while (current < end) {
        int opCode = readByte(current);
        
        int len = readLength(current);
        const byte* next = current + len;
        
        switch (opCode) {
            case CSSpriteOpColor:
                sprColor(current);
                break;
            case CSSpriteOpLightness:
                if (!shadow) sprLightness(current);
                break;
            case CSSpriteOpBrightness:
                if (!shadow) sprBrightness(current);
                break;
            case CSSpriteOpContrast:
                if (!shadow) sprContrast(current);
                break;
            case CSSpriteOpSaturation:
                if (!shadow) sprSaturation(current);
                break;
            case CSSpriteOpBlendMode:
                if (!shadow) sprBlendMode(current);
                break;
            case CSSpriteOpMaskingMode:
                if (!shadow) sprMaskingMode(current);
                break;
            case CSSpriteOpConvolution:
                if (!shadow) sprConvolution(current);
                break;
            case CSSpriteOpStroke:
                if (!shadow) sprStroke(current);
                break;
            case CSSpriteOpFont:
                if (!shadow) sprFont(current);
                break;
            case CSSpriteOpFontColor:
                if (!shadow) sprFontColor(current);
                break;
            case CSSpriteOpMaterial:
                if (!shadow) sprMaterial(current);
                break;
            case CSSpriteOpTranslate:
                sprTranslate(current);
                break;
            case CSSpriteOpScale:
                sprScale(current);
                break;
            case CSSpriteOpRotate:
                sprRotate(current);
                break;
            case CSSpriteOpHInvert:
                scale(CSVector3(-1.0f, 1.0f, 1.0f));
                break;
            case CSSpriteOpVInvert:
                scale(CSVector3(1.0f, -1.0f, 1.0f));
                break;
            case CSSpriteOpBillboard:
                if (!shadow) sprBillboard(current);
                break;
            case CSSpriteOpPush:
                push();
                break;
            case CSSpriteOpPop:
                pop();
                break;
            case CSSpriteOpReset:
                reset();
                break;
            case CSSpriteOpImage:
                if (resourceDelegate) {
                    sprImage(current, shadow, resourceDelegate);
                }
                break;
            case CSSpriteOpLine:
                if (!shadow) sprLine(current);
                break;
            case CSSpriteOpRect:
                if (!shadow) sprRect(current);
                break;
            case CSSpriteOpArc:
                if (!shadow) sprArc(current);
                break;
            case CSSpriteOpGradientLine:
                if (!shadow) sprLine(current);
                break;
            case CSSpriteOpGradientRect:
                if (!shadow) sprGradientRect(current);
                break;
            case CSSpriteOpGradientArc:
                if (!shadow) sprGradientArc(current);
                break;
            case CSSpriteOpString:
                if (!shadow) sprString(current, resourceDelegate);
                break;
            case CSSpriteOpExternElement:
                if (!shadow && spriteDelegate) {
                    (*spriteDelegate)(this, current);
                }
                break;
            case CSSpriteOpShockwave:
                if (!shadow) sprShockwave(current);
                break;
            case CSSpriteOpLens:
                if (!shadow) sprLens(current);
                break;
            case CSSpriteOpSwirl:
                if (!shadow) sprSwirl(current);
                break;
            case CSSpriteOpBlur:
                if (!shadow) sprBlur(current);
                break;
        }
        current = next;
    }
    pop(true);
}

static void sprPreloadImage(const byte*& cursor, CSResourceDelegate* resourceDelegate) {
    int indexCount = readByte(cursor);
    if (indexCount) {
		ushort* indices = (ushort*)alloca(indexCount * sizeof(ushort));
        memcpy(indices, cursor, indexCount * sizeof(ushort));
        (*resourceDelegate)(CSResourceTypeImage, indices, indexCount);
    }
}

void CSGraphics::preloadSprite(const CSArray<byte> *sprite, CSResourceDelegate* resourceDelegate) {
    const byte* current = sprite->pointer();
    const byte* end = current + sprite->count();
    
    while (current < end) {
        int opCode = readByte(current);
        
        int len = readLength(current);
        const byte* next = current + len;
        
        switch (opCode) {
            case CSSpriteOpImage:
                sprPreloadImage(current, resourceDelegate);
                break;
        }
        current = next;
    }
}
