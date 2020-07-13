#ifndef _CERT__H_
#define _CERT__H_
#define WIN32_ME
#ifdef WIN32_ME
#include "windows.h"
#endif

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <memory.h>
#include <malloc.h>
#include "sys/types.h"



#define		SoftWare_API		__declspec(dllexport)
#define		WINAPI				__stdcall




typedef unsigned int  UINT4;
typedef int INT4;
typedef unsigned int ATTRIB_TYPE;
typedef unsigned char UC;

#ifndef WIN32_ME
typedef unsigned char UCHAR;
typedef unsigned char BYTE;
typedef unsigned int  DWORD;
typedef int LONG;
#endif

typedef unsigned int  uint32;


#ifdef WIN32_ME
// typedef __int64  UINT8;
// typedef __int64  INT8;
#endif


#define STRING_FILED_MAX_LEN 256

typedef struct ydata_ {
	int oidlength;
	UCHAR oidvalue[120];
}ydata;

#define cert_v1 0
#define cert_v2 1
#define cert_v3 2


#define in_null 731
#define  COID__defaultCardNumber       1
#define  COID__md2WithRSAEncryption    2
#define  COID__md5WithRSAEncryption    3
#define  COID__sha1WithRSAEncryption   4
#define  COID__rsaEncryption           5
#define  COID__md4WithRSAEncryption    6
#define  COID__dsa		       			7
#define  COID__dsa_with_sha	       		8
/*2003-4-1 10:42*/
#define  COID__schWithRSAEncryption    9
#define  COID__schWithECCEncryption   10


#define  COID__other		      -1

#define md2HashId	12
#define md5HashId 	13
#define sha1HashId 	14
#define md4HahId   	16
#define shaHashId 	18
/*2003-4-1 10:42*/
#define schhashId	9

#define unknowinghashid 33
#define unknowingCOIDhashID 34
#define SEQUENSTRUCT					0x30
#define SETSTRUCT						0x31
#define TAG_EOC                        0x0
#define TAG_BOOLEAN                    0x1
#define TAG_INTEGER                    0x2
#define TAG_BIT_STRING                 0x3
#define TAG_OCTET_STRING               0x4
#define TAG_NULL                       0x5
#define TAG_OBJECT_IDENTIFIER          0x6
#define TAG_OBJECT_DESCRIPTOR          0x7
#define TAG_EXTERNAL                   0x8
#define TAG_REAL                       0x9
#define TAG_ENUMERATED                 0x0A
#define TAG_SEQUENCE                   0x10
#define TAG_SEQUENCE_OF                TAG_SEQUENCE
#define TAG_SET                        0x11
#define TAG_SET_OF                     TAG_SET
#define TAG_NumericString              0x12
#define TAG_PrintableString            0x13
#define TAG_TeletexString              0x14
#define TAG_VideotexString             0x15
#define TAG_IA5String                  0x16
#define TAG_UTCTime                    0x17
#define TAG_GeneralizedTime            0x18
#define TAG_GRAPHICSTRING              0x19
#define TAG_VisibleString              0x1A
#define TAG_GENERALSTRING              0x1B
#define TAG_UniversalString            0x1C
#define TAG_BMPString                  0x1E
/*#define CLASS_SPECIAL                  (((tag_t)0x1)<<(__T_CLASS_OFFSET-1))*/
#define TAG_LIST_END                   (252)
#define TAG_ANY                        (253)
#define TAG_CHOICE                     (254)
#define TAG_CHOICE_END                 (255)

#define TAGGING_BASE                   (0)
#define TAGGING_EXPLICIT               (1)
#define TAGGING_IMPLICIT               (2)

#define FLAG_BASE                      0x0000
#define FLAG_OPTIONAL                  0x0001
#define FLAG_ARRAY                     0x0002
#define FLAG_END_OF_ARRAY              0x0004
#define FLAG_CHOICE                    0x0008
#define FLAG_END_OF_CHOICE             0x0010
#define FLAG_DEFAULT                   0x0020
#define FLAG_POINTER                   0x0040
#define FLAG_STRUCT                    0x0080
#define FLAG_BOUNDED                   0x0100

/**/
#define DERERROR	-2
/**/
//#define  RTN_OK 			 0
#define  RTN_ERR 			 1 
#define  RTN_ERR_BUF_LEN                 2
#define  RTN_ERR_ATTRIB_TYPE             3
#define  RTN_ERR_PARAMETER               4
#define  RTN_ERR_INPUT_DATA              5
#define  RTN_ERR_MEMORY_OUT              6
#define  RTN_ERR_KEY_NUMBER              7
#define  RTN_DECRYPT_OUT_TOO_LONG	 8
#define  PriKey_OR_Passwd_ERROR		9

#define  ENCRYPT			1
#define  DECRYPT			0

#define  OPEN				1
#define  SEAL				0

#define  RSA_MODULUS_LONG_LEN		1024   
typedef unsigned int ATTRIB_TYPE;
#define MACHINE_HAVENO_PASS 0x10000181
/* -------------------------------------------
 * operation types definitions
 */
#define ATTRIB_RSA_PUBLIC_KEY		0x0011
#define ATTRIB_RSA_PRIVATE_KEY		0x0012
#define ATTRIB_RSA_KEY_PAIR_GEN		0x0081


#ifndef ATTRIB_CDEA_KEY
#define ATTRIB_CDEA_KEY	                0x0031
#endif
#define ATTRIB_SDBI_KEY		        0x0032
#define ATTRIB_IDEA_KEY		        0x0033


#define ATTRIB_MD5_DIGEST		0x0042
#define ATTRIB_SHA1_DIGEST		0x0043
#define ATTRIB_DHA_DIGEST		0x0044

#define ATTRIB_MD5_WITH_RSA_SIGN        0x0052
#define ATTRIB_SHA1_WITH_RSA_SIGN       0x0053
#define ATTRIB_DHA_WITH_RSA_SIGN        0x0054

#define ATTRIB_MD5_WITH_RSA_VERIFY      0x0062
#define ATTRIB_SHA1_WITH_RSA_VERIFY     0x0063
#define ATTRIB_DHA_WITH_RSA_VERIFY      0x0064

#define ATTRIB_MD5_WITH_RSA             0x0102
#define ATTRIB_SHA1_WITH_RSA            0x0103
#define ATTRIB_DHA_WITH_RSA             0x0104
#define ATTRIB_CDEA_WITH_RSA            0x0202
#define ATTRIB_IDEA_WITH_RSA            0x0201
#define  LENGTH_ERR						100
#define ENCRYPT		1
#define DECRYPT		0
 
/*/#define SignalKeyNumber 0*/
/*/#define EncryptKeyNumber 1*/


#define	ERR_GENERAL					0x4000

#define	ERR_CREAT_HEAP				(ERR_GENERAL + 0x0001)
#define	ERROR_FREE_MEMORY			(ERR_GENERAL + 0x0003)
#define	ERR_PARAM					(ERR_GENERAL + 0x0005)
#define	ERR_CALLOC_MEMORY			(ERR_GENERAL + 0x0007)
#define	ERR_DELETE_HEAP				(ERR_GENERAL + 0x0009)

#define	ERR_VERIFY_SIGNATURE		(ERR_GENERAL + 0x0101)
#define	ERR_NOT_USED_YET			(ERR_GENERAL + 0x0103)
#define	ERR_EXPIRED					(ERR_GENERAL + 0x0105)
#define	ERR_GET_CERTINFO			(ERR_GENERAL + 0x0107)
#define	ERR_CONVERT_SUBJECT_NAME	(ERR_GENERAL + 0x0109)
#define	ERR_CONVERT_ISSUER_NAME		(ERR_GENERAL + 0x010B)
#define	ERR_CONVERT_EXTENSION		(ERR_GENERAL + 0x010D)
#define	ERR_DECODE_X509CERT			(ERR_GENERAL + 0x010F)


typedef int INT4;
#ifdef __cplusplus
  extern "C" {
#endif
SoftWare_API UINT WINAPI CerExpand(UCHAR *cer,UCHAR *TBSCer,int *TBSCerLen,UCHAR *sign_result,INT4 *sign_resultlen);
SoftWare_API UINT WINAPI getcertlen(UCHAR *cert);
SoftWare_API UINT WINAPI GetCertInfo(
	int CertLen,				//DER编码的证书长度
	UCHAR *Cert,					//DER编码的证书
	int CertInfoNo,			//输入的待取出证书项的索引
	INT4 *IntOutData,		//(out)输出的证书项的值(当该值为整数时,从这儿输出)
	int *OutDataLen,		//(in/out)输出的证书项的值的长度(当该值为非整数时,从这儿输出)
	UCHAR *CertInfo				//(out)输出的证书项的值(当该值为非整数时,从这儿输出)
	);
SoftWare_API UINT WINAPI getTVL(UCHAR *TVL,UCHAR *V,int *L,int *offset);
SoftWare_API UINT WINAPI DecodePEMBlock(UCHAR *cert,UINT4 *cert_len,UCHAR *pem,UINT4 pem_len);		  //base64编码文件的长度;
SoftWare_API UINT WINAPI EncodePEMBlock(UCHAR *pem,UINT4 *pem_len,UCHAR *cert,UINT4 cert_len);
SoftWare_API UINT WINAPI DerToPem(UCHAR *cert,INT4 cert_len,char *pem,INT4 *pem_len);
SoftWare_API UINT WINAPI PemToDer(char *pem,INT4 pem_len,UCHAR *cert,INT4 *cert_len);

SoftWare_API UINT WINAPI GetUsrPubKey(UCHAR *cer,UCHAR *subjectPubKeyMod,INT4 *subjectPubKeyModSize);

#ifdef __cplusplus
 }
#endif

	
#endif
