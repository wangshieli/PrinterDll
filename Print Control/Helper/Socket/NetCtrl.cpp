#include "stdafx.h"
#include "NetCtrl.h"
#include <string>


CNetCtrl::CNetCtrl(void)
{
}


CNetCtrl::~CNetCtrl(void)
{
}


/*
* 函数名称: PostAndReceive
* 函数功能: 发送请求和接收数据
* 参    数: 
*            sUpStr		    - 请求内容
*            sServer		- 服务器地址IP
*            lPort			- 端口号
*            sUrl			- URL
*            DownStr		- 接收到的数据
* 返 回 值: 
*            TRUE                - 成功
*            FALSE               - 失败
*/
BOOL CNetCtrl::PostAndReceive(string sUpStr,
							   string sServer,
							   long lPort,
							   string sUrl,
							   string* DownStr)
{
 	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	DWORD dwPostSize = 0;
	BOOL bRet = TRUE;
	char pcBuffer[BUFFSIZE] = {NULL};
	DWORD dwBytesRead = 0;
	DWORD dwRet = 0;
//	int time = 5;

	hSession = InternetOpen( "HttpSendRequestEx", 
							INTERNET_OPEN_TYPE_PRECONFIG,
							NULL, NULL, 0);
	if(!hSession)
	{
		m_strLastError="Failed to open session\n";
		return FALSE;
	}
//	::InternetSetOption(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &time, sizeof(time));
	hConnect = InternetConnect(hSession,sServer.c_str(), 
							   (USHORT)lPort, NULL, NULL,
							   INTERNET_SERVICE_HTTP,
							   NULL, NULL);
	if (!hConnect)
	{
		m_strLastError="connect error";
		InternetCloseHandle( hSession ) ;
		return FALSE;
	}
	hRequest = HttpOpenRequest(hConnect, "POST", sUrl.c_str(), 
							   NULL, NULL, NULL, 
							   INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (!hRequest)
	{
		InternetCloseHandle( hConnect ) ;
		InternetCloseHandle( hSession ) ;
		m_strLastError="Failed to open request handle";
		return FALSE;
	}

	dwPostSize = sUpStr.length();
	if(UseHttpSendReqEx(hRequest, dwPostSize,&sUpStr))
	{	
		do
		{	
			dwBytesRead=0;
			if(InternetReadFile(hRequest, pcBuffer, BUFFSIZE-1, &dwBytesRead))
			{	
				DownStr->append(pcBuffer,dwBytesRead);
			}
			else
			{
				m_strLastError="\nInternet ReadFile failed";
				break;
			}
		}while(dwBytesRead>0);
	}
	else
	{
		bRet=FALSE;
	}	

	bRet &= InternetCloseHandle(hRequest);	
	bRet &= InternetCloseHandle(hConnect);
	bRet &= InternetCloseHandle( hSession );

	return bRet;
}


BOOL CNetCtrl::UseHttpSendReqEx(HINTERNET hRequest,
								DWORD dwPostSize,
								const string* pStr)
{
	INTERNET_BUFFERS BufferIn;
	DWORD dwBytesWritten = 0 ;
	int n = 0;
	CHAR pBuffer[1024] = {NULL};
	BOOL bRet = TRUE;
	long lRead = 0 ;

	BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
	BufferIn.Next = NULL; 
	BufferIn.lpcszHeader = NULL;
	BufferIn.dwHeadersLength = 0;
	BufferIn.dwHeadersTotal = 0;
	BufferIn.lpvBuffer = NULL;                
	BufferIn.dwBufferLength = 0;
	BufferIn.dwBufferTotal = dwPostSize; // This is the only member used other than dwStructSize
	BufferIn.dwOffsetLow = 0;
	BufferIn.dwOffsetHigh = 0;

	if(!HttpSendRequestEx( hRequest, &BufferIn, NULL, 0, 0))
	{
		m_strLastError="Error on HttpSendRequestEx %d\n";
        return FALSE;
	}

	while(bRet)
	{
		char pBuffer[1024] = {NULL};
	//	lRead = pStr->_Copy_s(pBuffer, 1024, 1024, n);
		lRead = pStr->copy(pBuffer, 1024, n); //VC 6.0
		n += lRead;
		if(lRead<=0)
		{	
			break;
		}
		bRet=InternetWriteFile( hRequest, pBuffer, lRead, &dwBytesWritten);
		
		TRACE("\r%d bytes sent.", n);
		TRACE( "\nError on InternetWriteFile %lu\n",::GetLastError() );
	}

	if(!bRet)
	{
		TRACE( "\nError on InternetWriteFile %lu\n",::GetLastError() );
		m_strLastError="Error on InternetWriteFile ";
        return FALSE;
	}

	if(!HttpEndRequest(hRequest, NULL, 0, 0))
	{
		TRACE( "Error on HttpEndRequest %lu \n", ::GetLastError());
		m_strLastError="Error on HttpEndRequest ";
        return FALSE;
	}

	return TRUE;
}