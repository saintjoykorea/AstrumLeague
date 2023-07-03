//
//  Message.hpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 9. 3..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef Message_h
#define Message_h

#include "Animation.h"

class Message : public CSObject {
public:
    ImageIndex icon;
    const CSString* content;
    bool essential;

    Message(const ImageIndex& icon, const CSString* content, bool essential = false);
private:
    ~Message();
public:
    static inline Message* message(const ImageIndex& icon, const CSString* content, bool essential = false) {
        return autorelease(new Message(icon, content, essential));
    }
    
    bool isEqualToMessage(const Message* other) const;
    
    inline bool isEqual(const CSObject* object) const override {
        const Message* other = dynamic_cast<const Message*>(object);
        
        return other && isEqualToMessage(other);
    }
};

#endif /* Message_hpp */
