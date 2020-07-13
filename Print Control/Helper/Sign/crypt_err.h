#ifndef __CRYPTAPI_ERR_H__
#define __CRYPTAPI_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************

1、错误代码采用32位无符号整数表示；
2、错误代码的最高4位为下层调用系统接口的分类码，后28位为被调用系统返回的实际错误码。
分类码为0x0-0xf, 其定义如下：
#define ERROR_CLASS_CAPI		0
- 由应用支撑平台接口自定义的错误码；
#define ERROR_CLASS_CDSA		1
 		- 由CDSA接口定义的错误码；
#define ERROR_CLASS_PKCS11		2
		- 由PKCS#11接口定义的错误码；
#define ERROR_CLASS_WIN32		3
		- 由Windows操作系统定义的错误码；
#define ERROR_CLASS_UNIX		4
		- 由UNIX/LINUX操作系统定义的错误码；
5-15 保留。

一般错误码转换使用下面的函数:
uint32 ErrMap(uint32 ulErrClass, uint32 ulErrCode)
{
	return ((ulErrClass << 28) + ulErrCode);
}
由于Windows操作系统定义的错误码也是32位无符号整数，在将Windows错误码转换成应用支撑平台统一的错误码时，使用如下函数：
uint32 ErrMap_Win2CAPI(uint32 ulWinErrCode)
{
	return ((ERROR_CLASS_WIN32 << 28) + 
((ulWinErrCode >> 4) & 0x0f000000) + (ulWinErrCode & 0xffffff)));
}

******************************************************************************/






//***************************************************************************
// 定义错误码类别
//***************************************************************************

#define CRYPT_ERROR_CLASS_CAPI		0		// 由应用支撑平台接口自定义的错误码；
#define CRYPT_ERROR_CLASS_CDSA		1 		// 由CDSA接口定义的错误码；
#define CRYPT_ERROR_CLASS_PKCS11	2		// 由PKCS#11接口定义的错误码；
#define CRYPT_ERROR_CLASS_WIN32		3		// 由Windows操作系统定义的错误码；
#define CRYPT_ERROR_CLASS_UNIX		4		// 由UNIX/LINUX操作系统定义的错误码；

// 错误码映射：下层错误码转换成CAPI错误码
#define ErrMap(ulErrClass, ulErrCode)	((ulErrClass << 28) + ulErrCode)

// 错误码映射：Windows错误码转换成CAPI错误码
unsigned int ErrMap_Win2CAPI(
		unsigned int ulWinErrCode);			// Windows错误码

// 错误码映射：CDSA错误码转换成CAPI错误码
unsigned int ErrMap_Cdsa2CAPI(
		unsigned int ulCdsaErrCode);		// CDSA错误码

// 错误码映射：PKCS#11错误码转换成CAPI错误码
unsigned int ErrMap_Pkcs112CAPI(
		unsigned int ulPkcs11ErrCode);		// PKCS11错误码

// 错误码映射：UNIX/Linux错误码转换成CAPI错误码
unsigned int ErrMap_Unix2CAPI(
		unsigned int ulUnixErrCode);		// UNIX错误码

/***************************************************************************
 *    自定义的错误码0x00000000--0x0fffffff
 ***************************************************************************/

/***************************************************************************
 *    一般通用的错误码
 ***************************************************************************/
#undef  ERROR_SUCCESS
#define ERROR_SUCCESS						0

#define CRYPT_ERR_SUCCESS					0
#define ERR_OK								CRYPT_ERR_SUCCESS
#define RTN_OK								CRYPT_ERR_SUCCESS
#define CRYPT_ERR_OK						CRYPT_ERR_SUCCESS	

#define CRYPT_ERR_CANCEL					1		// 取消操作
#define CRYPT_ERR_MEMORY_ERROR				2		// 内存分配错误
#define CRYPT_ERR_INVALID_HANDLE			3
#define CRYPT_ERR_INVALID_POINTER			4		// 数据指针错误
#define CRYPT_ERR_INVALID_INPUT_POINTER		5		// 输入数据指针错误
#define CRYPT_ERR_INVALID_OUTPUT_POINTER	6		// 输出数据指针错误
#define CRYPT_ERR_INPUT_LENGTH_ERROR		7		// 输入数据长度错误
#define CRYPT_ERR_OUTPUT_LENGTH_ERROR		8		// 输出数据长度错误

#define CRYPT_ERR_BUFFER_TOO_SMALL			CRYPT_ERR_OUTPUT_LENGTH_ERROR		// 缓冲区太小
#define CRYPT_ERR_NOT_ENOUGH_MEMORY			CRYPT_ERR_MEMORY_ERROR		// 缓冲区太小

#define CRYPT_ERR_DATA_LENGTH				9		// 数据长度错误
#define CRYPT_ERR_DATA_FORMAT				0xA		// 数据格式错误
#define CRYPT_ERR_FUNCTION_PARAM			0xB		// 函数参数错误
#define CRYPT_ERR_FUNCTION_FAILED			0xC		// 函数调用失败
#define CRYPT_ERR_FILE_NOT_FOUND			0xD		// 文件未找到
#define CRYPT_ERR_FILE_OPEN_FAILED			0xE		// 文件未找到
#define CRYPT_ERR_FILE_READ_FAILED			0xF		// 文件未找到
#define CRYPT_ERR_FILE_WRITE_FAILED			0x10	// 文件未找到
#define CRYPT_ERR_MODULE_LOAD_FAILED		0x11	// 模块装入失败
#define CRYPT_ERR_OBJECT_NOT_FOUND			0x12	// 未找到
#define CRYPT_ERR_TIME_OUT					0x13	// 超时
//以下为新增：
#define CRYPT_ERR_UNKNOWN					0x14	// 未知错误
#define CRYPT_ERR_DBERR						0x15	// 数据库错误
#define CRYPT_ERR_LICENSE					0x16	// 许可证错误


/***************************************************************************
 *   CSP错误号
 ***************************************************************************/
#ifndef __CSP_ERROR__
#define __CSP_ERROR__


#define CRYPT_ERR_CSP_BASE					0x20
#define CRYPT_ERR_CSP_NOT_INIT				(CRYPT_ERR_CSP_BASE+0)		// 加密模块未初始化
#define CRYPT_ERR_INTERNAL_ERROR			(CRYPT_ERR_CSP_BASE+1)		// 加密模块内部错误

#define CRYPT_ERR_PROVIDER_NOT_EXIST		(CRYPT_ERR_CSP_BASE+2)		// 加密模块不存在
#define CRYPT_ERR_PROVIDER_NOT_OPEN			(CRYPT_ERR_CSP_BASE+3)		// 加密模块未打开
#define CRYPT_ERR_CONTAINER_NOT_EXIST		(CRYPT_ERR_CSP_BASE+4)		// 密钥盒不存在
#define CRYPT_ERR_NOT_LOGGED_IN				(CRYPT_ERR_CSP_BASE+5)		// 未登录
#define CRYPT_ERR_ALREADY_LOGGED_IN			(CRYPT_ERR_CSP_BASE+6)		// 已登录

#define CRYPT_ERR_VERIFY_FAILED				(CRYPT_ERR_CSP_BASE+7)		// 签名验证错误
#define CRYPT_ERR_INVALID_SIGNATURE			(CRYPT_ERR_CSP_BASE+8)		// 签名错误

#define CRYPT_ERR_USER_CANCEL				CRYPT_ERR_CANCEL			// 用户取消操作
#define CRYPT_ERR_VERIFYSIGN				CRYPT_ERR_VERIFY_FAILED		// 签名验证错误
#define CRYPT_ERR_SIGNATURE					CRYPT_ERR_INVALID_SIGNATURE

#define CRYPT_ERR_BLOCK_SIZE_MISMATCH		(CRYPT_ERR_CSP_BASE+9)		// 
#define CRYPT_ERR_DEVICE_ERROR				(CRYPT_ERR_CSP_BASE+0xA)		// 未登录
#define CRYPT_ERR_INVALID_KEY				(CRYPT_ERR_CSP_BASE+0xB)		// 未登录
#define CRYPT_ERR_ALGID_MISMATCH			(CRYPT_ERR_CSP_BASE+0xC)		// 未登录
#define CRYPT_ERR_PRIVATE_KEY_NOT_FOUND		(CRYPT_ERR_CSP_BASE+0xD)		// 未登录
#define CRYPT_ERR_PRIVATE_KEY_ALREADY_EXISTS	(CRYPT_ERR_CSP_BASE+0xE)		// 未登录

#define CRYPT_ERR_KEY_NOT_CREATED			(CRYPT_ERR_CSP_BASE+0xF)	// 密钥未创建
#define CRYPT_ERR_HASH_NOT_CREATED			(CRYPT_ERR_CSP_BASE+0x10)	// 数据摘要未创建
#define CRYPT_ERR_HASH_FINISHED				(CRYPT_ERR_CSP_BASE+0x11)		// HASH已结束

#define CRYPT_ERR_EB_TYPE					(CRYPT_ERR_CSP_BASE+0x12)		// PKCS1块类型错误
#define CRYPT_ERR_EB_LENGTH					(CRYPT_ERR_CSP_BASE+0x13)		// PKCS1数据长度错误
#define CRYPT_ERR_DECODE					(CRYPT_ERR_CSP_BASE+0x14)		// 解编码时出错
#define CRYPT_ERR_CODE_OVERRUN				(CRYPT_ERR_CSP_BASE+0x15)		// 解编码时给定的编码数据太短
#define CRYPT_ERR_NOT_NUMSTRING				(CRYPT_ERR_CSP_BASE+0x16)		// 非数字串
#define CRYPT_ERR_INVALID_TAG				(CRYPT_ERR_CSP_BASE+0x17)	// 数据标志错误
#define CRYPT_ERR_INVALID_DERFMT			(CRYPT_ERR_CSP_BASE+0x18)	// 解编码数据格式错误
#define CRYPT_ERR_INVALID_ALGID				(CRYPT_ERR_CSP_BASE+0x19)	// 密码算法错误
#define CRYPT_ERR_INVALID_ALGOID			(CRYPT_ERR_CSP_BASE+0x1A)	// 密码算法OID错误
#define CRYPT_ERR_INVALID_CERT_CONTEXT		(CRYPT_ERR_CSP_BASE+0x1B)	// 编码时CONTEXT设置错误
#define CRYPT_ERR_INVALID_PARAMETER			(CRYPT_ERR_CSP_BASE+0x1C)	// 函数参数错误

#define CRYPT_ERR_NO_CAKEY					(CRYPT_ERR_CSP_BASE+0x1D)	// 不含CA密钥数据
#define CRYPT_ERR_NO_CERT					(CRYPT_ERR_CSP_BASE+0x1E)   // 无证书
#define CRYPT_ERR_INVALID_CERT				(CRYPT_ERR_CSP_BASE+0x1F)   // 证书错误

#define CRYPT_ERR_ICNOCAKEY					CRYPT_ERR_NO_CAKEY			// IC卡中不含CA密钥数据
#define CRYPT_ERR_ICNOCERT					CRYPT_ERR_NO_CERT			//100 ICNOCER IC卡内无证书
#define CRYPT_ERR_CERTTOOLONG				CRYPT_ERR_INVALID_CERT		//102 CERTOOLONG 读(写)证书长度>MAX_CERT_LENGTH

#define CRYPT_ERR_KEYFILE_WRITE				(CRYPT_ERR_CSP_BASE+0x20)	// 密钥文件写错误
#define CRYPT_ERR_KEYFILE_READ				(CRYPT_ERR_CSP_BASE+0x21)	// 密钥文件读错误
#define CRYPT_ERR_KEYFILE_BAD				(CRYPT_ERR_CSP_BASE+0x22)	// 密钥文件已捐坏
#define CRYPT_ERR_KEYFILE_NOTEXIST			(CRYPT_ERR_CSP_BASE+0x23)	// 密钥文件不存在

#define CRYPT_ERR_READERDLL_NOT_FIND		(CRYPT_ERR_CSP_BASE+0x24)	// 读卡器动态库未找到
#define CRYPT_ERR_READERDLL_NOT_LOAD		(CRYPT_ERR_CSP_BASE+0x25)	// 读卡器动态库未装入
#define CRYPT_ERR_READERDLL_FUNCNOTFIND		(CRYPT_ERR_CSP_BASE+0x26)	// 读卡器动态库中一个函数未找到


#define CRYPT_ERR_SEND						(CRYPT_ERR_CSP_BASE+0x27)	// 发送至密码机错误
#define CRYPT_ERR_RECV						(CRYPT_ERR_CSP_BASE+0x28)	// 接收自密码机错误
#define CRYPT_ERR_DECRYPT					(CRYPT_ERR_CSP_BASE+0x29)	// 解密错误

#endif // __CSP_ERROR__



/***************************************************************************
 *    身份认证错误码
 ***************************************************************************/

#define CRYPT_ERR_AUTH_BASE			0x50

#define CRYPT_ERR_AUTH_OK			0
#define CRYPT_ERR_AUTH_ERROR		(CRYPT_ERR_AUTH_BASE+0x01)	// 身份认证错误
#define CRYPT_ERR_INVALID_USER		(CRYPT_ERR_AUTH_BASE+0x02)	// 非法USER
#define CRYPT_ERR_INVALID_IP		(CRYPT_ERR_AUTH_BASE+0x03)	// 非法IP来源
#define CRYPT_ERR_INVALID_MAC		(CRYPT_ERR_AUTH_BASE+0x04)	// 数据包校验错误
#define CRYPT_ERR_INVALID_LEN		(CRYPT_ERR_AUTH_BASE+0x05)	// 数据包长度错误
#define CRYPT_ERR_INVALID_FMT		(CRYPT_ERR_AUTH_BASE+0x06)	// 数据包格式错误
#define CRYPT_ERR_INVALID_REQ		(CRYPT_ERR_AUTH_BASE+0x07)	// 无效的命令码
#define CRYPT_ERR_INVALID_PARAM		(CRYPT_ERR_AUTH_BASE+0x08)	// 无效的命令参数
#define CRYPT_ERR_INVALID_CERTREQ	(CRYPT_ERR_AUTH_BASE+0x09)	// 无效的证书申请

#define CRYPT_ERR_DBF_OPEN_FAILED	(CRYPT_ERR_AUTH_BASE+0x0A)	// 数据库打开失败
#define CRYPT_ERR_DBF_ACCESS_FAILED	(CRYPT_ERR_AUTH_BASE+0x0b)	// 数据库操作失败
#define CRYPT_ERR_MAKECERT_FAILED	(CRYPT_ERR_AUTH_BASE+0x0c)	// 生成证书失败
#define CRYPT_ERR_DEVICE_FAILED		(CRYPT_ERR_AUTH_BASE+0x0d)	// 加密设备运行错误
#define CRYPT_ERR_NET_ERROR			(CRYPT_ERR_AUTH_BASE+0x0f)	// 网络通信错误

#define CRYPT_ERR_CERT_NOT_EXIST		(CRYPT_ERR_AUTH_BASE+0x10)	// 证书不存在
#define CRYPT_ERR_CERT_REVOKED			(CRYPT_ERR_AUTH_BASE+0x11)	// 证书已作废
#define CRYPT_ERR_CERT_HANGED			(CRYPT_ERR_AUTH_BASE+0x12)	// 证书已挂失
#define CRYPT_ERR_CERT_CANCELED			(CRYPT_ERR_AUTH_BASE+0x13)	// 证书已注销
#define CRYPT_ERR_CERT_NOTVALID			(CRYPT_ERR_AUTH_BASE+0x14)	// 证书未生效
#define CRYPT_ERR_CERT_EXPIRED			(CRYPT_ERR_AUTH_BASE+0x15)	// 证书已过期
#define CRYPT_ERR_CERT_STOPED			(CRYPT_ERR_AUTH_BASE+0x16)	// 证书已停用
#define CRYPT_ERR_CERT_STATUS_UNKNOWN	(CRYPT_ERR_AUTH_BASE+0x17)	// 证书状态未知
#define CRYPT_ERR_CERT_VERIFY_ERROR		(CRYPT_ERR_AUTH_BASE+0x18)	// 证书验证错误
#define CRYPT_ERR_CERT_INVALID			(CRYPT_ERR_AUTH_BASE+0x19)	// 证书已损坏
#define CRYPT_ERR_CERT_EXIST			(CRYPT_ERR_AUTH_BASE+0x1A)	// 证书已存在
#define CRYPT_ERR_CERT_STATUS_INVALID	(CRYPT_ERR_AUTH_BASE+0x1B)	// 证书状态标记无效
#define CRYPT_ERR_CERT_NORIGHT			(CRYPT_ERR_AUTH_BASE+0x1C)	// 操作权限不足

#define CRYPT_ERR_NOT_AUTHED			(CRYPT_ERR_AUTH_BASE+0x1D)	// 未经过身份认证
#define CRYPT_ERR_NOT_CONNECT			(CRYPT_ERR_AUTH_BASE+0x1E)	// 未建立连接
#define CRYPT_ERR_INVALID_DATA			(CRYPT_ERR_AUTH_BASE+0x1F)	// 数据错误
#define CRYPT_ERR_AUTH_DATA_EXPIRED		(CRYPT_ERR_AUTH_BASE+0x20)	// 认证数据已过期
#define CRYPT_ERR_AUTH_SRV_BUSY			(CRYPT_ERR_AUTH_BASE+0x21)	// 认证服务器忙
#define CRYPT_ERR_AUTH_ROOT_NOT_TRUSTED	(CRYPT_ERR_AUTH_BASE+0x22)	// CA未被信任


// 下列定义用于与旧版本兼容

#define ERR_AUTHERR				CRYPT_ERR_AUTH_ERROR		// 身份认证错误
#define ERR_VERIFYSIGN			CRYPT_ERR_VERIFY_FAILED		// 签名验证错误
#define ERR_BADSIGNATURE		CRYPT_ERR_INVALID_SIGNATURE	// 签名值错误
#define ERR_NOT_ENOUGH_MEMORY	CRYPT_ERR_MEMORY_ERROR		// 缓冲区太小

#define ERR_INVALIDCLIENT		CRYPT_ERR_INVALID_IP		// 非法IP来源
#define ERR_INVALIDMAC			CRYPT_ERR_INVALID_MAC		// 数据包校验错误
#define ERR_INVALIDDATALEN		CRYPT_ERR_INVALID_LEN		// 数据包长度错误
#define ERR_INVALIDDATAFMT		CRYPT_ERR_INVALID_FMT		// 数据包格式错误
#define ERR_INVALIDREQUEST		CRYPT_ERR_INVALID_REQ		// 无效的命令码
#define ERR_INVALIDPARAM		CRYPT_ERR_INVALID_PARAM		// 无效的命令参数
#define ERR_INVALIDCERTREQ		CRYPT_ERR_INVALID_CERTREQ	// 无效的证书申请

#define ERR_DATABASEFAILED		CRYPT_ERR_DBF_ACCESS_FAILED	// 数据库操作失败
#define ERR_MAKECERTFAILED		CRYPT_ERR_MAKECERT_FAILED	// 生成证书失败
#define ERR_SECDEVICEFAILED		CRYPT_ERR_DEVICE_FAILED		// 加密设备运行错误
#define ERR_INTERNALERROR		CRYPT_ERR_INTERNAL_ERROR	// CA内部错误
#define ERR_NETERROR			CRYPT_ERR_NET_ERROR			// 网络通信错误

#define ERR_CERTNOTEXIST		CRYPT_ERR_CERT_NOT_EXIST	// 证书不存在
#define ERR_CERTSTOPED			CRYPT_ERR_CERT_STOPED		// 证书已停用
#define ERR_CERTHANGED			CRYPT_ERR_CERT_HANGED		// 证书已挂失
#define ERR_CERTCANCELED		CRYPT_ERR_CERT_CANCELED		// 证书已注销
#define ERR_CERTNOTVALID		CRYPT_ERR_CERT_NOTVALID		// 证书未生效
#define ERR_CERTEXPIRED			CRYPT_ERR_CERT_EXPIRED		// 证书已过期
#define ERR_CERTSTATUS_UNKNOWN	CRYPT_ERR_CERT_STATUS_UNKNOWN		// 证书状态未知
#define ERR_CERTVERIFYERROR		CRYPT_ERR_CERT_VERIFY_ERROR	// 证书验证错误
#define ERR_CERTINVALID			CRYPT_ERR_CERT_INVALID		// 证书已损坏
#define ERR_CERTEXIST			CRYPT_ERR_CERT_EXIST		// 证书已存在
#define ERR_CERTSTATUSINVALID	CRYPT_ERR_CERT_STATUS_INVALID	// 证书状态标记无效
#define ERR_CERTNORIGHT			CRYPT_ERR_CERT_NORIGHT		// 操作权限不足
#define ERR_NOTAUTHED			CRYPT_ERR_NOT_AUTHED		// 未经过身份认证

#define ERR_NOT_CONNECT			CRYPT_ERR_NOT_CONNECT		// 未建立连接
#define ERR_INVALID_DATA		CRYPT_ERR_INVALID_DATA		// 数据错误
#define ERR_FILE_NOT_FOUND		CRYPT_ERR_FILE_NOT_FOUND	// 文件未找到
#define ERR_DATABASEOPENFAILED	CRYPT_ERR_DBF_OPEN_FAILED	// 数据库打开失败
#define ERR_DBFDLL_OPENFAILED	CRYPT_ERR_MODULE_LOAD_FAILED	// 数据库DLL打开失败

#define ERR_DATA_LENGTH			CRYPT_ERR_DATA_LENGTH		// 数据长度错误


/*******************************************************************
 *  关于LDAP的错误信息 
 *******************************************************************/
#ifndef _LDAP_ERROR_
#define _LDAP_ERROR_

#define LDAP_ERR_BASE						0x80

#define	CRYPT_ERR_LDAP_OK					0 
#define	CRYPT_ERR_LDAP_NOT_INIT				(LDAP_ERR_BASE+0)	// LDAP未初始化
#define	CRYPT_ERR_LDAP_OBJ_ALREADY_EXIST	(LDAP_ERR_BASE+1)	// 对象已经存在
#define	CRYPT_ERR_LDAP_OBJ_NOT_EXIST		(LDAP_ERR_BASE+2)	// 对象未找到
#define	CRYPT_ERR_LDAP_INIT_FAIL			(LDAP_ERR_BASE+3)	// LDAP初始化失败
#define	CRYPT_ERR_LDAP_BIND_FAIL			(LDAP_ERR_BASE+4)	// LDAP绑定失败
#define	CRYPT_ERR_LDAP_UNBIND_FAIL			(LDAP_ERR_BASE+5)	// LDAP解除绑定失败
#define	CRYPT_ERR_LDAP_ADD_CRL_FAIL			(LDAP_ERR_BASE+6)	// 添加CRL失败
#define	CRYPT_ERR_LDAP_ADD_CERT_FAIL		(LDAP_ERR_BASE+7)	// 添加证书失败
#define	CRYPT_ERR_LDAP_DEL_CRL_FAIL			(LDAP_ERR_BASE+8)	// 删除CRL失败
#define	CRYPT_ERR_LDAP_DEL_CERT_FAIL		(LDAP_ERR_BASE+9)	// 删除证书失败
#define	CRYPT_ERR_LDAP_FIND_CERT_FAIL		(LDAP_ERR_BASE+10)	// 查找证书失败
#define	CRYPT_ERR_LDAP_FIND_CRL_FAIL		(LDAP_ERR_BASE+11)	// 查找黑名单失败
#define	CRYPT_ERR_LDAP_CONNECT_FAIL			(LDAP_ERR_BASE+12)	// LDAP连接失败

#define	CRYPT_ERR_OCSP_CONNECT_FAIL			(LDAP_ERR_BASE+13)	// OCSP连接失败
#define	CRYPT_ERR_OCSP_NOT_INIT				(LDAP_ERR_BASE+14)	// OCSP未初始化
#define	CRYPT_ERR_OCSP_SEND_FAIL			(LDAP_ERR_BASE+15)	// 发送至OCSP失败
#define	CRYPT_ERR_OCSP_RECV_FAIL			(LDAP_ERR_BASE+16)	// 接收自OCSP失败
#define	CRYPT_ERR_OCSP_VERIFY_ERROR			(LDAP_ERR_BASE+17)	// 接收自OCSP的数据包验证错误

#define	CRYPT_ERR_KMC_NOT_INIT				(LDAP_ERR_BASE+19)	// KMC未初始化
#define	CRYPT_ERR_KMC_CONNECT_FAIL			(LDAP_ERR_BASE+20)	// KMC连接失败


// 下列错误号用于与旧版本兼容

#define	LDAP_ERR_OK						CRYPT_ERR_LDAP_OK 
#define	LDAP_ERR_NOT_INIT				CRYPT_ERR_LDAP_NOT_INIT	// LDAP未初始化
#define	LDAP_ERR_ALREADY_EXISTS			CRYPT_ERR_LDAP_OBJ_ALREADY_EXIST	// 对象已经存在
#define	LDAP_ERR_NO_SUCH_OBJECT			CRYPT_ERR_LDAP_OBJ_NOT_EXIST	// 对象未找到

#define	LDAP_ERR_INIT_FAIL				CRYPT_ERR_LDAP_INIT_FAIL		// LDAP初始化失败
#define	LDAP_ERR_BIND_FAIL				CRYPT_ERR_LDAP_BIND_FAIL		// LDAP绑定失败
#define	LDAP_ERR_UNBIND_FAIL			CRYPT_ERR_LDAP_UNBIND_FAIL		// LDAP解除绑定失败
#define	LDAP_ERR_ADD_CRL_FAIL			CRYPT_ERR_LDAP_ADD_CRL_FAIL		// 添加CRL失败
#define	LDAP_ERR_ADD_CERT_FAIL			CRYPT_ERR_LDAP_ADD_CERT_FAIL	// 添加证书失败
#define	LDAP_ERR_DEL_CRL_FAIL			CRYPT_ERR_LDAP_DEL_CRL_FAIL		// 删除CRL失败
#define	LDAP_ERR_DEL_CERT_FAIL			CRYPT_ERR_LDAP_DEL_CERT_FAIL	// 删除证书失败
#define	LDAP_ERR_FIND_CERT_FAIL			CRYPT_ERR_LDAP_FIND_CERT_FAIL	// 查找证书失败
#define	LDAP_ERR_FIND_CRL_FAIL			CRYPT_ERR_LDAP_FIND_CRL_FAIL	// 查找黑名单失败

#define	OCSP_ERR_CONNECT_FAIL			CRYPT_ERR_OCSP_CONNECT_FAIL		// OCSP连接失败
#define	OCSP_ERR_NOT_INIT				CRYPT_ERR_OCSP_NOT_INIT			// OCSP未初始化

#define	KMC_ERR_NOT_INIT				CRYPT_ERR_KMC_NOT_INIT			// KMC未初始化
#define	KMC_ERR_CONNECT_FAIL			CRYPT_ERR_KMC_CONNECT_FAIL		// KMC连接失败


/*******************************************************************
 *     IC卡操作错误
 *******************************************************************/
#ifndef __IC_ERROR__
#define __IC_ERROR__

#define IC_ERR_BASE					0xA0
#define IC_ERR_OPR_OK			    0 					// 成功
#define IC_ERR_PORT					(IC_ERR_BASE+0)		// 端口号错误
#define IC_ERR_OPR_FAIL		        (IC_ERR_BASE+1)		// 操作失败
#define IC_ERR_COM_OPR_FAIL		    (IC_ERR_BASE+2)		// 串口故障/冲突/通讯错误
#define IC_ERR_USB_OPEN_FAIL	    (IC_ERR_BASE+3)		// USB故障/冲突/通讯错误
#define IC_ERR_AC_NOT_CONNECTED	    (IC_ERR_BASE+4)		// 未连读卡器/读卡器应答错误
#define IC_ERR_AC_FAIL			    (IC_ERR_BASE+5)		// 读卡器故障
#define IC_ERR_LOCKED				(IC_ERR_BASE+6)		// IC卡错误/锁死
#define IC_ERR_NOT_INSERT		    (IC_ERR_BASE+7)		// 未插IC卡
#define IC_ERR_PASSWORD_ERROR		(IC_ERR_BASE+8)		// 口令错误
#define IC_ERR_NO_SUCH_RIGHT		(IC_ERR_BASE+9)		// 无此权限,安全状态不满足
#define IC_ERR_IN_BUF_ERROR		    (IC_ERR_BASE+0xA)	// 输入BUF错
#define IC_ERR_TIME_OUT				(IC_ERR_BASE+0xb)	// 超时错误
#define IC_ERR_FILE_NOT_EXIST	    (IC_ERR_BASE+0xc)	// IC卡文件不存在
#define IC_ERR_FILE_EXIST		    (IC_ERR_BASE+0xd)	// IC卡文件已存在
#define IC_ERR_RECORDER_NOT_EXIST	(IC_ERR_BASE+0xe)	// IC卡记录不存在
#define IC_ERR_FILE_FULL		    (IC_ERR_BASE+0xf)	// IC卡文件空间已满
#define IC_ERR_ILLEGAL              (IC_ERR_BASE+0x10)	// IC卡非法
#define IC_ERR_VERIFY               (IC_ERR_BASE+0x11)	// 验证失败
#define IC_ERR_REC_FULL             (IC_ERR_BASE+0x12)	// 日志记录满
#define IC_ERR_REC_NOT_R            (IC_ERR_BASE+0x13)	// 日志记录未读取
#define IC_ERR_REC_PRM_ERR          (IC_ERR_BASE+0x14)	// 函数输入参数错误
#define IC_ERR_INVALID_NEWPWD		(IC_ERR_BASE+0x15)	// 无效的新口令(不能为全0)
#define IC_ERR_INVALID_PWDVALUE		(IC_ERR_BASE+0x16)	// 无效的口令(不能为全0)
#define IC_ERR_NOT_INIT				(IC_ERR_BASE+0x17)	// IC卡未初始化
#define IC_ERR_NEED_REINIT			(IC_ERR_BASE+0x18)	// 文件系统已存在，IC卡需要重新初始化

#define CRYPT_ERR_USBKEY_NOT_INSTALL	(IC_ERR_BASE+0x19)	// 没有安装USBKEY驱动--(原为IC_ERR_BASE+0x20)--modify by xrh(2011.4.19)
#define CRYPT_ERR_USBKEY_NOT_INSERT		IC_ERR_NOT_INSERT	// 未插入USBKEY
#define CRYPT_ERR_USBKEY_CHANGED		(IC_ERR_BASE+0x1A)	// USBKEY已改变--(原为IC_ERR_BASE+0x21)--modify by xrh(2011.4.19)
			

#endif	// __IC_ERR__



/********************************************************************
*	 56所加密卡错误号	 KFYL error code definitions				*
*    注：这里定义的错误号保存与原KFYL.h中定义的相同
*********************************************************************/
#ifndef __KFYL_ERROR__
#define __KFYL_ERROR__

#define KFYL_ERR_BASE						0xC0

#define KFYL_ERR_OK							0					// 操作成功
#define KFYL_ERR_CODE_UNKNOWN				(KFYL_ERR_BASE+0)		// 加密设备未知错误
#define KFYL_ERR_CODE_NODATA				(KFYL_ERR_BASE+0x01)	// 要读取的数据不存在
#define KFYL_ERR_CODE_NOSUCHCARD			(KFYL_ERR_BASE+0x02)	// 密码卡不存在
#define KFYL_ERR_CODE_DEVICEIO				(KFYL_ERR_BASE+0x03)	// IO传送标志错误
#define KFYL_ERR_CODE_RECV     				(KFYL_ERR_BASE+0x04)	// 动态库应答接收错误
#define KFYL_ERR_CODE_DATA_LENGTH			(KFYL_ERR_BASE+0x05)	// 数据长度错误

#define KFYL_ERR_CODE_BERFORMAT				(KFYL_ERR_BASE+0x06)	// BER编码格式错误
#define KFYL_ERR_CODE_MAC					(KFYL_ERR_BASE+0x07)	// MAC验证错误
#define KFYL_ERR_CODE_MAC_LENGTH			(KFYL_ERR_BASE+0x08)	// MAC长度错误
#define	KFYL_ERR_CODE_HASH					(KFYL_ERR_BASE+0X09)	// HASH比较错误
#define KFYL_ERR_CODE_BUSY					(KFYL_ERR_BASE+0X0A)	// 设备忙

#define KFYL_ERR_CODE_NOTOPEN				(KFYL_ERR_BASE+0X0B)	// 设备未打开
#define KFYL_ERR_CODE_OPENDEV				(KFYL_ERR_BASE+0X0C)	// 打开设备错误
#define KFYL_ERR_CODE_INTERNAL 				(KFYL_ERR_BASE+0x0D)	// 加密卡内部错误

#define KFYL_ERR_ATTRIB_TYPE				(KFYL_ERR_BASE+0x0E)	// 密码算法类型错误
#define KFYL_ERR_INPUT_DATA 				(KFYL_ERR_BASE+0x0F)	// 输入数据错 

#endif	// __KFYL_ERROR__


//系统自身信息和错误码
#define SYS_ERR_TWOCACONN				0xf0	// 双机同步连接建立
#define SYS_ERR_TWOCADISCONN		    0xf1	// 双机同步连接断开
#define SYS_ERR_NOTINITIALIZE			0xf2	// 系统未初始化
#define SYS_ERR_CACERTNOTEXIST			0xf3	// CA证书不存在
#define SYS_ERR_CACERTERROR				0xf4	// CA证书验证错误
#define SYS_ERR_MSGSTART				0xf5	// 开始

#endif

typedef struct _ERR_MAP {
		unsigned int		new_error;
		unsigned int		old_error;
} ERROR_MAP;

#define MakeError(op, err)	((op * 10000) + err)

// 从错误取错误字串
// 返回字串长度(不计结尾0)
int GetErrorString(unsigned int nErrCode, char *pszError, int nBufLen);

#ifdef __cplusplus
}
#endif

#endif
