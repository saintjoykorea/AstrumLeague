//
//  Speech.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Speech_h
#define Speech_h

#include "Asset.h"

class Speech : public CSObject {
public:
    ImageIndex portrait;
    const CSString* name;
    const CSString* msg;
    bool confirm;
public:
    Speech(const ImageIndex& portrait, const CSString* name, const CSString* msg);
private:
    ~Speech();
public:
    static inline Speech* speech(const ImageIndex& portrait, const CSString* name, const CSString* msg) {
        return autorelease(new Speech(portrait, name, msg));
    }
};

#endif /* Speech_h */
