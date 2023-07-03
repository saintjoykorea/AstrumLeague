//
//  AssetIndex.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "AssetIndex.h"

const AssetIndex2 AssetIndex2::None(-1);
const AssetIndex3 AssetIndex3::None(-1);
const AssetIndex4 AssetIndex4::None(-1);

AssetIndex2::AssetIndex2(const byte*& cursor) {
    indices[0] = readShort(cursor);
    indices[1] = readShort(cursor);
}

AssetIndex2::AssetIndex2(CSBuffer* buffer) {
    indices[0] = buffer->readShort();
    indices[1] = buffer->readShort();
}

AssetIndex2::AssetIndex2(int index0, int index1) {
    indices[0] = index0;
    indices[1] = index1;
}

AssetIndex2::AssetIndex2(int raw) {
    indices[0] = raw >> 16;
    indices[1] = raw & 0xffff;
}

void AssetIndex2::writeTo(CSBuffer* buffer) const {
    buffer->writeShort(indices[0]);
    buffer->writeShort(indices[1]);
}

AssetIndex3::AssetIndex3(const byte*& cursor) {
    indices[0] = readShort(cursor);
    indices[1] = readShort(cursor);
    indices[2] = readShort(cursor);
}

AssetIndex3::AssetIndex3(CSBuffer* buffer) {
    indices[0] = buffer->readShort();
    indices[1] = buffer->readShort();
    indices[2] = buffer->readShort();
}

AssetIndex3::AssetIndex3(int index0, int index1, int index2) {
    indices[0] = index0;
    indices[1] = index1;
    indices[2] = index2;
}

AssetIndex3::AssetIndex3(const AssetIndex2& group, int index2) {
    indices[0] = group.indices[0];
    indices[1] = group.indices[1];
    indices[2] = index2;
}

AssetIndex3::AssetIndex3(int64 raw) {
    indices[0] = (raw >> 32) & 0xffff;
    indices[1] = (raw >> 16) & 0xffff;
    indices[2] = raw & 0xffff;
}

void AssetIndex3::writeTo(CSBuffer* buffer) const {
    buffer->writeShort(indices[0]);
    buffer->writeShort(indices[1]);
    buffer->writeShort(indices[2]);
}

AssetIndex4::AssetIndex4(const byte*& cursor) {
    indices[0] = readShort(cursor);
    indices[1] = readShort(cursor);
    indices[2] = readShort(cursor);
    indices[3] = readShort(cursor);
}

AssetIndex4::AssetIndex4(CSBuffer* buffer) {
    indices[0] = buffer->readShort();
    indices[1] = buffer->readShort();
    indices[2] = buffer->readShort();
    indices[3] = buffer->readShort();
}

AssetIndex4::AssetIndex4(int index0, int index1, int index2, int index3) {
    indices[0] = index0;
    indices[1] = index1;
    indices[2] = index2;
    indices[3] = index3;
}

AssetIndex4::AssetIndex4(const AssetIndex2& group, int index2, int index3) {
    indices[0] = group.indices[0];
    indices[1] = group.indices[1];
    indices[2] = index2;
    indices[3] = index3;
}

AssetIndex4::AssetIndex4(const AssetIndex3& group, int index3) {
    indices[0] = group.indices[0];
    indices[1] = group.indices[1];
    indices[2] = group.indices[2];
    indices[3] = index3;
}

AssetIndex4::AssetIndex4(int64 raw) {
    indices[0] = (raw >> 48) & 0xffff;
    indices[1] = (raw >> 32) & 0xffff;
    indices[2] = (raw >> 16) & 0xffff;
    indices[3] = raw & 0xffff;
}

void AssetIndex4::writeTo(CSBuffer* buffer) const {
    buffer->writeShort(indices[0]);
    buffer->writeShort(indices[1]);
    buffer->writeShort(indices[2]);
    buffer->writeShort(indices[3]);
}
