//
//  MapThumb.h
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef MapThumb_h
#define MapThumb_h

#include "AssetIndex.h"

struct MapThumbSlot {
    byte alliance;
    
    MapThumbSlot(CSBuffer* buffer);
};

class MapThumb : public CSObject {
public:
    MapIndex index;
    const CSLocaleString* name;
    const CSLocaleString* description;
    GameType type;
	ImageIndex background;
	AnimationIndex button;
	const CSArray<MapThumbSlot>* slots;
    byte width;
    byte height;
private:
    ImageIndex _thumbnail;
    const CSRootImage* _minimap;
public:
    MapThumb(const MapIndex& index, CSBuffer* buffer);
private:
    ~MapThumb();
public:
    static MapThumb* map(const MapIndex& index);
    
    const CSImage* image() const;
    
    uint memory() const;
    
    bool compareSlots(const MapThumb* other) const;
};

#endif /* MapThumb_h */
