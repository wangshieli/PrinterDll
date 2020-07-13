/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* �ļ�����: TC_ComSocket.h
* �ļ�ժҪ: PC�����豸֮��Socketͨ����
*
* ��ǰ�汾:  
*
* ��ʷ�汾: 1.0��˰���з��飬2013-11-25
*           a) ������ʵ��
*/

#ifndef _TC_COMSOCKET_H_
#define _TC_COMSOCKET_H_

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
* ��������: StartSocketWinlib
* ��������: ����WINSOCKET��
* ��    ��: NO
* �� �� ֵ: NO
*/
int StartSocketWinlib(void);

/*
* ��������: CleanSocketWinlib
* ��������: ����WINSOCKET��
* ��    ��: NO
* �� �� ֵ: NO
*/
int CleanSocketWinlib(void);

#define TC_COMSOCKET_TIMEOUT                    5000        //Ĭ��5��
#define TC_COMSOCKET_RECV_COUNT                 2000        //ѭ��2000��
#define TC_COMSOCKET_CMD_SIZE                   16          //����� 
#define TC_COMSOCKET_DATA_SIZE                  0x10000     //���ݳ��� 64KB
#define TC_COMSOCKET_CACHE_SIZE                 0x10400     //���������� 65KB

#define TRANSFERBUFSIZE 17000 //��΢����16K
#define MAXENCRYPTTLEN 16384 //16k


class CTCComSocket
{

public:    

    CTCComSocket();
    virtual ~CTCComSocket();

    /*
    * ��������: ConnectTo
    * ��������: ���ӵ�������
    * ��    ��: 
    * �� �� ֵ: 
    */
    DWORD ConnectTo(IN const char *pcServiceIP, IN const int &iPort);
        
    /*
    * ��������: Disconnect
    * ��������: �Ͽ��������������
    * ��    ��: 
    * �� �� ֵ: 
    */
    DWORD Disconnect(void);

    /*
    * ��������: Send
    * ��������: ��������
    * ��    ��: 
    * �� �� ֵ: 
    */
    DWORD Send(IN BYTE *pbDataBuf, IN DWORD &dwDataLen);
    
    /*
    * ��������: Recv
    * ��������: ��������
    * ��    ��: 
    * �� �� ֵ: 
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