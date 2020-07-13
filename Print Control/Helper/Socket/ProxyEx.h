#pragma once

//#include <WinSock2.h>
#include <string>
#include <vector>

using namespace std;

enum ProxyStatus
{
	SUCCESS,
	CONNECT_PROXY_FAIL,
	NOT_CONNECT_PROXY,
	CONNECT_SERVER_FAIL
};

class CProxy
{
public:
/*	CProxy(long type, string ip, u_short port, string username, string password)
		:m_proxyType(type), m_proxyIp(ip), m_proxyPort(port), m_proxyUserName(username), m_proxyUserPwd(password)
	{}*/
	CProxy(){};
	~CProxy(void){};

	ProxyStatus ConnectProxyServer(SOCKET socket);
	ProxyStatus ConnectServer(SOCKET socket, string ip, u_short port);

	CString m_strLastError;
	int     m_lastErrorCode; 

private:
	ProxyStatus ConnectByHttp(SOCKET socket, string ip, u_short port);
	ProxyStatus ConnectBySock4(SOCKET socket, string ip, u_short port);
	ProxyStatus ConnectBySock5(SOCKET socket, string ip, u_short port);

public:
	bool Send(SOCKET socket, const char* buf, int len);
	int Receive(SOCKET socket, char* buf, int bufLen);


	long m_proxyType;
	CString m_proxyIp;
	u_short m_proxyPort;
	CString m_proxyUserName;
	CString m_proxyUserPwd;
	int     m_iSendTimeOuts;
	int     m_iRevTimeOuts;

	bool m_blnProxyServerOk;
};

struct TSock4req1 
{ 
	char VN; 
	char CD; 
	unsigned short Port; 
	unsigned long IPAddr; 
	char other; 
}; 

struct TSock4ans1 
{ 
	char VN; 
	char CD; 
};

struct TSock5req1 
{ 
	char Ver; 
	char nMethods; 
	char Methods; 
}; 

struct TSock5ans1 
{ 
	char Ver; 
	char Method; 
}; 

struct TSock5req2 
{ 
	char Ver; 
	char Cmd; 
	char Rsv; 
	char Atyp; 
	char other; 
}; 

struct TSock5ans2 
{ 
	char Ver; 
	char Rep; 
	char Rsv; 
	char Atyp; 
	char other; 
}; 

struct TAuthreq 
{ 
	char Ver; 
	char Ulen; 
	char Name; 
	char PLen; 
	char Pass; 
}; 

struct TAuthans 
{ 
	char Ver; 
	char Status; 
}; 
