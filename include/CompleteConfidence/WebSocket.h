/* Copyright (c) left
** FileName:		WebSocket.h
** Version:			2.0.1
** Update time:		2017-12-24
*/

#ifndef _LEFT_WEBSOCKET_H
#define _LEFT_WEBSOCKET_H


#define MAGIC_STR "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define MAGIC_STR_LEN 37
#define WEBSOCKET_KEY_LEN 24

#include "Socket.h"

namespace CCWebSocket {
    typedef struct _WEBSOCKET_HEAD_STRUCT {
        bool FIN;
        unsigned short Opcode;
        bool Mask;
        unsigned short PaylodLen;
        char Masking_key[4];
        unsigned int moreLenHigh;
        unsigned int moreLenLow;
        int wordOff;
    } webSkt, *pwebSkt;
    //char *CountMagicStr(
    //	char *AskKey, char *AnswerKey, int Len);
    char *GetAnswerStr(char *PacketAsk, char *PacketAnswer, int Len);
    //char *DecipheringStr(const char *CipherText, int TextLen,
    //	char key[], char *DecipheringStr, int DhStrLen);
    char *PackageAddHead(char *str, int strLen, char *answer, int answerLen);
    char *ReadPacket(char *buf, int bufLen, char *answer, int answerLen);
};

#endif