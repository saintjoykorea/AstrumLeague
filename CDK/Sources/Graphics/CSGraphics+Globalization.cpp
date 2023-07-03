//
//  CSGraphics+Globalization.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

#include "CSGraphicsImpl.h"

float CSGraphics::drawDigit(int value, bool comma, const CSVector3& point) {
	return drawString(CSString::cstringWithNumber(value, comma), point);
}

float CSGraphics::drawDigit(int value, bool comma, const CSVector3& point, CSAlign align) {
	return drawString(CSString::cstringWithNumber(value, comma), point, align);
}

float CSGraphics::drawDigitImages(uint value, const CSArray<CSImage>* images, bool comma, uint offset, const CSVector3& point, CSAlign align, float spacing) {
    if (!images) {
        return 0.0f;
    }
    float x = 0;
    
    uint temp;
    uint i;
    
    if (!(align & CSAlignRight)) {
        temp = value;
        i = 0;
        do {
            if (comma && i && i % 3 == 0) {
                x += images->objectAtIndex(offset + 10)->width() + spacing;
            }
            i++;
            x += images->objectAtIndex(offset + (temp % 10))->width() + spacing;
            temp /= 10;
        } while (temp);
        
        if (align & CSAlignCenter) {
            x *= 0.5f;
        }
    }
    
    temp = value;
    i = 0;
    
    float w = 0.0f;
    
    do {
        const CSImage* image;
        
        if (comma && i && i % 3 == 0) {
            float y = point.y;
            float h0 = images->objectAtIndex(offset)->height();
            if (align & CSAlignBottom) {
                y -= h0 * 0.25f;
            }
            else if (align & CSAlignMiddle) {
                y += h0 * 0.25f;
            }
            else {
                y += h0 * 0.75f;
            }
            image = images->objectAtIndex(offset + 10);
            drawImage(image, CSVector2(point.x + x - w, y), CSAlignRightMiddle);
            w += image->width() + spacing;
        }
        i++;
        image = images->objectAtIndex(offset + (temp % 10));
        drawImage(image, CSVector2(point.x + x - w, point.y), (CSAlign)(CSAlignRight | (align & (CSAlignMiddle | CSAlignBottom))));
        w += image->width() + spacing;
        temp /= 10;
    } while (temp);
    
    return w;
}
