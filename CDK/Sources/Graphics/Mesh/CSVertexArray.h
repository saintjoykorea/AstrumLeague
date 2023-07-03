//
//  CSVertexArray.h
//  CDK
//
//  Created by Kim Chan on 2016. 3. 4..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSVertexArray__
#define __CDK__CSVertexArray__

#include "CSObject.h"

#include "CSVertex.h"

#include "CSOpenGL.h"

template <typename V>
class CSTVertexArray : public CSObject {
private:
    struct Buffer {
        uint vao;
        uint vbo;
        uint ibo;
        bool vaoDirty;
    };
    struct Update {
        uint offset;
        uint length;
        void* content;
        Update* prev;
        Update* next;
    };
    Buffer* _buffers;
    mutable Update* _vertexUpdateFirst;
	mutable Update* _vertexUpdateLast;
	mutable Update* _vertexUpdateCurrent;
	mutable Update* _indexUpdateFirst;
	mutable Update* _indexUpdateLast;
	mutable Update* _indexUpdateCurrent;
    uint _bufferCount;
    uint _bufferUnit;
    uint _vertexUnit;
    uint _indexUnit;
    uint _vertexCount;
    uint _indexCount;
    const CSArray<CSVertexLayout>* _layouts;
    bool* _layoutEnabled;
    CSGLContextIdentifier _identifier;
    
public:
    static CSTVertexArray<V>* create(uint unitCapacity, uint vertexUnit, uint indexUnit, bool dynamicVertexBuffer, bool dynamicIndexBuffer, const CSArray<CSVertexLayout>* layouts);
    
    static inline CSTVertexArray<V>* array(uint unitCapacity, uint vertexUnit, uint indexUnit, bool dynamicVertexBuffer, bool dynamicIndexBuffer, const CSArray<CSVertexLayout>* layouts) {
        return autorelease(create(unitCapacity, vertexUnit, indexUnit, dynamicVertexBuffer, dynamicIndexBuffer, layouts));
    }
    
private:
    CSTVertexArray(Buffer* buffers, uint bufferCount, uint bufferUnit, uint vertexUnit, uint indexUnit, const CSArray<CSVertexLayout>* layouts);
    ~CSTVertexArray();

    void releaseUpdate(Update* first);
    Update* addUpdate(Update*& first, Update*& last, uint offset, uint length, uint size);
public:
    void beginVertex();
    void beginVertex(uint offset, uint length);
    
    inline uint vertexCount() const {
        return _vertexCount;
    }
    inline void setVertexCount(uint vertexCount) {
        _vertexCount = vertexCount;
    }
    void addVertex(const V& vertex);
    void setVertex(uint indexOfVertices, const V& vertex);
    inline void clearVertices() {
        _vertexCount = 0;
    }
    void beginIndex();
    void beginIndex(uint offset, uint length);
    
    inline uint indexCount() const {
        return _indexCount;
    }
    inline void setIndexCount(uint indexCount) {
        _indexCount = indexCount;
    }
    void addIndex(uint index);
    void setIndex(uint indexOfIndices, uint index);
    inline void clearIndices() {
        _indexCount = 0;
    }
    void setLayoutEnabled(uint index, bool enabled) const;

	void transfer() const;
    
    void render(uint indexOffset, uint indexCount, uint mode) const;
    inline void render(uint mode) const {
        render(0, _indexCount, mode);
    }
};

using CSVertexArray = CSTVertexArray<CSVertex>;

//=================================================================================================================================================================
template <typename V>
CSTVertexArray<V>* CSTVertexArray<V>::create(uint unitCapacity, uint vertexUnit, uint indexUnit, bool dynamicVertexBuffer, bool dynamicIndexBuffer, const CSArray<CSVertexLayout>* layouts) {
    uint bufferCount = (unitCapacity * vertexUnit + 65535) / 65536;
    uint bufferUnit = unitCapacity / bufferCount;
    if (unitCapacity % bufferCount) {
        bufferUnit++;
    }
    glGetErrorCS();
    
    bool success = true;
    
    GLenum err;
    
    Buffer* buffers = (Buffer*)fcalloc(bufferCount, sizeof(Buffer));
        
    for (uint i = 0; i < bufferCount; i++) {
        glGenVertexArraysCS(1, &buffers[i].vao);
        glBindVertexArrayCS(buffers[i].vao);
        glGenBuffersCS(2, &buffers[i].vbo);
        glBindBufferCS(GL_ARRAY_BUFFER, buffers[i].vbo);
        glBufferDataCS(GL_ARRAY_BUFFER, sizeof(V) * bufferUnit * vertexUnit, NULL, dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            
        err = glGetErrorCS();
            
        if (err) {
            CSErrorLog("loading error:%04x", err);
            success = false;
            break;
        }
        glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, buffers[i].ibo);
        glBufferDataCS(GL_ELEMENT_ARRAY_BUFFER, 2 * bufferUnit * indexUnit, NULL, dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            
        err = glGetErrorCS();
            
        if (err) {
            CSErrorLog("loading error:%04x", err);
            success = false;
            break;
        }
        foreach(const CSVertexLayout, layout, layouts) {
            glVertexAttribPointerCS(layout.attrib, layout.size, layout.type, layout.normalized, sizeof(V), (const GLvoid*)(int64)layout.offset);
            glEnableVertexAttribArrayCS(layout.attrib);
        }
    }
    glBindVertexArrayCS(0);
    glBindBufferCS(GL_ARRAY_BUFFER, 0);
    glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    if (success) {
        return new CSTVertexArray<V>(buffers, bufferCount, bufferUnit, vertexUnit, indexUnit, layouts);
    }
    else {
        for (uint i = 0; i < bufferCount; i++) {
            if (buffers[i].vbo) {
                glDeleteBuffers(1, &buffers[i].vbo);
            }
            if (buffers[i].ibo) {
                glDeleteBuffers(1, &buffers[i].ibo);
            }
            if (buffers[i].vao) {
                glDeleteVertexArraysCS(1, &buffers[i].vao);
            }
        }
        free(buffers);
        return NULL;
    }
}

template <typename V>
CSTVertexArray<V>::CSTVertexArray(Buffer* buffers, uint bufferCount, uint bufferUnit, uint vertexUnit, uint indexUnit, const CSArray<CSVertexLayout>* layouts) :
    _buffers(buffers),
    _bufferCount(bufferCount),
    _bufferUnit(bufferUnit),
    _vertexUnit(vertexUnit),
    _indexUnit(indexUnit),
    _layouts(retain(layouts))
{
    _identifier.reset();
    
    _layoutEnabled = new bool[layouts->count()];
    memset(_layoutEnabled, true, layouts->count());
}

template <typename V>
CSTVertexArray<V>::~CSTVertexArray() {
    _layouts->release();
    delete[] _layoutEnabled;
    
    if (_identifier.isValid()) {
        for (uint i = 0; i < _bufferCount; i++) {
            glDeleteBuffersCS(2, &_buffers[i].vbo);
            glDeleteVertexArraysCS(1, &_buffers[i].vao);
        }
    }
    free(_buffers);
        
    releaseUpdate(_vertexUpdateFirst);
    releaseUpdate(_indexUpdateFirst);
}

template <typename V>
void CSTVertexArray<V>::releaseUpdate(Update *first) {
    while (first) {
        Update* next = first->next;
        free(first->content);
        free(first);
        first = next;
    }
}

template <typename V>
typename CSTVertexArray<V>::Update* CSTVertexArray<V>::addUpdate(Update*& first, Update*& last, uint offset, uint length, uint size) {
    Update** current = &last;
    
    while (*current) {
        uint left0 = (*current)->offset;
        uint left1 = offset;
        uint right0 = left0 + (*current)->length;
        uint right1 = left1 + length;
        
        if (left0 <= left1 && right0 >= right1) {
            return *current;
        }
        if (left0 >= left1 && right0 <= right1) {
            Update* prev = (*current)->prev;
            if (prev) {
                prev->next = (*current)->next;
            }
            if (*current == first) {
                first = (*current)->next;
            }
            free((*current)->content);
            free((*current));
            *current = prev;
        }
        else if (CSMath::max(left0, left1) <= CSMath::min(right0, right1)) {
            current = &(*current)->prev;
        }
        else {
            break;
        }
    }
    Update* newUpdate = (Update*)fmalloc(sizeof(Update));
    newUpdate->content = fmalloc(length * size);
    newUpdate->offset = offset;
    newUpdate->length = length;
    newUpdate->prev = last;
    newUpdate->next = NULL;
    if (last) {
        last->next = newUpdate;
    }
    last = newUpdate;
    if (!first) {
        first = newUpdate;
    }
    return newUpdate;
}

template <typename V>
void CSTVertexArray<V>::beginVertex(uint offset, uint length) {
    _vertexUpdateCurrent = addUpdate(_vertexUpdateFirst, _vertexUpdateLast, offset, length, sizeof(V));
}

template <typename V>
void CSTVertexArray<V>::beginVertex() {
    beginVertex(0, _bufferCount * _bufferUnit * _vertexUnit);
}

template <typename V>
void CSTVertexArray<V>::setVertex(uint indexOfVertices, const V& vertex) {
    CSAssert(indexOfVertices >= _vertexUpdateCurrent->offset && indexOfVertices < _vertexUpdateCurrent->offset + _vertexUpdateCurrent->length, "out of range");
    ((V*)_vertexUpdateCurrent->content)[indexOfVertices - _vertexUpdateCurrent->offset] = vertex;
}

template <typename V>
void CSTVertexArray<V>::addVertex(const V& vertex) {
    setVertex(_vertexCount, vertex);
    _vertexCount++;
}

template <typename V>
void CSTVertexArray<V>::beginIndex(uint offset, uint length) {
    _indexUpdateCurrent = addUpdate(_indexUpdateFirst, _indexUpdateLast, offset, length, 2);
}

template <typename V>
void CSTVertexArray<V>::beginIndex() {
    beginIndex(0, _bufferCount * _bufferUnit * _indexUnit);
}

template <typename V>
void CSTVertexArray<V>::setIndex(uint indexOfIndices, uint index) {
    CSAssert(indexOfIndices >= _indexUpdateCurrent->offset && indexOfIndices < _indexUpdateCurrent->offset + _indexUpdateCurrent->length, "out of range");
    
    index %= _bufferUnit * _vertexUnit;
    
    ((ushort*)_indexUpdateCurrent->content)[indexOfIndices - _indexUpdateCurrent->offset] = index;
}

template <typename V>
void CSTVertexArray<V>::addIndex(uint index) {
    setIndex(_indexCount, index);
    _indexCount++;
}

template <typename V>
void CSTVertexArray<V>::setLayoutEnabled(uint index, bool enabled) const {
    CSAssert(index < _layouts->count(), "out of index");
    
    if (_layoutEnabled[index] != enabled) {
        _layoutEnabled[index] = enabled;
        
        for (uint i = 0; i < _bufferCount; i++) {
            _buffers[i].vaoDirty = true;
        }
    }
}

template <typename V>
void CSTVertexArray<V>::transfer() const {
    //안드로이드에서는 VBO업데이트시 VAO바인딩 해줘야 함
    
    Update* current = _vertexUpdateFirst;
    
    while (current) {
        uint first = current->offset / (_bufferUnit * _vertexUnit);
        uint last = (current->offset + current->length - 1) / (_bufferUnit * _vertexUnit);
        V* content = (V*)current->content;
        
        for (uint i = first; i <= last; i++) {
            uint localOffset = i == first ? current->offset - i * _bufferUnit * _vertexUnit : 0;
            uint localLength = (i == last ? current->offset + current->length - i * _bufferUnit * _vertexUnit : _bufferUnit * _vertexUnit) - localOffset;
            glBindVertexArrayCS(_buffers[i].vao);
            glBindBufferCS(GL_ARRAY_BUFFER, _buffers[i].vbo);
            glBufferSubDataCS(GL_ARRAY_BUFFER, sizeof(V) * localOffset, sizeof(V) * localLength, content);
            content += localLength;
        }
        
        Update* next = current->next;
        free(current->content);
        free(current);
        current = next;
    }
    _vertexUpdateFirst = NULL;
    _vertexUpdateLast = NULL;
    _vertexUpdateCurrent = NULL;
    glBindVertexArrayCS(0);
    glBindBufferCS(GL_ARRAY_BUFFER, 0);
    
    current = _indexUpdateFirst;
    
    while (current) {
        uint first = current->offset / (_bufferUnit * _indexUnit);
        uint last = (current->offset + current->length - 1) / (_bufferUnit * _indexUnit);
        ushort* content = (ushort*)current->content;
        
        for (uint i = first; i <= last; i++) {
            uint localOffset = i == first ? current->offset - i * _bufferUnit * _indexUnit : 0;
            uint localLength = (i == last ? current->offset + current->length - i * _bufferUnit * _indexUnit : _bufferUnit * _indexUnit) - localOffset;
            glBindVertexArrayCS(_buffers[i].vao);
            glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, _buffers[i].ibo);
            glBufferSubDataCS(GL_ELEMENT_ARRAY_BUFFER, 2 * localOffset, 2 * localLength, content);
            content += localLength;
        }
        
        Update* next = current->next;
        free(current->content);
        free(current);
        current = next;
    }
    _indexUpdateFirst = NULL;
    _indexUpdateLast = NULL;
    _indexUpdateCurrent = NULL;
    glBindVertexArrayCS(0);
    glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename V>
void CSTVertexArray<V>::render(uint indexOffset, uint indexCount, uint mode) const {
    transfer();
    
    indexCount = CSMath::min(indexOffset + indexCount, _indexCount);
    
    for (uint i = 0; i < _bufferCount && indexCount; i++) {
        uint localIndexCount = CSMath::min(indexCount, _bufferUnit * _indexUnit);
        indexCount -= localIndexCount;
        
        if (indexOffset < localIndexCount) {
            localIndexCount -= indexOffset;
            
            glBindVertexArrayCS(_buffers[i].vao);
                
            if (_buffers[i].vaoDirty) {
                for (uint j = 0; j < _layouts->count(); j++) {
                    const CSVertexLayout& layout = _layouts->objectAtIndex(j);
                    if (_layoutEnabled[j]) {
                        glEnableVertexAttribArrayCS(layout.attrib);
                    }
                    else {
                        glDisableVertexAttribArrayCS(layout.attrib);
                    }
                }
                _buffers[i].vaoDirty = false;
            }
            
            glDrawElementsCS(mode, localIndexCount, GL_UNSIGNED_SHORT, (const GLvoid*)(sizeof(ushort) * indexOffset));
            
            indexOffset = 0;
        }
        else {
            indexOffset -= localIndexCount;
        }
    }
    glBindVertexArrayCS(0);
}

#endif /* __CDK__CSVertexArray__ */
