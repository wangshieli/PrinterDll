// SSL_S.cpp: implementation of the CSSL_S class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SSL_S.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern BOOL bProxy;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSSL_S::CSSL_S(void)
{
}


CSSL_S::~CSSL_S(void)
{
}


BOOL CSSL_S::PostToSSL(
						string csServer,
						long lPort,
						string csObj,
						string sInput,
						string& sOutput)
{
	
	SetUploadDataBuffer(sInput);		// д���ݵ�upload_data_buffer
	

//	buffer_file = NULL;

	SetServerADR(csServer,  lPort, csObj);		// дIP�Ͷ˿ڵ���Ա������

	if(!SSL_Channel())
	{
		DEBUG_TRACELOG_STR("sslMsg", ErrMsg.GetBuffer(0));

		unsigned long ulErr = ERR_get_error(); // ��ȡ�����
		char szErrMsg[1024] = {0};
		char *pTmp = NULL;

		ERR_load_ERR_strings();
		ERR_load_crypto_strings(); 

		pTmp = ERR_error_string(ulErr,szErrMsg); // ��ʽ��error:errId:��:����:ԭ��

		DEBUG_TRACELOG_STR("���� = ",pTmp);		

		return FALSE;
	}

	DEBUG_TRACELOG_STR("sslMsg", ErrMsg.GetBuffer(0));

	sOutput.append(receive_buffer.GetBuffer(0));

	return TRUE;
}


//BOOL CSSL_S::IsServerCanConnect(CString strServer, long lPort)
//{
//	flag_can_connect = FALSE;
//	SetServerADR(strServer,  lPort);
//	if(!SSL_Chnnel())
//	{
//		return FALSE;a
//	}
//	return flag_can_connect;
// }

void CSSL_S::SetServerADR(string strServer, long lPort, string strServletName)
{
	server_ip.Format("%s", strServer.data()); 
	server_port = lPort;
	servlet_name.Format("%s", strServletName.data());
}

void CSSL_S::SetUploadDataBuffer(string sInput)
{
	upload_data_buffer.Format("%s", sInput.data());
}

void CSSL_S::SetSendBuffer()
{
	CString buf[7];
	send_buffer.Format("");

	buf[6].Format("%s",upload_data_buffer.GetBuffer(0));
	buf[0].Format("%s /%s HTTP/1.1\r\n", "POST",
		servlet_name.GetBuffer(0));
	buf[1].Format(
		"Content-type: application/x-www-form-urlencoded\r\n");
	buf[2].Format(
		"User-Agent: %s\r\n", "POST");
	buf[3].Format(
		"Host: %s:%d\r\n", server_ip, server_port);
	buf[4].Format(
		"Content-Length: %d\r\n", buf[6].GetLength());
	buf[5].Format(
		"Cache-Control: no-cache\r\n\r\n");

	i = 0;
	for(i;i<7;i++)
	{
		send_buffer += buf[i];
	}
	i = 0;
}

BOOL CSSL_S::ParseBusinessData()
{
	CString str[3];
	str[1].Format("%s",receive_buffer.GetBuffer(0)); 

	if( 0 == strlen( str[1].GetBuffer(0) ))
	{
		 	ErrMsg = "δ���յ�����������";
		return FALSE;
	}
	int startPos = 0;

	curPos = str[1].Find("\r\n\r\n",startPos);
	str[2] = str[1].Mid(startPos ,curPos + strlen("\r\n"));
	startPos = 0;
	while(-1 != curPos)
	{
		curPos = -1;
		curPos = str[2].Find("\r\n",startPos);
		str[0] = str[2].Mid(startPos,curPos - startPos );
		startPos += str[0].GetLength() + strlen("\r\n") ;
		int curPos1 = str[0].Find("Content-Length: ");
		if(-1 != curPos1 )
		{
			int nPos = strlen("Content-Length: ")+ curPos1;
			int nSize =str[0].GetLength()- nPos ;
			str[2] = str[0].Mid(nPos,nSize);
			break;
		}
		
	}

	//curPos = str[1].Find("\r\n\r\n",0);
	//curPos = str[1].Find("Content-Length: ");
	//curPos += strlen("Content-Length: ");
	//str[0] = str[1].Tokenize(" \r\n",curPos); //���ݳ���ֵ

	curPos = str[1].Find("\r\n\r\n");

	curPos += strlen("\r\n\r\n");
	
	receive_buffer = str[1].Mid(curPos,atoi(str[2])) ;	

	curPos = 0;

	return TRUE;
}

BOOL CSSL_S::SSL_Channel_Init()
{	

	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	// meth = (SSL_METHOD *)SSLv23_client_method();
	meth = (SSL_METHOD *)TLSv1_client_method();

	ctx = SSL_CTX_new (meth);
	if (!ctx) 
	{
		ErrMsg = ERROR_INIT; 
		return FALSE;//return -1;
	}

	// ����TCP���� //
	if ( 0 != WSAStartup( MAKEWORD(2, 2), &wsaData ) ) 
	{
		ErrMsg = "����Socketʧ��"; 
		//	std::cout<<"WSAStartup error."<<std::endl;
		return FALSE;
	}
	client_socket = socket (AF_INET, SOCK_STREAM, 0);
	if( client_socket == INVALID_SOCKET )
	{
		ErrMsg = "��ʼ��socketʧ��"; 
		//	std::cout<<"socket error."<<std::endl;
		return FALSE;
	}

		//20160507���Ӵ���
		//CSocket socket;
	int TimeOut;

	if(!bProxy)
	{
		iProxyType = 3;
	}

	//��Ҫ�޸���
	switch(iProxyType)
	{
	case 0://http
	case 1://socket4
	case 2://socket5
		
		//if(!socket.Create())
		//{
		//	AfxMessageBox("socket create error!");
		//	return FALSE;
		//}
		/*
		proxy.m_proxyIp = csProxyIP;//"192.168.14.144";//
		proxy.m_proxyPort = lProxyPort;//1080;//8090;// 8090 http 1080 socket
		proxy.m_proxyType = iProxyType;//0;//
		proxy.m_iSendTimeOuts = iSendTimeOuts;
		proxy.m_iRevTimeOuts = iRevTimeOuts;
		if(2 == iProxyType)//socket5
		{
			proxy.m_proxyUserName = csProxyUserName;
			proxy.m_proxyUserPwd = csProxyUserPwd;
		}
*/
		if(proxy.ConnectProxyServer(client_socket) == 0)
		{
			if (proxy.ConnectServer(client_socket, server_ip.GetBuffer(0), server_port) == 0)
			{
				//return TRUE;
			}
			else
			{
				SSL_CTX_free(ctx);
				//_ssl_item_Code = -10007;
			    ErrMsg = "proxy.ConnectServer error. IP��ַ��˿ںſ��ܲ���";
				//MessageLog(LOGLEVEL_MIN,"�����������ʧ��,Server:[%s:%d],�������:[%d],������Ϣ:[%s]",strServer,lPort,proxy.m_lastErrorCode,proxy.m_strLastError);
				return FALSE;
			}
		}
		else
		{
			SSL_CTX_free(ctx);
			//_ssl_item_Code = -10007;
			ErrMsg = "proxy.ConnectProxyServer error. IP��ַ��˿ںſ��ܲ���";
			//MessageLog(LOGLEVEL_MIN,"���������ʧ��,ProxyServer:[%s:%d],�������:[%d],������Ϣ:[%s]",proxy.m_proxyIp,proxy.m_proxyPort,proxy.m_lastErrorCode,proxy.m_strLastError);
			return FALSE;
		}
		//return FALSE;
		break;

    //case 2://socket5
	//	client_socket = Sock5Proxy_Connect(client_socket);
	//	break;
	case 3://ֱ��
				//set Recv and Send time out
		//TimeOut=6000; //���÷��ͳ�ʱ6��
		if(::setsockopt(client_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&iSendTimeOuts,sizeof(TimeOut))==SOCKET_ERROR)
		{
			SSL_CTX_free(ctx);
			//_ssl_item_Code = -10007;
			ErrMsg = "setsockopt error. ���÷��ͳ�ʱʧ��";
			return FALSE;
		}
		//TimeOut=6000;//���ý��ճ�ʱ
		if(::setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&iRevTimeOuts,sizeof(TimeOut))==SOCKET_ERROR)
		{
			SSL_CTX_free(ctx);
			//_ssl_item_Code = -10007;
			ErrMsg = "setsockopt error. ���ý��ճ�ʱʧ��";
			return FALSE;
		}
		break;
	}

	memset (&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.S_un.S_addr = inet_addr(server_ip.GetBuffer(0));
	addr_server.sin_port = htons(server_port);

	ret = connect(client_socket, (struct sockaddr*) &addr_server, sizeof(addr_server));
	if( client_socket == SOCKET_ERROR ) 
	{
		ErrMsg = "socket����ʧ��"; 
		//	std::cout<<"connect error."<<std::endl;
		return FALSE;
	}

	// TCP�����Ѿ�������ִ��Client SSL //
	ssl = SSL_new (ctx);                                                                
	if( ssl == NULL )
	{     
		ErrMsg = ERROR_INIT; 
		//	std::cout<<"SSL_new error."<<std::endl;                                           
		return FALSE;                                                                      
	}                                                                                   
	SSL_set_fd (ssl, client_socket);                                                    
	ret = SSL_connect (ssl); 
	if( ret == -1 )
	{                                                                   
		ErrMsg = ERROR_INIT; 
		//	std::cout<<"SSL_accept error."<<std::endl;                                        
		return FALSE;                                                                   
	}                                
	// ��ȡ�������˵�֤��                                                               
	server_certificate = SSL_get_peer_certificate (ssl);       

	return TRUE;
}

BOOL CSSL_S::SSL_Connect()
{
	if(!ssl)
	{
		ErrMsg = "����δ����";
		return FALSE;
	}
	ret = SSL_write(ssl, send_buffer.GetBuffer(0), send_buffer.GetLength() );
	if( ret == -1 ) 
	{
		ErrMsg = "SSL_write error";
		return FALSE;
	}
	
	receive_buffer.Format("%s","\0");
	char ch[1];	ch[0] = '\0';
	CString strLength;
	int iLength = 0;
	int countl = 0;
	curPos = -1;
	int curPos1 = -1;
	int curPos2 = -1;
	int startPos = 0;
	CString str[3];
	
	/*char buff[9100];
	 
	ret = SSL_read(ssl,buff,9100);*/

	while( (ret = SSL_read(ssl,ch,1)) > 0 ) //һ�ζ�ȡһ���ַ�������
	{      
		receive_buffer.Insert(receive_buffer.GetLength(),ch[0]);
		if(iLength == 0)
		{
			curPos = receive_buffer.Find("\r\n\r\n");
			if(-1 != curPos )
			{

				DEBUG_TRACELOG_STR("hander", receive_buffer.GetBuffer(0));
				str[0].Format("%s",receive_buffer.GetBuffer(0));
				while(0 == iLength)
				{
					curPos1 = str[0].Find("\r\n",startPos);
					str[1] = str[0].Mid(startPos,curPos1) ;
					
					if(-1 == curPos2)
					{
						curPos2 = str[1].Find("Content-Length: ");
						if(-1 != curPos2)
						{	
							int a,b;
							a = curPos2 += strlen("Content-Length: ");	
							b = str[1].GetLength() - curPos2;
							iLength =  atoi(str[1].Mid(a,b)); //���ݳ���ֵ

							DEBUG_TRACELOG_U32("Content-Length", iLength);
						}
					}
					startPos += curPos1;
				}
				
			}
		}
		else
		{	
			countl ++ ;
			if(countl == iLength)
			{
				DEBUG_TRACELOG_U32("countl", countl);
				break;
			}
		}
		
	} 
	curPos = 0;
	if( ret == -1 ) 
	{
		ErrMsg =  "SSL_read error.";
		DEBUG_TRACELOG_STR("sslMsg11", ErrMsg.GetBuffer(0));

		unsigned long ulErr = ERR_get_error(); // ��ȡ�����
		char szErrMsg[1024] = {0};
		char *pTmp = NULL;

		ERR_load_ERR_strings();
		ERR_load_crypto_strings(); 

		pTmp = ERR_error_string(ulErr,szErrMsg); // ��ʽ��error:errId:��:����:ԭ��

		DEBUG_TRACELOG_STR("���� = ",pTmp);		

		return FALSE;
	}
	flag_can_connect = TRUE;
	return TRUE;
}


void CSSL_S::SSL_Channel_Cleanup()
{
	SSL_shutdown(ssl);  /* send SSL/TLS close_notify */
	// Cleanup //
	closesocket(client_socket); client_socket = NULL;
	SSL_free (ssl);				ssl = NULL;
	SSL_CTX_free (ctx);			ctx = NULL;
	WSACleanup();
}


BOOL CSSL_S::SSL_Channel()
{
	if(!SSL_Channel_Init())
	{
		DEBUG_TRACELOG_STR("ssl_init", "sslinit����");
	}

	// ҵ�����ݴ���
	SetSendBuffer();

	DEBUG_TRACELOG_STR("ssl_SendData", send_buffer.GetBuffer(0));

	if(!SSL_Connect())
	{
		DEBUG_TRACELOG_STR("ssl_connect", "ssl_connect����");
		return FALSE;
	}

	// ������յ�ҵ������
	if(!ParseBusinessData())
	{
		return FALSE;
	}
	SSL_Channel_Cleanup();

	return TRUE;
}

BOOL CSSL_S::SetProxyServerPara(long iConnType, LPCTSTR szProxyIP, long lProxyPort, LPCTSTR strUserName, LPCTSTR strPassword) 
{
	proxy.m_proxyType = iConnType;
	proxy.m_proxyIp = szProxyIP;
    proxy.m_proxyPort = lProxyPort;
	proxy.m_proxyUserName = strUserName;
	proxy.m_proxyUserPwd = strPassword;
	return TRUE;
}

BOOL CSSL_S::SetTimeOuts(long lSendValue, long lRevValue)//��λ��
{
    proxy.m_iSendTimeOuts = lSendValue * 1000;
	proxy.m_iRevTimeOuts = lRevValue * 1000;

	return TRUE;
}