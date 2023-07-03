//
//  MapThumb.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "MapThumb.h"

#include "Terrain.h"

#include "Asset.h"

MapThumbSlot::MapThumbSlot(CSBuffer* buffer) : alliance(buffer->readByte()) {
    buffer->readByte();     //color
    buffer->readByte();     //starting point
}
MapThumb::MapThumb(const MapIndex& index, CSBuffer* buffer) : index(index), background(ImageIndex::None), button(AnimationIndex::None), _thumbnail(ImageIndex::None) {
    int levelCount = buffer->readLength();
    CSAssert(index.indices[3] < levelCount, "invalid data");
    for (int i = 0; i < levelCount; i++) {
        int length = buffer->readInt();
        int nextpos = buffer->position() + length;
        if (i == index.indices[3]) {
            name = retain(buffer->readLocaleString());
            description = retain(buffer->readLocaleString());
            type = (GameType)buffer->readByte();
			if (buffer->readBoolean()) background = ImageIndex(buffer);
			if (buffer->readBoolean()) _thumbnail = ImageIndex(buffer);
			if (buffer->readBoolean()) button = AnimationIndex(buffer);
            slots = retain(buffer->readArray<MapThumbSlot>());
        }
        buffer->setPosition(nextpos);
    }
    width = buffer->readByte();
    height = buffer->readByte();
    if (!_thumbnail) {
        _minimap = CSRootImage::createWithBuffer(buffer);
    }
}
MapThumb::~MapThumb() {
    name->release();
    description->release();
    release(slots);
    release(_minimap);
}

MapThumb* MapThumb::map(const MapIndex& index) {
    const char* subpath = CSString::cstringWithFormat("maps/map[%03d][%03d][%03d].xmf", index.indices[0], index.indices[1], index.indices[2]);
    
    CSBuffer* buffer = Asset::readFromFileWithDecryption(subpath, false, true);
    
    if (!buffer) {
        return NULL;
    }
    MapThumb* rtn = autorelease(new MapThumb(index, buffer));
    buffer->release();
    return rtn;
}

const CSImage* MapThumb::image() const {
    return _thumbnail ? Asset::sharedAsset()->image(_thumbnail) : _minimap;
}

uint MapThumb::memory() const {
    return _minimap ? _minimap->memory() + 1024 : 1024;
}

bool MapThumb::compareSlots(const MapThumb* other) const {
    if (slots->count() != other->slots->count()) return false;
    for (int i = 0; i < slots->count(); i++) {
        if (slots->objectAtIndex(i).alliance != other->slots->objectAtIndex(i).alliance) {
            return false;
        }
    }
    return true;
}
