#ifndef __CRYPTAPI_ERR_H__
#define __CRYPTAPI_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************

1������������32λ�޷���������ʾ��
2�������������4λΪ�²����ϵͳ�ӿڵķ����룬��28λΪ������ϵͳ���ص�ʵ�ʴ����롣
������Ϊ0x0-0xf, �䶨�����£�
#define ERROR_CLASS_CAPI		0
- ��Ӧ��֧��ƽ̨�ӿ��Զ���Ĵ����룻
#define ERROR_CLASS_CDSA		1
 		- ��CDSA�ӿڶ���Ĵ����룻
#define ERROR_CLASS_PKCS11		2
		- ��PKCS#11�ӿڶ���Ĵ����룻
#define ERROR_CLASS_WIN32		3
		- ��Windows����ϵͳ����Ĵ����룻
#define ERROR_CLASS_UNIX		4
		- ��UNIX/LINUX����ϵͳ����Ĵ����룻
5-15 ������

һ�������ת��ʹ������ĺ���:
uint32 ErrMap(uint32 ulErrClass, uint32 ulErrCode)
{
	return ((ulErrClass << 28) + ulErrCode);
}
����Windows����ϵͳ����Ĵ�����Ҳ��32λ�޷����������ڽ�Windows������ת����Ӧ��֧��ƽ̨ͳһ�Ĵ�����ʱ��ʹ�����º�����
uint32 ErrMap_Win2CAPI(uint32 ulWinErrCode)
{
	return ((ERROR_CLASS_WIN32 << 28) + 
((ulWinErrCode >> 4) & 0x0f000000) + (ulWinErrCode & 0xffffff)));
}

******************************************************************************/






//***************************************************************************
// ������������
//***************************************************************************

#define CRYPT_ERROR_CLASS_CAPI		0		// ��Ӧ��֧��ƽ̨�ӿ��Զ���Ĵ����룻
#define CRYPT_ERROR_CLASS_CDSA		1 		// ��CDSA�ӿڶ���Ĵ����룻
#define CRYPT_ERROR_CLASS_PKCS11	2		// ��PKCS#11�ӿڶ���Ĵ����룻
#define CRYPT_ERROR_CLASS_WIN32		3		// ��Windows����ϵͳ����Ĵ����룻
#define CRYPT_ERROR_CLASS_UNIX		4		// ��UNIX/LINUX����ϵͳ����Ĵ����룻

// ������ӳ�䣺�²������ת����CAPI������
#define ErrMap(ulErrClass, ulErrCode)	((ulErrClass << 28) + ulErrCode)

// ������ӳ�䣺Windows������ת����CAPI������
unsigned int ErrMap_Win2CAPI(
		unsigned int ulWinErrCode);			// Windows������

// ������ӳ�䣺CDSA������ת����CAPI������
unsigned int ErrMap_Cdsa2CAPI(
		unsigned int ulCdsaErrCode);		// CDSA������

// ������ӳ�䣺PKCS#11������ת����CAPI������
unsigned int ErrMap_Pkcs112CAPI(
		unsigned int ulPkcs11ErrCode);		// PKCS11������

// ������ӳ�䣺UNIX/Linux������ת����CAPI������
unsigned int ErrMap_Unix2CAPI(
		unsigned int ulUnixErrCode);		// UNIX������

/***************************************************************************
 *    �Զ���Ĵ�����0x00000000--0x0fffffff
 ***************************************************************************/

/***************************************************************************
 *    һ��ͨ�õĴ�����
 ***************************************************************************/
#undef  ERROR_SUCCESS
#define ERROR_SUCCESS						0

#define CRYPT_ERR_SUCCESS					0
#define ERR_OK								CRYPT_ERR_SUCCESS
#define RTN_OK								CRYPT_ERR_SUCCESS
#define CRYPT_ERR_OK						CRYPT_ERR_SUCCESS	

#define CRYPT_ERR_CANCEL					1		// ȡ������
#define CRYPT_ERR_MEMORY_ERROR				2		// �ڴ�������
#define CRYPT_ERR_INVALID_HANDLE			3
#define CRYPT_ERR_INVALID_POINTER			4		// ����ָ�����
#define CRYPT_ERR_INVALID_INPUT_POINTER		5		// ��������ָ�����
#define CRYPT_ERR_INVALID_OUTPUT_POINTER	6		// �������ָ�����
#define CRYPT_ERR_INPUT_LENGTH_ERROR		7		// �������ݳ��ȴ���
#define CRYPT_ERR_OUTPUT_LENGTH_ERROR		8		// ������ݳ��ȴ���

#define CRYPT_ERR_BUFFER_TOO_SMALL			CRYPT_ERR_OUTPUT_LENGTH_ERROR		// ������̫С
#define CRYPT_ERR_NOT_ENOUGH_MEMORY			CRYPT_ERR_MEMORY_ERROR		// ������̫С

#define CRYPT_ERR_DATA_LENGTH				9		// ���ݳ��ȴ���
#define CRYPT_ERR_DATA_FORMAT				0xA		// ���ݸ�ʽ����
#define CRYPT_ERR_FUNCTION_PARAM			0xB		// ������������
#define CRYPT_ERR_FUNCTION_FAILED			0xC		// ��������ʧ��
#define CRYPT_ERR_FILE_NOT_FOUND			0xD		// �ļ�δ�ҵ�
#define CRYPT_ERR_FILE_OPEN_FAILED			0xE		// �ļ�δ�ҵ�
#define CRYPT_ERR_FILE_READ_FAILED			0xF		// �ļ�δ�ҵ�
#define CRYPT_ERR_FILE_WRITE_FAILED			0x10	// �ļ�δ�ҵ�
#define CRYPT_ERR_MODULE_LOAD_FAILED		0x11	// ģ��װ��ʧ��
#define CRYPT_ERR_OBJECT_NOT_FOUND			0x12	// δ�ҵ�
#define CRYPT_ERR_TIME_OUT					0x13	// ��ʱ
//����Ϊ������
#define CRYPT_ERR_UNKNOWN					0x14	// δ֪����
#define CRYPT_ERR_DBERR						0x15	// ���ݿ����
#define CRYPT_ERR_LICENSE					0x16	// ���֤����


/***************************************************************************
 *   CSP�����
 ***************************************************************************/
#ifndef __CSP_ERROR__
#define __CSP_ERROR__


#define CRYPT_ERR_CSP_BASE					0x20
#define CRYPT_ERR_CSP_NOT_INIT				(CRYPT_ERR_CSP_BASE+0)		// ����ģ��δ��ʼ��
#define CRYPT_ERR_INTERNAL_ERROR			(CRYPT_ERR_CSP_BASE+1)		// ����ģ���ڲ�����

#define CRYPT_ERR_PROVIDER_NOT_EXIST		(CRYPT_ERR_CSP_BASE+2)		// ����ģ�鲻����
#define CRYPT_ERR_PROVIDER_NOT_OPEN			(CRYPT_ERR_CSP_BASE+3)		// ����ģ��δ��
#define CRYPT_ERR_CONTAINER_NOT_EXIST		(CRYPT_ERR_CSP_BASE+4)		// ��Կ�в�����
#define CRYPT_ERR_NOT_LOGGED_IN				(CRYPT_ERR_CSP_BASE+5)		// δ��¼
#define CRYPT_ERR_ALREADY_LOGGED_IN			(CRYPT_ERR_CSP_BASE+6)		// �ѵ�¼

#define CRYPT_ERR_VERIFY_FAILED				(CRYPT_ERR_CSP_BASE+7)		// ǩ����֤����
#define CRYPT_ERR_INVALID_SIGNATURE			(CRYPT_ERR_CSP_BASE+8)		// ǩ������

#define CRYPT_ERR_USER_CANCEL				CRYPT_ERR_CANCEL			// �û�ȡ������
#define CRYPT_ERR_VERIFYSIGN				CRYPT_ERR_VERIFY_FAILED		// ǩ����֤����
#define CRYPT_ERR_SIGNATURE					CRYPT_ERR_INVALID_SIGNATURE

#define CRYPT_ERR_BLOCK_SIZE_MISMATCH		(CRYPT_ERR_CSP_BASE+9)		// 
#define CRYPT_ERR_DEVICE_ERROR				(CRYPT_ERR_CSP_BASE+0xA)		// δ��¼
#define CRYPT_ERR_INVALID_KEY				(CRYPT_ERR_CSP_BASE+0xB)		// δ��¼
#define CRYPT_ERR_ALGID_MISMATCH			(CRYPT_ERR_CSP_BASE+0xC)		// δ��¼
#define CRYPT_ERR_PRIVATE_KEY_NOT_FOUND		(CRYPT_ERR_CSP_BASE+0xD)		// δ��¼
#define CRYPT_ERR_PRIVATE_KEY_ALREADY_EXISTS	(CRYPT_ERR_CSP_BASE+0xE)		// δ��¼

#define CRYPT_ERR_KEY_NOT_CREATED			(CRYPT_ERR_CSP_BASE+0xF)	// ��Կδ����
#define CRYPT_ERR_HASH_NOT_CREATED			(CRYPT_ERR_CSP_BASE+0x10)	// ����ժҪδ����
#define CRYPT_ERR_HASH_FINISHED				(CRYPT_ERR_CSP_BASE+0x11)		// HASH�ѽ���

#define CRYPT_ERR_EB_TYPE					(CRYPT_ERR_CSP_BASE+0x12)		// PKCS1�����ʹ���
#define CRYPT_ERR_EB_LENGTH					(CRYPT_ERR_CSP_BASE+0x13)		// PKCS1���ݳ��ȴ���
#define CRYPT_ERR_DECODE					(CRYPT_ERR_CSP_BASE+0x14)		// �����ʱ����
#define CRYPT_ERR_CODE_OVERRUN				(CRYPT_ERR_CSP_BASE+0x15)		// �����ʱ�����ı�������̫��
#define CRYPT_ERR_NOT_NUMSTRING				(CRYPT_ERR_CSP_BASE+0x16)		// �����ִ�
#define CRYPT_ERR_INVALID_TAG				(CRYPT_ERR_CSP_BASE+0x17)	// ���ݱ�־����
#define CRYPT_ERR_INVALID_DERFMT			(CRYPT_ERR_CSP_BASE+0x18)	// ��������ݸ�ʽ����
#define CRYPT_ERR_INVALID_ALGID				(CRYPT_ERR_CSP_BASE+0x19)	// �����㷨����
#define CRYPT_ERR_INVALID_ALGOID			(CRYPT_ERR_CSP_BASE+0x1A)	// �����㷨OID����
#define CRYPT_ERR_INVALID_CERT_CONTEXT		(CRYPT_ERR_CSP_BASE+0x1B)	// ����ʱCONTEXT���ô���
#define CRYPT_ERR_INVALID_PARAMETER			(CRYPT_ERR_CSP_BASE+0x1C)	// ������������

#define CRYPT_ERR_NO_CAKEY					(CRYPT_ERR_CSP_BASE+0x1D)	// ����CA��Կ����
#define CRYPT_ERR_NO_CERT					(CRYPT_ERR_CSP_BASE+0x1E)   // ��֤��
#define CRYPT_ERR_INVALID_CERT				(CRYPT_ERR_CSP_BASE+0x1F)   // ֤�����

#define CRYPT_ERR_ICNOCAKEY					CRYPT_ERR_NO_CAKEY			// IC���в���CA��Կ����
#define CRYPT_ERR_ICNOCERT					CRYPT_ERR_NO_CERT			//100 ICNOCER IC������֤��
#define CRYPT_ERR_CERTTOOLONG				CRYPT_ERR_INVALID_CERT		//102 CERTOOLONG ��(д)֤�鳤��>MAX_CERT_LENGTH

#define CRYPT_ERR_KEYFILE_WRITE				(CRYPT_ERR_CSP_BASE+0x20)	// ��Կ�ļ�д����
#define CRYPT_ERR_KEYFILE_READ				(CRYPT_ERR_CSP_BASE+0x21)	// ��Կ�ļ�������
#define CRYPT_ERR_KEYFILE_BAD				(CRYPT_ERR_CSP_BASE+0x22)	// ��Կ�ļ��Ѿ軵
#define CRYPT_ERR_KEYFILE_NOTEXIST			(CRYPT_ERR_CSP_BASE+0x23)	// ��Կ�ļ�������

#define CRYPT_ERR_READERDLL_NOT_FIND		(CRYPT_ERR_CSP_BASE+0x24)	// ��������̬��δ�ҵ�
#define CRYPT_ERR_READERDLL_NOT_LOAD		(CRYPT_ERR_CSP_BASE+0x25)	// ��������̬��δװ��
#define CRYPT_ERR_READERDLL_FUNCNOTFIND		(CRYPT_ERR_CSP_BASE+0x26)	// ��������̬����һ������δ�ҵ�


#define CRYPT_ERR_SEND						(CRYPT_ERR_CSP_BASE+0x27)	// ���������������
#define CRYPT_ERR_RECV						(CRYPT_ERR_CSP_BASE+0x28)	// ���������������
#define CRYPT_ERR_DECRYPT					(CRYPT_ERR_CSP_BASE+0x29)	// ���ܴ���

#endif // __CSP_ERROR__



/***************************************************************************
 *    �����֤������
 ***************************************************************************/

#define CRYPT_ERR_AUTH_BASE			0x50

#define CRYPT_ERR_AUTH_OK			0
#define CRYPT_ERR_AUTH_ERROR		(CRYPT_ERR_AUTH_BASE+0x01)	// �����֤����
#define CRYPT_ERR_INVALID_USER		(CRYPT_ERR_AUTH_BASE+0x02)	// �Ƿ�USER
#define CRYPT_ERR_INVALID_IP		(CRYPT_ERR_AUTH_BASE+0x03)	// �Ƿ�IP��Դ
#define CRYPT_ERR_INVALID_MAC		(CRYPT_ERR_AUTH_BASE+0x04)	// ���ݰ�У�����
#define CRYPT_ERR_INVALID_LEN		(CRYPT_ERR_AUTH_BASE+0x05)	// ���ݰ����ȴ���
#define CRYPT_ERR_INVALID_FMT		(CRYPT_ERR_AUTH_BASE+0x06)	// ���ݰ���ʽ����
#define CRYPT_ERR_INVALID_REQ		(CRYPT_ERR_AUTH_BASE+0x07)	// ��Ч��������
#define CRYPT_ERR_INVALID_PARAM		(CRYPT_ERR_AUTH_BASE+0x08)	// ��Ч���������
#define CRYPT_ERR_INVALID_CERTREQ	(CRYPT_ERR_AUTH_BASE+0x09)	// ��Ч��֤������

#define CRYPT_ERR_DBF_OPEN_FAILED	(CRYPT_ERR_AUTH_BASE+0x0A)	// ���ݿ��ʧ��
#define CRYPT_ERR_DBF_ACCESS_FAILED	(CRYPT_ERR_AUTH_BASE+0x0b)	// ���ݿ����ʧ��
#define CRYPT_ERR_MAKECERT_FAILED	(CRYPT_ERR_AUTH_BASE+0x0c)	// ����֤��ʧ��
#define CRYPT_ERR_DEVICE_FAILED		(CRYPT_ERR_AUTH_BASE+0x0d)	// �����豸���д���
#define CRYPT_ERR_NET_ERROR			(CRYPT_ERR_AUTH_BASE+0x0f)	// ����ͨ�Ŵ���

#define CRYPT_ERR_CERT_NOT_EXIST		(CRYPT_ERR_AUTH_BASE+0x10)	// ֤�鲻����
#define CRYPT_ERR_CERT_REVOKED			(CRYPT_ERR_AUTH_BASE+0x11)	// ֤��������
#define CRYPT_ERR_CERT_HANGED			(CRYPT_ERR_AUTH_BASE+0x12)	// ֤���ѹ�ʧ
#define CRYPT_ERR_CERT_CANCELED			(CRYPT_ERR_AUTH_BASE+0x13)	// ֤����ע��
#define CRYPT_ERR_CERT_NOTVALID			(CRYPT_ERR_AUTH_BASE+0x14)	// ֤��δ��Ч
#define CRYPT_ERR_CERT_EXPIRED			(CRYPT_ERR_AUTH_BASE+0x15)	// ֤���ѹ���
#define CRYPT_ERR_CERT_STOPED			(CRYPT_ERR_AUTH_BASE+0x16)	// ֤����ͣ��
#define CRYPT_ERR_CERT_STATUS_UNKNOWN	(CRYPT_ERR_AUTH_BASE+0x17)	// ֤��״̬δ֪
#define CRYPT_ERR_CERT_VERIFY_ERROR		(CRYPT_ERR_AUTH_BASE+0x18)	// ֤����֤����
#define CRYPT_ERR_CERT_INVALID			(CRYPT_ERR_AUTH_BASE+0x19)	// ֤������
#define CRYPT_ERR_CERT_EXIST			(CRYPT_ERR_AUTH_BASE+0x1A)	// ֤���Ѵ���
#define CRYPT_ERR_CERT_STATUS_INVALID	(CRYPT_ERR_AUTH_BASE+0x1B)	// ֤��״̬�����Ч
#define CRYPT_ERR_CERT_NORIGHT			(CRYPT_ERR_AUTH_BASE+0x1C)	// ����Ȩ�޲���

#define CRYPT_ERR_NOT_AUTHED			(CRYPT_ERR_AUTH_BASE+0x1D)	// δ���������֤
#define CRYPT_ERR_NOT_CONNECT			(CRYPT_ERR_AUTH_BASE+0x1E)	// δ��������
#define CRYPT_ERR_INVALID_DATA			(CRYPT_ERR_AUTH_BASE+0x1F)	// ���ݴ���
#define CRYPT_ERR_AUTH_DATA_EXPIRED		(CRYPT_ERR_AUTH_BASE+0x20)	// ��֤�����ѹ���
#define CRYPT_ERR_AUTH_SRV_BUSY			(CRYPT_ERR_AUTH_BASE+0x21)	// ��֤������æ
#define CRYPT_ERR_AUTH_ROOT_NOT_TRUSTED	(CRYPT_ERR_AUTH_BASE+0x22)	// CAδ������


// ���ж���������ɰ汾����

#define ERR_AUTHERR				CRYPT_ERR_AUTH_ERROR		// �����֤����
#define ERR_VERIFYSIGN			CRYPT_ERR_VERIFY_FAILED		// ǩ����֤����
#define ERR_BADSIGNATURE		CRYPT_ERR_INVALID_SIGNATURE	// ǩ��ֵ����
#define ERR_NOT_ENOUGH_MEMORY	CRYPT_ERR_MEMORY_ERROR		// ������̫С

#define ERR_INVALIDCLIENT		CRYPT_ERR_INVALID_IP		// �Ƿ�IP��Դ
#define ERR_INVALIDMAC			CRYPT_ERR_INVALID_MAC		// ���ݰ�У�����
#define ERR_INVALIDDATALEN		CRYPT_ERR_INVALID_LEN		// ���ݰ����ȴ���
#define ERR_INVALIDDATAFMT		CRYPT_ERR_INVALID_FMT		// ���ݰ���ʽ����
#define ERR_INVALIDREQUEST		CRYPT_ERR_INVALID_REQ		// ��Ч��������
#define ERR_INVALIDPARAM		CRYPT_ERR_INVALID_PARAM		// ��Ч���������
#define ERR_INVALIDCERTREQ		CRYPT_ERR_INVALID_CERTREQ	// ��Ч��֤������

#define ERR_DATABASEFAILED		CRYPT_ERR_DBF_ACCESS_FAILED	// ���ݿ����ʧ��
#define ERR_MAKECERTFAILED		CRYPT_ERR_MAKECERT_FAILED	// ����֤��ʧ��
#define ERR_SECDEVICEFAILED		CRYPT_ERR_DEVICE_FAILED		// �����豸���д���
#define ERR_INTERNALERROR		CRYPT_ERR_INTERNAL_ERROR	// CA�ڲ�����
#define ERR_NETERROR			CRYPT_ERR_NET_ERROR			// ����ͨ�Ŵ���

#define ERR_CERTNOTEXIST		CRYPT_ERR_CERT_NOT_EXIST	// ֤�鲻����
#define ERR_CERTSTOPED			CRYPT_ERR_CERT_STOPED		// ֤����ͣ��
#define ERR_CERTHANGED			CRYPT_ERR_CERT_HANGED		// ֤���ѹ�ʧ
#define ERR_CERTCANCELED		CRYPT_ERR_CERT_CANCELED		// ֤����ע��
#define ERR_CERTNOTVALID		CRYPT_ERR_CERT_NOTVALID		// ֤��δ��Ч
#define ERR_CERTEXPIRED			CRYPT_ERR_CERT_EXPIRED		// ֤���ѹ���
#define ERR_CERTSTATUS_UNKNOWN	CRYPT_ERR_CERT_STATUS_UNKNOWN		// ֤��״̬δ֪
#define ERR_CERTVERIFYERROR		CRYPT_ERR_CERT_VERIFY_ERROR	// ֤����֤����
#define ERR_CERTINVALID			CRYPT_ERR_CERT_INVALID		// ֤������
#define ERR_CERTEXIST			CRYPT_ERR_CERT_EXIST		// ֤���Ѵ���
#define ERR_CERTSTATUSINVALID	CRYPT_ERR_CERT_STATUS_INVALID	// ֤��״̬�����Ч
#define ERR_CERTNORIGHT			CRYPT_ERR_CERT_NORIGHT		// ����Ȩ�޲���
#define ERR_NOTAUTHED			CRYPT_ERR_NOT_AUTHED		// δ���������֤

#define ERR_NOT_CONNECT			CRYPT_ERR_NOT_CONNECT		// δ��������
#define ERR_INVALID_DATA		CRYPT_ERR_INVALID_DATA		// ���ݴ���
#define ERR_FILE_NOT_FOUND		CRYPT_ERR_FILE_NOT_FOUND	// �ļ�δ�ҵ�
#define ERR_DATABASEOPENFAILED	CRYPT_ERR_DBF_OPEN_FAILED	// ���ݿ��ʧ��
#define ERR_DBFDLL_OPENFAILED	CRYPT_ERR_MODULE_LOAD_FAILED	// ���ݿ�DLL��ʧ��

#define ERR_DATA_LENGTH			CRYPT_ERR_DATA_LENGTH		// ���ݳ��ȴ���


/*******************************************************************
 *  ����LDAP�Ĵ�����Ϣ 
 *******************************************************************/
#ifndef _LDAP_ERROR_
#define _LDAP_ERROR_

#define LDAP_ERR_BASE						0x80

#define	CRYPT_ERR_LDAP_OK					0 
#define	CRYPT_ERR_LDAP_NOT_INIT				(LDAP_ERR_BASE+0)	// LDAPδ��ʼ��
#define	CRYPT_ERR_LDAP_OBJ_ALREADY_EXIST	(LDAP_ERR_BASE+1)	// �����Ѿ�����
#define	CRYPT_ERR_LDAP_OBJ_NOT_EXIST		(LDAP_ERR_BASE+2)	// ����δ�ҵ�
#define	CRYPT_ERR_LDAP_INIT_FAIL			(LDAP_ERR_BASE+3)	// LDAP��ʼ��ʧ��
#define	CRYPT_ERR_LDAP_BIND_FAIL			(LDAP_ERR_BASE+4)	// LDAP��ʧ��
#define	CRYPT_ERR_LDAP_UNBIND_FAIL			(LDAP_ERR_BASE+5)	// LDAP�����ʧ��
#define	CRYPT_ERR_LDAP_ADD_CRL_FAIL			(LDAP_ERR_BASE+6)	// ���CRLʧ��
#define	CRYPT_ERR_LDAP_ADD_CERT_FAIL		(LDAP_ERR_BASE+7)	// ���֤��ʧ��
#define	CRYPT_ERR_LDAP_DEL_CRL_FAIL			(LDAP_ERR_BASE+8)	// ɾ��CRLʧ��
#define	CRYPT_ERR_LDAP_DEL_CERT_FAIL		(LDAP_ERR_BASE+9)	// ɾ��֤��ʧ��
#define	CRYPT_ERR_LDAP_FIND_CERT_FAIL		(LDAP_ERR_BASE+10)	// ����֤��ʧ��
#define	CRYPT_ERR_LDAP_FIND_CRL_FAIL		(LDAP_ERR_BASE+11)	// ���Һ�����ʧ��
#define	CRYPT_ERR_LDAP_CONNECT_FAIL			(LDAP_ERR_BASE+12)	// LDAP����ʧ��

#define	CRYPT_ERR_OCSP_CONNECT_FAIL			(LDAP_ERR_BASE+13)	// OCSP����ʧ��
#define	CRYPT_ERR_OCSP_NOT_INIT				(LDAP_ERR_BASE+14)	// OCSPδ��ʼ��
#define	CRYPT_ERR_OCSP_SEND_FAIL			(LDAP_ERR_BASE+15)	// ������OCSPʧ��
#define	CRYPT_ERR_OCSP_RECV_FAIL			(LDAP_ERR_BASE+16)	// ������OCSPʧ��
#define	CRYPT_ERR_OCSP_VERIFY_ERROR			(LDAP_ERR_BASE+17)	// ������OCSP�����ݰ���֤����

#define	CRYPT_ERR_KMC_NOT_INIT				(LDAP_ERR_BASE+19)	// KMCδ��ʼ��
#define	CRYPT_ERR_KMC_CONNECT_FAIL			(LDAP_ERR_BASE+20)	// KMC����ʧ��


// ���д����������ɰ汾����

#define	LDAP_ERR_OK						CRYPT_ERR_LDAP_OK 
#define	LDAP_ERR_NOT_INIT				CRYPT_ERR_LDAP_NOT_INIT	// LDAPδ��ʼ��
#define	LDAP_ERR_ALREADY_EXISTS			CRYPT_ERR_LDAP_OBJ_ALREADY_EXIST	// �����Ѿ�����
#define	LDAP_ERR_NO_SUCH_OBJECT			CRYPT_ERR_LDAP_OBJ_NOT_EXIST	// ����δ�ҵ�

#define	LDAP_ERR_INIT_FAIL				CRYPT_ERR_LDAP_INIT_FAIL		// LDAP��ʼ��ʧ��
#define	LDAP_ERR_BIND_FAIL				CRYPT_ERR_LDAP_BIND_FAIL		// LDAP��ʧ��
#define	LDAP_ERR_UNBIND_FAIL			CRYPT_ERR_LDAP_UNBIND_FAIL		// LDAP�����ʧ��
#define	LDAP_ERR_ADD_CRL_FAIL			CRYPT_ERR_LDAP_ADD_CRL_FAIL		// ���CRLʧ��
#define	LDAP_ERR_ADD_CERT_FAIL			CRYPT_ERR_LDAP_ADD_CERT_FAIL	// ���֤��ʧ��
#define	LDAP_ERR_DEL_CRL_FAIL			CRYPT_ERR_LDAP_DEL_CRL_FAIL		// ɾ��CRLʧ��
#define	LDAP_ERR_DEL_CERT_FAIL			CRYPT_ERR_LDAP_DEL_CERT_FAIL	// ɾ��֤��ʧ��
#define	LDAP_ERR_FIND_CERT_FAIL			CRYPT_ERR_LDAP_FIND_CERT_FAIL	// ����֤��ʧ��
#define	LDAP_ERR_FIND_CRL_FAIL			CRYPT_ERR_LDAP_FIND_CRL_FAIL	// ���Һ�����ʧ��

#define	OCSP_ERR_CONNECT_FAIL			CRYPT_ERR_OCSP_CONNECT_FAIL		// OCSP����ʧ��
#define	OCSP_ERR_NOT_INIT				CRYPT_ERR_OCSP_NOT_INIT			// OCSPδ��ʼ��

#define	KMC_ERR_NOT_INIT				CRYPT_ERR_KMC_NOT_INIT			// KMCδ��ʼ��
#define	KMC_ERR_CONNECT_FAIL			CRYPT_ERR_KMC_CONNECT_FAIL		// KMC����ʧ��


/*******************************************************************
 *     IC����������
 *******************************************************************/
#ifndef __IC_ERROR__
#define __IC_ERROR__

#define IC_ERR_BASE					0xA0
#define IC_ERR_OPR_OK			    0 					// �ɹ�
#define IC_ERR_PORT					(IC_ERR_BASE+0)		// �˿ںŴ���
#define IC_ERR_OPR_FAIL		        (IC_ERR_BASE+1)		// ����ʧ��
#define IC_ERR_COM_OPR_FAIL		    (IC_ERR_BASE+2)		// ���ڹ���/��ͻ/ͨѶ����
#define IC_ERR_USB_OPEN_FAIL	    (IC_ERR_BASE+3)		// USB����/��ͻ/ͨѶ����
#define IC_ERR_AC_NOT_CONNECTED	    (IC_ERR_BASE+4)		// δ��������/������Ӧ�����
#define IC_ERR_AC_FAIL			    (IC_ERR_BASE+5)		// ����������
#define IC_ERR_LOCKED				(IC_ERR_BASE+6)		// IC������/����
#define IC_ERR_NOT_INSERT		    (IC_ERR_BASE+7)		// δ��IC��
#define IC_ERR_PASSWORD_ERROR		(IC_ERR_BASE+8)		// �������
#define IC_ERR_NO_SUCH_RIGHT		(IC_ERR_BASE+9)		// �޴�Ȩ��,��ȫ״̬������
#define IC_ERR_IN_BUF_ERROR		    (IC_ERR_BASE+0xA)	// ����BUF��
#define IC_ERR_TIME_OUT				(IC_ERR_BASE+0xb)	// ��ʱ����
#define IC_ERR_FILE_NOT_EXIST	    (IC_ERR_BASE+0xc)	// IC���ļ�������
#define IC_ERR_FILE_EXIST		    (IC_ERR_BASE+0xd)	// IC���ļ��Ѵ���
#define IC_ERR_RECORDER_NOT_EXIST	(IC_ERR_BASE+0xe)	// IC����¼������
#define IC_ERR_FILE_FULL		    (IC_ERR_BASE+0xf)	// IC���ļ��ռ�����
#define IC_ERR_ILLEGAL              (IC_ERR_BASE+0x10)	// IC���Ƿ�
#define IC_ERR_VERIFY               (IC_ERR_BASE+0x11)	// ��֤ʧ��
#define IC_ERR_REC_FULL             (IC_ERR_BASE+0x12)	// ��־��¼��
#define IC_ERR_REC_NOT_R            (IC_ERR_BASE+0x13)	// ��־��¼δ��ȡ
#define IC_ERR_REC_PRM_ERR          (IC_ERR_BASE+0x14)	// ���������������
#define IC_ERR_INVALID_NEWPWD		(IC_ERR_BASE+0x15)	// ��Ч���¿���(����Ϊȫ0)
#define IC_ERR_INVALID_PWDVALUE		(IC_ERR_BASE+0x16)	// ��Ч�Ŀ���(����Ϊȫ0)
#define IC_ERR_NOT_INIT				(IC_ERR_BASE+0x17)	// IC��δ��ʼ��
#define IC_ERR_NEED_REINIT			(IC_ERR_BASE+0x18)	// �ļ�ϵͳ�Ѵ��ڣ�IC����Ҫ���³�ʼ��

#define CRYPT_ERR_USBKEY_NOT_INSTALL	(IC_ERR_BASE+0x19)	// û�а�װUSBKEY����--(ԭΪIC_ERR_BASE+0x20)--modify by xrh(2011.4.19)
#define CRYPT_ERR_USBKEY_NOT_INSERT		IC_ERR_NOT_INSERT	// δ����USBKEY
#define CRYPT_ERR_USBKEY_CHANGED		(IC_ERR_BASE+0x1A)	// USBKEY�Ѹı�--(ԭΪIC_ERR_BASE+0x21)--modify by xrh(2011.4.19)
			

#endif	// __IC_ERR__



/********************************************************************
*	 56�����ܿ������	 KFYL error code definitions				*
*    ע�����ﶨ��Ĵ���ű�����ԭKFYL.h�ж������ͬ
*********************************************************************/
#ifndef __KFYL_ERROR__
#define __KFYL_ERROR__

#define KFYL_ERR_BASE						0xC0

#define KFYL_ERR_OK							0					// �����ɹ�
#define KFYL_ERR_CODE_UNKNOWN				(KFYL_ERR_BASE+0)		// �����豸δ֪����
#define KFYL_ERR_CODE_NODATA				(KFYL_ERR_BASE+0x01)	// Ҫ��ȡ�����ݲ�����
#define KFYL_ERR_CODE_NOSUCHCARD			(KFYL_ERR_BASE+0x02)	// ���뿨������
#define KFYL_ERR_CODE_DEVICEIO				(KFYL_ERR_BASE+0x03)	// IO���ͱ�־����
#define KFYL_ERR_CODE_RECV     				(KFYL_ERR_BASE+0x04)	// ��̬��Ӧ����մ���
#define KFYL_ERR_CODE_DATA_LENGTH			(KFYL_ERR_BASE+0x05)	// ���ݳ��ȴ���

#define KFYL_ERR_CODE_BERFORMAT				(KFYL_ERR_BASE+0x06)	// BER�����ʽ����
#define KFYL_ERR_CODE_MAC					(KFYL_ERR_BASE+0x07)	// MAC��֤����
#define KFYL_ERR_CODE_MAC_LENGTH			(KFYL_ERR_BASE+0x08)	// MAC���ȴ���
#define	KFYL_ERR_CODE_HASH					(KFYL_ERR_BASE+0X09)	// HASH�Ƚϴ���
#define KFYL_ERR_CODE_BUSY					(KFYL_ERR_BASE+0X0A)	// �豸æ

#define KFYL_ERR_CODE_NOTOPEN				(KFYL_ERR_BASE+0X0B)	// �豸δ��
#define KFYL_ERR_CODE_OPENDEV				(KFYL_ERR_BASE+0X0C)	// ���豸����
#define KFYL_ERR_CODE_INTERNAL 				(KFYL_ERR_BASE+0x0D)	// ���ܿ��ڲ�����

#define KFYL_ERR_ATTRIB_TYPE				(KFYL_ERR_BASE+0x0E)	// �����㷨���ʹ���
#define KFYL_ERR_INPUT_DATA 				(KFYL_ERR_BASE+0x0F)	// �������ݴ� 

#endif	// __KFYL_ERROR__


//ϵͳ������Ϣ�ʹ�����
#define SYS_ERR_TWOCACONN				0xf0	// ˫��ͬ�����ӽ���
#define SYS_ERR_TWOCADISCONN		    0xf1	// ˫��ͬ�����ӶϿ�
#define SYS_ERR_NOTINITIALIZE			0xf2	// ϵͳδ��ʼ��
#define SYS_ERR_CACERTNOTEXIST			0xf3	// CA֤�鲻����
#define SYS_ERR_CACERTERROR				0xf4	// CA֤����֤����
#define SYS_ERR_MSGSTART				0xf5	// ��ʼ

#endif

typedef struct _ERR_MAP {
		unsigned int		new_error;
		unsigned int		old_error;
} ERROR_MAP;

#define MakeError(op, err)	((op * 10000) + err)

// �Ӵ���ȡ�����ִ�
// �����ִ�����(���ƽ�β0)
int GetErrorString(unsigned int nErrCode, char *pszError, int nBufLen);

#ifdef __cplusplus
}
#endif

#endif
