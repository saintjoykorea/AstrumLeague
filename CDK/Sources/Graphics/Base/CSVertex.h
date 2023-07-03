//
//  CSVertex.h
//  CDK
//
//  Created by 김찬 on 13. 11. 12..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSVertex__
#define __CDK__CSVertex__

#include "CSColor.h"
#include "CSVector2.h"
#include "CSVector3.h"

#include "CSArray.h"

class CSBuffer;

struct CSVertex {
    CSVector3 position;
    CSColor color;
    CSVector2 textureCoord;
    CSVector3 normal;
    
    inline CSVertex() {
    
    }
    
    inline CSVertex(const CSVector3& position,
                    const CSColor& color,
                    const CSVector2& textureCoord,
                    const CSVector3& normal) :
        position(position),
        color(color),
        textureCoord(textureCoord),
        normal(normal)
    {
    }
    
    inline CSVertex(const CSVector3& position) :
        position(position),
        color(CSColor::White),
        textureCoord(CSVector2::Zero),
        normal(CSVector3::UnitZ)
    {
    }
    
    inline CSVertex(const CSVector3& position, const CSColor& color) :
        position(position),
        color(color),
        textureCoord(CSVector2::Zero),
        normal(CSVector3::UnitZ)
    {
    }
    
    inline CSVertex(const CSVector3& position, const CSVector2& textureCoord) :
        position(position),
        color(CSColor::White),
        textureCoord(textureCoord),
        normal(CSVector3::UnitZ)
    {
    }
    
    inline CSVertex(const CSVector3& position, const CSVector3& normal) :
        position(position),
        color(CSColor::White),
        textureCoord(CSVector2::Zero),
        normal(normal)
    {
    }
    
    inline CSVertex(const CSVector3& position, const CSColor& color, const CSVector2& textureCoord) :
        position(position),
        color(color),
        textureCoord(textureCoord),
        normal(CSVector3::UnitZ)
    {
    }
    
    inline CSVertex(const CSVector3& position, const CSColor& color, const CSVector3& normal) :
        position(position),
        color(color),
        textureCoord(CSVector2::Zero),
        normal(normal)
    {
    }
    
    inline CSVertex(const CSVector3& position, const CSVector2& textureCoord, const CSVector3& normal) :
        position(position),
        color(CSColor::White),
        textureCoord(textureCoord),
        normal(normal)
    {
    }

	CSVertex(CSBuffer* buffer);
	CSVertex(const byte*& raw);
};

enum CSVertexAttrib {
    CSVertexAttribPosition,
    CSVertexAttribColor,
    CSVertexAttribTextureCoord,
    CSVertexAttribNormal
};

struct CSVertexLayout {
    int attrib;
    int size;
    int type;
    int normalized;
    int offset;
    
    inline CSVertexLayout() {
    
    }
    inline CSVertexLayout(int attrib, int size, int type, int normalized, int offset) :
        attrib(attrib),
        size(size),
        type(type),
        normalized(normalized),
        offset(offset)
    {
    
    }
};

extern const CSArray<CSVertexLayout>* CSVertexLayouts;

#endif
