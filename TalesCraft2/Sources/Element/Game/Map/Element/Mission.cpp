//
//  Mission.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "Mission.h"

#include "Map.h"

Mission::Mission(int key, const byte*& cursor) :
    key(key)
{
	switch (readByte(cursor)) {
		case 0:
			font = Asset::sharedAsset()->boldExtraSmallFont;
			break;
		case 1:
			font = Asset::sharedAsset()->boldSmallFont;
			break;
		case 2:
			font = Asset::sharedAsset()->boldMediumFont;
			break;
		case 3:
			font = Asset::sharedAsset()->boldLargeFont;
			break;
		case 4:
			font = Asset::sharedAsset()->boldExtraLargeFont;
			break;
		case 5:
			font = Asset::sharedAsset()->extraSmallFont;
			break;
		case 6:
			font = Asset::sharedAsset()->smallFont;
			break;
		case 7:
			font = Asset::sharedAsset()->mediumFont;
			break;
		case 8:
			font = Asset::sharedAsset()->largeFont;
			break;
		case 9:
			font = Asset::sharedAsset()->extraLargeFont;
			break;
	}
	font->retain();

    if (readBoolean(cursor)) {
        int index0 = readShort(cursor);
        int index1 = readShort(cursor);
        
        _format = retain(Asset::sharedAsset()->strings->objectAtIndex(index0)->objectAtIndex(index1)->value());
    }
    else {
        _format = retain(readLocaleString(cursor)->value());
    }
    
    _valueCount = readLength(cursor);
    
    if (_valueCount) {
        _valueIndices = cursor;
        cursor += _valueCount * 2;
    }
    else {
        _valueIndices = NULL;
    }
    position = CSVector2(cursor);
    size = CSSize(cursor);
	widthFixed = readByte(cursor);
	heightFixed = readByte(cursor);
    align = (CSAlign)readByte(cursor);
    time = readFloat(cursor);
}

Mission::~Mission() {
	font->release();
    release(_format);
}

const CSString* Mission::message() const {
    return Map::sharedMap()->settingValueString(_format, _valueIndices, _valueCount);
}

bool Mission::update() {
    if (!time) {
        return true;
    }
    float frameDelay = Map::sharedMap()->frameDelayFloat();
    
    if (time > frameDelay) {
        time -= frameDelay;
        return true;
    }
    return false;
}
