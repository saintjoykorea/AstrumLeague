//
//  CSVertexLayout.cpp
//  CDK
//
//  Created by Kim Chan on 2016. 3. 4..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSVertex.h"

#include "CSOpenGL.h"

#include "CSBuffer.h"

static const CSVertexLayout __CSVertexLayouts[] = {
    CSVertexLayout(CSVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, offsetof(CSVertex, position)),
    CSVertexLayout(CSVertexAttribColor, 4, GL_FLOAT, GL_FALSE, offsetof(CSVertex, color)),
    CSVertexLayout(CSVertexAttribTextureCoord, 2, GL_FLOAT, GL_FALSE, offsetof(CSVertex, textureCoord)),
    CSVertexLayout(CSVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, offsetof(CSVertex, normal))
};

const CSArray<CSVertexLayout>* CSVertexLayouts = new CSArray<CSVertexLayout>(__CSVertexLayouts, 4);

CSVertex::CSVertex(CSBuffer* buffer) :
	position(buffer),
	color(buffer, true),
	textureCoord(buffer),
	normal(buffer)
{

}

CSVertex::CSVertex(const byte*& raw) :
	position(raw),
	color(raw, true),
	textureCoord(raw),
	normal(raw)
{

}
