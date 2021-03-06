﻿
#ifndef LEFT_COMPLETE_CONFIDENCE_DEFINES_H_
#define LEFT_COMPLETE_CONFIDENCE_DEFINES_H_

#ifdef _WIN32
#define CC_OS_WIN
#endif
#ifdef __linux__
#define CC_OS_LINUX				0x1991
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


#endif
