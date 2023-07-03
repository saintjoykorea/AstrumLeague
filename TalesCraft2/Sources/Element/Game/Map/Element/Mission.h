//
//  Mission.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Mission_h
#define Mission_h

#include "Application.h"

class Mission : public CSObject {
public:
    byte key;
	const CSFont* font;
private:
    const CSString* _format;
    const byte* _valueIndices;
    uint _valueCount;
public:
    CSVector2 position;
    CSSize size;
    CSAlign align;
    float time;
    bool confirm;
    bool replaced;
	bool widthFixed;
	bool heightFixed;

    Mission(int key, const byte*& cursor);
private:
    ~Mission();
public:
    static inline Mission* mission(int key, const byte*& cursor) {
        return autorelease(new Mission(key, cursor));
    }
    
    const CSString* message() const;

    bool update();
};

#endif /* Mission_h */
