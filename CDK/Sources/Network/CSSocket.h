//
//  CSSocket.h
//  CDK
//
//  Created by chan on 13. 2. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSSocket__
#define __CDK__CSSocket__

#include "CSBuffer.h"

#include "CSThread.h"

#ifdef CDK_WINDOWS
#include <WinSock2.h>
typedef SOCKET SOCKET_FD;
#else
typedef int SOCKET_FD;
#define INVALID_SOCKET	-1
#endif

class CSSocket;

enum CSSocketEvent {
    CSSocketEventConnected,
    CSSocketEventReceived,
    CSSocketEventSended,
    CSSocketEventTimeout,
    CSSocketEventError
};

class CSSocket;

typedef CSDelegate<void, CSSocket*, CSSocketEvent> CSSocketDelegate;

typedef CSDelegate0<void, CSSocket*, CSSocketEvent> CSSocketDelegate0;

class CSSocket : public CSObject {
private:
    char* _host;
    uint _port;
    CSSocketDelegate* _delegate;
    CSTask* _task;
	SOCKET_FD _socket;
    float _readTimeout;
    float _writeTimeout;
    double _readElapsed;
    double _writeElapsed;
    enum : byte {
        ConnectNone,
        ConnectWaiting,
        ConnectDone
    }_connect;
    bool _send;
    bool _receive;
    bool _tcpNoDelay;
public:
    CSSocket(const char* host, uint port, CSSocketDelegate* delegate);
private:
    ~CSSocket();
public:
    static inline CSSocket* socket(const char* host, uint port, CSSocketDelegate* delegate) {
        return autorelease(new CSSocket(host, port, delegate));
    }
    
    inline void setReadTimeout(float timeout) {
        _readTimeout = timeout;
    }
    
    inline float readTimeout() const {
        return _readTimeout;
    }
    
    inline void setWriteTimeout(float timeout) {
        _writeTimeout = timeout;
    }

    inline float writeTimeout() const {
        return _writeTimeout;
    }

    inline void setTcpNoDelay(bool tcpNoDelay) {
        _tcpNoDelay = tcpNoDelay;
    }
    
    inline bool tcpNoDelay() const {
        return _tcpNoDelay;
    }

    void open();
    void close();
    
    int read(CSBuffer* buffer);
    int write(CSBuffer* buffer);
private:
    void update();
};

#endif /* defined(__CDK__CSSocket__) */
