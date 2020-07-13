#pragma once


/*
* Copyright (c) 2014, 国家信息安全工程技术研究中心.
* All rights reserved.
*
* 文件名称: NetCtrl.h
* 文件摘要: 税控服务器客户端开票数据处理接口
*
* 当前版本: 1.0
*
* 历史版本: 1.0，税控研发组，2014-02-21
*            a) 创建并实现
*/

#include <stdio.h>
#include <string>
#include <WinINet.h>

using std::string;

#define BUFFSIZE 1000//500

#pragma comment (lib, "Wininet.lib")

class CNetCtrl
{
public:
	CNetCtrl(void);
	~CNetCtrl(void);

private:
	CString m_strLastError;

public:
	BOOL PostAndReceive(IN string UpStr,
						IN string csServer,
						IN long lPort,
						IN string csUrl,
						OUT string* DownStr	);
	BOOL UseHttpSendReqEx(IN HINTERNET hRequest, 
						  IN DWORD dwPostSize, 
						  IN const string* pStr);
	CString GetLastError(void){return m_strLastError;}

};
