//
//  Message.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 9. 3..
//  Copyright © 2019년 brgames. All rights reserved.
//
#include "Message.h"

Message::Message(const ImageIndex& icon, const CSString* content, bool essential) : icon(icon), content(retain(content)), essential(essential) {
    
}

Message::~Message() {
    release(content);
}

bool Message::isEqualToMessage(const Message* other) const {
    return CSString::isEqual(content, other->content);
}
