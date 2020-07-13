// modify at 2012.1
//
#ifndef __ICSCRYPTAPI_H__
#define __ICSCRYPTAPI_H__

#include "basetype.h"
#include "crypt_err.h"

#ifdef __cplusplus
extern "C" {
#endif



// 加密算法ID定义

#define CRYPT_ALGID_NONE				0x00000000
#define CRYPT_ALGID_RSA_PKCS			0x00000001	//RSA公钥密码算法
#define CRYPT_ALGID_RSA_X_509			0x00000003
#define CRYPT_ALGID_MD5_RSA_PKCS        0x00000005
#define CRYPT_ALGID_SHA1_RSA_PKCS       0x00000006
#define CRYPT_ALGID_SHA256_RSA_PKCS     0x00000040
#define CRYPT_ALGID_SHA512_RSA_PKCS     0x00000042

#define CRYPT_ALGID_GB_ECC				0x00000010  //椭圆曲线公钥密码算法:公钥加密、私钥解密、私钥加密、公钥解密
#define CRYPT_ALGID_GB_ECDSA_SHA1		0x00000012  //SHA1+椭圆曲线签名算法
#define CRYPT_ALGID_GB_ECDSA_SHA256		0x00000013  //SHA256+椭圆曲线签名算法
#define CRYPT_ALGID_GB_ECDSA_SHA512		0x00000015  //SHA512+椭圆曲线签名算法
#define CRYPT_ALGID_GB_ECDSA_SM3		0x00000017  //SM3杂凑算法+椭圆曲线签名算法
#define CRYPT_ALGID_GB_ECDH				0x00000018  //椭圆曲线密钥交换协议

#define CRYPT_ALGID_GB_SM3				0x00000021	//GB SM3杂凑算法
#define CRYPT_ALGID_GB_SHA1				0x00000022  //GB SHA1
#define CRYPT_ALGID_GB_SHA256			0x00000023  //GB SHA256
#define CRYPT_ALGID_GB_SHA512			0x00000025  //GB SHA512

/*
//(国标?)非对称算法标识
#define SGD_RSA						0x00010000	//RSA算法
#define SGD_SM2_1					0x00020100	//椭圆曲线签名算法
#define SGD_SM2_2					0x00020200	//椭圆曲线密钥交换协议
#define SGD_SM2_3					0x00020400	//椭圆曲线加密算法

#define SGD_SM3						0x00000001	//SM3杂凑算法
#define SGD_SHA1					0x00000002	//SHA1杂凑算法
#define SGD_SHA256					0x00000004	//SHA256杂凑算法
#define SGD_SHA512					0x00000008	//SHA512杂凑算法
#define S_HASH_HARD128				0x00000040	//国产硬件Hash算法
#define S_HASH_MD5					0x00000080	//MD5
*/

// (国标?)对称算法 
#define SGD_SM1_ECB					0x00000101	//SM1算法ECB加密模式
#define SGD_SM1_CBC					0x00000102	//SM1算法CBC加密模式
#define SGD_SM1_CFB					0x00000104	//SM1算法CFB加密模式
#define SGD_SM1_OFB					0x00000108	//SM1算法OFB加密模式
#define SGD_SM1_MAC					0x00000110	//SM1算法MAC加密模式
#define SGD_SSF33_ECB				0x00000201	//SSF33算法ECB加密模式
#define SGD_SSF33_CBC				0x00000202	//SSF33算法CBC加密模式
#define SGD_SSF33_CFB				0x00000204	//SSF33算法CFB加密模式
#define SGD_SSF33_OFB				0x00000208	//SSF33算法OFB加密模式
//#define SGD_SSF33_MAC				0x00000210	//SSF33算法MAC加密模式
#define SGD_SMS4_ECB				0x00000401	//SMS4算法ECB加密模式
#define SGD_SMS4_CBC				0x00000402	//SMS4算法CBC加密模式
#define SGD_SMS4_CFB				0x00000404	//SMS4算法CFB加密模式
#define SGD_SMS4_OFB				0x00000408	//SMS4算法OFB加密模式
#define SGD_SMS4_MAC				0x00000410	//SMS4算法MAC加密模式

#define CRYPT_ALGID_GB_SSF33		SGD_SSF33_ECB	
#define CRYPT_ALGID_GB_SM1			SGD_SM1_ECB	
#define CRYPT_ALGID_GB_SMS4			SGD_SMS4_ECB	
#define CRYPT_ALGID_GB_SMS4_CBC		SGD_SMS4_CBC	

#define CRYPT_ALGID_MD5					0x00000210
#define CRYPT_ALGID_MD5_HMAC			0x00000211
#define CRYPT_ALGID_SHA1				0x00000220
#define CRYPT_ALGID_SHA1_HMAC			0x00000221
#define CRYPT_ALGID_SHA256				0x00000250	//SHA256杂凑算法
#define CRYPT_ALGID_SHA384				0x00000260
#define CRYPT_ALGID_SHA512				0x00000270	//SHA512杂凑算法

#define CRYPT_ALGID_ECC					0x00001040  //椭圆曲线签名算法
#define CRYPT_ALGID_ECDSA				0x00001041  //椭圆曲线签名算法
#define CRYPT_ALGID_ECDSA_SHA1			0x00001042  //SHA1+椭圆曲线签名算法
#define CRYPT_ALGID_ECDSA_SHA256		0x00001044  //SHA256+椭圆曲线签名算法
#define CRYPT_ALGID_ECDSA_SHA384		0x00001045  //SHA384+椭圆曲线签名算法
#define CRYPT_ALGID_ECDSA_SHA512		0x00001046  //SHA512+椭圆曲线签名算法


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
#define PUBKEY_ISSUER_USERID		0x50	//  (51,52)证书签发者和用户标识,表示为: issuer\userid

#define SIGN_INCLUDE_NONE			0			// 在签名中不包含数据和证书
#define SIGN_INCLUDE_DATA			1			// 在签名中包含数据
#define SIGN_INCLUDE_CERT			2			// 在签名中包含证书
#define SIGN_EXCLUDE_SIGN			4			// 在签名中不包含签名
#define SIGN_HASHED_DATA			0x10		// 被签名的为已HASH过的数据


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
 *  注：
 *  所有函数的返回值为0时正确，非0时为错误号
 *  通过函数 Crypt_GetErrorMsg 可以取回错误信息
 *****************************************************************************/

/*----------------------------------------------------------------------------
 *  打开加密设备,返回句柄
 *---------------------------------------------------------------------------*/

CRYPT_FUNCTION_INFO(Crypt_OpenDevice)
(
		HCRYPTPROV	*phProv,			// out 输出创建的环境句柄
		LPTSTR		pszContainer,		// in/out 加密设备密钥盒的名字
		LPTSTR		pszProvider,		// in 加密设备的名字
		DWORD		dwProvType,			// in 加密设备的类型
		DWORD		dwFlags				// in 参数
);

/*----------------------------------------------------------------------------
 *  关闭加密设备
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_CloseDevice)(HCRYPTPROV hProv);


/*----------------------------------------------------------------------------
 *  读出证书
 *  dwCertNum(证书号):0,AT_KEYEXCHANGE(=1),AT_SIGNATURE(=2)
 *  0:CACERT 1:加密公钥证书 2:签名公钥证书,
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ReadCert)
(
		HCRYPTPROV	hProv,
		DWORD		dwCertNum,		// in 选择的证书号
		BYTE		*pbCert,		// out 读出的证书
		DWORD		*pcbCert		// in/out 缓冲区长度/读出的证书长度
);

/*----------------------------------------------------------------------------
 *  通过签名服务器根据用户信息,获取用户的证书
 * dwKeySpec:
 * AT_KEYEXCHANGE				1 -- 加密公钥证书
 * AT_SIGNATURE					2 -- 签名公钥证书
 * PUBKEY_ISSUER_SERIALNUMBER	0x10--证书签发者和序列号
 * PUBKEY_ISSUER_USERID			0x50(0x51,0x52)	证书签发者和用户标识,表示为: issuer\userid
 * PUBKEY_SERIALNUMBER			0x60 证书序列号
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_FindUserCert)(
		HCRYPTPROV	hProv,					// in 打开的密码设备句柄或NULL
		BYTE		*pbUserInfo,			// in 用户信息
		DWORD		cbUserInfo,				// in 字节数
		DWORD		dwKeySpec,				// in 用户信息类别
		BYTE		*pbCert,				// out 返回证书
		DWORD		*pcbCert);				// in/out 缓冲区长度/证书长度	

/*-------------------------------------------------------------------------
 * 对数据作HASH运算: 
 * Algid=CRYPT_ALGID_MD5,CRYPT_ALGID_SHA1
 *-------------------------------------------------------------------------
 */
CRYPT_FUNCTION_INFO(Crypt_Hash)(
		HCRYPTPROV	hProv,
		BYTE		*pbData,		// in 数据
		DWORD		cbData,			// in 数据长度
		DWORD		dwHashAlgId,	// in HASH算法CRYPT_ALGID_MD5,CRYPT_ALGID_MD5
		BYTE		*pbHash,		// out HASH结果
		DWORD		*pcbHash);		// in/out 缓冲区长度/HASH结果长度


/*-------------------------------------------------------------------------
 * 对数据作数字签名: 
 *  dwSignAlgId=CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS或CRYPT_ALGID_SHA1_RSA_PKCS
 *  dwKeySpec(RSA私钥标识):AT_KEYEXCHANGE(=1),AT_SIGNATURE(=2)
 *  先对数据HASH，再签名。
 *-------------------------------------------------------------------------
 */
CRYPT_FUNCTION_INFO(Crypt_Sign)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbData,			// in 数据
		DWORD		cbData,				// in 数据长度
		DWORD		dwSignAlgId,		// in CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS或CRYPT_ALGID_SHA1_RSA_PKCS
		DWORD		dwKeySpec,			// in 私钥标识
		BYTE		*pbSignature,		// out 签名结果
		DWORD		*pcbSignature);		// in/out 签名块的长度/实际长度

CRYPT_FUNCTION_INFO(Crypt_SignHash)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbHash,			// in 待签名HASH结果
		DWORD		cbHash,				// in HASH结果字节数
		DWORD		dwSignAlgId,		// in CRYPT_ALGID_MD5_RSA_PKCS或CRYPT_ALGID_SHA1_RSA_PKCS
		DWORD		dwKeySpec,			// in 私钥标识
		BYTE		*pbSignature,		// out 签名结果
		DWORD		*pcbSignature);		// in/out 缓冲区长度/实际长度

/*----------------------------------------------------------------------------
 *  使用卡内公钥验证签名或者使用外部公钥验证签名
 *  dwSignAlgId=CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS或CRYPT_ALGID_SHA1_RSA_PKCS
 *  pbData 签名源数据
 *  dwKeySpec(公钥标识): 
 *  (dwKeySpec的含义参见Crypt_PublicEncrypt的说明)
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_VerifySign)(
		HCRYPTPROV	hProv,
		BYTE		*pbData,			// in 签名源数据
		DWORD		cbData	,			// in 源数据长度
		DWORD		dwSignAlgId,		// in CRYPT_ALGID_RSA_PKCS,CRYPT_ALGID_MD5_RSA_PKCS或CRYPT_ALGID_SHA1_RSA_PKCS
		DWORD		dwKeySpec,			// in RSA公钥标识
		BYTE		*pbSignature,		// in 待验证的签名
		DWORD		cbSignature,		// in 签名长度
		BYTE		*pbPubKeyInfo,		// in 公钥信息
		DWORD		cbPubKeyInfo);		// in 公钥信息长度


/*----------------------------------------------------------------------------
 *  用户登录并验证口令
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_Login)(
		HCRYPTPROV	hProv,
		char		*pPwd);			// in 口令串

/*----------------------------------------------------------------------------
 *  验证修改口令
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ChangePin)(
		HCRYPTPROV	hProv,
		char		*pOldPwd,			// in 口令串
		char		*pNewPwd);			// in 口令串

/*----------------------------------------------------------------------------
 *  退出登录
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_Logout)(HCRYPTPROV hProv);


/*----------------------------------------------------------------------------
 * 获取错误码对应的错误信息
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_GetErrorMsg)(
		int			nErrCode,			// in 错误代码
		char		*pszError,			// out 返回错误信息
		int			nBufLen);			// in 缓冲区长度



// 对数据签名，生成PKCS7格式签名数据SignedData
//
CRYPT_FUNCTION_INFO(Crypt_SignData)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbData,			// in 被签名数据
		DWORD		cbData,				// in 被签数据字节数
		DWORD		dwSignAlgId,		// in 签名算法(实际为HASH算法)
		DATE_TIME	*signTime,			// in 签名日期时间
		DWORD		dwFlags,			// in 0,SIGN_INCLUDE_DATA,SIGN_INCLUDE_CERT
		BYTE		*pbSignedData,		// out 签名结果
		DWORD		*pcbSignedData);	// in/out 缓冲区长度/实际长度

// 验证PKCS7格式的签名数据
//
CRYPT_FUNCTION_INFO(Crypt_VerifySignedData)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbSignedData,		// in 签名结果
		DWORD		cbSignedData,		// in 签名结果长度
		DWORD		dwFlags,			// in 0,SIGN_HASHED_DATA
		BYTE		*pbData,			// in 被签名数据(当被签数据分离存放时使用，否则可为NULL)
		DWORD		cbData,				// in 被签数据字节数
		BYTE		*pbCert,			// in 签名者证书(可以为NULL)
		DWORD		cbCert);			// in 签名者证书长度

// 制作PKCS7格式的数据信封
// 当dwSignAlgId != 0时，数字信封带签名
//
CRYPT_FUNCTION_INFO(Crypt_EnvelopData)(
		HCRYPTPROV	hProv,
		BYTE		*pbData,			// in 数据
		DWORD		cbData,				// in 数据长度
		DWORD		dwEncAlgId,			// in 加密算法
		BYTE		*pbRecipientCert,	// in 接收者证书
		DWORD		cbRecipientCert,	// in 接收者证书长度
		DWORD		dwSignAlgId,		// in 签名算法(实际为HASH算法)
		DATE_TIME	*signTime,			// in 签名时间(当dwSignAlgId为0时可为NULL)
		DWORD		dwFlags,			// in 0,SIGN_INCLUDE_DATA,SIGN_INCLUDE_CERT
		BYTE		*pbEnvelopedData,	// out 封装结果
		DWORD		*pcbEnvelopedData);	// in/out 缓冲区长度/封装结果长度

// 解PKCS7格式的数字信封，如果存在签名，则验证签名
// 返回加密密钥和解密后的被封装内容
//
CRYPT_FUNCTION_INFO(Crypt_VerifyEnvelopedData)(
		HCRYPTPROV	hProv,				// in
		BYTE		*pbEnvelopedData,	// in 数字信封
		DWORD		cbEnvelopedData,	// in 数字信封长度
		BYTE		*pbEncedData,		// in 加密的数据(当数据分离存放时使用，否则为NULL)
		DWORD		cbEncedData,		// in 加密数据字节数
		BYTE		*pbCert,			// in 签名者证书(可以为NULL)
		DWORD		cbCert,				// in 签名者证书长度
		BYTE		*pbData,			// out 返回被封装原数据
		DWORD		*pcbData);			// in/out 被封装数据字节数


/*----------------------------------------------------------------------------
 *  生成客户端请求数据包ClientHello
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ClientHello)(
		HCRYPTPROV hProv,
		DWORD		dwFlags,			// in 0:单向认证，1：双向，2：单向约定密钥，3：双向
		BYTE		*pbClientHello,		// out 数据包
		DWORD		*pcbClientHello);	// in/out 缓冲区长度/包长度

/*----------------------------------------------------------------------------
 * 服务器端生成回应数据包(ServerHello)
 * 生成的pbServerHello将回送至客户端
 * 生成的pbServerRandom由调用者自已保存，用于对客户身份进行认证
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ServerHello)(
		HCRYPTPROV hProv,
		BYTE		*pbClientHello,			// in 客户端请求数据包
		DWORD		cbClientHello,			// in 客户端请求数据包长度
		BYTE		*pbServerHello,			// out 服务器应答数据包
		DWORD		*pcbServerHello,		// in/out 缓冲区长度/数据包长度
		BYTE		*pbServerRandom,		// out 服务器生成的随机数
		DWORD		*pcbServerRandom);		// in/out 缓冲区长度/随机数长度


/*----------------------------------------------------------------------------
 * 验证ServerHello,生成客户端认证数据
 *---------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ClientAuth)(
		HCRYPTPROV	hProv,
		BYTE		*pbServerHello,		// in 服务器端信息
		DWORD		cbServerHello,		// in
		BYTE		*pbClientAuth,		// out 客户端认证码
		DWORD		*pcbClientAuth);	// in/out


/*----------------------------------------------------------------------------------------
 * 服务器端验证客户的身份
 * 如果参数pbServerRandom!=NULL,认证时要比较pbServerRandom与客户上传的数据
 * 否则只检查时间差
 *---------------------------------------------------------------------------------------*/
CRYPT_FUNCTION_INFO(Crypt_ServerAuth)(
		HCRYPTPROV hProv,
		BYTE		*pbClientAuth,			// in 客户端上传的认证数据
		DWORD		cbClientAuth,			// in 认证数据包长度
		BYTE		*pbServerRandom,		// in 服务器生成的随机数
		DWORD		cbServerRandom,			// in 随机数长度
		BYTE		*pbClientInfo,			// out 客户ID或证书
		DWORD		*pcbClientInfo,			// in/out
		BYTE		*pbSessionKey,			// out 会话密钥
		DWORD		*pcbSessionKey);		// in/out


// 二进制数组转换为Base64字符串
// 成功时返回0, 失败时为错误码
//
CRYPT_FUNCTION_INFO(Crypt_BinaryToBase64String)
(
		BYTE	*pbBinary,		//in 2进制数组
		DWORD	cbBinary,		//in 数组长度
		DWORD	dwFlags,		//in
		LPSTR	pszString,		//out 输出Base64字串
		DWORD	*pcchString);	//in/out 缓冲区长度/输出的实际长度

// Base64串转换成2进制字节数组
//
CRYPT_FUNCTION_INFO(Crypt_Base64StringToBinary)
(
		char	*pszString,			//in 字符串(可含回车换行符和头尾标记)
		DWORD	cchString,			//in 字符串最大长度
		BYTE	*pbBinary,			//out 输出二进制数组的缓冲区(可以为NULL)
		DWORD	*pcbBinary);		//in/out 缓冲区长度/返回长度

// 16进制串转换成2进制字节数组
// 只支持以下两种16进制串类型
// #define CRYPT_STRING_TYPE_HEX					0x00000010
// #define CRYPT_STRING_TYPE_HEXCRLF				0x00000011
//
int __stdcall Crypt_HexStringToBinary(
									  char	*pszString,			//in 字符串(可含回车换行符)
									  DWORD	cchString,			//in 字符串最大长度
									  BYTE	*pbBinary,			//out 输出二进制数组的缓冲区(可以为NULL)
									  DWORD	*pcbBinary);		//in/out 缓冲区长度/返回长度

// 判断字串的类型是否为16进制串或BASE64字串(无头尾标记)
// 返回		0  -- 类型不定
//			16 -- 16进制字符
//			64 -- BASE64字符
//
int __stdcall Crypt_GetStringType(char *pszString);

// 根据证书信息取证书状态
//
// dwFlags = 0  pbCertInfo为证书序列号(BYTE数组)
//				cbCertInfo为证书序列号长度
// dwFlags = 1  pbCertInfo为证书(BYTE数组)
//				cbCertInfo为证书长度
// dwFlags = 2  pbCertInfo为签发者与证书序列号的der编码(BYTE数组)
//				cbCertInfo为签发者与证书序列号的der编码长度
//#define PUBKEY_ISSUER_SERIALNUMBER	0x10	// 以结构形式表示的签名者和序列号(BYTE数组)
//#define PUBKEY_ISSUER_USERID			0x50(0x51,0x52)	// 证书签发者和用户标识,表示为: issuer/userid
//#define PUBKEY_SERIALNUMBER			0x60	// "szIssuer/szSerial"--add at 2008.10.31
// 返回: 错误码 0 正常，非0为错误码
//       当错误码为0时，*nCertStatus为证书状态
// nCertStatus=证书状态：0，有效；1，作废；2，挂起；5，过期；63，未知
//
CRYPT_FUNCTION_INFO(Crypt_GetCertStatus)
(
		HCRYPTPROV	hProv,				// in 打开的密码设备句柄或NULL
		BYTE		*pbCertInfo,		// in
		DWORD		cbCertInfo,			// in
		DWORD		dwFlags,			// in  0,1,2,0x10,0x20,0x51,0x52,0x60
		DWORD		*nCertStatus);		// out 返回证书状态

#undef CRYPT_FUNCTION_INFO

#ifdef __cplusplus
}
#endif

#endif
