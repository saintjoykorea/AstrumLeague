//
//  User.h
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 11. 29..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef User_h
#define User_h

#include "Application.h"

class User : public CSObject {
public:
    int64 userId;
    const CSString* name;
    ushort level;
public:
    User();
    User(CSBuffer* buffer, bool dummy);
    User(const CSJSONObject* json);
    User(const User* other);
protected:
    virtual ~User();
public:
    static inline User* userWithBuffer(CSBuffer* buffer, bool dummy) {
        return autorelease(new User(buffer, dummy));
    }
    static inline User* userWithJSON(const CSJSONObject* json) {
        return autorelease(new User(json));
    }
    static inline User* userWithUser(const User* other) {
        return autorelease(new User(other));
    }

    void writeTo(CSBuffer* buffer) const;
    void copyFrom(const User* user);
};

#endif /* User_h */
