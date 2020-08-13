/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* 文件名称: TraceLog.h
* 文件摘要: 调试日志
*
* 当前版本: 1.0，税控研发组，2013-12-16
*            a) 创建并实现
*
* 历史版本: 
*/

#ifndef _TRACELOG_H_
#define _TRACELOG_H_
#if PATH
extern char sLogFile[MAX_PATH];
#endif

class CTraceLog
{
public:
    CTraceLog();
    ~CTraceLog();

public:
    void InitLogPath(const char *path);
    void CleanPathFile(const char *path);
    void AddLogInfo(const char *pcHead, const char *pcData);

protected:
    //日志路径
    char m_acLogPath[MAX_PATH+1];
    //设置一个临界区
    CRITICAL_SECTION m_crit;
    //文件数量
    enum {LOG_FILE_SIZE = 30};
};

//////////////////////////////////////////////////////////////////////////////////////////

void TRACELOG_STR(const char *pcPath, const char *pcInfo, const char *pcBuf);
void TRACELOG_HEX(const char *pcPath, const char *pcInfo, BYTE *pbBuf, DWORD dwLen);
void TRACELOG_U32(const char *pcPath, const char *pcInfo, DWORD dwValue);

void TRACELOG_STR_HD(const char *pcPath, HANDLE hdFileName, const char *pcInfo, const char *pcBuf);
void TRACELOG_HEX_HD(const char *pcPath, HANDLE hdFileName, const char *pcInfo, BYTE *pbBuf, DWORD dwLen);
void TRACELOG_U32_HD(const char *pcPath, HANDLE hdFileName, const char *pcInfo, DWORD dwValue);

//////////////////////////////////////////////////////////////////////////////////////////

#define TRACELOG_ENABLE                         1

#if TRACELOG_ENABLE
#if PATH
#define TRACELOG_PATH                           sLogFile
#else
#define TRACELOG_PATH                           "_SKSClog"//CONFIG_PATH	
#endif
#define DEBUG_TRACELOG_STR(a,b)                 TRACELOG_STR(TRACELOG_PATH,a,b) 
#define DEBUG_TRACELOG_HEX(a,b,c)               TRACELOG_HEX(TRACELOG_PATH,a,b,c) 
#define DEBUG_TRACELOG_U32(a,b)                 TRACELOG_U32(TRACELOG_PATH,a,b) 

#define DEBUG_TRACELOG_STR_HD(a,b,c)            TRACELOG_STR_HD(TRACELOG_PATH,a,b,c) 
#define DEBUG_TRACELOG_HEX_HD(a,b,c,d)          TRACELOG_HEX_HD(TRACELOG_PATH,a,b,c,d) 
#define DEBUG_TRACELOG_U32_HD(a,b,c)            TRACELOG_U32_HD(TRACELOG_PATH,a,b,c) 

#else

#define DEBUG_TRACELOG_STR(a,b)
#define DEBUG_TRACELOG_HEX(a,b,c)
#define DEBUG_TRACELOG_U32(a,b)

#define DEBUG_TRACELOG_STR_HD(a,b,c)
#define DEBUG_TRACELOG_HEX_HD(a,b,c,d)
#define DEBUG_TRACELOG_U32_HD(a,b,c)

#endif

//////////////////////////////////////////////////////////////////////////////////////////

#endif
