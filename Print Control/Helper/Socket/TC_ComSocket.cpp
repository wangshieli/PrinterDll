/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* �ļ�����: TC_ComSocket.cpp
* �ļ�ժҪ: Socketͨ����
*
* ��ǰ�汾:  
*
* ��ʷ�汾: 1.0��˰���з��飬2013-11-25
*            a) ������ʵ��
*/

#include "StdAfx.h"
#include "TC_ComSocket.h"
#include "Log/TraceLog.h"

#include "skp/Tools.h"
#include "alg/Aes.h"
#include "alg/Base64.h"
#include "alg/sm3sch.h"

WSADATA g_sWSAData;

extern BYTE proxykey[16];//�����ͨ�ŵĸ���Կ

/*
* ��������: StartSocketWinlib
* ��������: ����WINSOCKET��
* ��    ��: NO
* �� �� ֵ: NO
*/
int StartSocketWinlib(void)
{
    return WSAStartup(MAKEWORD(2,2), &g_sWSAData);
}

/*
* ��������: CleanSocketWinlib
* ��������: ����WINSOCKET��
* ��    ��: NO
* �� �� ֵ: NO
*/
int CleanSocketWinlib(void)
{
    return WSACleanup(); 
}

CTCComSocket::CTCComSocket(): m_hSocket(INVALID_SOCKET), m_dwTimeOut(TC_COMSOCKET_TIMEOUT), m_dwCacheSize(TC_COMSOCKET_CACHE_SIZE)
{
	StartSocketWinlib();
	xmlhead="<?XML VERSION=\"1.0\"?>\r\n";
	xmlend="<!--OVER-->";
}

CTCComSocket::~CTCComSocket()
{
    Disconnect();
}

/*
* ��������: ConnectTo
* ��������: ���ӵ�������
* ��    ��: 
* �� �� ֵ: 
*/
DWORD CTCComSocket::ConnectTo(IN const char *pcServiceIP, IN const int &iPort)
{
    int iRet = 0;
    struct sockaddr_in sServerAddr = {0};
    unsigned long ulArgp = 0;

    //����SOCKET
    m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if((INVALID_SOCKET == m_hSocket))
    {
        return 0x80000000;
    }
    
    //���÷�������ʽ����
    ulArgp = 1;
    iRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ulArgp);
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }

    //���ӵ�������
    sServerAddr.sin_family = AF_INET;
    sServerAddr.sin_addr.s_addr = inet_addr(pcServiceIP);
    sServerAddr.sin_port = htons(iPort);
    memset(sServerAddr.sin_zero, 0x00, 8);
    iRet = connect(m_hSocket, (struct sockaddr*)&sServerAddr, sizeof(sServerAddr));
    if(SOCKET_ERROR == iRet)
    {
        //�ȴ���ʱ2��
        struct timeval tm;
        tm.tv_sec = 2;
        tm.tv_usec = 0;
        fd_set set;
        FD_ZERO(&set);
        FD_SET(m_hSocket, &set);
        if(select(m_hSocket+1, NULL, &set, NULL, &tm) > 0)
        {
            //�жϵ�ʱ��errno�Ƿ�ΪEINPROGRESS��Ҳ����˵�ǲ����������ڽ����У�����ǣ�������������ǣ����ش��� 
            int errval = -1;
            int errlen = sizeof(int);
            getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)&errval, &errlen);
            if(0 != errval)
            {
                return 0x80000000;
            }
        } 
        else
        {
            return 0x80000000;
        }
    }

    //����������ʽ
    ulArgp = 0;
    iRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ulArgp);
    if(SOCKET_ERROR == iRet)
    {
         return 0x80000000;
    }

    //���÷��ͳ�ʱ
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&m_dwTimeOut, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }
    //���ý��ճ�ʱ
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_dwTimeOut, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }

    //���÷��ͻ���
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&m_dwCacheSize, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }
    //���ý��ջ���
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&m_dwCacheSize, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }
    
    return 0;
}

/*
* ��������: Disconnect
* ��������: �Ͽ��������������
* ��    ��: 
* �� �� ֵ: 
*/
DWORD CTCComSocket::Disconnect(void)
{
    closesocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
    return 0;
}

/*
* ��������: Send
* ��������: ��������
* ��    ��: 
* �� �� ֵ: 
*/
DWORD CTCComSocket::Send(IN BYTE *pbDataBuf, IN DWORD &dwDataLen)
{
    int iRet = 0;

    //��������
    iRet = send(m_hSocket, (char *)pbDataBuf, dwDataLen, 0);
    if(SOCKET_ERROR == iRet)
    {
        return 0xF105;
    }

    return 0;
}

// ��������
DWORD CTCComSocket::SendData(LPSTR xmlresult)
{
	int iRet;

	int xmllength=strlen(xmlresult);
	for(int i=0;i<xmllength;i++)
	{
		xmlresult[i]=toupper(xmlresult[i]); //ת���ɴ�д��ĸ
	}
		
	int sendlen = 0, retlen = 0;
	while(sendlen < xmllength){ //���ݷ��Ͳ���ȫ
		iRet = send(m_hSocket, xmlresult+retlen, xmllength-sendlen, 0);
		int rtn=GetLastError();
		if((iRet == 0)||(iRet == SOCKET_ERROR))
		{
			return 0x80010000; //����������ݳ�ʱ
		}
		sendlen+=iRet;
	}
	return 0;
}

/*
* ��������: Recv
* ��������: ��������
* ��    ��: 
* �� �� ֵ: 
*/
DWORD CTCComSocket::Recv(OUT BYTE *pbDataBuf, IN OUT DWORD &dwDataLen)
{
    int iRet = 0;
    DWORD dwCacheLen = 0;
    DWORD dwRecvLen = 0;
    DWORD dwCount = 0;
    
    dwCacheLen = dwDataLen;
    iRet = recv(m_hSocket, (char *)pbDataBuf, dwCacheLen, 0);
    if(SOCKET_ERROR == iRet)
    {
        return 0xF105;
    }
    dwDataLen = iRet;
    dwCacheLen -= iRet;

//    dwRecvLen = MAKEU32_P8_BE(pbDataBuf);
//    while((dwCount++) < TC_COMSOCKET_RECV_COUNT)
//    {
//        if(dwDataLen >= (dwRecvLen+4))
//        {
//            break;
//        }
//        if(0 == dwCacheLen)
//        {
//            break;
//        }
//
//        iRet = recv(m_hSocket, (char *)&pbDataBuf[dwDataLen], dwCacheLen, 0);
//        if(SOCKET_ERROR == iRet)
//        {
//            return 0xF105;
//        }
//        dwDataLen += iRet;
//        dwCacheLen -= iRet;
//    }
//
//    //���ջ���������
//    if(0 == dwCacheLen)
//    {
//        DWORD dwCleanLen = dwDataLen;
//        BYTE abCleanBuf[512] = {0};
//        while((dwRecvLen+4) > dwCleanLen)
//        {
//            iRet = recv(m_hSocket, (char *)&abCleanBuf[0], MIN(512, (dwRecvLen+4-dwCleanLen)), 0);
//            if(SOCKET_ERROR == iRet)
//            {
//                break;
//            }
//            dwCleanLen += iRet;
//        }
//    }

    return 0;
}

// ��������
DWORD CTCComSocket::RecvData(LPSTR xmlresult)
{
	int iRet,ipos;
	char *pos,recvbuf[TC_COMSOCKET_DATA_SIZE+1];
	
	xmlresult[0]=0;
	ipos=0;
	while(1){
		pos=strstr(xmlresult,"<?XML");
		if(pos!=NULL) xmlresult=pos;

		iRet = recv(m_hSocket, recvbuf, TC_COMSOCKET_DATA_SIZE, 0);
		if(SOCKET_ERROR == iRet)
		{
			return 0x80020000; //�����ص����ݲ�����
		}
	
		memcpy(xmlresult+ipos,recvbuf,iRet); ipos+=iRet;

		if(iRet<TC_COMSOCKET_DATA_SIZE)
		{
			*(xmlresult+ipos)=0;
			break;
        }
	}
	return 0;
}

// ����У��ֵ
CString CTCComSocket::GetVerify(LPSTR xmlresult)
{
	DWORD len;
	char ctmp[128],verify[128];
	BYTE pro_key[32],btmp[32];
	CString strReturn;

	Hex2Bcd(m_ClientRand.GetBuffer(0),&len,pro_key);
	if(m_ProxyRand.GetLength()!=0){
		Hex2Bcd(m_ProxyRand.GetBuffer(0),&len,btmp);
		for(int i=0;i<(int)len;i++) pro_key[i] ^= btmp[i];
	}
	ideapublicfunc(0,proxykey,pro_key,len);//���ɹ�����Կ
	Bcd2Hex(&len,pro_key,ctmp);
	TRACE("pro_key:%s\r\n",ctmp);

	md5publicfunc(xmlresult,ctmp);//����16�ֽ�MD5ժҪֵ
	TRACE("MD5:%s\r\n",ctmp);
	Hex2Bcd(ctmp,&len,btmp);
	ideapublicfunc(0,pro_key,btmp,len);//����16�ֽ�MD5ժҪֵ
	
	Bcd2Hex(&len,btmp,verify);
	for(int i=0;i<(int)strlen(verify);i++)
		verify[i]=toupper(verify[i]); //ת���ɴ�д��ĸ
	TRACE("Verify:%s\r\n",verify);
	strReturn=verify;
	return strReturn;
}

// У��У��ֵ
DWORD CTCComSocket::CheckVerify(LPSTR xmlresult)
{
	char* ctmp=new char[strlen(xmlresult)+1];
	char client_verify[64],proxy_verify[64];
	
	char* pos=strstr(xmlresult,"<!--CLIENTVERIFY:");
	if(pos==NULL) //���յ��������Ҳ�����Ӧ��ǩ
	{
		delete ctmp;
		return 0x80020004; //���յ��������Ҳ�����Ӧ��ǩ
	}
	memcpy(ctmp,xmlresult,pos-xmlresult);ctmp[pos-xmlresult]=0; //��ȡ��У��������ַ���
	strcpy(client_verify,GetVerify(ctmp)); //����32λУ��ֵ
	delete ctmp;
	memcpy(proxy_verify,pos+17,32);proxy_verify[32]=0;
	if(strcmp(client_verify,proxy_verify))
	{
		return 0x80020002; //У��ֵ�ȶԲ�ͨ��
	}
	return 0;//У��ֵ�ȶ�ͨ��
}

// �ͻ��������֤����ÿ�η��������Ӻ�
DWORD CTCComSocket::CheckClient()
{
	DWORD rtn;
	char xmlresult[1024],ipaddr[64],ctmp[1024];
	
	GetIPAddress(ipaddr); //��ÿͻ���IP��ַ
	m_ClientRand=ZLib_GetRand(16);
	sprintf(xmlresult,"%s<CLIENTIDENTIFY>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8)); //�������16λTagֵ
	sprintf(xmlresult,"%s<CLIENTIP>%s</CLIENTIP>\r\n",xmlresult,ipaddr);
	sprintf(xmlresult,"%s<CLIENTRAND>%s</CLIENTRAND>\r\n</CLIENTIDENTIFY>\r\n",xmlresult,m_ClientRand);
	sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);

	rtn=SendData(xmlresult);
	if(rtn!=0) return rtn;
	rtn=RecvData(xmlresult);
	if(rtn!=0) return rtn;

	rtn=GetXmlNode(xmlresult,"����",ctmp);
	if(rtn==0)
	{
		sscanf(ctmp,"%x",&rtn);
		return rtn;
	}

	rtn=GetXmlNode(xmlresult,"PROXYRAND",ctmp);//��ȡ���д����32λ�����
	if(rtn!=0) return rtn;
	m_ProxyRand=ctmp;

	return CheckVerify(xmlresult);//У�����ݰ���У��ֵ
}

// ʱ��ģ�鰲ȫ����(���ݼӽ���)
DWORD CTCComSocket::RTCEnDecrypt(BYTE bDealType, LPSTR sDiskNumber, LPSTR sChallenge, LPSTR sCmdHeader, LPSTR sData, LPSTR sRespBuffer) 
{
	DWORD rtn;
	char xmlresult[1024],ctmp[64];

	sprintf(xmlresult,"%s<ģ�鰲ȫ����>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8));
	sprintf(xmlresult,"%s<�̱��>0000%s</�̱��>\r\n",xmlresult,sDiskNumber);
	sprintf(xmlresult,"%s<��ɢ����>%s</��ɢ����>\r\n",xmlresult,sChallenge);
	sprintf(xmlresult,"%s<������>%s</������>\r\n",xmlresult,sCmdHeader);
	sprintf(xmlresult,"%s<����>%s</����>\r\n",xmlresult,sData);
	sprintf(xmlresult,"%s<��������>%d</��������>\r\n</ģ�鰲ȫ����>\r\n",xmlresult,bDealType);
	sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);
	
	rtn=SendData(xmlresult);
	if(rtn!=0) return rtn;
	rtn=RecvData(xmlresult);
	if(rtn!=0) return rtn;
	rtn=GetXmlNode(xmlresult,"����",ctmp);
	if(rtn==0)
	{
		sscanf(ctmp,"%x",&rtn);
		return rtn;
	}
	rtn=CheckVerify(xmlresult);
	if(rtn!=0) return rtn;
	return GetXmlNode(xmlresult,"���ݼ��ܽ��",sRespBuffer);
}

// ���ܷ�Ʊ�����Ϣ��
DWORD CTCComSocket::DecryptRTCInvoice(LPSTR sSKDiskNumber, LPSTR sCSDiskNumber, LPSTR sRTCInvoice, LPSTR sRespBuffer) 
{
	DWORD rtn;
	char challenge[64],clock[64],ctmp1[256],ctmp2[256];

	sscanf(sRTCInvoice,"%96s%16s",ctmp1,clock);
	CalReversedCode(clock,ctmp2); //����ʱ�����ݵķ���
	sprintf(challenge,"%s%s",clock,ctmp2); //���16�ֽڷ�ɢ����

	if(strlen(sCSDiskNumber)==12) //�����̱�˰
	{
		rtn=RTCEnDecrypt(1,sCSDiskNumber,challenge,sSKDiskNumber,ctmp1,ctmp2); //���ܷ�Ʊ��Ϣ��
	}
	else //˰���̱�˰
	{
		rtn=RTCEnDecrypt(1,sSKDiskNumber,challenge,"",ctmp1,ctmp2); //���ܷ�Ʊ��Ϣ��
	}
	if(rtn) return rtn;
	if(strlen(ctmp2)!=96)
	{
		return 0x80020005; //���������������
	}
	//�������ݷ�������֤��ֻ����ǰ40�ֽڷ�Ʊ��Ϣ��
	rtn=DealWithRTCData(1,ctmp2,sRespBuffer);
	if(rtn)
	{
		return 0x09FFF0; //ʱ��ģ�����ݷ�������֤��ͨ��
	}
	return 0;
}


// ����ʱ�Ӽ������
DWORD CTCComSocket::EncryptRTCInvoice(LPSTR sDiskNo, LPSTR sRTCInvoice, LPSTR sRespBuffer) 
{
	DWORD rtn;
	char challenge[64],clock[64],ctmp1[255],ctmp2[255];
	
	if(strlen(sRTCInvoice)!=96) return -3;
	sscanf(sRTCInvoice,"%80s%16s",ctmp2,clock);
	rtn=DealWithRTCData(0,ctmp2,ctmp1); //�������ݷ��ϱ任��40�ֽڵ�48�ֽڱ任
	if(rtn) return -3;
	CalReversedCode(clock,ctmp2); //����ʱ�����ݵķ���
	sprintf(challenge,"%s%s",clock,ctmp2); //���16�ֽڷ�ɢ����
	rtn=RTCEnDecrypt(0,sDiskNo,challenge,"",ctmp1,ctmp2); //���ܷ�Ʊ��Ϣ��
	if(rtn) return rtn;
	sprintf(sRespBuffer,"%s%s",ctmp2,clock); //ʱ����Ȼ�����ģ����56�ֽڷ�Ʊ�����Ϣ��
	return 0;
}

// ���ݼ���(Ŀǰ���м��ܶ�����Ӧ�ü���)(�ڲ�����)
DWORD CTCComSocket::DataEncrypt(BYTE ucType, LPSTR sKeyID, LPSTR disknumber, LPSTR sData, LPSTR sFactor, LPSTR sRespBuffer) 
{
	DWORD rtn;
	int lens,len,pos=0;
	char xmlresult[TRANSFERBUFSIZE],ctmp[MAXENCRYPTTLEN+1];

	lens=strlen(sData);
	sRespBuffer[0]=0;
	while(pos<lens){
		len=lens-pos+1;
		if(len>MAXENCRYPTTLEN) len=MAXENCRYPTTLEN;
		memcpy(ctmp,sData+pos,len);ctmp[len]=0;
		
		sprintf(xmlresult,"%s<���ݼ���>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8));
		sprintf(xmlresult,"%s<�̱��>0000%s</�̱��>\r\n",xmlresult,disknumber);
		sprintf(xmlresult,"%s<��ɢ����>%s</��ɢ����>\r\n",xmlresult,sFactor);
		if(len<MAXENCRYPTTLEN)
		{
			sprintf(xmlresult,"%s<����>%s</����>\r\n",xmlresult,FillinData(ctmp));
		}
		else
		{			
			sprintf(xmlresult,"%s<����>%s</����>\r\n",xmlresult,ctmp);
		}
		sprintf(xmlresult,"%s<��Կ��ʶ>%s</��Կ��ʶ>\r\n</���ݼ���>\r\n",xmlresult,sKeyID);
		sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);
		
		rtn=SendData(xmlresult);
		if(rtn!=0) return rtn;
		rtn=RecvData(xmlresult);
		if(rtn!=0) return rtn;	
		rtn=GetXmlNode(xmlresult,"����",ctmp);
		if(rtn==0)
		{
			sscanf(ctmp,"%x",&rtn);
			return rtn;
		}
		rtn=CheckVerify(xmlresult);
		if(rtn!=0) return rtn;
		
		rtn=GetXmlNode(xmlresult,"���ݼ��ܽ��",ctmp);
		if(rtn!=0) return rtn;
		strcat(sRespBuffer,ctmp);
		pos+=len;
	}
	return 0;
}

// ���д������ݽ���
DWORD CTCComSocket::ProxyDecrypt(LPSTR sKeyID, LPSTR sDiskNumber, LPSTR sData, LPSTR sFactor, LPSTR sRespBuffer) 
{
	DWORD rtn;
	int lens,len,pos=0;
	char xmlresult[TRANSFERBUFSIZE],ctmp[MAXENCRYPTTLEN+1];

	lens=strlen(sData);
	sRespBuffer[0]=0;
	while(pos<lens){
		len=lens-pos+1;
		if(len>MAXENCRYPTTLEN) len=MAXENCRYPTTLEN;
		memcpy(ctmp,sData+pos,len);ctmp[len]=0;
		
		sprintf(xmlresult,"%s<���ݽ���>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8));
		sprintf(xmlresult,"%s<�̱��>0000%s</�̱��>\r\n",xmlresult,sDiskNumber);
		sprintf(xmlresult,"%s<��ɢ����>%s</��ɢ����>\r\n",xmlresult,sFactor);
		if(len<MAXENCRYPTTLEN)
		{
			sprintf(xmlresult,"%s<����>%s</����>\r\n",xmlresult,FillinData(ctmp));
		}
		else
		{			
			sprintf(xmlresult,"%s<����>%s</����>\r\n",xmlresult,ctmp);
		}
		sprintf(xmlresult,"%s<��Կ��ʶ>%s</��Կ��ʶ>\r\n</���ݽ���>\r\n",xmlresult,sKeyID);
		sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);
		
		rtn=SendData(xmlresult);
		if(rtn!=0) return rtn;
		rtn=RecvData(xmlresult);
		if(rtn!=0) return rtn;	
		rtn=GetXmlNode(xmlresult,"����",ctmp);
		if(rtn==0)
		{
			sscanf(ctmp,"%x",&rtn);
			return rtn;
		}
		rtn=CheckVerify(xmlresult);
		if(rtn!=0) return rtn;
		
		GetXmlNode(xmlresult,"���ݽ��ܽ��",ctmp);
		strcat(sRespBuffer,ctmp);
		pos+=len;
	}
	return 0;
}

// ���ϳ������ݽ��ܺ���
INT CTCComSocket::UploadDecrypt(BYTE AppType, LPSTR InData, LPSTR OutData)
{	
	// TODO: Add your dispatch handler code here

	int i, ret;
	int inlen = 0;
	int outlen = 0;

	unsigned char *indatabin;
	unsigned char *outdatabin;

	FILE *fp = NULL;
	CString strFilename = "";
	CString strInAppType = "";
	CString strOutAppType = "";
	CString strDiskNo = "";
	CString strCodeHead = "";
	CString strHash1 = "";
	CString strHash2 = "";

	CString strInData = "";
	CString strOutData = "";

	unsigned char KEYt[256] = {0};			//ʵ�����ݳ���Ϊ32�ֽ�
	unsigned char KEYp[256] = {0};			//δ֪
	unsigned char Hash[256] = {0};			//ʵ�����ݳ���Ϊ32�ֽ�
	unsigned char CodeHead[256] = {0};		//ʵ�����ݳ���Ϊ64�ֽ�

	unsigned char Key[256] = {0};
	unsigned char Iv[256] = {0};

	char DiskNo[256] = {0};
	char InCodeHead[256] = {0};
	char OutCodeHead[256] = {0};

	unsigned int dig_len;

	std::string str_data;

	Aes MyAes;

	Base64 myBase64;

	string strBase64;

	if(AppType!=1 && AppType!=2)
	{
		return -101;	//�����������
	}

	strInData = InData;
	strBase64 = myBase64.Decode(strInData,strInData.GetLength(),inlen);
	strInData = strBase64.c_str();

	if(inlen < 81)
	{
		return -102;	//�����������
	}

	indatabin = new unsigned char[inlen+1024];

	memcpy(indatabin, strBase64.c_str(), inlen);

	indatabin[inlen] = '\0';

	memcpy(DiskNo, indatabin, 2);
	DiskNo[2] = '\0';
	strInAppType = DiskNo;

	if(AppType == 1)
	{
		strOutAppType = "01";
	}
	else if(AppType == 2)
	{
		strOutAppType = "02";
	}

	if(strInAppType.Compare(strOutAppType) != 0)
	{
		delete []indatabin;
		return -103;	//˰��Ӧ�����ʹ���
	}

	memcpy(DiskNo, indatabin+4, 12);
	DiskNo[12] = '\0';
	strDiskNo = DiskNo;

	memcpy(CodeHead, indatabin+16, 64);
	strCodeHead = ConvertBinToHexStr(CodeHead, 64, 0x00);

	memcpy(InCodeHead, strCodeHead, 128);
	InCodeHead[128] = '\0';

	for(i = 0; i < 128; i += 16)
	{
		ret = ProxyDecrypt("3001", DiskNo, InCodeHead+i, "", OutCodeHead+i); //�Ƚ���
		if(ret != 0)
		{
			delete []indatabin;
			return ret;
		}
	}
	OutCodeHead[128] = '\0';

	strCodeHead = OutCodeHead;
	ConvertHexStrToBin(strCodeHead, CodeHead, 64);

	memcpy(Iv, CodeHead, 16);
	memcpy(Key, CodeHead+16, 16);
	memcpy(Hash, CodeHead+32, 32);

	strHash1 = Hash;
	sch256_digest(indatabin+16+64, inlen-16-64, Hash, &dig_len); //sch256_digest�������Ǽ���SM3��ϣֵ�ĺ��������볤�����⣬���Ϊ32�ֽڵĹ�ϣֵ
	strHash2 = Hash;

	if (strHash1.Compare(strHash2) != 0)
	{
		delete []indatabin;
		return -104;     //����У��ʧ��
	}

	outdatabin = new unsigned char[inlen+1024];

	outlen = MyAes.AesDecrypt_CBC(Key, Iv, indatabin+16+64, inlen-16-64, outdatabin);
	//outlen = MyAes.AesDecrypt_CBC_NoPad(Key, Iv, indatabin+16+64, inlen-16-64, outdatabin);

	outdatabin[outlen] = '\0';

	delete []indatabin;

	//this->m_cReturn = ConvertBinToHexStr(outdatabin, outlen, 0x00);
	//this->m_cReturn = outdatabin;

	memcpy(OutData, outdatabin, outlen);
	OutData[outlen] = '\0';

	delete []outdatabin;

	return 0;
}
