//
//  CSSocket.cpp
//  CDK
//
//  Created by chan on 13. 2. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSSocket.h"
#include "CSTime.h"
#include "CSApplication.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CDK_WINDOWS
#include <WinSock2.h>
#include <Ws2tcpip.h>
#define RecvFlags	0
#define SendFlags	0

#else

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <err.h>

#ifdef CDK_IOS
#include <netinet/in.h>
#include <netinet/tcp.h>
#elif defined(CDK_ANDROID)
#include <linux/tcp.h>
#endif

#define RecvFlags	MSG_DONTWAIT

#ifdef MSG_NOSIGNAL
#define SendFlags	(MSG_DONTWAIT | MSG_NOSIGNAL)
#else
#define SendFlags   MSG_DONTWAIT
#endif

#define closesocket(s)  ::close(s)

#endif

#define CSSocketReadTimeoutDefault      30
#define CSSocketWriteTimeoutDefault     5

CSSocket::CSSocket(const char* host, uint port, CSSocketDelegate* delegate) {
    _host = strdup(host);
    _port = port;
    _socket = INVALID_SOCKET;
    _readTimeout = CSSocketReadTimeoutDefault;
    _writeTimeout = CSSocketWriteTimeoutDefault;
    _delegate = retain(delegate);
}

CSSocket::~CSSocket() {
    close();
    
    free(_host);
    _delegate->release();
}

#ifdef CDK_WINDOWS
static bool wsaInit() {
	static int __wsaInit = 0;

	if (__wsaInit != 2) {
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			return false;
		}
		if (__wsaInit == 0) {
			CSApplication::sharedApplication()->OnDestroy.add([]() {
				WSACleanup();
				__wsaInit = 1;
			});
		}

		__wsaInit = 2;
	}
	return true;
}

void CSSocket::open() {
	if (!wsaInit()) {
		(*_delegate)(this, CSSocketEventError);
		return;
	}

	close();

	struct addrinfo hints, *res, *res0;
	int error;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	error = getaddrinfo(_host, CSString::cstringWithFormat("%d", _port), &hints, &res0);

	if (!error) {
		for (res = res0; res; res = res->ai_next) {
			_socket = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

			if (_socket == INVALID_SOCKET) {
				continue;
			}
			
			{
				unsigned long opt = 1;
				if (ioctlsocket(_socket, FIONBIO, &opt)) {
					closesocket(_socket);
					_socket = INVALID_SOCKET;
					break;
				}
			}
			struct linger ling;
			ling.l_onoff = 1;
			ling.l_linger = 0;
			if (setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling)) == -1) {
				closesocket(_socket);
				_socket = INVALID_SOCKET;
				break;
			}
			if (_tcpNoDelay) {
				int opt = 1;
				if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt)) == -1) {
					closesocket(_socket);
					_socket = INVALID_SOCKET;
					break;
				}
			}
			int connectResult = connect(_socket, res->ai_addr, res->ai_addrlen);

			if (connectResult == 0) {
				_connect = ConnectDone;
				break;
			}
			else if (WSAGetLastError() == WSAEWOULDBLOCK) {
				_connect = ConnectWaiting;
				break;
			}
			else {
				closesocket(_socket);
				_socket = INVALID_SOCKET;
			}
		}
		freeaddrinfo(res0);
	}
	if (_socket == INVALID_SOCKET) {
		(*_delegate)(this, CSSocketEventError);
		return;
	}

	_readElapsed = _writeElapsed = CSTime::currentTime();

	_task = CSTask::task(CSDelegate0<void>::delegate(this, &CSSocket::update), 0, CSTaskActivityRepeat);

	CSThread::mainThread()->addTask(_task);

	if (_connect == ConnectDone) {
		(*_delegate)(this, CSSocketEventConnected);
	}
}
#else
void CSSocket::open() {
    close();
    
    struct addrinfo hints, *res, *res0;
    int error;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
#ifdef CDK_IOS
    hints.ai_flags = AI_DEFAULT;
#else
    hints.ai_protocol = IPPROTO_TCP;
#endif
    
    error = getaddrinfo(_host, CSString::cstringWithFormat("%d",_port), &hints, &res0);

    if (!error) {
        for (res = res0; res; res = res->ai_next) {
            _socket = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            
            if (_socket == INVALID_SOCKET) {
                continue;
            }
            
            long arg = fcntl(_socket, F_GETFL, NULL);
            if (arg < 0) {
                closesocket(_socket);
                _socket = INVALID_SOCKET;
                break;
            }
            
            arg |= O_NONBLOCK;
            if (fcntl(_socket, F_SETFL, arg) < 0) {
                closesocket(_socket);
                _socket = INVALID_SOCKET;
                break;
            }
            
            struct linger ling;
            ling.l_onoff = 1;
            ling.l_linger = 0;
            if (setsockopt(_socket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == -1) {
                closesocket(_socket);
                _socket = INVALID_SOCKET;
                break;
            }
#ifdef SO_NOSIGPIPE
            {
                int opt = 1;
                if (setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) == -1) {
                    closesocket(_socket);
                    _socket = -1;
                    break;
                }
            }
#endif
            if (_tcpNoDelay) {
                int opt = 1;
                if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == -1) {
                    closesocket(_socket);
                    _socket = INVALID_SOCKET;
                    break;
                }
            }
            
            int connectResult = connect(_socket, res->ai_addr, res->ai_addrlen);
            
            if (connectResult == 0) {
                _connect = ConnectDone;
                break;
            }
            else if (errno == EINPROGRESS) {
                _connect = ConnectWaiting;
                break;
            }
            else {
                ::close(_socket);
                _socket = INVALID_SOCKET;
            }
        }
        freeaddrinfo(res0);
    }
    if (_socket == INVALID_SOCKET) {
        (*_delegate)(this, CSSocketEventError);
        return;
    }
    _readElapsed = _writeElapsed = CSTime::currentTime();
    
    _task = CSTask::task(CSDelegate0<void>::delegate(this, &CSSocket::update), 0, CSTaskActivityRepeat);
    
    CSThread::mainThread()->addTask(_task);
    
    if (_connect == ConnectDone) {
        (*_delegate)(this, CSSocketEventConnected);
    }
}
#endif

void CSSocket::update() {
    if (_connect == ConnectNone) {
        return;
    }
    double current = CSTime::currentTime();
    
    fd_set readfds;
    fd_set writefds;
    fd_set excfds;
    
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&excfds);
    FD_SET(_socket, &readfds);
    FD_SET(_socket, &writefds);
    FD_SET(_socket, &excfds);
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int res = select(_socket + 1, &readfds, &writefds, &excfds, &tv);
    
    retain();
    
    if (res > 0) {
        if (FD_ISSET(_socket, &excfds)) {
            close();
            (*_delegate)(this, CSSocketEventError);
            goto exit;
        }
        if (FD_ISSET(_socket, &writefds)) {
            if (_connect == ConnectWaiting) {
                int optType;
                socklen_t optLen = sizeof(optType);
                if (getsockopt(_socket, SOL_SOCKET, SO_TYPE, (char*)&optType, &optLen) == 0) {
                    _connect = ConnectDone;
                    _readElapsed = _writeElapsed = current;
                    (*_delegate)(this, CSSocketEventConnected);
                }
                else {
                    close();
                    (*_delegate)(this, CSSocketEventError);
                    goto exit;
                }
            }
            else if (_send) {
                _send = false;
                _writeElapsed = current;
                (*_delegate)(this, CSSocketEventSended);
            }
        }
        if (_socket == INVALID_SOCKET) {
            goto exit;
        }
        if (FD_ISSET(_socket, &readfds)) {
            if (!_receive) {
                _receive = true;
                _readElapsed = current;
                (*_delegate)(this, CSSocketEventReceived);
            }
        }
        if (_socket == INVALID_SOCKET) {
            goto exit;
        }
    }
    else if (res < 0) {
        close();
        (*_delegate)(this, CSSocketEventError);
        goto exit;
    }
    
    if (current > _readElapsed + _readTimeout) {
        close();
        (*_delegate)(this, CSSocketEventError);
    }
    else if (_connect == ConnectDone && current > _writeElapsed + _writeTimeout) {
        (*_delegate)(this, CSSocketEventTimeout);
    }
    
exit:
    release();
}

static bool isWouldBlock() {
#ifdef CDK_WINDOWS
	return WSAGetLastError() == WSAEWOULDBLOCK;
#else
	return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}

int CSSocket::read(CSBuffer* buffer) {
    if (_socket == INVALID_SOCKET || !_receive) {
        return 0;
    }
    _receive = false;
    
    int totalReadLen = 0;
    buffer->shrink();
    
    char bytes[1024];

    for (;;) {
        int readLen = recv(_socket, bytes, sizeof(bytes), RecvFlags);
        
        if (readLen > 0) {
            buffer->write(bytes, readLen);
            totalReadLen += readLen;
        }
		else if (readLen < 0 && isWouldBlock()) {
			break;
		}
        else {
            close();
            (*_delegate)(this, CSSocketEventError);
            return 0;
        }
    }
    return totalReadLen;
}

int CSSocket::write(CSBuffer* buffer) {
    if (_socket == INVALID_SOCKET) {
        return 0;
    }
    int totalWriteLen = 0;
    
    for (;;) {
        if (buffer->remaining() == 0) {
            buffer->clear();
            break;
        }
        else {
            _send = true;
            
            int writeLen = send(_socket, (const char*)buffer->bytes(), buffer->remaining(), SendFlags);
            
            if (writeLen > 0) {
                buffer->setPosition(buffer->position() + writeLen);
                totalWriteLen += writeLen;
            }
			else if (writeLen == 0 || isWouldBlock()) {
				break;
			}
            else {
                close();
                (*_delegate)(this, CSSocketEventError);
                return 0;
            }
        }
    }
    return totalWriteLen;
}

void CSSocket::close() {
    if (_socket >= 0) {
        if (_task) {
            _task->stop();
            _task = NULL;
        }
        //::shutdown(_socket, SHUT_RDWR);
        closesocket(_socket);
        _socket = INVALID_SOCKET;
        _connect = ConnectNone;
        _receive = false;
        _send = false;
    }
}

