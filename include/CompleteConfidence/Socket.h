/* Copyright (c) left
** FileName:		Socket.h
** Version:			2.0.1
** Update time:		2017-12-24
*/


#ifndef _LEFT_SOCKET_CURRENCY_H_
#define _LEFT_SOCKET_CURRENCY_H_

// CompleteConfidence defines
#ifdef _WIN32
#define CC_OS_WIN
#endif
#ifdef __linux__
#define CC_OS_LINUX
#endif

#define CC_ERROR				unsigned long

#define CC_SUCCESS				0x0L
#define CCERROR_LEN				0x1L
#define CCERROR_ARGVS			0x2L
#define CCERROR_ACCIDENT		0x3L
#define CCERROR_PACKET_VERIFY	0x4L

/* Socket Errors */
#define CCERROR_SOCKET_CREATE	0xFFFF0001L
#define CCERROR_SOCKET_CONNECT	0xFFFF0002L
#define CCERROR_SOCKET_SEND		0xFFFF0003L
#define CCERROR_SOCKET_RECV		0xFFFF0004L
#define CCERROR_SOCKET_BIND		0xFFFF0005L
#define CCERROR_SOCKET_LISTEN	0xFFFF0006L
#define CCERROR_SOCKET_OPT		0xFFFF0007L


class _LEFT_SOCKET_CURRENCY {
protected:
    _LEFT_SOCKET_CURRENCY() {}
    //virtual int InitializeSocket() = 0;
    //virtual void CleanSocket() = 0;
    short						port;
    long						ip;
};

#ifdef CC_OS_WIN
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")  
#define IPFromAddr(addr)		&addr.sin_addr.S_un
#define LeftCloseSocket			closesocket
class LeftSocketWin :public _LEFT_SOCKET_CURRENCY {
public:
    static int	InitializeSocket() {
        WSADATA	wsa; return WSAStartup(MAKEWORD(2, 2), &wsa);
    }
    static void CleanSocket() { WSACleanup(); }
};
typedef LeftSocketWin			LeftSocket;
typedef SOCKET					LeftSokt;
typedef int						LeftSoktLen;
#endif
#ifdef CC_OS_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET_ERROR			-1
#define INVALID_SOCKET			-1
#define IPFromAddr(addr)		&addr.sin_addr
#define LeftCloseSocket			close
class LeftSocketLinux :public _LEFT_SOCKET_CURRENCY {
public:
    static int	InitializeSocket() { return 0; }
    static void CleanSocket() {  }
};
typedef LeftSocketLinux			LeftSocket;
typedef int						LeftSokt;
typedef socklen_t				LeftSoktLen;
#endif

#endif


