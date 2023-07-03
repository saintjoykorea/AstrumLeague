//
//  CSGraphics+String.cpp
//  CDK
//
//  Created by 김찬 on 2015. 10. 13..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

#include "CSGraphicsImpl.h"

#define LineBreakEpsilon    0.001f

const CSArray<CSGraphics::ParagraphDisplay>* CSGraphics::paragraphDisplays(const StringParam& str, const CSFont* font) {
    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();

    CSArray<ParagraphDisplay>* result = CSArray<ParagraphDisplay>::arrayWithCapacity(paras->count());

    foreach(const CSStringParagraph&, pa, str.display()->paragraphs()) {
        switch (pa.type) {
            case CSStringParagraphTypeLinebreak:
                {
                    ParagraphDisplay& d = result->addObject();
                    d.font = font;
                    d.size = CSSize::Zero;
                    if (pa.attr.text.end <= str.start) d.visible = ParagraphDisplay::Forward;
                    else if (pa.attr.text.start >= str.end) d.visible = ParagraphDisplay::Backward;
                    else {
                        d.size.height = font->size();
                        d.visible = ParagraphDisplay::Visible;
                    }
                }
                break;
            case CSStringParagraphTypeNeutral:
            case CSStringParagraphTypeLTR:
            case CSStringParagraphTypeRTL:
            case CSStringParagraphTypeSpace:
                {
                    ParagraphDisplay& d = result->addObject();
                    d.font = font;
                    d.size = CSSize::Zero;
                    if (pa.attr.text.end <= str.start) d.visible = ParagraphDisplay::Forward;
                    else if (pa.attr.text.start >= str.end) d.visible = ParagraphDisplay::Backward;
                    else {
                        const CSCharSequence* characters = str.display()->characters();
                        const uchar* content = str.display()->content();

                        for (uint ci = pa.attr.text.start; ci < pa.attr.text.end; ci++) {
                            const uchar* cc = content + characters->from(ci);
                            const uint cclen = characters->length(ci);

                            const CSSize& ccs = cs->size(cc, cclen, font);

                            d.size.width += ccs.width;
                            if (d.size.height < ccs.height) d.size.height = ccs.height;
                        }
                        d.visible = ParagraphDisplay::Visible;
                    }
                }
                break;
            case CSStringParagraphTypeFont:
                {
                    CSFontStyle style = pa.attr.font.style >= 0 ? pa.attr.font.style : font->style();
                    font = pa.attr.font.name ? CSFont::font(pa.attr.font.name, pa.attr.font.size, style) : font->derivedFont(pa.attr.font.size, style);
                }
            default:
                {
                    ParagraphDisplay& d = result->addObject();
                    d.font = font;
                    d.size = CSSize::Zero;
                    d.visible = ParagraphDisplay::Visible;
                }
                break;
        }
    }

    return result;
}

uint CSGraphics::stringIndex(const StringParam& str, uint ci) {
    foreach(const CSStringParagraph&, pa, str.display()->paragraphs()) {
        switch (pa.type) {
            case CSStringParagraphTypeNeutral:
            case CSStringParagraphTypeLTR:
            case CSStringParagraphTypeRTL:
            case CSStringParagraphTypeSpace:
            case CSStringParagraphTypeLinebreak:
                if (ci >= pa.attr.text.start && ci < pa.attr.text.end) return ci;
                else if (ci < pa.attr.text.start) return pa.attr.text.start;
                break;
        }
    }
    return ci;
}

CSSize CSGraphics::stringSizeImpl(const StringParam& str, float width, const CSArray<ParagraphDisplay>* paraDisplays) {
    CSSize size = CSSize::Zero;
    CSSize lineSize = CSSize::Zero;
    float space = 0;

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeLinebreak:
                    {
                        if (size.width < lineSize.width) size.width = lineSize.width;
                        size.height += CSMath::max(lineSize.height, pd.size.height);
                        lineSize = CSSize::Zero;
                        space = 0;
                    }
                    break;
                case CSStringParagraphTypeSpace:
                    if (lineSize.width) {
                        if (lineSize.height < pd.size.height) lineSize.height = pd.size.height;
                        space += pd.size.width;
                    }
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    {
                        if (lineSize.width && lineSize.width + space + pd.size.width > width) {
                            if (size.width < lineSize.width) size.width = lineSize.width;
                            size.height += CSMath::max(lineSize.height, pd.size.height);
                            lineSize = pd.size;
                        }
                        else {
                            if (lineSize.height < pd.size.height) lineSize.height = pd.size.height;
                            lineSize.width += space + pd.size.width;
                        }
                        space = 0;
                    }
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) break;
    }

    if (size.width < lineSize.width) size.width = lineSize.width;
    if (lineSize.width) size.height += lineSize.height;

    return size;
}

CSSize CSGraphics::stringSize(const StringParam& str, const CSFont* font, float width) {
    if (!str) return CSSize::Zero;

    return stringSizeImpl(str, width, paragraphDisplays(str, font));
}

CSSize CSGraphics::stringSize(const StringParam& str, float width) const {
	return stringSize(str, _state->font, width);
}

CSSize CSGraphics::stringLineSizeImpl(const StringParam& str, uint pi, float width, const CSArray<ParagraphDisplay>* paraDisplays) {
    CSSize size = CSSize::Zero;
    float space = 0;

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    while (pi < paras->count()) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeLinebreak:
                    if (size.height < pd.size.height) size.height = pd.size.height;
                    goto exit;
                case CSStringParagraphTypeSpace:
                    if (size.width) {
                        if (size.height < pd.size.height) size.height = pd.size.height;
                        space += pd.size.width;
                    }
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    {
                        if (size.width && size.width + space + pd.size.width > width) goto exit;
                        if (size.height < pd.size.height) size.height = pd.size.height;
                        size.width += space + pd.size.width;
                        space = 0;
                    }
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) break;

        pi++;
    }
exit:
    return size;
}

CSString* CSGraphics::shrinkString(const StringParam& str, const CSFont* font, float width, float scroll) {
    if (!str) return NULL;
        
    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, font);

    CSSize lineSize = CSSize::Zero;
    float space = 0;
    uint fromIndex = 0;

    const CSCharSequence* characters = str.display()->characters();

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeLinebreak:
                    scroll -= CSMath::max(lineSize.height, pd.size.height);
                    fromIndex = characters->to(pa.attr.text.end);
                    if (scroll <= 0) goto exit;
                    lineSize = CSSize::Zero;
                    space = 0;
                    break;
                case CSStringParagraphTypeSpace:
                    if (lineSize.width) {
                        if (lineSize.height < pd.size.height) lineSize.height = pd.size.height;
                        space += pd.size.width;
                    }
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    {
                        if (lineSize.width && lineSize.width + space + pd.size.width > width) {
                            scroll -= CSMath::max(lineSize.height, pd.size.height);
                            fromIndex = characters->from(pa.attr.text.start);
                            if (scroll <= 0) goto exit;
                            lineSize = pd.size;
                        }
                        else {
                            if (lineSize.height < pd.size.height) lineSize.height = pd.size.height;
                            lineSize.width += space + pd.size.width;
                        }
                        space = 0;
                    }
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) break;
    }
exit:
    if (scroll > lineSize.height) return CSString::string();

    const uchar* content = str.display()->content();

    return CSString::string(CSString::cstring(content + fromIndex, u_strlen(content) - fromIndex));
}

static float formatStringX(int align, float width, float lineWidth, bool rtl) {
    switch (align) {
        case 0:
            return rtl ? width - lineWidth : 0;
        case 1:
            return (rtl ? width + lineWidth : width - lineWidth) * 0.5f;
        case 2:
            return rtl ? width : width - lineWidth;
    }
    CSAssert(false, "invalid paramter");
    return 0;
}

static uint paragraphCursor(const CSGraphics::StringParam& str, const CSStringParagraph& pa, const CSFont* font, float cx) {
    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

    const CSCharSequence* characters = str.display()->characters();
    const uchar* content = str.display()->content();

    if (str.display()->isRTL() && pa.type != CSStringParagraphTypeLTR) {
        if (cx <= 0) return pa.attr.text.end;
        for (int ci = (int)pa.attr.text.end - 1; ci >= (int)pa.attr.text.start; ci--) {
            const uchar* cc = content + characters->from(ci);
            uint cclen = characters->length(ci);
            cx -= cs->size(cc, cclen, font).width;
            if (cx <= 0) return ci;
        }
        return pa.attr.text.start;
    }
    else {
        if (cx <= 0) return pa.attr.text.start;
        for (uint ci = pa.attr.text.start; ci < pa.attr.text.end; ci++) {
            const uchar* cc = content + characters->from(ci);
            uint cclen = characters->length(ci);
            cx -= cs->size(cc, cclen, font).width;
            if (cx <= 0) return ci;
        }
        return pa.attr.text.end;
    }
}

uint CSGraphics::stringCursor(const StringParam& str, const CSFont* font, const CSVector2& target, float width) {
	if (!str) return 0;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, font);

    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

    bool rtl = str.display()->isRTL();
    int align = rtl ? 2 : 0;
    float x = rtl ? width : 0, y = 0, space = 0, lineWidth = 0;
    CSSize lineSize = stringLineSizeImpl(str, 0, width, paraDisplays);

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeAlign:
                    if (align != pa.attr.align) {
                        align = pa.attr.align;
                        x = formatStringX(align, width, lineSize.width, rtl);
                    }
                    break;
                case CSStringParagraphTypeLinebreak:
                    {
                        y += lineSize.height;
                        lineSize = stringLineSizeImpl(str, pi + 1, width, paraDisplays);
                        x = formatStringX(align, width, lineSize.width, rtl);
                        lineWidth = 0;
                        space = 0;
                    }
                    break;
                case CSStringParagraphTypeSpace:
                    if (lineWidth) {
                        if (y + lineSize.height >= target.y) {
                            if (rtl) {
                                if (x - space - pd.size.width <= target.x) return paragraphCursor(str, pa, pd.font, x - space - target.x);
                            }
                            else {
                                if (x + space + pd.size.width >= target.x) return paragraphCursor(str, pa, pd.font, target.x - x - space);
                            }
                        }
                        space += pd.size.width;
                    }
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    if (lineWidth && lineWidth + space + pd.size.width > width + LineBreakEpsilon) {
                        y += lineSize.height;
                        lineSize = stringLineSizeImpl(str, pi, width, paraDisplays);
                        x = formatStringX(align, width, lineSize.width, rtl);
                        lineWidth = 0;
                        space = 0;
                    }
                    if (y + lineSize.height >= target.y) {
                        if (rtl) {
                            if (x - space - pd.size.width <= target.x) return paragraphCursor(str, pa, pd.font, x - space - target.x);
                        }
                        else {
                            if (x + space + pd.size.width >= target.x) return paragraphCursor(str, pa, pd.font, target.x - x - space);
                        }
                    }
                    {
                        float pw = space + pd.size.width;
                        lineWidth += pw;
                        x += rtl ? -pw : pw;
                        space = 0;
                    }
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) return pa.attr.text.start;
    }
    return str.display()->characters()->count();
}

uint CSGraphics::stringCursor(const StringParam& str, const CSVector2& target, float width) const {
	return stringCursor(str, _state->font, target, width);
}

CSVector2 CSGraphics::stringPosition(const StringParam& str, const CSFont* font, float width) {
	if (!str) return CSVector2::Zero;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, font);

    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

    bool rtl = str.display()->isRTL();
    int align = rtl ? 2 : 0;
    float x = rtl ? width : 0, y = 0, space = 0, lineWidth = 0;
    CSSize lineSize = stringLineSizeImpl(str, 0, width, paraDisplays);

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeAlign:
                    if (align != pa.attr.align) {
                        align = pa.attr.align;
                        x = formatStringX(align, width, lineSize.width, rtl);
                    }
                    break;
                case CSStringParagraphTypeLinebreak:
                    {
                        y += lineSize.height;
                        lineSize = stringLineSizeImpl(str, pi + 1, width, paraDisplays);
                        x = formatStringX(align, width, lineSize.width, rtl);
                        lineWidth = 0;
                        space = 0;
                    }
                    break;
                case CSStringParagraphTypeSpace:
                    if (lineWidth) space += pd.size.width;
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    if (lineWidth && lineWidth + space + pd.size.width > width + LineBreakEpsilon) {
                        y += lineSize.height;
                        lineSize = stringLineSizeImpl(str, pi, width, paraDisplays);
                        x = formatStringX(align, width, lineSize.width, rtl);
                        lineWidth = 0;
                        space = 0;
                    }
                    {
                        float pw = space + pd.size.width;
                        lineWidth += pw;
                        x += rtl ? -pw : pw;
                        space = 0;
                    }
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) break;
    }
    return CSVector2(x, y);
}

CSVector2 CSGraphics::stringPosition(const StringParam& str, float width) const {
	return stringPosition(str, _state->font, width);
}

void CSGraphics::drawStringCharacter(const CSImage* image, const CSVector3& pos, const CSRootImage*& root) {
    const CSRootImage* newRoot = image->root();
    if (!root) {
        root = newRoot;
        begin();
    }
    else if (root != newRoot) {
        end(root, GL_TRIANGLES);
        root = newRoot;
        begin();
    }
    const CSSize& textureSize = root->textureSize();

    const CSRect& frame = image->frame();

    float lu = frame.left() / textureSize.width;
    float ru = frame.right() / textureSize.width;
    float tv = frame.top() / textureSize.height;
    float bv = frame.bottom() / textureSize.height;

    uint vi = vertexCount();

    addVertex(CSVertex(pos, _state->fontColors[0], CSVector2(lu, tv)));
    addVertex(CSVertex(CSVector3(pos.x + frame.size.width, pos.y, pos.z), _state->fontColors[1], CSVector2(ru, tv)));
    addVertex(CSVertex(CSVector3(pos.x, pos.y + frame.size.height, pos.z), _state->fontColors[2], CSVector2(lu, bv)));
    addVertex(CSVertex(CSVector3(pos.x + frame.size.width, pos.y + frame.size.height, pos.z), _state->fontColors[3], CSVector2(ru, bv)));

    addIndex(vi + 0);
    addIndex(vi + 1);
    addIndex(vi + 2);
    addIndex(vi + 1);
    addIndex(vi + 3);
    addIndex(vi + 2);
}

void CSGraphics::drawStringCharacterEnd(const CSRootImage*& root) {
    if (root) {
        end(root, GL_TRIANGLES);
        root = NULL;
    }
}

void CSGraphics::drawStringParagraphs(const StringParam& str, const CSVector3& point) {
    push();

    setLayer(1);

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

    CSColor originColor = this->color();
    CSColor currentColor = originColor;

    const uchar* content = str.display()->content();
    const CSCharSequence* characters = str.display()->characters();

    float y = point.y;

    const CSRootImage* root = NULL;

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeColor:
                    if (pa.attr.color != CSColor::Transparent) {
                        setColor(pa.attr.color);
                        currentColor = this->color();
                    }
                    else {
                        currentColor = originColor;
                        setColor(currentColor, false);
                    }
                    break;
                case CSStringParagraphTypeStroke:
                    if (pa.attr.stroke.color != CSColor::Transparent) setStrokeColor(pa.attr.stroke.color);
                    setStrokeWidth(pa.attr.stroke.width);
                    drawStringCharacterEnd(root);
                    break;
                case CSStringParagraphTypeGradient:
                    setColor(originColor, false);
                    if (pa.attr.gradient.horizontal) {
                        setFontColorH(pa.attr.gradient.color[0], pa.attr.gradient.color[1]);
                    }
                    else {
                        setFontColorV(pa.attr.gradient.color[0], pa.attr.gradient.color[1]);
                    }
                    break;
                case CSStringParagraphTypeGradientReset:
                    setColor(currentColor, false);
                    resetFontColor();
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    {
                        float x = point.x;
                        bool crtl = pa.type == CSStringParagraphTypeRTL;
                        if (crtl) x += pd.size.width;

                        for (uint ci = pa.attr.text.start; ci < pa.attr.text.end; ci++) {
                            const uchar* cc = content + characters->from(ci);
                            uint cclen = characters->length(ci);

                            float ccw = cs->size(cc, cclen, pd.font).width;

                            float offy = 0;
                            const CSImage* image = cs->image(cc, cclen, pd.font, offy);

                            if (image) {
                                drawStringCharacter(image, CSVector3(crtl ? x - ccw : x, y + offy, point.z), root);
                            }
                            x += crtl ? -ccw : ccw;
                        }
                    }
                    y += pd.font->size();
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) break;
    }

    drawStringCharacterEnd(root);

    pop();
}

void CSGraphics::drawStringImpl(const StringParam& str, const CSZRect& destRect, float scroll, const CSArray<ParagraphDisplay>* paraDisplays) {
    push();

    setLayer(1);

    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();
    
    CSColor originColor = this->color();
    CSColor currentColor = originColor;
    
    bool rtl = str.display()->isRTL();
    int align = rtl ? 2 : 0;
    float x = rtl ? destRect.size.width : 0, y = -scroll, space = 0, lineWidth = 0;
    CSSize lineSize = stringLineSizeImpl(str, 0, destRect.size.width, paraDisplays);

    const uchar* content = str.display()->content();
    const CSCharSequence* characters = str.display()->characters();

    const CSRootImage* root = NULL;

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeColor:
                    if (pa.attr.color != CSColor::Transparent) {
                        setColor(pa.attr.color);
                        currentColor = this->color();
                    }
                    else {
                        currentColor = originColor;
                        setColor(currentColor, false);
                    }
                    break;
                case CSStringParagraphTypeStroke:
                    if (pa.attr.stroke.color != CSColor::Transparent) setStrokeColor(pa.attr.stroke.color);
                    setStrokeWidth(pa.attr.stroke.width);
                    drawStringCharacterEnd(root);
                    break;
                case CSStringParagraphTypeAlign:
                    if (align != pa.attr.align) {
                        align = pa.attr.align;
                        x = formatStringX(align, destRect.size.width, lineSize.width, rtl);
                    }
                    break;
                case CSStringParagraphTypeGradient:
                    setColor(originColor, false);
                    if (pa.attr.gradient.horizontal) {
                        setFontColorH(pa.attr.gradient.color[0], pa.attr.gradient.color[1]);
                    }
                    else {
                        setFontColorV(pa.attr.gradient.color[0], pa.attr.gradient.color[1]);
                    }
                    break;
                case CSStringParagraphTypeGradientReset:
                    setColor(currentColor, false);
                    resetFontColor();
                    break;
                case CSStringParagraphTypeLinebreak:
                    {
                        y += lineSize.height;
                        lineSize = stringLineSizeImpl(str, pi + 1, destRect.size.width, paraDisplays);
                        if (y + lineSize.height > destRect.size.height) goto exit;
                        x = formatStringX(align, destRect.size.width, lineSize.width, rtl);
                        lineWidth = 0;
                        space = 0;
                    }
                    break;
                case CSStringParagraphTypeSpace:
                    if (lineWidth) space += pd.size.width;
                    break;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                    if (lineWidth && lineWidth + space + pd.size.width > destRect.size.width + LineBreakEpsilon) {
                        y += lineSize.height;
                        lineSize = stringLineSizeImpl(str, pi, destRect.size.width, paraDisplays);
                        if (y + lineSize.height > destRect.size.height) goto exit;
                        x = formatStringX(align, destRect.size.width, lineSize.width, rtl);
                        lineWidth = 0;
                        space = 0;
                    }
                    if (y + pd.size.height > 0) {
                        float cx = destRect.origin.x + x + (rtl ? -space : space);
                        float cy = destRect.origin.y + y;
                        bool crtl = pa.type == CSStringParagraphTypeRTL;
                        if (rtl && !crtl) cx -= pd.size.width;

                        uint start = CSMath::max(str.start, pa.attr.text.start);
                        uint end = CSMath::min(str.end, pa.attr.text.end);

                        for (uint ci = start; ci < end; ci++) {
                            const uchar* cc = content + characters->from(ci);
                            uint cclen = characters->length(ci);

                            float ccw = cs->size(cc, cclen, pd.font).width;

                            float offy = 0;
                            const CSImage* image = cs->image(cc, cclen, pd.font, offy);

                            if (image) {
                                drawStringCharacter(image, CSVector3(crtl ? cx - ccw : cx, cy + offy, destRect.origin.z), root);
                            }
                            cx += crtl ? -ccw : ccw;
                        }
                    }
                    {
                        float pw = space + pd.size.width;
                        lineWidth += pw;
                        x += rtl ? -pw : pw;
                        space = 0;
                    }
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) goto exit;
    }
exit:
    drawStringCharacterEnd(root);

    pop();
}

void CSGraphics::drawStringScrolled(const StringParam& str, const CSZRect& destRect, float scroll) {
    if (!str) return;
        
    drawStringImpl(str, destRect, scroll, paragraphDisplays(str, _state->font));
}

void CSGraphics::drawString(const StringParam& str, CSZRect destRect, CSAlign align) {
	if (!str) return;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    if (align & (CSAlignCenter | CSAlignRight | CSAlignMiddle | CSAlignBottom)) {
        CSSize size = stringSizeImpl(str, destRect.size.width, paraDisplays);
        
        float scroll = 0;
        
        if (align & CSAlignCenter) {
            destRect.origin.x += (destRect.size.width - size.width) * 0.5f;
            destRect.size.width = size.width;
        }
        else if (align & CSAlignRight) {
            destRect.origin.x += destRect.size.width - size.width;
            destRect.size.width = size.width;
        }
        if (align & CSAlignMiddle) {
            if (size.height > destRect.size.height) {
                scroll = (size.height - destRect.size.height) * 0.5f;
            }
            else {
                destRect.origin.y += (destRect.size.height - size.height) * 0.5f;
                destRect.size.height = size.height;
            }
        }
        else if (align & CSAlignBottom) {
            if (size.height > destRect.size.height) {
                scroll = size.height - destRect.size.height;
            }
            else {
                destRect.origin.y += destRect.size.height - size.height;
                destRect.size.height = size.height;
            }
        }
        drawStringImpl(str, destRect, scroll, paraDisplays);
    }
    else {
        drawStringImpl(str, destRect, 0, paraDisplays);
    }
}

void CSGraphics::drawStringScaled(const StringParam& str, const CSZRect& destRect) {
    if (!str) return;
        
    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    CSSize size = stringSizeImpl(str, destRect.size.width, paraDisplays);
    
    if (size.height > destRect.size.height) {
        float s = 0;
        
        for (; ; ) {
            float ns = CSMath::min(destRect.size.width / size.width, destRect.size.height / size.height);
            if (ns > s) {
                s = ns;
                size = stringSizeImpl(str, destRect.size.width / s, paraDisplays);
            }
            else {
                break;
            }
        }
        pushTransform();
        translate(destRect.origin);
        scale(s);
        drawStringImpl(str, CSZRect(CSVector3::Zero, size), 0, paraDisplays);
        popTransform();
    }
    else {
        drawStringImpl(str, destRect, 0, paraDisplays);
    }
}

void CSGraphics::drawStringScaled(const StringParam& str, CSZRect destRect, CSAlign align) {
    if (!str) return;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

	CSSize size = stringSizeImpl(str, destRect.size.width, paraDisplays);
    
    if (size.height > destRect.size.height) {
        float s = 0;
        
        for (; ; ) {
            float ns = CSMath::min(destRect.size.width / size.width, destRect.size.height / size.height);
            if (ns > s) {
                s = ns;
                size = stringSizeImpl(str, destRect.size.width / s, paraDisplays);
            }
            else {
                break;
            }
        }
        
        if (align & CSAlignCenter) {
            destRect.origin.x += (destRect.size.width - size.width * s) * 0.5f;
        }
        else if (align & CSAlignRight) {
            destRect.origin.x += destRect.size.width - size.width * s;
        }
        if (align & CSAlignMiddle) {
            destRect.origin.y += (destRect.size.height - size.height * s) * 0.5f;
        }
        else if (align & CSAlignBottom) {
            destRect.origin.y += destRect.size.height - size.height * s;
        }
        destRect.size = size;

        pushTransform();
        translate(destRect.origin);
        scale(s);
        
        destRect.origin = CSVector3::Zero;
        
        drawStringImpl(str, destRect, 0, paraDisplays);
        popTransform();
    }
    else {
        if (align & CSAlignCenter) {
            destRect.origin.x += (destRect.size.width - size.width) * 0.5f;
        }
        else if (align & CSAlignRight) {
            destRect.origin.x += destRect.size.width - size.width;
        }
        if (align & CSAlignMiddle) {
            destRect.origin.y += (destRect.size.height - size.height) * 0.5f;
        }
        else if (align & CSAlignBottom) {
            destRect.origin.y += destRect.size.height - size.height;
        }
        destRect.size = size;
        
        drawStringImpl(str, destRect, 0, paraDisplays);
    }
}

float CSGraphics::drawString(const StringParam& str, const CSVector3& point, float width) {
	if (!str) return 0;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    if (width == CSStringWidthUnlimited) width = stringSizeImpl(str, CSStringWidthUnlimited, paraDisplays).width;
        
    drawStringImpl(str, CSZRect(point, CSSize(width, CSStringWidthUnlimited)), 0, paraDisplays);

    return width;
}

float CSGraphics::drawString(const StringParam& str, CSVector3 point, CSAlign align, float width) {
	if (!str) return 0;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    CSSize size = stringSizeImpl(str, width, paraDisplays);

    applyAlign(point, size, align);
    drawStringImpl(str, CSZRect(point, size), 0, paraDisplays);

    return width == CSStringWidthUnlimited ? size.width : width;
}

void CSGraphics::drawStringScaled(const StringParam& str, const CSVector3& point, float width) {
	if (!str) return;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    CSSize size = stringSizeImpl(str, CSStringWidthUnlimited, paraDisplays);
    
    if (size.width > width) {
        float s = width / size.width;
        
        pushTransform();
        translate(point);
        scale(s);
        drawStringImpl(str, CSZRect(CSVector3::Zero, CSSize(width / s, size.height)), 0, paraDisplays);
        popTransform();
    }
    else {
        size.width = width;
        drawStringImpl(str, CSZRect(point, size), 0, paraDisplays);
    }
}

void CSGraphics::drawStringScaled(const StringParam& str, CSVector3 point, CSAlign align, float width) {
	if (!str) return;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    CSSize size = stringSizeImpl(str, CSStringWidthUnlimited, paraDisplays);
    
    if (size.width > width) {
        float s = width / size.width;
        
        applyAlign(point, size * s, align);

        pushTransform();
        translate(point);
        scale(s);
        drawStringImpl(str, CSZRect(CSVector3::Zero, size), 0, paraDisplays);
        popTransform();
    }
    else {
        applyAlign(point, size, align);
        drawStringImpl(str, CSZRect(point, size), 0, paraDisplays);
    }
}

void CSGraphics::drawStringTruncated(const StringParam& str, const CSVector3& point, float width) {
    drawStringTruncated(str, point, CSAlignNone, width);
}

void CSGraphics::drawStringTruncated(StringParam str, CSVector3 point, CSAlign align, float width) {
    if (!str) return;

    CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

    static const CSString* trstr = new CSString("...");

    const CSArray<ParagraphDisplay>* trpd = paragraphDisplays(trstr, _state->font);
    float trw = stringSizeImpl(trstr, CSStringWidthUnlimited, trpd).width;

    const CSArray<ParagraphDisplay>* paraDisplays = paragraphDisplays(str, _state->font);

    bool rtl = str.display()->isRTL();

    CSSize lineSize = CSSize::Zero;

    const CSArray<CSStringParagraph>* paras = str.display()->paragraphs();
    for (uint pi = 0; pi < paras->count(); pi++) {
        const CSStringParagraph& pa = paras->objectAtIndex(pi);
        const ParagraphDisplay& pd = paraDisplays->objectAtIndex(pi);

        if (pd.visible == ParagraphDisplay::Visible) {
            switch (pa.type) {
                case CSStringParagraphTypeLinebreak:
                    return;
                case CSStringParagraphTypeNeutral:
                case CSStringParagraphTypeLTR:
                case CSStringParagraphTypeRTL:
                case CSStringParagraphTypeSpace:
                    //if (lineSize.width + pd.size.width + trw > width) {
                    if (lineSize.width + pd.size.width > width) {
                        applyAlign(point, lineSize, align);
                        str.end = pa.attr.text.start;
                        if (str.start < str.end) {
                            drawStringImpl(str, CSZRect(point, CSSize(width, CSStringWidthUnlimited)), 0, paraDisplays);
                        }
                        drawStringImpl(trstr, CSZRect(rtl ? point.x - trw : point.x + lineSize.width, point.y, point.z, trw, CSStringWidthUnlimited), 0, trpd);
                        return;
                    }
                    if (lineSize.height < pd.size.height) lineSize.height = pd.size.height;
                    lineSize.width += pd.size.width;
                    break;
            }
        }
        else if (pd.visible == ParagraphDisplay::Backward) break;
    }
    applyAlign(point, lineSize, align);
    drawStringImpl(str, CSZRect(point, CSSize(width, CSStringWidthUnlimited)), 0, paraDisplays);
}
