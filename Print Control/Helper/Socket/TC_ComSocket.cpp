/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* 文件名称: TC_ComSocket.cpp
* 文件摘要: Socket通信类
*
* 当前版本:  
*
* 历史版本: 1.0，税控研发组，2013-11-25
*            a) 创建并实现
*/

#include "StdAfx.h"
#include "TC_ComSocket.h"
#include "Log/TraceLog.h"

#include "skp/Tools.h"
#include "alg/Aes.h"
#include "alg/Base64.h"
#include "alg/sm3sch.h"

WSADATA g_sWSAData;

extern BYTE proxykey[16];//与代理通信的根密钥

/*
* 函数名称: StartSocketWinlib
* 函数功能: 启动WINSOCKET库
* 参    数: NO
* 返 回 值: NO
*/
int StartSocketWinlib(void)
{
    return WSAStartup(MAKEWORD(2,2), &g_sWSAData);
}

/*
* 函数名称: CleanSocketWinlib
* 函数功能: 清理WINSOCKET库
* 参    数: NO
* 返 回 值: NO
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
* 函数名称: ConnectTo
* 函数功能: 连接到服务器
* 参    数: 
* 返 回 值: 
*/
DWORD CTCComSocket::ConnectTo(IN const char *pcServiceIP, IN const int &iPort)
{
    int iRet = 0;
    struct sockaddr_in sServerAddr = {0};
    unsigned long ulArgp = 0;

    //创建SOCKET
    m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if((INVALID_SOCKET == m_hSocket))
    {
        return 0x80000000;
    }
    
    //设置非阻塞方式连接
    ulArgp = 1;
    iRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ulArgp);
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }

    //连接到服务器
    sServerAddr.sin_family = AF_INET;
    sServerAddr.sin_addr.s_addr = inet_addr(pcServiceIP);
    sServerAddr.sin_port = htons(iPort);
    memset(sServerAddr.sin_zero, 0x00, 8);
    iRet = connect(m_hSocket, (struct sockaddr*)&sServerAddr, sizeof(sServerAddr));
    if(SOCKET_ERROR == iRet)
    {
        //等待超时2秒
        struct timeval tm;
        tm.tv_sec = 2;
        tm.tv_usec = 0;
        fd_set set;
        FD_ZERO(&set);
        FD_SET(m_hSocket, &set);
        if(select(m_hSocket+1, NULL, &set, NULL, &tm) > 0)
        {
            //判断当时的errno是否为EINPROGRESS，也就是说是不是连接正在进行中，如果是，继续，如果不是，返回错误。 
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

    //设置阻塞方式
    ulArgp = 0;
    iRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ulArgp);
    if(SOCKET_ERROR == iRet)
    {
         return 0x80000000;
    }

    //设置发送超时
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&m_dwTimeOut, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }
    //设置接收超时
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_dwTimeOut, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }

    //设置发送缓存
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&m_dwCacheSize, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }
    //设置接收缓存
    iRet = ::setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&m_dwCacheSize, sizeof(DWORD));
    if(SOCKET_ERROR == iRet)
    {
        return 0x80000000;
    }
    
    return 0;
}

/*
* 函数名称: Disconnect
* 函数功能: 断开与服务器的连接
* 参    数: 
* 返 回 值: 
*/
DWORD CTCComSocket::Disconnect(void)
{
    closesocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
    return 0;
}

/*
* 函数名称: Send
* 函数功能: 发送数据
* 参    数: 
* 返 回 值: 
*/
DWORD CTCComSocket::Send(IN BYTE *pbDataBuf, IN DWORD &dwDataLen)
{
    int iRet = 0;

    //发送数据
    iRet = send(m_hSocket, (char *)pbDataBuf, dwDataLen, 0);
    if(SOCKET_ERROR == iRet)
    {
        return 0xF105;
    }

    return 0;
}

// 发送数据
DWORD CTCComSocket::SendData(LPSTR xmlresult)
{
	int iRet;

	int xmllength=strlen(xmlresult);
	for(int i=0;i<xmllength;i++)
	{
		xmlresult[i]=toupper(xmlresult[i]); //转换成大写字母
	}
		
	int sendlen = 0, retlen = 0;
	while(sendlen < xmllength){ //数据发送不完全
		iRet = send(m_hSocket, xmlresult+retlen, xmllength-sendlen, 0);
		int rtn=GetLastError();
		if((iRet == 0)||(iRet == SOCKET_ERROR))
		{
			return 0x80010000; //向代理发送数据超时
		}
		sendlen+=iRet;
	}
	return 0;
}

/*
* 函数名称: Recv
* 函数功能: 接收数据
* 参    数: 
* 返 回 值: 
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
//    //接收缓冲区已满
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

// 接收数据
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
			return 0x80020000; //代理返回的数据不完整
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

// 计算校验值
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
	ideapublicfunc(0,proxykey,pro_key,len);//生成过程密钥
	Bcd2Hex(&len,pro_key,ctmp);
	TRACE("pro_key:%s\r\n",ctmp);

	md5publicfunc(xmlresult,ctmp);//生产16字节MD5摘要值
	TRACE("MD5:%s\r\n",ctmp);
	Hex2Bcd(ctmp,&len,btmp);
	ideapublicfunc(0,pro_key,btmp,len);//加密16字节MD5摘要值
	
	Bcd2Hex(&len,btmp,verify);
	for(int i=0;i<(int)strlen(verify);i++)
		verify[i]=toupper(verify[i]); //转换成大写字母
	TRACE("Verify:%s\r\n",verify);
	strReturn=verify;
	return strReturn;
}

// 校验校验值
DWORD CTCComSocket::CheckVerify(LPSTR xmlresult)
{
	char* ctmp=new char[strlen(xmlresult)+1];
	char client_verify[64],proxy_verify[64];
	
	char* pos=strstr(xmlresult,"<!--CLIENTVERIFY:");
	if(pos==NULL) //接收的数据中找不到相应标签
	{
		delete ctmp;
		return 0x80020004; //接收的数据中找不到相应标签
	}
	memcpy(ctmp,xmlresult,pos-xmlresult);ctmp[pos-xmlresult]=0; //获取待校验的明文字符串
	strcpy(client_verify,GetVerify(ctmp)); //产生32位校验值
	delete ctmp;
	memcpy(proxy_verify,pos+17,32);proxy_verify[32]=0;
	if(strcmp(client_verify,proxy_verify))
	{
		return 0x80020002; //校验值比对不通过
	}
	return 0;//校验值比对通过
}

// 客户端身份验证，在每次服务器连接后
DWORD CTCComSocket::CheckClient()
{
	DWORD rtn;
	char xmlresult[1024],ipaddr[64],ctmp[1024];
	
	GetIPAddress(ipaddr); //获得客户端IP地址
	m_ClientRand=ZLib_GetRand(16);
	sprintf(xmlresult,"%s<CLIENTIDENTIFY>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8)); //随机生成16位Tag值
	sprintf(xmlresult,"%s<CLIENTIP>%s</CLIENTIP>\r\n",xmlresult,ipaddr);
	sprintf(xmlresult,"%s<CLIENTRAND>%s</CLIENTRAND>\r\n</CLIENTIDENTIFY>\r\n",xmlresult,m_ClientRand);
	sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);

	rtn=SendData(xmlresult);
	if(rtn!=0) return rtn;
	rtn=RecvData(xmlresult);
	if(rtn!=0) return rtn;

	rtn=GetXmlNode(xmlresult,"错误",ctmp);
	if(rtn==0)
	{
		sscanf(ctmp,"%x",&rtn);
		return rtn;
	}

	rtn=GetXmlNode(xmlresult,"PROXYRAND",ctmp);//获取发行代理的32位随机数
	if(rtn!=0) return rtn;
	m_ProxyRand=ctmp;

	return CheckVerify(xmlresult);//校验数据包的校验值
}

// 时钟模块安全命令(数据加解密)
DWORD CTCComSocket::RTCEnDecrypt(BYTE bDealType, LPSTR sDiskNumber, LPSTR sChallenge, LPSTR sCmdHeader, LPSTR sData, LPSTR sRespBuffer) 
{
	DWORD rtn;
	char xmlresult[1024],ctmp[64];

	sprintf(xmlresult,"%s<模块安全命令>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8));
	sprintf(xmlresult,"%s<盘编号>0000%s</盘编号>\r\n",xmlresult,sDiskNumber);
	sprintf(xmlresult,"%s<分散因子>%s</分散因子>\r\n",xmlresult,sChallenge);
	sprintf(xmlresult,"%s<命令字>%s</命令字>\r\n",xmlresult,sCmdHeader);
	sprintf(xmlresult,"%s<数据>%s</数据>\r\n",xmlresult,sData);
	sprintf(xmlresult,"%s<操作类型>%d</操作类型>\r\n</模块安全命令>\r\n",xmlresult,bDealType);
	sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);
	
	rtn=SendData(xmlresult);
	if(rtn!=0) return rtn;
	rtn=RecvData(xmlresult);
	if(rtn!=0) return rtn;
	rtn=GetXmlNode(xmlresult,"错误",ctmp);
	if(rtn==0)
	{
		sscanf(ctmp,"%x",&rtn);
		return rtn;
	}
	rtn=CheckVerify(xmlresult);
	if(rtn!=0) return rtn;
	return GetXmlNode(xmlresult,"数据加密结果",sRespBuffer);
}

// 解密发票监控信息块
DWORD CTCComSocket::DecryptRTCInvoice(LPSTR sSKDiskNumber, LPSTR sCSDiskNumber, LPSTR sRTCInvoice, LPSTR sRespBuffer) 
{
	DWORD rtn;
	char challenge[64],clock[64],ctmp1[256],ctmp2[256];

	sscanf(sRTCInvoice,"%96s%16s",ctmp1,clock);
	CalReversedCode(clock,ctmp2); //计算时间数据的反码
	sprintf(challenge,"%s%s",clock,ctmp2); //组成16字节分散因子

	if(strlen(sCSDiskNumber)==12) //传输盘报税
	{
		rtn=RTCEnDecrypt(1,sCSDiskNumber,challenge,sSKDiskNumber,ctmp1,ctmp2); //解密发票信息块
	}
	else //税控盘报税
	{
		rtn=RTCEnDecrypt(1,sSKDiskNumber,challenge,"",ctmp1,ctmp2); //解密发票信息块
	}
	if(rtn) return rtn;
	if(strlen(ctmp2)!=96)
	{
		return 0x80020005; //插件接收数据有误
	}
	//进行数据符合性验证，只返回前40字节发票信息块
	rtn=DealWithRTCData(1,ctmp2,sRespBuffer);
	if(rtn)
	{
		return 0x09FFF0; //时钟模块数据符合性验证不通过
	}
	return 0;
}


// 加密时钟监控数据
DWORD CTCComSocket::EncryptRTCInvoice(LPSTR sDiskNo, LPSTR sRTCInvoice, LPSTR sRespBuffer) 
{
	DWORD rtn;
	char challenge[64],clock[64],ctmp1[255],ctmp2[255];
	
	if(strlen(sRTCInvoice)!=96) return -3;
	sscanf(sRTCInvoice,"%80s%16s",ctmp2,clock);
	rtn=DealWithRTCData(0,ctmp2,ctmp1); //进行数据符合变换，40字节到48字节变换
	if(rtn) return -3;
	CalReversedCode(clock,ctmp2); //计算时间数据的反码
	sprintf(challenge,"%s%s",clock,ctmp2); //组成16字节分散因子
	rtn=RTCEnDecrypt(0,sDiskNo,challenge,"",ctmp1,ctmp2); //加密发票信息块
	if(rtn) return rtn;
	sprintf(sRespBuffer,"%s%s",ctmp2,clock); //时钟仍然用明文，组成56字节发票监控信息块
	return 0;
}

// 数据加密(目前所有加密都是在应用级下)(内部调用)
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
		
		sprintf(xmlresult,"%s<数据加密>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8));
		sprintf(xmlresult,"%s<盘编号>0000%s</盘编号>\r\n",xmlresult,disknumber);
		sprintf(xmlresult,"%s<分散因子>%s</分散因子>\r\n",xmlresult,sFactor);
		if(len<MAXENCRYPTTLEN)
		{
			sprintf(xmlresult,"%s<数据>%s</数据>\r\n",xmlresult,FillinData(ctmp));
		}
		else
		{			
			sprintf(xmlresult,"%s<数据>%s</数据>\r\n",xmlresult,ctmp);
		}
		sprintf(xmlresult,"%s<密钥标识>%s</密钥标识>\r\n</数据加密>\r\n",xmlresult,sKeyID);
		sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);
		
		rtn=SendData(xmlresult);
		if(rtn!=0) return rtn;
		rtn=RecvData(xmlresult);
		if(rtn!=0) return rtn;	
		rtn=GetXmlNode(xmlresult,"错误",ctmp);
		if(rtn==0)
		{
			sscanf(ctmp,"%x",&rtn);
			return rtn;
		}
		rtn=CheckVerify(xmlresult);
		if(rtn!=0) return rtn;
		
		rtn=GetXmlNode(xmlresult,"数据加密结果",ctmp);
		if(rtn!=0) return rtn;
		strcat(sRespBuffer,ctmp);
		pos+=len;
	}
	return 0;
}

// 发行代理数据解密
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
		
		sprintf(xmlresult,"%s<数据解密>\r\n<TAG>%s</TAG>\r\n",xmlhead,ZLib_GetRand(8));
		sprintf(xmlresult,"%s<盘编号>0000%s</盘编号>\r\n",xmlresult,sDiskNumber);
		sprintf(xmlresult,"%s<分散因子>%s</分散因子>\r\n",xmlresult,sFactor);
		if(len<MAXENCRYPTTLEN)
		{
			sprintf(xmlresult,"%s<数据>%s</数据>\r\n",xmlresult,FillinData(ctmp));
		}
		else
		{			
			sprintf(xmlresult,"%s<数据>%s</数据>\r\n",xmlresult,ctmp);
		}
		sprintf(xmlresult,"%s<密钥标识>%s</密钥标识>\r\n</数据解密>\r\n",xmlresult,sKeyID);
		sprintf(xmlresult,"%s<!--CLIENTVERIFY:%s>\r\n%s",xmlresult,GetVerify(xmlresult),xmlend);
		
		rtn=SendData(xmlresult);
		if(rtn!=0) return rtn;
		rtn=RecvData(xmlresult);
		if(rtn!=0) return rtn;	
		rtn=GetXmlNode(xmlresult,"错误",ctmp);
		if(rtn==0)
		{
			sscanf(ctmp,"%x",&rtn);
			return rtn;
		}
		rtn=CheckVerify(xmlresult);
		if(rtn!=0) return rtn;
		
		GetXmlNode(xmlresult,"数据解密结果",ctmp);
		strcat(sRespBuffer,ctmp);
		pos+=len;
	}
	return 0;
}

// 网上抄报数据解密函数
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

	unsigned char KEYt[256] = {0};			//实际数据长度为32字节
	unsigned char KEYp[256] = {0};			//未知
	unsigned char Hash[256] = {0};			//实际数据长度为32字节
	unsigned char CodeHead[256] = {0};		//实际数据长度为64字节

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
		return -101;	//输入参数错误
	}

	strInData = InData;
	strBase64 = myBase64.Decode(strInData,strInData.GetLength(),inlen);
	strInData = strBase64.c_str();

	if(inlen < 81)
	{
		return -102;	//输入参数错误
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
		return -103;	//税务应用类型错误
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
		ret = ProxyDecrypt("3001", DiskNo, InCodeHead+i, "", OutCodeHead+i); //先解密
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
	sch256_digest(indatabin+16+64, inlen-16-64, Hash, &dig_len); //sch256_digest函数就是计算SM3哈希值的函数，输入长度任意，输出为32字节的哈希值
	strHash2 = Hash;

	if (strHash1.Compare(strHash2) != 0)
	{
		delete []indatabin;
		return -104;     //数据校验失败
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
