/* Copyright (c) left
** FileName:		Mycodes.cpp
** Version:			1.3.2
** Update time:		2018-1-9
*/

#include "MyCodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef LEFT_OS_WIN
#include <ShlObj.h>
#endif

unsigned long long hash_(char const *str) {
    unsigned long long ret{ basis };
    while (*str) { ret ^= *str; ret *= prime; str++; }
    return ret;
}

namespace leftName {
#ifdef LEFT_OS_WIN

    void ShowLastError() {
        HLOCAL lpMsgBuf;
        setlocale(LC_ALL, "");
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(),
            MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED),
            (LPTSTR)&lpMsgBuf, 0, NULL);
        std::wcout << (WCHAR*)lpMsgBuf << std::endl;
        LocalFree(lpMsgBuf);
    }

    void GetToken() {
        if (IsUserAnAdmin())
            return;
        else {
            SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
            sei.lpVerb = TEXT("runas");
            WCHAR path[512] = { 0 };
            GetModuleFileName(NULL, (LPSTR)path, sizeof(path));
            sei.lpFile = (LPSTR)path;
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteEx(&sei);
            exit(0);
        }
    }

    HMODULE GetSelfModuleHandle() {
        MEMORY_BASIC_INFORMATION mbi;
        return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
            ? (HMODULE)mbi.AllocationBase : NULL);
    }
#endif

    char *GetTimeStr(char *TimeStr, int SizeOfBuf) {
        return GetTimeStr(TimeStr, SizeOfBuf, 0);
    }

    char *GetTimeStr(char *TimeStr, int SizeOfBuf, int off) {
        time_t tt = time(NULL) + off;
#ifdef LEFT_OS_WIN
        tm tmt;
        tm* t = &tmt;
        localtime_s(t, &tt);
#endif
#ifdef LEFT_OS_LINUX
        tm* t = localtime(&tt);
#endif
        snprintf(TimeStr, SizeOfBuf, "%d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
        return TimeStr;
    }

    char *StrToHex(const char *buf, char *Answer, int LenOfAnswer) {
        if ((int)strlen(buf) % 2 != 0 || (int)strlen(buf) / 2 >= LenOfAnswer)
            return NULL;
        for (size_t i = 0; i < strlen(buf) / 2; i++) {
            if (buf[i * 2] >= '0' && buf[i * 2] <= '9') {
                Answer[i] = (buf[i * 2] - '0') * 0x10;
                if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9')
                    Answer[i] += buf[i * 2 + 1] - '0';
                else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'F')
                    Answer[i] += buf[i * 2 + 1] - 'A' + 0xA;
                else if (buf[i * 2 + 1] >= 'a' && buf[i * 2 + 1] <= 'f')
                    Answer[i] += buf[i * 2 + 1] - 'a' + 0xA;
                else
                    break;
            }
            else if (buf[i * 2] >= 'A' && buf[i * 2] <= 'F') {
                Answer[i] = (buf[i * 2] - 'A') * 0x10 + 0xA0;
                if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9')
                    Answer[i] += buf[i * 2 + 1] - '0';
                else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'F')
                    Answer[i] += buf[i * 2 + 1] - 'A' + 0xA;
                else if (buf[i * 2 + 1] >= 'a' && buf[i * 2 + 1] <= 'f')
                    Answer[i] += buf[i * 2 + 1] - 'a' + 0xA;
                else
                    break;
            }
            else if (buf[i * 2] >= 'a' && buf[i * 2] <= 'f') {
                Answer[i] = (buf[i * 2] - 'a') * 0x10 + 0xA0;
                if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9')
                    Answer[i] += buf[i * 2 + 1] - '0';
                else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'F')
                    Answer[i] += buf[i * 2 + 1] - 'A' + 0xA;
                else if (buf[i * 2 + 1] >= 'a' && buf[i * 2 + 1] <= 'f')
                    Answer[i] += buf[i * 2 + 1] - 'a' + 0xA;
                else
                    break;
            }
            else
                break;
            if (i == (strlen(buf) / 2 - 1)) {
                Answer[LenOfAnswer - 1] = '\0';
                return Answer;
            }
        }
        return NULL;
    }

    char *HexToStr(
        const char *order, int LenOfOrder, char *Answer, int LenOfAnswer) {
        if (LenOfOrder * 2 >= LenOfAnswer)
            return NULL;
        char low, high;
        for (int i = 0; i < LenOfOrder; i++) {
            high = (order[i] & 0xF0) / 0x10;
            if (high >= 0 && high <= 9)
                Answer[i * 2] = high + '0';
            else
                Answer[i * 2] = high + 'A' - 0xA;
            low = order[i] & 0xF;
            if (low >= 0 && low <= 9)
                Answer[i * 2 + 1] = low + '0';
            else
                Answer[i * 2 + 1] = low + 'A' - 0xA;
        }
        Answer[LenOfOrder * 2] = '\0';
        return Answer;
    }

    LEFT_ERROR AnalysisIniFile(char *path, pIniInfo *InfoHead) {
        if (!path || !InfoHead)
            return LEFT_ERROR_ARGVS;
        *InfoHead = new IniInfo;
        memset(*InfoHead, 0, sizeof(IniInfo));
        pIniInfo fake = *InfoHead;
        std::ifstream ini(path, std::ios::in);
        if (!ini.is_open())
            return LEFT_ERROR_IO_FILEOPEN;
        char words[MAX_INI_INFO_LEN] = { 0 };
        while (ini.getline(words, sizeof(words) - 1)) {
            std::cout << words << std::endl;
            char* cut = strstr(words, "=");
            if (!cut) continue;
            *cut = '\0';
            char* key = new char[strlen(words) + 1];
            char* value = new char[strlen(cut + 1) + 1];
            snprintf(key, strlen(words) + 1, words);
            snprintf(value, strlen(cut + 1) + 1, cut + 1);
            pIniInfo p = new IniInfo;
            fake->Next = p;
            p->Next = NULL;
            p->Key = key;
            p->Value = value;
            fake = fake->Next;
            std::cout << "Check info >>> " << p->Key << ":" << p->Value << std::endl;
        }
        ini.close();
        return LEFT_SUCCESS;
    }

    LEFT_ERROR GetIniInfo(
        const char *key, char *ValueBuf, int ValueLen, pIniInfo InfoHead) {
        if (!key || !ValueBuf || !InfoHead)
            return LEFT_ERROR_ARGVS;
        pIniInfo info = InfoHead;
        while (info) {
            if (info->Key && !strcmp(info->Key, key) && info->Value) {
                if (ValueLen <= (int)strlen(info->Value))
                    return LEFT_ERROR_LEN;
                else {
                    snprintf(ValueBuf, ValueLen, info->Value);
                    return LEFT_SUCCESS;
                }
            }
            info = info->Next;
        }
        return LEFT_ERROR_INIINFO_NOFOUND;
    }

    LEFT_ERROR DelIniInfoList(pIniInfo infoList) {
        do {
            if (infoList->Key) delete[] infoList->Key;
            if (infoList->Value) delete[] infoList->Value;
            pIniInfo p = infoList->Next;
            delete[] infoList;
            infoList = p;
        } while (infoList);
        return LEFT_SUCCESS;
    }

    char* getNextOrder(char **buf, char *order, int Len, char SignChar) {
        char CutStr[2] = { SignChar, '\0' };
        if ((int)strlen(*buf) <= 1 && (int)strlen(*buf) >= Len)
            return NULL;
        char* head = strstr(*buf, CutStr);
        if (!head || head[1] == '\0') return NULL;
        char* last = strstr(head + 1, CutStr);
        if (!last) return NULL;
        size_t len = last - head;
        memset(order, 0, Len);
        memcpy(order, head + 1, len - 1);
        order[len] = '\0';
        *buf += len;
        return order;
    }

    LEFT_ERROR AddSign(const char *orderBuff, char *buff, char SignChar) {
        if (orderBuff == NULL || buff == NULL)
            return LEFT_ERROR_ARGVS;
        size_t firstLen = strlen(orderBuff);
        int j = 0;
        if (orderBuff[j] != SignChar) {
            buff[j++] = SignChar;
            firstLen++;
        }
        for (size_t i = 0; i < strlen(orderBuff); i++) {
            while (orderBuff[i + 1] == SignChar && orderBuff[i] == SignChar
                && i < strlen(orderBuff) - 1) {
                i++;
                firstLen--;
            }
            buff[j++] = orderBuff[i];
        }
        if (buff[firstLen - 1] != SignChar)
            buff[firstLen++] = SignChar;
        buff[firstLen] = '\0';
        return LEFT_SUCCESS;
    }

    ThrSfeList::ThrSfeList() :
        buf(NULL),
        len(0),
        role(Head),
        Last(NULL),
        Next(this) {
        LeftInitSimpleLock(&lock);
    }

    ThrSfeList::ThrSfeList(char *buf, unsigned int len, LeftSimpleLock lock,
        ThrSfeList *Last, ThrSfeList *Next) :
        role(boddy),
        Last(Last),
        Next(Next),
        lock(lock) {
        LeftGetSimpleLock(&lock);
        if (len && buf) {
            this->buf = new char[len];
            this->len = len;
            memcpy(this->buf, buf, len);
        }
        if (Last)
            Last->Next = this;
        if (Next)
            Next->Last = this;
        LeftGiveSimpleLock(&lock);
    }

    ThrSfeList::~ThrSfeList() {
        switch (role) {
        case Head:
            LeftDelSimpleLock(&lock);
        case boddy:
            delete[] buf;
        case Tail:
            break;
        }
        if (Last)
            Last->Next = Next;
        if (Next)
            Next->Last = Last;
    }

    unsigned int ThrSfeList::GetLast(char *buf, unsigned int len) {
        if (!buf || !Last || Last->role != boddy || len < Last->len)
            return 0;
        unsigned int rt = Last->len;
        LeftGetSimpleLock(&lock);
        memcpy(buf, Last->buf, Last->len);
        LeftGiveSimpleLock(&lock);
        delete Last;
        return rt;
    }

    char *ThrSfeList::GetBuf(char *buf, unsigned int len) {
        if (!buf || len < this->len)
            return NULL;
        LeftGetSimpleLock(&lock);
        memcpy(buf, this->buf, this->len);
        LeftGiveSimpleLock(&lock);
        return buf;
    }

    bool ThrSfeList::Add(char *buf, unsigned int len) {
        if (!buf || !len)
            return false;
        new ThrSfeList(buf, len, lock, this, Next);
        return true;
    }

}

