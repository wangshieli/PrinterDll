
#ifndef __SM3SCH_H__
#define __SM3SCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
/// WIN32ƽ̨�¶���uint64_t����
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

//�������Ͷ���
/// ����֧�ֵ�ժҪ�㷨����
typedef enum
{
    /// 0-63������׼ժҪ�㷨, ����Ϊ�Զ����㷨
    DIGALG_SHA1 = 0, 
    DIGALG_SHA256, 
    DIGALG_SHA512, 

    DIGALG_SM3SCH160 = 64, 
    DIGALG_SM3SCH192, 
    DIGALG_SM3SCH256
} digalg_t;

/// OTP��������ַ�����
#define OTP_MAX_DIGITS 10

/// ��Կ��󳤶�
#define MAX_KEY_SIZE 128

/// ��ս��ʽ
typedef enum 
{
    /// δ֪��ʽ
    CF_UNKNOWN = 0, 

    /// ������
    CF_NUMERIC, 

    /// ��ĸ������
    CF_ALPHANUM, 

    /// 16�����ַ���
    CF_HEX
} cf_t;

/// ��С��ս����
#define MIN_CR_CHLG_LEN         4
#define MIN_OCRA_CHALLENGE_LEN  MIN_CR_CHLG_LEN

/// �����ս����
#define MAX_CR_CHLG_LEN         64
#define MAX_OCRA_CHALLENGE_LEN  MAX_CR_CHLG_LEN

/// �����ս�ֽ���(����'\0')
#define MAX_CR_CHLG_SIZE        (MAX_CR_CHLG_LEN + 1)
#define MAX_OCRA_CHALLENGE_SIZE MAX_CR_CHLG_SIZE

/// ��սֵ���ݻ�����(������)����
#define CR_CHLG_BUF_SIZE        128
#define OCRA_CHLG_BUF_SIZE      CR_CHLG_BUF_SIZE

/// ��СӦ�𳤶�
#define MIN_CR_RESP_LEN         4
#define MIN_OCRA_RESPONSE_LEN   MIN_CR_RESP_LEN

/// ���Ӧ�𳤶�
#define MAX_CR_RESP_LEN         10
#define MAX_OCRA_RESPONSE_LEN   MAX_CR_RESP_LEN

/// ���Ӧ���ֽ���(����'\0')
#define MAX_CR_RESP_SIZE        (MAX_CR_RESP_LEN + 1)
#define MAX_OCRA_RESPONSE_SIZE  MAX_CR_RESP_SIZE
//================================================================================================

//����SM3SCH_160ժҪ�ֽ���
#define SM3SCH_160_DIGEST_BYTES 20

//����SM3SCH_192ժҪ�ֽ���
#define SM3SCH_192_DIGEST_BYTES 24

//����SM3SCH_256ժҪ�ֽ���
#define SM3SCH_256_DIGEST_BYTES 32

//����SM3SCHժҪ����ֽ���
#define SM3SCH_DIGEST_MAX_BYTES 32

/********************************************************************
 SM3����ʵ����غ���
 *******************************************************************/

//����SM3ժҪ����ṹ
typedef struct
{
    unsigned long total[2];
    unsigned long state[8];
    unsigned char buffer[64];

    unsigned char ipad[64];
    unsigned char opad[64];

    digalg_t alg;
    int hlen;
} sm3_context;

//sm3_256ժҪ�㷨
void sch256_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//sm3_160ժҪ�㷨
void sch160_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//sm3_192ժҪ�㷨
void sch192_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//��ʼ��SM3
void sm3_init(sm3_context *ctx, digalg_t alg);

//���´���������
void sm3_update(sm3_context *ctx, unsigned char *input, int ilen);

//��������ժҪ���
void sm3_final(sm3_context *ctx, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES]);

//����ָ����������ժҪ
void sch_digest(unsigned char *input, int ilen, digalg_t alg, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//��ʼ��HMAC_SM3
void sm3_hmac_init(sm3_context *ctx, unsigned char *key, int key_len, 
    digalg_t alg);

//���´���������
void sm3_hmac_update(sm3_context *ctx, unsigned char *input, int ilen);

//�������ս��
void sm3_hmac_final(sm3_context *ctx, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES]);

//hmac_sm3ժҪ�㷨
void sm3_hmac(unsigned char *key, unsigned int key_len, 
    unsigned char *data, unsigned int data_len, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len, digalg_t alg);

//hmac_sch256ժҪ�㷨
void sch256_hmac(unsigned char *key, unsigned int key_len, 
    unsigned char *data, unsigned int data_len, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

#ifdef __cplusplus
}
#endif

#endif //__SM3SCH_H__
