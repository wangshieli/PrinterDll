#pragma once


/*
* Copyright (c) 2014, ������Ϣ��ȫ���̼����о�����.
* All rights reserved.
*
* �ļ�����: NetCtrl.h
* �ļ�ժҪ: ˰�ط������ͻ��˿�Ʊ���ݴ���ӿ�
*
* ��ǰ�汾: 1.0
*
* ��ʷ�汾: 1.0��˰���з��飬2014-02-21
*            a) ������ʵ��
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
