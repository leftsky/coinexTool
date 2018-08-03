/* Copyright (c) left
** FileName:		Mycodes.h
** Version:			1.3.2
** Update time:		2018-1-9
*/

#ifndef _LEFT_MYCODES_CURRENCY_H_
#define _LEFT_MYCODES_CURRENCY_H_

#ifdef _WIN32
#define LEFT_OS_WIN
#endif
#ifdef __linux__
#define LEFT_OS_LINUX
#endif

/* Error definition */
#define LEFT_ERROR					unsigned long
#define LEFT_SUCCESS				0x0L
#define LEFT_ERROR_LEN				0x1L
#define LEFT_ERROR_ARGVS			0x2L
#define LEFT_ERROR_ACCIDENT			0x3L
// File IO error
#define LEFT_ERROR_IO_FILEOPEN		0xFFFE0001L
// Ini errors
#define MAX_INI_INFO_LEN			128
#define LEFT_ERROR_INIINFO_NOFOUND	0xFFFD0001L

#define NONE4100(p) (p)

/***************************************************************************************/
/* Currency support */
/* Thread */
#ifdef LEFT_OS_WIN
#include <Windows.h>
#define CURRENCY_StartThread(thr, arg, id) \
									CreateThread(NULL, NULL, thr, arg, NULL, id)
//typedef DWORD _stdcall	LeftThrReturn;
#define EndThread
#define LeftThrReturn				DWORD _stdcall
typedef LPVOID						LeftThrArgv;
typedef LPDWORD						LeftThrNo;
#endif
#ifdef LEFT_OS_LINUX
#include <pthread.h>
#define CURRENCY_StartThread(thr, arg, id) \
									pthread_create(&id, NULL, thr, arg)
#define EndThread					pthread_detach(pthread_self())
#define LeftThrReturn				void*
#define LeftThrArgv					void*
typedef pthread_t					LeftThrNo;
#endif
/* Lock */
#ifdef LEFT_OS_WIN
typedef CRITICAL_SECTION			LeftSimpleLock;
#define LeftInitSimpleLock(lock)	InitializeCriticalSection(lock)
#define	LeftDelSimpleLock(lock)		DeleteCriticalSection(lock)
#define LeftGetSimpleLock(lock)		EnterCriticalSection(lock)
#define LeftGiveSimpleLock(lock)	LeaveCriticalSection(lock)
#endif
#ifdef LEFT_OS_LINUX
#include <pthread.h> 
typedef pthread_mutex_t				LeftSimpleLock;
#define LeftInitSimpleLock(lock)	pthread_mutex_init(lock, NULL)
#define	LeftDelSimpleLock(lock)		pthread_mutex_destroy(lock)
#define LeftGetSimpleLock(lock)		pthread_mutex_lock(lock)
#define LeftGiveSimpleLock(lock)	pthread_mutex_unlock(lock)
#endif
/* Sleep */
#ifdef LEFT_OS_WIN
#include <Windows.h>
#define LeftSleep(n) Sleep(n)
#endif
#ifdef LEFT_OS_LINUX
#include <unistd.h>
#define LeftSleep(n) usleep(n * 1000)
#endif	
/* GetSelfPath */
#ifdef LEFT_OS_WIN
#define GetSelfModulePath(path,len)	GetModuleFileName(NULL, path, len); \
									*(strrchr(path, '\\') + 1) = 0
#endif
#ifdef LEFT_OS_LINUX
#define GetSelfModulePath(path,len)	getcwd(path,len)
#endif
/***************************************************************************************/

/***************************************************************************************/
/* Operator sign to change string to hash. */
constexpr unsigned long long prime = 0x100000001B3ull;
constexpr unsigned long long basis = 0xCBF29CE484222325ull;
// Funciont support.
unsigned long long hash_(char const *str);
constexpr unsigned long long hash_compile_time(
    char const *str, unsigned long long last_value = basis) {
    return *str ? hash_compile_time(str + 1, (*str ^ last_value) *prime) : last_value;
}
constexpr unsigned long long operator"" _hash(char const *p, size_t) {
    return hash_compile_time(p);
}
/***************************************************************************************/

/***************************************************************************************/
/* Point the path,it will out file and screem both */
#include <fstream>
#include <iostream>
class CoutExA {
public:
    CoutExA(char const *filename) :m_fout(filename, std::ios::out | std::ios::app) {}
    template <class T>
    CoutExA& operator<<(const T &text) {
        m_fout << text; std::cout << text; return *this;
    }
    CoutExA& operator<<(std::basic_ostream<char>& (*fp)(std::basic_ostream<char>&)) {
        m_fout << fp; std::cout << fp; return *this;
    }
private:
    std::ofstream m_fout;
};
#ifdef LEFT_OS_WIN
class CoutExW {
public:
    CoutExW(const wchar_t *filename) :m_wfout(filename, std::wios::out | std::wios::app) {
        m_wfout.imbue(std::locale(std::locale("chs"), "", LC_CTYPE));
    }
    template <class T>
    CoutExW& operator<<(const T &text) {
        m_wfout << text; std::wcout << text; return *this;
    }
    CoutExW& operator<<(std::basic_ostream<wchar_t>& (*fp)(std::basic_ostream<wchar_t>&)) {
        m_wfout << fp; std::wcout << fp; return *this;
    }
private:
    std::wofstream m_wfout;
};
#endif
/***************************************************************************************/

// leftName namespace
namespace leftName {
#ifdef LEFT_OS_WIN
    // Show last error message in chinese.
    extern void ShowLastError();
    // Get the token.
    extern void GetToken();
    // Get self module handle.
    extern HMODULE GetSelfModuleHandle();
#endif
    // GetTimeStr(Y-M-D h:m:s)
    char *GetTimeStr(char *TimeStr, int SizeOfBuf);
    char *GetTimeStr(char *TimeStr, int SizeOfBuf, int off);
    // Converts a string command to a half-length hexadecimal string.
    char *StrToHex(const char *buf, char *Answer, int LenOfAnswer);
    // Convert a hexadecimal string to double the length of a string.
    char *HexToStr(
        const char *order, int LenOfOrder, char *Answer, int LenOfAnswer);
    // Analysis ini file. Just give a pointer to init function.
    typedef struct _INI_INFO_LIST {
        _INI_INFO_LIST		*Next;
        char				*Key;
        char				*Value;
    } IniInfo, *pIniInfo;
    LEFT_ERROR AnalysisIniFile(char *path, pIniInfo *InfoHead);
    LEFT_ERROR GetIniInfo(
        const char *key, char *ValueBuf, int ValueLen, pIniInfo InfoHead);
    LEFT_ERROR DelIniInfoList(pIniInfo infoList);
    // Cut the str with interval character.
    char *getNextOrder(char **buf, char *order, int Len, char SignChar);
    LEFT_ERROR AddSign(const char *orderBuff, char *buff, char SignChar);
    // Thread safe list class
    class ThrSfeList {
    public:
        ThrSfeList();
        ThrSfeList(char *buf, unsigned int len, LeftSimpleLock lock,
            ThrSfeList *Last, ThrSfeList *Next);
        ~ThrSfeList();
        unsigned int		GetLast(char *buf, unsigned int len);
        char				*GetBuf(char *buf, unsigned int len);
        bool				Add(char *buf, unsigned int len);
        unsigned int		len;
    private:
        LeftSimpleLock		lock;
        char				*buf;
        enum				character { Head, boddy, Tail };
        character			role;
        ThrSfeList			*Last;
        ThrSfeList			*Next;
    };
}
#endif
/***************************************************************************************/

