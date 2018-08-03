/* Copyright (c) left
** FileName:		WebSocket.cpp
** Version:			2.1.0
** Update time:		2018-07-11
*/

#include "WebSocket.h"
#include <stdio.h>
#include <memory.h>
#ifdef CC_OS_LINUX
#include <arpa/inet.h>
#endif

#define PER_OF_HTTP "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "
#define SUF_OF_HTTP "\r\nUpgrade: websocket\r\n\r\n"

#include "sha1.h"
#include "base64.h"
#include "easylogging/easylogging++.h"

namespace CCWebSocket {

    char *CountMagicStr(char *AskKey, char *AnswerKey, int Len) {
        if (strlen(AskKey) != WEBSOCKET_KEY_LEN)
            return NULL;
        char request[WEBSOCKET_KEY_LEN + MAGIC_STR_LEN + 1];
        memcpy(request, AskKey, WEBSOCKET_KEY_LEN);
        memcpy(request + WEBSOCKET_KEY_LEN, MAGIC_STR, MAGIC_STR_LEN);
        request[WEBSOCKET_KEY_LEN + MAGIC_STR_LEN] = '\0';
        SHA1 sha;
        unsigned int message_digest[5];
        sha.Reset();
        sha << request;
        sha.Result(message_digest);
        for (int i = 0; i < 5; i++)
            message_digest[i] = htonl(message_digest[i]);
        std::string base20 = base64_encode(
            reinterpret_cast<const unsigned char*>(message_digest), 20);
        snprintf(AnswerKey, Len, base20.c_str());
        return AnswerKey;
    }

    char *GetAnswerStr(char *PacketAsk, char *PacketAnswer, int Len) {
        char *webKey = (char*)strstr(PacketAsk, "Sec-WebSocket-Key");
        if (webKey == NULL) return NULL;
        char AskKey[WEBSOCKET_KEY_LEN + 1];
        char AnswerKey[128];
        memset(AskKey, 0, sizeof(AskKey));
        memset(AnswerKey, 0, sizeof(AnswerKey));
        memcpy(AskKey, webKey + sizeof("Sec-WebSocket-Key:"), WEBSOCKET_KEY_LEN);
        memset(PacketAnswer, 0, Len);
        snprintf(PacketAnswer, Len, "%s%s%s", PER_OF_HTTP,
            CountMagicStr(AskKey, AnswerKey, sizeof(AnswerKey)), SUF_OF_HTTP);
        return PacketAnswer;
    }

    char *DecipheringStr(
        const char *CipherText, int TextLen, char key[], char *DecipheringStr, int DhStrLen) {
        if (DhStrLen <= TextLen)
            return NULL;
        DecipheringStr[TextLen] = '\0';
        memcpy(DecipheringStr, CipherText, TextLen);
        for (int i = 0; i < TextLen; i++)
            DecipheringStr[i] = CipherText[i] ^ key[i % 4];
        return DecipheringStr;
    }

    char *PackageAddHead(
        char *str, int strLen, char *answer, int answerLen) {
        if (strLen < 0) return NULL;
        memset(answer, 0, answerLen);
        unsigned int off = 0;
        answer[off] = (char)0x81;
        off++;
        if (strLen < 126)
        {
            answer[off++] = strLen;
            answer[strLen + 2 + 4 + 8 + 1] = '\0';
        }
        else if (strLen > 125 && strLen < 0xFFFF) 
        {
            answer[off++] = 126;
            answer[off++] = (strLen & 0xFF00) / 0x100;
            answer[off++] = (strLen & 0xFF);
            answer[strLen + 2 + 4 + 8 + 3] = '\0';
        }
        else if (strLen > 0xFFFF) 
        {
            answer[off++] = 127;
        }
        memcpy(answer + off, str, strLen);
        return answer;
    }

    char *ReadPacket(char *buf, int bufLen, char *answer, int answerLen) {
        webSkt skt;
        short header = *(short*)buf;
        char DecipheringStr[1024];
        memset(&skt, 0, sizeof(webSkt));
        skt.wordOff = 0;
        skt.wordOff += 2;
        if (header & 0x80) skt.FIN = true;
        else skt.FIN = false;
        skt.Opcode = header & 0xF;
        if (header & 0x8000) {
            skt.Mask = true;
            skt.wordOff += 4;
        }
        else skt.Mask = false;
        skt.PaylodLen = (header & 0x7F00) >> 8;
        if (skt.PaylodLen == 126) {
            skt.PaylodLen = *(short*)(buf + 2);
            skt.PaylodLen = (skt.PaylodLen & 0xFF00) / 0x100
                + (skt.PaylodLen & 0xFF) * 0x100;
            if (skt.Mask) {
                skt.Masking_key[0] = *(buf + 4);
                skt.Masking_key[1] = *(buf + 5);
                skt.Masking_key[2] = *(buf + 6);
                skt.Masking_key[3] = *(buf + 7);
            };
            skt.wordOff += 2;
        }
        else if (skt.PaylodLen == 127) {
        }
        else {
            skt.Masking_key[0] = *(buf + 2);
            skt.Masking_key[1] = *(buf + 3);
            skt.Masking_key[2] = *(buf + 4);
            skt.Masking_key[3] = *(buf + 5);
        }
        if (skt.PaylodLen > bufLen || skt.Opcode != 1 || skt.FIN == false)
            return NULL;
        if (skt.Mask && skt.PaylodLen) {
            LOG(INFO) << "Get Len : " << skt.PaylodLen;
            CCWebSocket::DecipheringStr(
                buf + skt.wordOff, skt.PaylodLen, skt.Masking_key,
                DecipheringStr, sizeof(DecipheringStr));
        }
        else if (!skt.Mask && skt.PaylodLen) {
            memset(DecipheringStr, 0, sizeof(DecipheringStr));
            LOG(INFO) << "Get Len : " << skt.PaylodLen;
            memcpy(DecipheringStr, buf + skt.wordOff, skt.PaylodLen);
        }
        else
            return NULL;
        //snprintf(answer, answerLen, DecipheringStr);
        memcpy(answer, DecipheringStr, skt.PaylodLen <= answerLen ? skt.PaylodLen : answerLen);
        answer[skt.PaylodLen < answerLen ? skt.PaylodLen : answerLen - 1] = '\0';
        return answer;
    }

}
