/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* 文件名称: TraceLog.cpp
* 文件摘要: 调试日志
*
* 当前版本: 1.0，税控研发组，2013-12-16
*            a) 创建并实现
*
* 历史版本: 
*/

#include "../../pch.h"
#include <time.h>
#include "TraceLog.h"

/*
* 函数名称: TCLib_Asc2Hex
* 函数功能: 16进制转化为ASCII
* 参    数:
*            pbBin        - 字节流缓存
*            pcAsc        - 字符流缓存
*            dwLen        - 数据长度
* 返 回 值: NO
*/
void TCLib_Hex2Asc(IN const BYTE *pbBin, IN DWORD dwLen, OUT char *pcAsc)
{
    BYTE *pbP;
    BYTE *pbQ;

    pbP = (BYTE *)pcAsc;
    pbQ = (BYTE *)pbBin;

    while(dwLen--)
    {
        *pbP = ((*pbQ) & 0xF0) >> 4;
        if (*pbP >= 10)
        {
            *pbP += ('A' - 10);
        }
        else
        {
            *pbP += '0';
        }

        pbP++;
        *pbP = (*pbQ) & 0x0F;
        if(*pbP >= 10)
        {
            *pbP += ('A' - 10);
        }
        else
        {
            *pbP += '0';
        }
        pbP++;
        pbQ++;
    }

    *pbP = 0;

    return;
}

//构造函数，设置日志文件的默认路径
CTraceLog::CTraceLog() 
{
    //初始化临界区
    ::InitializeCriticalSection(&m_crit);
}

CTraceLog::~CTraceLog()
{
    //释放里临界区
    ::DeleteCriticalSection(&m_crit);
}

void CTraceLog::InitLogPath(const char *path)
{
	strcpy_s(m_acLogPath, path);
}

void CTraceLog::CleanPathFile(const char *path)
{
    /*-----------------------进入临界区(写文件)------------- -------------*/ 
    ::EnterCriticalSection(&m_crit);

    char acPathName[MAX_PATH+1] = {0};
    DWORD dwFileCount = 0;
    DWORD dwWhileCount = 0;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA sFindData;
  
    sprintf_s(acPathName, MAX_PATH+1, "%s\\*.log", path);
    hFind = FindFirstFile(acPathName, &sFindData);  
    if(INVALID_HANDLE_VALUE != hFind) 
    {
        dwWhileCount = 1000;
        while(dwWhileCount--) 
        {
			if(0 == dwFileCount)
            {
                sprintf_s(acPathName, MAX_PATH+1, "%s\\%s", path, sFindData.cFileName);
            }
            if(FALSE == FindNextFile(hFind, &sFindData))
            {
                break;
            }
            if(FILE_ATTRIBUTE_DIRECTORY == (sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                continue;
            }
            dwFileCount += 1;
        }

        //超过删除首个文件
        if(dwFileCount > LOG_FILE_SIZE)
        {
            DeleteFile(acPathName); 
        }

        FindClose(hFind);    
    }

    ::LeaveCriticalSection(&m_crit); 
    /*-------------------退出临界区----------------------------------------*/ 
}

void CTraceLog::AddLogInfo(const char *pcHead, const char *pcData)
{
    if(strlen(m_acLogPath) <= 0) return;

    /*-----------------------进入临界区(写文件)------------- -------------*/ 
    ::EnterCriticalSection(&m_crit);
    
    //以添加的方式输出到文件
	FILE *fp = NULL;
    fopen_s(&fp, m_acLogPath, "a+");
    if(fp)
    {
        //当前时间
		SYSTEMTIME time1;
		GetLocalTime(&time1);
		fprintf(fp, "%04d-%02d-%02d ", time1.wYear, time1.wMonth, time1.wDay);
		fprintf(fp, "%02d:%02d:%02d ", time1.wHour, time1.wMinute, time1.wSecond);
        //线程号
        DWORD dwThreadId = GetCurrentThreadId();
        fprintf(fp, "%08x ", (unsigned int)dwThreadId);
        //日志信息
        fprintf(fp, "%s : ", pcHead);
        fprintf(fp, "%s\n", pcData);
        //关闭文件
        fclose(fp);
    }
    
    ::LeaveCriticalSection(&m_crit); 
    /*-------------------退出临界区----------------------------------------*/ 
}

//////////////////////////////////////////////////////////////////////////////////////////

void TRACELOG_STR(const char *pcPath, const char *pcInfo, const char *pcBuf)
{
    if(GetFileAttributes(pcPath) == 0xffffffff)
    {
        CreateDirectory(pcPath, NULL);
    }

	SYSTEMTIME time1;
	GetLocalTime(&time1);
    char acPathName[MAX_PATH] = {0};
    sprintf_s(acPathName, MAX_PATH, "%s\\_%04d-%02d-%02d_.log", pcPath, time1.wYear, time1.wMonth, time1.wDay);
 
    CTraceLog m_CTraceLog;
    m_CTraceLog.CleanPathFile(pcPath);
    m_CTraceLog.InitLogPath(acPathName);
    
    if((NULL != pcInfo) && (NULL != pcBuf))
    {
        m_CTraceLog.AddLogInfo(pcInfo, pcBuf);
    }        
    else
    {
        m_CTraceLog.AddLogInfo("错误", "数据为NULL");
    }

    return;
}

void TRACELOG_HEX(const char *pcPath, const char *pcInfo, BYTE *pbBuf, DWORD dwLen)
{
    if(GetFileAttributes(pcPath) == 0xffffffff)
    {
        CreateDirectory(pcPath, NULL);
    }

    char *pcTempBuf = NULL;
    pcTempBuf = new char[2*dwLen+1];
    memset(pcTempBuf, 0x00, 2*dwLen+1);
    TCLib_Hex2Asc(pbBuf, dwLen, pcTempBuf);

	SYSTEMTIME time1;
	GetLocalTime(&time1);
    char acPathName[MAX_PATH] = {0};
    sprintf_s(acPathName, MAX_PATH, "%s\\_%04d-%02d-%02d_.log", pcPath, time1.wYear, time1.wMonth, time1.wDay);
   
    CTraceLog m_CTraceLog;
    m_CTraceLog.CleanPathFile(pcPath);
    m_CTraceLog.InitLogPath(acPathName);
    
    if((NULL != pcInfo) && (NULL != pbBuf))
    {
        m_CTraceLog.AddLogInfo(pcInfo, pcTempBuf);
    }
    else
    {
        m_CTraceLog.AddLogInfo("错误", "数据为NULL");
    }

    delete[] pcTempBuf;
    return;
}

void TRACELOG_U32(const char *pcPath, const char *pcInfo, DWORD dwValue)
{
    if(GetFileAttributes(pcPath) == 0xffffffff)
    {
        CreateDirectory(pcPath, NULL);
    }
    
    char acTempBuf[32] = {0};
    sprintf_s(acTempBuf, 32, "%08x", (unsigned int)dwValue);

	SYSTEMTIME time1;
	GetLocalTime(&time1);
    char acPathName[MAX_PATH] = {0};
    sprintf_s(acPathName, MAX_PATH, "%s\\_%04d-%02d-%02d_.log", pcPath, time1.wYear, time1.wMonth, time1.wDay);

    CTraceLog m_CTraceLog;
    m_CTraceLog.CleanPathFile(pcPath);
    m_CTraceLog.InitLogPath(acPathName);
    
    if(NULL != pcInfo)
    {
        m_CTraceLog.AddLogInfo(pcInfo, acTempBuf);
    }
    else
    {
        m_CTraceLog.AddLogInfo("错误", "数据为NULL");
    }

    return;
}

void TRACELOG_STR_HD(const char *pcPath, HANDLE hdFileName, const char *pcInfo, const char *pcBuf)
{
    if(GetFileAttributes(pcPath) == 0xffffffff)
    {
        CreateDirectory(pcPath, NULL);
    }
    
    char acPathName[MAX_PATH] = {0};
    sprintf_s(acPathName, MAX_PATH, "%s\\_%08x.log", pcPath, (unsigned int)hdFileName);

    CTraceLog m_CTraceLog;
    m_CTraceLog.CleanPathFile(pcPath);
    m_CTraceLog.InitLogPath(acPathName);
    
    if((NULL != pcInfo) && (NULL != pcBuf))
    {
        m_CTraceLog.AddLogInfo(pcInfo, pcBuf);
    }        
    else
    {
        m_CTraceLog.AddLogInfo("错误", "数据为NULL");
    }
    
    return;
}

void TRACELOG_HEX_HD(const char *pcPath, HANDLE hdFileName, const char *pcInfo, BYTE *pbBuf, DWORD dwLen)
{
    if(GetFileAttributes(pcPath) == 0xffffffff)
    {
        CreateDirectory(pcPath, NULL);
    }
    
    char *pcTempBuf = NULL;
    pcTempBuf = new char[2*dwLen+1];
    memset(pcTempBuf, 0x00, 2*dwLen+1);
    TCLib_Hex2Asc(pbBuf, dwLen, pcTempBuf);
  
    char acPathName[MAX_PATH] = {0};
    sprintf_s(acPathName, MAX_PATH, "%s\\_%08x.log", pcPath, (unsigned int)hdFileName);

    CTraceLog m_CTraceLog;
    m_CTraceLog.CleanPathFile(pcPath);
    m_CTraceLog.InitLogPath(acPathName);    
    
    if((NULL != pcInfo) && (NULL != pbBuf))
    {
        m_CTraceLog.AddLogInfo(pcInfo, pcTempBuf);
    }        
    else
    {
        m_CTraceLog.AddLogInfo("错误", "数据为NULL");
    }

    delete[] pcTempBuf;
    return;
}

void TRACELOG_U32_HD(const char *pcPath, HANDLE hdFileName, const char *pcInfo, DWORD dwValue)
{
    if(GetFileAttributes(pcPath) == 0xffffffff)
    {
        CreateDirectory(pcPath, NULL);
    }
    
    char acTempBuf[32] = {0};
    sprintf_s(acTempBuf, 32, "%08x", (unsigned int)dwValue);

    char acPathName[MAX_PATH] = {0};
    sprintf_s(acPathName, MAX_PATH, "%s\\_%08x.log", pcPath, (unsigned int)hdFileName);

    CTraceLog m_CTraceLog;
    m_CTraceLog.CleanPathFile(pcPath);
    m_CTraceLog.InitLogPath(acPathName);    
    
    if(NULL != pcInfo)
    {
        m_CTraceLog.AddLogInfo(pcInfo, acTempBuf);
    }        
    else
    {
        m_CTraceLog.AddLogInfo("错误", "数据为NULL");
    }

    return;
}

