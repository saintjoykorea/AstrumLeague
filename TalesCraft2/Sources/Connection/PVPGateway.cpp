//
//  PvPGateWay.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2015. 5. 18..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define AccountImpl

#include "PVPGateway.h"

#include "GatewayConfig.h"

#define PVPSocketReadTimeout	70
#define PVPSocketWriteTimeout	20

PVPGateway* PVPGateway::__gateway = NULL;

enum OpCode {
    OpLogin,
    OpUpdateProfile,
    OpPartyCreate,
    OpPartyEnter,
    OpPartyExit,
    OpPartyMap,
    OpPartySlot,
    OpPartyStart,
	OpPartyMatch,
    OpPartySync,
    OpPartyChat,
	OpPartyBack,
    OpMatchCreate,
    OpMatchEnter,
    OpMatchExit,
    OpMatchLevelCorrection,
    OpMatchObserverDelay,
    OpMatchSlot,
    OpMatchQuit,
    OpMatchSwap,
    OpMatchReady,
    OpMatchStart,
    OpMatchSync,
    OpMatchChat,
    OpMatchCommand,
    OpMatchLatency,
    OpMatchAuto,
	OpMatchPause,
	OpMatchResult,
    OpPing = 100
};

#define PVPHandle(func, ...);    \
	{	\
		CSArray<PVPGatewayDelegate*>* __delegates = new CSArray<PVPGatewayDelegate*>(_delegates, false);	\
		foreach(PVPGatewayDelegate *, delegate, __delegates) delegate->func(__VA_ARGS__);	\
		__delegates->release();	\
	}

PVPGateway::PVPGateway() : 
	_socket(NULL),
    _delegates(new CSArray<PVPGatewayDelegate*>()),
    _connIndex(0),
    _rPacketIndex(0),
    _sPacketIndex(0),
    _inpkt(NULL),
    _outpkt(NULL),
    _outpktPosition(0),
    _connState(PVPConnStateShutdown)
{
}

PVPGateway::~PVPGateway() {
    requestShutdown(false);
    
    _delegates->release();
}

void PVPGateway::load() {
    if (!__gateway) {
        __gateway = new PVPGateway();
    }
}

void PVPGateway::destroy() {
    if (__gateway) {
        delete __gateway;
        __gateway = NULL;
    }
}

void PVPGateway::addDelegate(PVPGatewayDelegate* delegate) {
    if (!_delegates->containsObjectIdenticalTo(delegate)) {
        _delegates->addObject(delegate);
    }
}

void PVPGateway::removeDelegate(PVPGatewayDelegate* delegate) {
    _delegates->removeObjectIdenticalTo(delegate);
}

bool PVPGateway::beginSendPacket() {
    if (_socket) {
        _outpktPosition = _outpkt->length();
        
        _outpkt->setLength(_outpktPosition + 10);
        
        return true;
    }
    else {
        CSErrorLog("not connected");
        return false;
    }
}

void PVPGateway::endSendPacket() {
    byte* header = (byte*)_outpkt->data()->bytes() + _outpktPosition;
    writeByte(header, 'T');
    writeByte(header, 'C');
    writeInt(header, _sPacketIndex++);
    writeInt(header, _outpkt->length() - _outpktPosition - 10);
    _socket->write(_outpkt);
}

void PVPGateway::receivePacket() {
    int readLen = _socket->read(_inpkt);
    
    if (readLen <= 0) {
        return;
    }
    
    uint connIndex = _connIndex;
    
    while (_inpkt->remaining() >= 10) {
        _inpkt->mark();
        
        if (_inpkt->readByte() == 'T' && _inpkt->readByte() == 'C') {
            uint packetIndex = _inpkt->readInt();
            
            if (packetIndex == _rPacketIndex) {
                uint dataLen = _inpkt->readInt();
                
                if (_inpkt->remaining() >= dataLen) {
                    _inpkt->mark();
                    _inpkt->setLength(_inpkt->position() + dataLen);
                    
                    OpCode opCode = (OpCode)_inpkt->readByte();
                    
                    //CSLog("receivePacket:%.3f, %d", CSTime::currentTime(), opCode);      //TODO
                    
                    switch (opCode) {
                        case OpLogin:
                            receiveLogin();
                            break;
                        case OpPartyCreate:
                            receivePartyCreate();
                            break;
                        case OpPartyEnter:
                            receivePartyEnter();
                            break;
                        case OpPartyExit:
                            receivePartyExit();
                            break;
                        case OpPartyMap:
                            receivePartyMap();
                            break;
                        case OpPartySlot:
                            receivePartySlot();
                            break;
                        case OpPartyStart:
                            receivePartyStart();
                            break;
						case OpPartyMatch:
							receivePartyMatch();
							break;
                        case OpPartySync:
                            receivePartySync();
                            break;
                        case OpPartyChat:
                            receivePartyChat();
                            break;
						case OpPartyBack:
							receivePartyBack();
							break;
                        case OpMatchCreate:
                            receiveMatchCreate();
                            break;
                        case OpMatchEnter:
                            receiveMatchEnter();
                            break;
                        case OpMatchExit:
                            receiveMatchExit();
                            break;
                        case OpMatchLevelCorrection:
                            receiveMatchLevelCorrection();
                            break;
                        case OpMatchObserverDelay:
                            receiveMatchObserverDelay();
                            break;
                        case OpMatchSlot:
                            receiveMatchSlot();
                            break;
                        case OpMatchQuit:
                            receiveMatchQuit();
                            break;
                        case OpMatchSwap:
                            receiveMatchSwap();
                            break;
                        case OpMatchReady:
                            receiveMatchReady();
                            break;
                        case OpMatchStart:
                            receiveMatchStart();
                            break;
                        case OpMatchSync:
                            receiveMatchSync();
                            break;
                        case OpMatchChat:
                            receiveMatchChat();
                            break;
                        case OpMatchCommand:
                            receiveMatchCommand();
                            break;
						case OpMatchPause:
							receiveMatchPause();
							break;
                    }
                    if (_connIndex == connIndex) {
                        _rPacketIndex++;
                        
                        _inpkt->rewind();
                        _inpkt->setPosition(_inpkt->position() + dataLen);
                        continue;
                    }
                    else {
                        break;
                    }
                }
                else {
                    _inpkt->rewind();
                    break;
                }
            }
        }
        receiveShutdown();
        break;
    }
}

void PVPGateway::receiveSocketEvent(CSSocket* socket, CSSocketEvent event) {
    switch (event) {
        case CSSocketEventConnected:
            _connState = PVPConnStateConnected;
            PVPHandle(onPVPConnect);
            break;
        case CSSocketEventReceived:
            receivePacket();
            break;
        case CSSocketEventSended:
            _socket->write(_outpkt);
            break;
        case CSSocketEventTimeout:
            requestPing();
            break;
        case CSSocketEventError:
            receiveShutdown();
            break;
    }
}

void PVPGateway::requestConnect() {
    if (_connState == PVPConnStateShutdown) {
        CSLog("pvp connect");

        _connState = PVPConnStateConnecting;

        _socket = new CSSocket(PVPGatewayHost, PVPGatewayPort, CSSocketDelegate0::delegate(this, &PVPGateway::receiveSocketEvent));
        _socket->setReadTimeout(PVPSocketReadTimeout);
        _socket->setWriteTimeout(PVPSocketWriteTimeout);
        _socket->setTcpNoDelay(true);

        _inpkt = new CSBuffer(PVPPacketCapacity);
        _outpkt = new CSBuffer(PVPPacketCapacity);
        _rPacketIndex = 0;
        _sPacketIndex = 0;
        _socket->open();
    }
}

void PVPGateway::requestShutdown(bool clear) {
    Account* account = Account::sharedAccount();
    
    if (account) {
        CSObject::release(account->match());
        CSObject::release(account->party());
        
        if (clear) {
            CSObject::release(account->lastPartyId);
        }
    }
    
    CSObject::release(_socket);
    CSObject::release(_outpkt);
    CSObject::release(_inpkt);
    
    _connIndex++;
    _connState = PVPConnStateShutdown;
}

void PVPGateway::requestLogin(bool channelEnter) {
    if (_connState != PVPConnStateConnected || !beginSendPacket()) return;
    _outpkt->writeByte(OpLogin);
    
    _outpkt->writeShort(PVPProtocolVersion);
    _outpkt->writeShort(GameVersion);

    const Account* account = Account::sharedAccount();
    account->writeTo(_outpkt, true);

    endSendPacket();

    _connState = PVPConnStateAuthoring;
}

void PVPGateway::requestUpdateProfile() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpUpdateProfile);
    const Account* account = Account::sharedAccount();
    account->writeTo(_outpkt, true);
    endSendPacket();
}

void PVPGateway::requestPartyCreate(GameOnlineMode mode, int playerCount, int teamPlayerCount, int totalPlayerCount, const char* msg) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartyCreate);
    _outpkt->writeByte(mode);
    _outpkt->writeByte(playerCount);
    _outpkt->writeByte(teamPlayerCount);
    _outpkt->writeByte(totalPlayerCount);
    _outpkt->writeString(msg);
    endSendPacket();
}

void PVPGateway::requestPartyEnter(const char* partyId) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartyEnter);
    _outpkt->writeString(partyId);
    endSendPacket();
}

void PVPGateway::requestPartyExit() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartyExit);
    endSendPacket();
}

void PVPGateway::requestPartyMap(const MapIndex* map) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartyMap);
    _outpkt->writeBoolean(map != NULL);
    if (map) map->writeTo(_outpkt);
    endSendPacket();
}

void PVPGateway::requestPartySlot(int index) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartySlot);
    _outpkt->writeByte(index);
    endSendPacket();
}

void PVPGateway::requestPartyStart(bool on) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartyStart);
    _outpkt->writeBoolean(on);
    endSendPacket();
}

void PVPGateway::requestPartySync() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartySync);
    endSendPacket();
}

void PVPGateway::requestPartyChat(const char* msg) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpPartyChat);
    _outpkt->writeString(msg);
    endSendPacket();
}

void PVPGateway::requestPartyBack() {
	if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
	_outpkt->writeByte(OpPartyBack);
	endSendPacket();
}

void PVPGateway::requestMatchCreate(GameOnlineMode mode, GameLevelCorrection levelCorrection, int maxCardLevel, int maxRuneLevel, int observerDelay, const MapThumb* map, const char* msg) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchCreate);
    _outpkt->writeByte(mode);
    _outpkt->writeByte(levelCorrection);
    _outpkt->writeShort(maxCardLevel);
    _outpkt->writeShort(maxRuneLevel);
    _outpkt->writeShort(observerDelay);
    map->index.writeTo(_outpkt);
    _outpkt->writeString(msg);
    
    _outpkt->writeByte(map->slots->count());
    foreach (const MapThumbSlot&, slot, map->slots) _outpkt->writeByte(slot.alliance);
    endSendPacket();
}

void PVPGateway::requestMatchEnter(uint matchId, uint frame, bool asPlayer) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchEnter);
    _outpkt->writeInt(matchId);
    _outpkt->writeInt(frame);
    _outpkt->writeBoolean(asPlayer);
    endSendPacket();
}

void PVPGateway::requestMatchExit() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchExit);
    endSendPacket();
}

void PVPGateway::requestMatchLevelCorrection(GameLevelCorrection levelCorrection) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchLevelCorrection);
    _outpkt->writeByte(levelCorrection);
    endSendPacket();
}

void PVPGateway::requestMatchObserverDelay(int observerDelay) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchObserverDelay);
    _outpkt->writeShort(observerDelay);
    endSendPacket();
}

void PVPGateway::requestMatchSlot(int index, GameControl control) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchSlot);
    _outpkt->writeByte(index);
    _outpkt->writeByte(control);
    endSendPacket();
}

void PVPGateway::requestMatchQuit(int64 userId) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchQuit);
    _outpkt->writeLong(userId);
    endSendPacket();
}

void PVPGateway::requestMatchSwap(int64 userId, int toIndex) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchSwap);
    _outpkt->writeLong(userId);
    _outpkt->writeByte(toIndex);
    endSendPacket();
}

void PVPGateway::requestMatchReady() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchReady);
    endSendPacket();
}

void PVPGateway::requestMatchStart() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchStart);
    endSendPacket();
}

void PVPGateway::requestMatchSync() {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchSync);
    endSendPacket();
    
    Match* match = Account::sharedAccount()->match();
    if (match) match->timestamp = CSTime::currentTimeSecond();
}

void PVPGateway::requestMatchChat(const char* msg, bool alliance) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchChat);
    _outpkt->writeString(msg);
    _outpkt->writeBoolean(alliance);
    endSendPacket();
}

void PVPGateway::requestMatchCommand(const void* data, uint length) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchCommand);
    _outpkt->write(data, length);
    endSendPacket();
}

void PVPGateway::requestMatchLatency(int framePerSecond, int bottleneck, float totalLatency, uint totalLatencyCount) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchLatency);
    _outpkt->writeByte(framePerSecond);
	_outpkt->writeByte(bottleneck);
    _outpkt->writeFloat(totalLatency);
    _outpkt->writeInt(totalLatencyCount);
    endSendPacket();
}

void PVPGateway::requestMatchAuto(bool on) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchAuto);
    _outpkt->writeBoolean(on);
    endSendPacket();
}

void PVPGateway::requestMatchPause(bool on) {
	if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
	_outpkt->writeByte(OpMatchPause);
	_outpkt->writeBoolean(on);
	endSendPacket();
}

void PVPGateway::requestMatchResult(GameResult result) {
    if (_connState != PVPConnStateLogin || !beginSendPacket()) return;
    _outpkt->writeByte(OpMatchResult);
    _outpkt->writeByte(result);
    endSendPacket();
}

void PVPGateway::requestPing() {
    if (!beginSendPacket()) return;
    _outpkt->writeByte(OpPing);
    endSendPacket();
}

void PVPGateway::receiveShutdown() {
    requestShutdown(false);
    
    CSErrorLog("pvp shutdown");
    
    PVPHandle(onPVPShutdown);
}

void PVPGateway::receiveLogin() {
    _connState = PVPConnStateLogin;
    
    PVPHandle(onPVPLogin);
}

void PVPGateway::receivePartyCreate() {
    Account* account = Account::sharedAccount();
    
    CSObject::retain(account->party(), Party::partyWithBuffer(_inpkt));
    CSObject::retain(account->lastPartyId, account->party()->partyId);
    
    PVPHandle(onPVPPartyCreate);
}

void PVPGateway::receivePartyEnter() {
    Account* account = Account::sharedAccount();
    
    Party*& party = account->party();
    
    int rtn = _inpkt->readByte();
    
    switch (rtn) {
        case 0:
            CSObject::retain(party, Party::partyWithBuffer(_inpkt));
            CSObject::retain(account->lastPartyId, party->partyId);
            PVPHandle(onPVPPartyEnter);
            break;
        case 1:
            CSAssert(account->party(), "invalid state");
            {
                int index = _inpkt->readByte();
                User* user = User::userWithBuffer(_inpkt, false);
                party->enter(index, user);
                PVPHandle(onPVPPartyEnterNotify, user);
            }
            break;
        case 2:
            PVPHandle(onPVPPartyEnterFail);
            break;
    }
}

void PVPGateway::receivePartyExit() {
    Account* account = Account::sharedAccount();
    
    Party*& party = account->party();
    
    int rtn = _inpkt->readByte();
    
    switch (rtn) {
        case 0:
            CSObject::release(party);
            CSObject::release(account->lastPartyId);
            PVPHandle(onPVPPartyExit);
            break;
        case 1:
            CSAssert(party, "invalid state");
            {
                int index = _inpkt->readByte();
                const User* user = account->party()->slots()->objectAtIndex(index).player();
                CSAssert(user, "invalid state");
                user->retain();
                party->exit(index);
                PVPHandle(onPVPPartyExitNotify, user);
                user->release();
            }
            break;
        case 2:
            CSAssert(party, "invalid state");
            party->hostId = _inpkt->readLong();
            PVPHandle(onPVPPartyChangeHost);
            break;
    }
}

void PVPGateway::receivePartyMap() {
    Party* party = Account::sharedAccount()->party();
    
    CSAssert(party, "invalid state");
    if (party->mapIndex) delete party->mapIndex;
    party->mapIndex = _inpkt->readBoolean() ? new MapIndex(_inpkt) : NULL;
    PVPHandle(onPVPPartyMap);
}

void PVPGateway::receivePartySlot() {
    Party* party = Account::sharedAccount()->party();
    
    CSAssert(party, "invalid state");
    int index = _inpkt->readByte();
    
    int accountIndex = party->accountIndex();
    
    party->exit(index);
    
    if (accountIndex == index) {
        PVPHandle(onPVPPartyBan);
    }
    else {
        PVPHandle(onPVPPartySlot, index);
    }
}

void PVPGateway::receivePartyStart() {
    Account* account = Account::sharedAccount();
    
    Party* party = account->party();
    
    CSAssert(party, "invalid state");
    
    PartyState state = (PartyState)_inpkt->readByte();
    int index = _inpkt->readByte();
    
    party->state = state;
    
	switch (state) {
		case PartyStateInit:
			party->matchingPlayerCount = 0;
			foreach(PartySlot&, slot, party->slots()) {
				if (slot.state == PartySlotStateNone) {
					CSObject::release(slot.player());
				}
				else {
					slot.state = PartySlotStateReady;
				}
			}
			break;
		case PartyStateReady:
			party->matchingPlayerCount = party->slots()->count();
			party->readyTimeElapsed = CSTime::currentTime();
			break;
		case PartyStateLink:
			party->matchingPlayerCount = party->totalPlayerCount;
			break;
		case PartyStatePlay:
			party->matchingPlayerCount = party->totalPlayerCount;
			foreach(PartySlot&, slot, party->slots()) {
				if (slot.state == PartySlotStateReady) {
					slot.state = PartySlotStatePlay;
				}
			}
			break;
	}

    PVPHandle(onPVPPartyStart);
    
    if (index >= 0) {
        const User* user = party->slots()->objectAtIndex(index).player();
        if (user && user->userId != account->userId) {
            PVPHandle(onPVPPartyStopNotify, user);
        }
    }
}

void PVPGateway::receivePartyMatch() {
	Account* account = Account::sharedAccount();

	Party* party = account->party();

	CSAssert(party, "invalid state");

	party->matchingPlayerCount = _inpkt->readByte();
}

void PVPGateway::receivePartySync() {
    Party* party = Account::sharedAccount()->party();
    
    CSAssert(party, "invalid state");
    
    requestPartySync();
}

void PVPGateway::receivePartyChat() {
    Account* account = Account::sharedAccount();
    
    Party* party = account->party();
    
    CSAssert(party, "invalid state");
    int index = _inpkt->readByte();
    const User* user = party->slots()->objectAtIndex(index).player();
    
    const CSString* msg = _inpkt->readString();

    PVPHandle(onPVPPartyChat, user, msg);
}

void PVPGateway::receivePartyBack() {
	Account* account = Account::sharedAccount();

	Party* party = account->party();

	CSAssert(party, "invalid state");

	int index = _inpkt->readByte();

	PartySlot& slot = party->slots()->objectAtIndex(index);

	slot.state = PartySlotStateReady;
}

void PVPGateway::receiveMatchCreate() {
    Account* account = Account::sharedAccount();
    
    Match*& match = account->match();
    
    CSObject::retain(match, Match::matchWithBuffer(_inpkt));
    account->lastMatchId = match->matchId;
    
    PVPHandle(onPVPMatchCreate);
}

void PVPGateway::receiveMatchEnter() {
    Account* account = Account::sharedAccount();
    
    Match*& match = account->match();
    
    int rtn = _inpkt->readByte();
    
    switch (rtn) {
        case 0:
            CSObject::retain(match, Match::matchWithBuffer(_inpkt));
            account->lastMatchId = match->matchId;
            PVPHandle(onPVPMatchEnter);
            break;
        case 1:
            CSAssert(match, "invalid state");
            {
                int index = _inpkt->readByte();
                Player* player = Player::playerWithBuffer(_inpkt, false);
                MatchSlot& slot = match->slots()->objectAtIndex(index);
                CSObject::retain(slot.player(), player);
                slot.control = GameControlUser;
                PVPHandle(onPVPMatchEnterPlayerNotify, player);
            }
            break;
        case 2:
            CSAssert(match, "invalid state");
            {
                User* user = User::userWithBuffer(_inpkt, false);
                match->observers()->addObject(user);
                PVPHandle(onPVPMatchEnterObserverNotify, user);
            }
            break;
        case 3:
            PVPHandle(onPVPMatchEnterFail);
            break;
    }
}

void PVPGateway::receiveMatchExit() {
    Account* account = Account::sharedAccount();
    
    Match*& match = account->match();
    
    int rtn = _inpkt->readByte();
    
    switch (rtn) {
        case 0:
            CSObject::release(match);
            PVPHandle(onPVPMatchExit);
            break;
        case 1:
            CSAssert(match, "invalid state");
            {
                int index = _inpkt->readByte();
                MatchSlot& slot = match->slots()->objectAtIndex(index);
                const Player* player = CSObject::retain(slot.player());
                if (match->state == MatchStateInit) {
                    CSObject::release(slot.player());
                }
                PVPHandle(onPVPMatchExitPlayerNotify, player);
                player->release();
            }
            break;
        case 2:
            CSAssert(match, "invalid state");
            {
                int index = _inpkt->readInt();
                const User* user = CSObject::retain(match->observers()->objectAtIndex(index));
                match->observers()->removeObjectAtIndex(index);
                PVPHandle(onPVPMatchExitObserverNotify, user);
                user->release();
            }
            break;
        case 3:
            CSAssert(match, "invalid state");
            match->hostId = _inpkt->readLong();
            PVPHandle(onPVPMatchChangeHost);
            break;
    }
}

void PVPGateway::receiveMatchLevelCorrection() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    match->levelCorrection = (GameLevelCorrection)_inpkt->readByte();
    
    PVPHandle(onPVPMatchLevelCorrection);
}

void PVPGateway::receiveMatchObserverDelay() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    match->observerDelay = _inpkt->readShort();
    
    PVPHandle(onPVPMatchObserverDelay);
}

void PVPGateway::receiveMatchSlot() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    int index = _inpkt->readByte();
    GameControl control = (GameControl)_inpkt->readByte();
    Player* player = NULL;
    if (_inpkt->readBoolean()) {
        player = Player::playerWithBuffer(_inpkt, false);
    }
    
    int accountIndex = match->accountIndex();
    
    MatchSlot& slot = match->slots()->objectAtIndex(index);
    CSObject::retain(slot.player(), player);
    slot.control = control;
    
    if (accountIndex == index) {
        PVPHandle(onPVPMatchBan);
    }
    else {
        PVPHandle(onPVPMatchSlot, index);
    }
}

void PVPGateway::receiveMatchQuit() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    int index = _inpkt->readInt();
    const User* user = match->observers()->objectAtIndex(index);
    user->retain();
    match->observers()->removeObjectAtIndex(index);
    if (user->userId == Account::sharedAccount()->userId) {
        PVPHandle(onPVPMatchBan);
    }
    else {
        PVPHandle(onPVPMatchQuit, user);
    }
    user->release();
}

void PVPGateway::receiveMatchSwap() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    if (_inpkt->readBoolean()) {
        int fromIndex = _inpkt->readByte();
        int toIndex = _inpkt->readByte();
        
        if (toIndex >= 0) {
            match->swap(fromIndex, toIndex);
            
            PVPHandle(onPVPMatchSwap, fromIndex);
            PVPHandle(onPVPMatchSwap, toIndex);
        }
        else {
            MatchSlot& slot = match->slots()->objectAtIndex(fromIndex);
            if (slot.player() && slot.control == GameControlUser) {
                User* observer = new User(slot.player());
                match->observers()->addObject(observer);
                observer->release();
                
                PVPHandle(onPVPMatchSwapToObserver, observer);
            }
            CSObject::release(slot.player());
            slot.control = GameControlUser;
            PVPHandle(onPVPMatchSwap, fromIndex);
        }
    }
    else {
        Player* player = Player::createWithBuffer(_inpkt, false);
        int fromIndex = _inpkt->readInt();
        int toIndex = _inpkt->readByte();
        
        MatchSlot& slot = match->slots()->objectAtIndex(toIndex);
        if (slot.player() && slot.control == GameControlUser) {
            User* observer = new User(slot.player());
            match->observers()->addObject(observer);
            observer->release();
            
            PVPHandle(onPVPMatchSwapToObserver, observer);
        }
        {
            const User* observer = CSObject::retain(match->observers()->objectAtIndex(fromIndex));
            match->observers()->removeObjectAtIndex(fromIndex);
            PVPHandle(onPVPMatchSwapFromObserver, observer);
            observer->release();
        }
        CSObject::retain(slot.player(), player);
        slot.control = GameControlUser;
        player->release();
        
        PVPHandle(onPVPMatchSwap, toIndex);
    }
}

void PVPGateway::receiveMatchReady() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    PVPHandle(onPVPMatchReady);
}

void PVPGateway::receiveMatchStart() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    match->state = MatchStateSync;
    PVPHandle(onPVPMatchStart);
}

void PVPGateway::receiveMatchSync() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    
    match->state = MatchStatePlay;
    
    PVPHandle(onPVPMatchSync);
}

void PVPGateway::receiveMatchChat() {
    Account* account = Account::sharedAccount();
    
    Match* match = account->match();
    
    CSAssert(match, "invalid state");
    
    const User* user;
    
    if (_inpkt->readBoolean()) {
        int index = _inpkt->readByte();
        user = match->slots()->objectAtIndex(index).player();
    }
    else {
        int index = _inpkt->readInt();
        user = match->observers()->objectAtIndex(index);
    }
    const CSString* msg = _inpkt->readString();
        
    PVPHandle(onPVPMatchChat, user, msg);
}

void PVPGateway::receiveMatchCommand() {
    Match* match = Account::sharedAccount()->match();
    
    CSAssert(match, "invalid state");
    int framePerStep = _inpkt->readByte();
    uint frame = _inpkt->readInt();
    float serverDelay = _inpkt->readFloat();
    const void* data = _inpkt->bytes();
    uint length = _inpkt->remaining();
    
    PVPHandle(onPVPMatchCommand, framePerStep, frame, serverDelay, data, length);
}

void PVPGateway::receiveMatchPause() {
	Match* match = Account::sharedAccount()->match();

	CSAssert(match, "invalid state");

	const User* user;

	if (_inpkt->readBoolean()) {
		int index = _inpkt->readByte();
		user = match->slots()->objectAtIndex(index).player();
	}
	else {
		int index = _inpkt->readInt();
		user = match->observers()->objectAtIndex(index);
	}
	bool on = _inpkt->readBoolean();

	match->state = on ? MatchStatePause : MatchStatePlay;

	PVPHandle(onPVPMatchPause, user, on);
}

