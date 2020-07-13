// SSL_S.h: interface for the CSSL_S class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSL_S_H__B3F880B7_D54E_407A_A773_86D7E74B0A22__INCLUDED_)
#define AFX_SSL_S_H__B3F880B7_D54E_407A_A773_86D7E74B0A22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSSL_S
{
private:
	CString ErrMsg;

	CProxy proxy;

	CString server_ip;
	int	server_port;
	CString servlet_name;

	SSL_CTX* ctx;
	SSL_METHOD *meth;
	int ret;
	int i;
	int curPos;

	WSADATA wsaData;
	SOCKET client_socket;
	struct sockaddr_in addr_server;

	SSL* ssl;
	X509* server_certificate;
//	char* str;

	CString send_buffer;
	CString receive_buffer;

	CString upload_data_buffer;
	BOOL flag_can_connect;
public:
	CSSL_S(void);
	~CSSL_S(void);
	BOOL PostToSSL(
		string csServer, 
		long lPort, 
		string csUrlObj, 
		string sInput, 
		string& sOutput);
// 	BOOL IsServerCanConnect(CString strServer, long lPort);
	CString GetErrMsg(){return ErrMsg;};
private:	
	void SetServerADR(string strServer, long lPort, string strServletName=(""));

	BOOL SSL_Channel();			// SSL_通道
	BOOL SSL_Channel_Init();	// 初始化
	void SSL_Channel_Cleanup();	// 清理
	BOOL SSL_Connect();			// 连接
	
	void SetUploadDataBuffer(string sInput);		
	void SetSendBuffer();
	BOOL ParseBusinessData();
	//20160507增加代理
public:
	CString csProxyIP;
	long    lProxyPort;
	int     iProxyType;
	CString csProxyUserName;
	CString csProxyUserPwd;
	int     iSendTimeOuts;
	int     iRevTimeOuts;

public:
	BOOL SetProxyServerPara(long iConnType, LPCTSTR szProxyIP, long lProxyPort, LPCTSTR strUserName, LPCTSTR strPassword);

	BOOL SetTimeOuts(long lSendValue, long lRevValue);//单位秒
};

             
#define ERROR_INIT "连接初始化未成功"

#endif // !defined(AFX_SSL_S_H__B3F880B7_D54E_407A_A773_86D7E74B0A22__INCLUDED_)
