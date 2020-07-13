/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* 文件名称: TC_ComSocket.h
* 文件摘要: PC机和设备之间Socket通信类
*
* 当前版本:  
*
* 历史版本: 1.0，税控研发组，2013-11-25
*           a) 创建并实现
*/

#ifndef _TC_COMSOCKET_H_
#define _TC_COMSOCKET_H_

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
* 函数名称: StartSocketWinlib
* 函数功能: 启动WINSOCKET库
* 参    数: NO
* 返 回 值: NO
*/
int StartSocketWinlib(void);

/*
* 函数名称: CleanSocketWinlib
* 函数功能: 清理WINSOCKET库
* 参    数: NO
* 返 回 值: NO
*/
int CleanSocketWinlib(void);

#define TC_COMSOCKET_TIMEOUT                    5000        //默认5秒
#define TC_COMSOCKET_RECV_COUNT                 2000        //循环2000次
#define TC_COMSOCKET_CMD_SIZE                   16          //命令长度 
#define TC_COMSOCKET_DATA_SIZE                  0x10000     //数据长度 64KB
#define TC_COMSOCKET_CACHE_SIZE                 0x10400     //缓存区长度 65KB

#define TRANSFERBUFSIZE 17000 //稍微大于16K
#define MAXENCRYPTTLEN 16384 //16k


class CTCComSocket
{

public:    

    CTCComSocket();
    virtual ~CTCComSocket();

    /*
    * 函数名称: ConnectTo
    * 函数功能: 连接到服务器
    * 参    数: 
    * 返 回 值: 
    */
    DWORD ConnectTo(IN const char *pcServiceIP, IN const int &iPort);
        
    /*
    * 函数名称: Disconnect
    * 函数功能: 断开与服务器的连接
    * 参    数: 
    * 返 回 值: 
    */
    DWORD Disconnect(void);

    /*
    * 函数名称: Send
    * 函数功能: 发送数据
    * 参    数: 
    * 返 回 值: 
    */
    DWORD Send(IN BYTE *pbDataBuf, IN DWORD &dwDataLen);
    
    /*
    * 函数名称: Recv
    * 函数功能: 接收数据
    * 参    数: 
    * 返 回 值: 
    */
    DWORD Recv(OUT BYTE *pbDataBuf, IN OUT DWORD &dwDataLen);

	DWORD SendData(LPSTR xmlresult);

	DWORD RecvData(LPSTR xmlresult);

	CString GetVerify(LPSTR xmlresult);

	DWORD CheckVerify(LPSTR xmlresult);

	DWORD CheckClient();

	DWORD RTCEnDecrypt(BYTE bDealType, LPSTR sDiskNumber, LPSTR sChallenge, LPSTR sCmdHeader, LPSTR sData, LPSTR sRespBuffer);
	DWORD DecryptRTCInvoice(LPSTR sSKDiskNumber, LPSTR sCSDiskNumber, LPSTR sRTCInvoice, LPSTR sRespBuffer);
	DWORD EncryptRTCInvoice(LPSTR sDiskNo, LPSTR sRTCInvoice, LPSTR sRespBuffer);

	DWORD DataEncrypt(BYTE ucType, LPSTR sKeyID, LPSTR disknumber, LPSTR sData, LPSTR sFactor, LPSTR sRespBuffer);

	DWORD ProxyDecrypt(LPSTR sKeyID, LPSTR disknumber, LPSTR sData, LPSTR sFactor, LPSTR sRespBuffer);
	INT UploadDecrypt(BYTE AppType, LPSTR InData, LPSTR OutData);
    
private:

     SOCKET m_hSocket;                  //socket handle
     DWORD m_dwTimeOut;
     DWORD m_dwCacheSize;
	 CString m_ClientRand;
	 CString m_ProxyRand;
	 CString m_ErrorCode;
	 CString xmlhead,xmlend;
};

#endif