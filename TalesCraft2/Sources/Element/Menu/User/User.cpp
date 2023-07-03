//
//  User.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 11. 29..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "User.h"

#include "Asset.h"

User::User() : name(retain(Asset::sharedAsset()->string(MessageSetGame, MessageGameAIPlayer))) {
    
}

User::User(CSBuffer* buffer, bool dummy) : userId(buffer->readLong()), name(retain(dummy ? buffer->readLocaleString()->value() : buffer->readString())), level(buffer->readShort()) {

}

User::User(const CSJSONObject* json) : userId(json->numberForKey("userId")), name(retain(json->stringForKey("name"))), level(json->numberForKey("level")) {

}

User::User(const User* other) : userId(other->userId), name(retain(other->name)) {

}

User::~User() {
    release(name);
}

void User::writeTo(CSBuffer* buffer) const {
    buffer->writeLong(userId);
    buffer->writeString(name);
    buffer->writeShort(level);
}

void User::copyFrom(const User* otherUser) {
    CSAssert(userId == otherUser->userId, "invalid operation");
    retain(name, otherUser->name);
    level = otherUser->level;
}
