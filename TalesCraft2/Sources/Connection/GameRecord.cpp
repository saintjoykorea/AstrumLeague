//
//  GameRecord.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 11. 23..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "GameRecord.h"

#include "Account.h"

#define GameResumeRecordPath    "gameResume.xmf"

uint GameRecord::resumeMatch() {
    CSBuffer* buffer = CSBuffer::bufferWithContentOfFile(CSFile::storagePath(GameResumeRecordPath));
    
    if (buffer) {
        int64 time = buffer->readLong();
        uint matchId = buffer->readInt();
        if (CSTime::currentTimeSecond() - time <= GameReconnectWaitingTime) return matchId;
    }
    return 0;
}

bool GameRecord::recordMatch() {
    const Match* match = Account::sharedAccount()->match();

    if (!match) return false;

    CSBuffer* buffer = CSBuffer::bufferWithCapacity(12);
    buffer->writeLong(CSTime::currentTimeSecond());
    buffer->writeInt(match->matchId);
    return buffer->writeToFile(CSFile::storagePath(GameResumeRecordPath));
}

void GameRecord::expireMatch() {
    CSFile::deleteFile(CSFile::storagePath(GameResumeRecordPath));
}
