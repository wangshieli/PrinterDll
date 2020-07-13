// modify at 2012.1
//
#ifndef __ICSCRYPTAPI_H__
#define __ICSCRYPTAPI_H__

#include "basetype.h"
#include "crypt_err.h"

#ifdef __cplusplus
extern "C" {
#endif



// �����㷨ID����

#define CRYPT_ALGID_NONE				0x00000000
#define CRYPT_ALGID_RSA_PKCS			0x00000001	//RSA��Կ�����㷨
#define CRYPT_ALGID_RSA_X_509			0x00000003
#define CRYPT_ALGID_MD5_RSA_PKCS        0x00000005
#define CRYPT_ALGID_SHA1_RSA_PKCS       0x00000006
#define CRYPT_ALGID_SHA256_RSA_PKCS     0x00000040
#define CRYPT_ALGID_SHA512_RSA_PKCS     0x00000042

#define CRYPT_ALGID_GB_ECC				0x00000010  //��Բ���߹�Կ�����㷨:��Կ���ܡ�˽Կ���ܡ�˽Կ���ܡ���Կ����
#define CRYPT_ALGID_GB_ECDSA_SHA1		0x00000012  //SHA1+��Բ����ǩ���㷨
#define CRYPT_ALGID_GB_ECDSA_SHA256		0x00000013  //SHA256+��Բ����ǩ���㷨
#define CRYPT_ALGID_GB_ECDSA_SHA512		0x00000015  //SHA512+��Բ����ǩ���㷨
#define CRYPT_ALGID_GB_ECDSA_SM3		0x00000017  //SM3�Ӵ��㷨+��Բ����ǩ���㷨
#define CRYPT_ALGID_GB_ECDH				0x00000018  //��Բ������Կ����Э��

#define CRYPT_ALGID_GB_SM3				0x00000021	//GB SM3�Ӵ��㷨
#define CRYPT_ALGID_GB_SHA1				0x00000022  //GB SHA1
#define CRYPT_ALGID_GB_SHA256			0x00000023  //GB SHA256
#define CRYPT_ALGID_GB_SHA512			0x00000025  //GB SHA512

/*
//(����?)�ǶԳ��㷨��ʶ
#define SGD_RSA						0x00010000	//RSA�㷨
#define SGD_SM2_1					0x00020100	//��Բ����ǩ���㷨
#define SGD_SM2_2					0x00020200	//��Բ������Կ����Э��
#define SGD_SM2_3					0x00020400	//��Բ���߼����㷨

#define SGD_SM3						0x00000001	//SM3�Ӵ��㷨
#define SGD_SHA1					0x00000002	//SHA1�Ӵ��㷨
#define SGD_SHA256					0x00000004	//SHA256�Ӵ��㷨
#define SGD_SHA512					0x00000008	//SHA512�Ӵ��㷨
#define S_HASH_HARD128				0x00000040	//����Ӳ��Hash�㷨
#define S_HASH_MD5					0x00000080	//MD5
*/

// (����?)�Գ��㷨 
#define SGD_SM1_ECB					0x00000101	//SM1�㷨ECB����ģʽ
#define SGD_SM1_CBC					0x00000102	//SM1�㷨CBC����ģʽ
#define SGD_SM1_CFB					0x00000104	//SM1�㷨CFB����ģʽ
#define SGD_SM1_OFB					0x00000108	//SM1�㷨OFB����ģʽ
#define SGD_SM1_MAC					0x00000110	//SM1�㷨MAC����ģʽ
#define SGD_SSF33_ECB				0x00000201	//SSF33�㷨ECB����ģʽ
#define SGD_SSF33_CBC				0x00000202	//SSF33�㷨CBC����ģʽ
#define SGD_SSF33_CFB				0x00000204	//SSF33�㷨CFB����ģʽ
#define SGD_SSF33_OFB				0x00000208	//SSF33�㷨OFB����ģʽ
//#define SGD_SSF33_MAC				0x00000210	//SSF33�㷨MAC����ģʽ
#define SGD_SMS4_ECB				0x00000401	//SMS4�㷨ECB����ģʽ
#define SGD_SMS4_CBC				0x00000402	//SMS4�㷨CBC����ģʽ
#define SGD_SMS4_CFB				0x00000404	//SMS4�㷨CFB����ģʽ
#define SGD_SMS4_OFB				0x00000408	//SMS4�㷨OFB����ģʽ
#define SGD_SMS4_MAC				0x00000410	//SMS4�㷨MAC����ģʽ

#define CRYPT_ALGID_GB_SSF33		SGD_SSF33_ECB	
#define CRYPT_ALGID_GB_SM1			SGD_SM1_ECB	
#define CRYPT_ALGID_GB_SMS4			SGD_SMS4_ECB	
#define CRYPT_ALGID_GB_SMS4_CBC		SGD_SMS4_CBC	

#define CRYPT_ALGID_MD5					0x00000210
#define CRYPT_ALGID_MD5_HMAC			0x00000211
#define CRYPT_ALGID_SHA1				0x00000220
#define CRYPT_ALGID_SHA1_HMAC			0x00000221
#define CRYPT_ALGID_SHA256				0x00000250	//SHA256�Ӵ��㷨
#define CRYPT_ALGID_SHA384				0x00000260
#define CRYPT_ALGID_SHA512				0x00000270	//SHA512�Ӵ��㷨

#define CRYPT_ALGID_ECC					0x00001040  //��Բ����ǩ���㷨
#define CRYPT_ALGID_ECDSA				0x00001041  //��Բ����ǩ���㷨
#define CRYPT_ALGID_ECDSA_SHA1			0x00001042  //SHA1+��Բ����ǩ���㷨
#define CRYPT_ALGID_ECDSA_SHA256		0x00001044  //SHA256+��Բ����ǩ���㷨
#define CRYPT_ALGID_ECDSA_SHA384		0x00001045  //SHA384+��Բ����ǩ���㷨
#define CRYPT_ALGID_ECDSA_SHA512		0x00001046  //SHA512+��Բ����ǩ���㷨


#define CRYPT_ALGID_RC4_KEY_GEN			0x00000110
#define CRYPT_ALGID_RC4					0x00000111

#define CRYPT_ALGID_DES_KEY_GEN                0x00000120
#define CRYPT_ALGID_DES_ECB                    0x00000121
#define CRYPT_ALGID_DES_CBC                    0x00000122
#define CRYPT_ALGID_DES_MAC                    0x00000123
#define CRYPT_ALGID_DES_MAC_GENERAL            0x00000124
#define CRYPT_ALGID_DES_CBC_PAD                0x00000125
#define CRYPT_ALGID_DES2_KEY_GEN               0x00000130
#define CRYPT_ALGID_DES3_KEY_GEN               0x00000131
#define CRYPT_ALGID_DES3_ECB                   0x00000132
#define CRYPT_ALGID_DES3_CBC                   0x00000133
#define CRYPT_ALGID_DES3_MAC                   0x00000134
#define CRYPT_ALGID_DES3_MAC_GENERAL           0x00000135
#define CRYPT_ALGID_DES3_CBC_PAD               0x00000136

#define CRYPT_ALGID_IDEA_KEY_GEN               0x00000340
#define CRYPT_ALGID_IDEA_ECB                   0x00000341
#define CRYPT_ALGID_IDEA_CBC                   0x00000342
#define CRYPT_ALGID_IDEA_CBC_PAD               0x00000345

#define CRYPT_ALGID_UEA							0x80000010
#define CRYPT_ALGID_SSF33						0x80001061
#define CRYPT_ALGID_SSF33_KEY_GEN				0x80004113


#define PUBKEY_ISSUER_SERIALNUMBER	0x10
#define PUBKEY_CERTIFICATE			0x20
#define PUBKEY_RSA_PKCS1			0x30
#define PUBKEY_RSA_MODULUS			0x40
#define PUBKEY_ISSUER_USERID		0x50	//  (51,52)֤��ǩ���ߺ��û���ʶ,��ʾΪ: issuer\userid

#define SIGN_INCLUDE_NONE			0			// ��ǩ���в��������ݺ�֤��
#define SIGN_INCLUDE_DATA			1			// ��ǩ���а�������
#define SIGN_INCLUDE_CERT			2			// ��ǩ���а���֤��
#define SIGN_EXCLUDE_SIGN			4			// ��ǩ���в�����ǩ��
#define SIGN_HASHED_DATA			0x10		// ��ǩ����Ϊ��HASH��������


typedef void * HCRYPTPROV;

#ifndef TYPE_DATE_TIME
#define TYPE_DATE_TIME

typedef struct _DATE_TIME {
	unsigned short	year;
	unsigned char	month;
	unsigned char	day;
	unsigned char	hour;
	unsigned char	min;
	unsigned char	sec;
	unsigned char	x;
} DATE_TIME, *LPDATE_TIME;

#endif

#define __PASTE(x,y)      x##y

#ifdef CRYPT_FUNCTION_INFO
#undef CRYPT_FUNCTION_INFO
#endif

#define CRYPT_FUNCTION_INFO(name) \
  int __stdcall name


/*****************************************************************************
 *  ע��
 *  ���к����ķ���ֵΪ0ʱ��ȷ����0ʱΪ�����
 *  ͨ������ Crypt_GetErrorMsg ����ȡ�ش�����Ϣ
 *****************************************************************************/

/*----------------------------------------------------------------------------
 *  �򿪼����豸,���ؾ��
 *---------------------------------------------------------------------------*/

CRYPT_FUNCTION_INFO(Crypt_OpenDevice)
(
		HCRYPTPROV	*phProv,			// out ��������Ļ������
		LPTSTR		pszContainer,		// in/out �����豸��Կ�е�����
		LPTSTR		pszProvider,		// in �����豸������
		DWORD		dwProvType,			// in �����豸������
		DWORD		dwFlags				// in ����
);

/*----------------------------------------------------------------------------
 *  �رռ����豸
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_CloseDevice)(HCRYPTPROV hProv);


/*----------------------------------------------------------------------------
 *  ����֤��
 *  dwCertNum(֤���):0,AT_KEYEXCHANGE(=1),AT_SIGNATURE(=2)
 *  0:CACERT 1:���ܹ�Կ֤�� 2:ǩ����Կ֤��,
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ReadCert)
(
		HCRYPTPROV	hProv,
		DWORD		dwCertNum,		// in ѡ���֤���
		BYTE		*pbCert,		// out ������֤��
		DWORD		*pcbCert		// in/out ����������/������֤�鳤��
);

/*----------------------------------------------------------------------------
 *  ͨ��ǩ�������������û���Ϣ,��ȡ�û���֤��
 * dwKeySpec:
 * AT_KEYEXCHANGE				1 -- ���ܹ�Կ֤��
 * AT_SIGNATURE					2 -- ǩ����Կ֤��
 * PUBKEY_ISSUER_SERIALNUMBER	0x10--֤��ǩ���ߺ����к�
 * PUBKEY_ISSUER_USERID			0x50(0x51,0x52)	֤��ǩ���ߺ��û���ʶ,��ʾΪ: issuer\userid
 * PUBKEY_SERIALNUMBER			0x60 ֤�����к�
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_FindUserCert)(
		HCRYPTPROV	hProv,					// in �򿪵������豸�����NULL
		BYTE		*pbUserInfo,			// in �û���Ϣ
		DWORD		cbUserInfo,				// in �ֽ���
		DWORD		dwKeySpec,				// in �û���Ϣ���
		BYTE		*pbCert,				// out ����֤��
		DWORD		*pcbCert);				// in/out ����������/֤�鳤��	

/*-------------------------------------------------------------------------
 * ��������HASH����: 
 * Algid=CRYPT_ALGID_MD5,CRYPT_ALGID_SHA1
 *-------------------------------------------------------------------------
 */
CRYPT_FUNCTION_INFO(Crypt_Hash)(
		HCRYPTPROV	hProv,
		BYTE		*pbData,		// in ����
		DWORD		cbData,			// in ���ݳ���
		DWORD		dwHashAlgId,	// in HASH�㷨CRYPT_ALGID_MD5,CRYPT_ALGID_MD5
		BYTE		*pbHash,		// out HASH���
		DWORD		*pcbHash);		// in/out ����������/HASH�������


/*-------------------------------------------------------------------------
 * ������������ǩ��: 
 *  dwSignAlgId=CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS��CRYPT_ALGID_SHA1_RSA_PKCS
 *  dwKeySpec(RSA˽Կ��ʶ):AT_KEYEXCHANGE(=1),AT_SIGNATURE(=2)
 *  �ȶ�����HASH����ǩ����
 *-------------------------------------------------------------------------
 */
CRYPT_FUNCTION_INFO(Crypt_Sign)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbData,			// in ����
		DWORD		cbData,				// in ���ݳ���
		DWORD		dwSignAlgId,		// in CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS��CRYPT_ALGID_SHA1_RSA_PKCS
		DWORD		dwKeySpec,			// in ˽Կ��ʶ
		BYTE		*pbSignature,		// out ǩ�����
		DWORD		*pcbSignature);		// in/out ǩ����ĳ���/ʵ�ʳ���

CRYPT_FUNCTION_INFO(Crypt_SignHash)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbHash,			// in ��ǩ��HASH���
		DWORD		cbHash,				// in HASH����ֽ���
		DWORD		dwSignAlgId,		// in CRYPT_ALGID_MD5_RSA_PKCS��CRYPT_ALGID_SHA1_RSA_PKCS
		DWORD		dwKeySpec,			// in ˽Կ��ʶ
		BYTE		*pbSignature,		// out ǩ�����
		DWORD		*pcbSignature);		// in/out ����������/ʵ�ʳ���

/*----------------------------------------------------------------------------
 *  ʹ�ÿ��ڹ�Կ��֤ǩ������ʹ���ⲿ��Կ��֤ǩ��
 *  dwSignAlgId=CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS��CRYPT_ALGID_SHA1_RSA_PKCS
 *  pbData ǩ��Դ����
 *  dwKeySpec(��Կ��ʶ): 
 *  (dwKeySpec�ĺ���μ�Crypt_PublicEncrypt��˵��)
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_VerifySign)(
		HCRYPTPROV	hProv,
		BYTE		*pbData,			// in ǩ��Դ����
		DWORD		cbData	,			// in Դ���ݳ���
		DWORD		dwSignAlgId,		// in CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS��CRYPT_ALGID_SHA1_RSA_PKCS
		DWORD		dwKeySpec,			// in RSA��Կ��ʶ
		BYTE		*pbSignature,		// in ����֤��ǩ��
		DWORD		cbSignature,		// in ǩ������
		BYTE		*pbPubKeyInfo,		// in ��Կ��Ϣ
		DWORD		cbPubKeyInfo);		// in ��Կ��Ϣ����


/*----------------------------------------------------------------------------
 *  �û���¼����֤����
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_Login)(
		HCRYPTPROV	hProv,
		char		*pPwd);			// in ���

/*----------------------------------------------------------------------------
 *  ��֤�޸Ŀ���
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ChangePin)(
		HCRYPTPROV	hProv,
		char		*pOldPwd,			// in ���
		char		*pNewPwd);			// in ���

/*----------------------------------------------------------------------------
 *  �˳���¼
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_Logout)(HCRYPTPROV hProv);


/*----------------------------------------------------------------------------
 * ��ȡ�������Ӧ�Ĵ�����Ϣ
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_GetErrorMsg)(
		int			nErrCode,			// in �������
		char		*pszError,			// out ���ش�����Ϣ
		int			nBufLen);			// in ����������



// ������ǩ��������PKCS7��ʽǩ������SignedData
//
CRYPT_FUNCTION_INFO(Crypt_SignData)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbData,			// in ��ǩ������
		DWORD		cbData,				// in ��ǩ�����ֽ���
		DWORD		dwSignAlgId,		// in ǩ���㷨(ʵ��ΪHASH�㷨)
		DATE_TIME	*signTime,			// in ǩ������ʱ��
		DWORD		dwFlags,			// in 0,SIGN_INCLUDE_DATA,SIGN_INCLUDE_CERT
		BYTE		*pbSignedData,		// out ǩ�����
		DWORD		*pcbSignedData);	// in/out ����������/ʵ�ʳ���

// ��֤PKCS7��ʽ��ǩ������
//
CRYPT_FUNCTION_INFO(Crypt_VerifySignedData)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbSignedData,		// in ǩ�����
		DWORD		cbSignedData,		// in ǩ���������
		DWORD		dwFlags,			// in 0,SIGN_HASHED_DATA
		BYTE		*pbData,			// in ��ǩ������(����ǩ���ݷ�����ʱʹ�ã������ΪNULL)
		DWORD		cbData,				// in ��ǩ�����ֽ���
		BYTE		*pbCert,			// in ǩ����֤��(����ΪNULL)
		DWORD		cbCert);			// in ǩ����֤�鳤��

// ����PKCS7��ʽ�������ŷ�
// ��dwSignAlgId != 0ʱ�������ŷ��ǩ��
//
CRYPT_FUNCTION_INFO(Crypt_EnvelopData)(
		HCRYPTPROV	hProv,
		BYTE		*pbData,			// in ����
		DWORD		cbData,				// in ���ݳ���
		DWORD		dwEncAlgId,			// in �����㷨
		BYTE		*pbRecipientCert,	// in ������֤��
		DWORD		cbRecipientCert,	// in ������֤�鳤��
		DWORD		dwSignAlgId,		// in ǩ���㷨(ʵ��ΪHASH�㷨)
		DATE_TIME	*signTime,			// in ǩ��ʱ��(��dwSignAlgIdΪ0ʱ��ΪNULL)
		DWORD		dwFlags,			// in 0,SIGN_INCLUDE_DATA,SIGN_INCLUDE_CERT
		BYTE		*pbEnvelopedData,	// out ��װ���
		DWORD		*pcbEnvelopedData);	// in/out ����������/��װ�������

// ��PKCS7��ʽ�������ŷ⣬�������ǩ��������֤ǩ��
// ���ؼ�����Կ�ͽ��ܺ�ı���װ����
//
CRYPT_FUNCTION_INFO(Crypt_VerifyEnvelopedData)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbEnvelopedData,	// in �����ŷ�
		DWORD		cbEnvelopedData,	// in �����ŷⳤ��
		BYTE		*pbEncedData,		// in ���ܵ�����(�����ݷ�����ʱʹ�ã�����ΪNULL)
		DWORD		cbEncedData,		// in ���������ֽ���
		BYTE		*pbCert,			// in ǩ����֤��(����ΪNULL)
		DWORD		cbCert,				// in ǩ����֤�鳤��
		BYTE		*pbData,			// out ���ر���װԭ����
		DWORD		*pcbData);			// in/out ����װ�����ֽ���


/*----------------------------------------------------------------------------
 *  ���ɿͻ����������ݰ�ClientHello
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ClientHello)(
		HCRYPTPROV hProv,
		DWORD		dwFlags,			// in 0:������֤��1��˫��2������Լ����Կ��3��˫��
		BYTE		*pbClientHello,		// out ���ݰ�
		DWORD		*pcbClientHello);	// in/out ����������/������

/*----------------------------------------------------------------------------
 * �����������ɻ�Ӧ���ݰ�(ServerHello)
 * ���ɵ�pbServerHello���������ͻ���
 * ���ɵ�pbServerRandom�ɵ��������ѱ��棬���ڶԿͻ���ݽ�����֤
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ServerHello)(
		HCRYPTPROV hProv,
		BYTE		*pbClientHello,			// in �ͻ����������ݰ�
		DWORD		cbClientHello,			// in �ͻ����������ݰ�����
		BYTE		*pbServerHello,			// out ������Ӧ�����ݰ�
		DWORD		*pcbServerHello,		// in/out ����������/���ݰ�����
		BYTE		*pbServerRandom,		// out ���������ɵ������
		DWORD		*pcbServerRandom);		// in/out ����������/���������


/*----------------------------------------------------------------------------
 * ��֤ServerHello,���ɿͻ�����֤����
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ClientAuth)(
		HCRYPTPROV	hProv,
		BYTE		*pbServerHello,		// in ����������Ϣ
		DWORD		cbServerHello,		// in
		BYTE		*pbClientAuth,		// out �ͻ�����֤��
		DWORD		*pcbClientAuth);	// in/out


/*----------------------------------------------------------------------------------------
 * ����������֤�ͻ������
 * �������pbServerRandom!=NULL,��֤ʱҪ�Ƚ�pbServerRandom��ͻ��ϴ�������
 * ����ֻ���ʱ���
 *---------------------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ServerAuth)(
		HCRYPTPROV hProv,
		BYTE		*pbClientAuth,			// in �ͻ����ϴ�����֤����
		DWORD		cbClientAuth,			// in ��֤���ݰ�����
		BYTE		*pbServerRandom,		// in ���������ɵ������
		DWORD		cbServerRandom,			// in ���������
		BYTE		*pbClientInfo,			// out �ͻ�ID��֤��
		DWORD		*pcbClientInfo,			// in/out
		BYTE		*pbSessionKey,			// out �Ự��Կ
		DWORD		*pcbSessionKey);		// in/out


// ����������ת��ΪBase64�ַ���
// �ɹ�ʱ����0, ʧ��ʱΪ������
//
CRYPT_FUNCTION_INFO(Crypt_BinaryToBase64String)
(
		BYTE	*pbBinary,		//in 2��������
		DWORD	cbBinary,		//in ���鳤��
		DWORD	dwFlags,		//in
		LPSTR	pszString,		//out ���Base64�ִ�
		DWORD	*pcchString);	//in/out ����������/�����ʵ�ʳ���

// Base64��ת����2�����ֽ�����
//
CRYPT_FUNCTION_INFO(Crypt_Base64StringToBinary)
(
		char	*pszString,			//in �ַ���(�ɺ��س����з���ͷβ���)
		DWORD	cchString,			//in �ַ�����󳤶�
		BYTE	*pbBinary,			//out �������������Ļ�����(����ΪNULL)
		DWORD	*pcbBinary);		//in/out ����������/���س���

// 16���ƴ�ת����2�����ֽ�����
// ֻ֧����������16���ƴ�����
// #define CRYPT_STRING_TYPE_HEX					0x00000010
// #define CRYPT_STRING_TYPE_HEXCRLF				0x00000011
//
int __stdcall Crypt_HexStringToBinary(
									  char	*pszString,			//in �ַ���(�ɺ��س����з�)
									  DWORD	cchString,			//in �ַ�����󳤶�
									  BYTE	*pbBinary,			//out �������������Ļ�����(����ΪNULL)
									  DWORD	*pcbBinary);		//in/out ����������/���س���

// �ж��ִ��������Ƿ�Ϊ16���ƴ���BASE64�ִ�(��ͷβ���)
// ����		0  -- ���Ͳ���
//			16 -- 16�����ַ�
//			64 -- BASE64�ַ�
//
int __stdcall Crypt_GetStringType(char *pszString);

// ����֤����Ϣȡ֤��״̬
//
// dwFlags = 0  pbCertInfoΪ֤�����к�(BYTE����)
//				cbCertInfoΪ֤�����кų���
// dwFlags = 1  pbCertInfoΪ֤��(BYTE����)
//				cbCertInfoΪ֤�鳤��
// dwFlags = 2  pbCertInfoΪǩ������֤�����кŵ�der����(BYTE����)
//				cbCertInfoΪǩ������֤�����кŵ�der���볤��
//#define PUBKEY_ISSUER_SERIALNUMBER	0x10	// �Խṹ��ʽ��ʾ��ǩ���ߺ����к�(BYTE����)
//#define PUBKEY_ISSUER_USERID			0x50(0x51,0x52)	// ֤��ǩ���ߺ��û���ʶ,��ʾΪ: issuer/userid
//#define PUBKEY_SERIALNUMBER			0x60	// "szIssuer/szSerial"--add at 2008.10.31
// ����: ������ 0 ��������0Ϊ������
//       ��������Ϊ0ʱ��*nCertStatusΪ֤��״̬
// nCertStatus=֤��״̬��0����Ч��1�����ϣ�2������5�����ڣ�63��δ֪
//
CRYPT_FUNCTION_INFO(Crypt_GetCertStatus)
(
		HCRYPTPROV	hProv,				// in �򿪵������豸�����NULL
		BYTE		*pbCertInfo,		// in
		DWORD		cbCertInfo,			// in
		DWORD		dwFlags,			// in  0,1,2,0x10,0x20,0x51,0x52,0x60
		DWORD		*nCertStatus);		// out ����֤��״̬

#undef CRYPT_FUNCTION_INFO

#ifdef __cplusplus
}
#endif

#endif
