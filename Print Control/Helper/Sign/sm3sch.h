
#ifndef __SM3SCH_H__
#define __SM3SCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
/// WIN32平台下定义uint64_t类型
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

//特殊类型定义
/// 定义支持的摘要算法类型
typedef enum
{
    /// 0-63留给标准摘要算法, 后续为自定义算法
    DIGALG_SHA1 = 0, 
    DIGALG_SHA256, 
    DIGALG_SHA512, 

    DIGALG_SM3SCH160 = 64, 
    DIGALG_SM3SCH192, 
    DIGALG_SM3SCH256
} digalg_t;

/// OTP最大数字字符长度
#define OTP_MAX_DIGITS 10

/// 密钥最大长度
#define MAX_KEY_SIZE 128

/// 挑战格式
typedef enum 
{
    /// 未知格式
    CF_UNKNOWN = 0, 

    /// 数字型
    CF_NUMERIC, 

    /// 字母数字型
    CF_ALPHANUM, 

    /// 16进制字符型
    CF_HEX
} cf_t;

/// 最小挑战长度
#define MIN_CR_CHLG_LEN         4
#define MIN_OCRA_CHALLENGE_LEN  MIN_CR_CHLG_LEN

/// 最大挑战长度
#define MAX_CR_CHLG_LEN         64
#define MAX_OCRA_CHALLENGE_LEN  MAX_CR_CHLG_LEN

/// 最大挑战字节数(包含'\0')
#define MAX_CR_CHLG_SIZE        (MAX_CR_CHLG_LEN + 1)
#define MAX_OCRA_CHALLENGE_SIZE MAX_CR_CHLG_SIZE

/// 挑战值数据缓冲区(补齐后的)长度
#define CR_CHLG_BUF_SIZE        128
#define OCRA_CHLG_BUF_SIZE      CR_CHLG_BUF_SIZE

/// 最小应答长度
#define MIN_CR_RESP_LEN         4
#define MIN_OCRA_RESPONSE_LEN   MIN_CR_RESP_LEN

/// 最大应答长度
#define MAX_CR_RESP_LEN         10
#define MAX_OCRA_RESPONSE_LEN   MAX_CR_RESP_LEN

/// 最大应答字节数(包含'\0')
#define MAX_CR_RESP_SIZE        (MAX_CR_RESP_LEN + 1)
#define MAX_OCRA_RESPONSE_SIZE  MAX_CR_RESP_SIZE
//================================================================================================

//定义SM3SCH_160摘要字节数
#define SM3SCH_160_DIGEST_BYTES 20

//定义SM3SCH_192摘要字节数
#define SM3SCH_192_DIGEST_BYTES 24

//定义SM3SCH_256摘要字节数
#define SM3SCH_256_DIGEST_BYTES 32

//定义SM3SCH摘要最大字节数
#define SM3SCH_DIGEST_MAX_BYTES 32

/********************************************************************
 SM3具体实现相关函数
 *******************************************************************/

//定义SM3摘要计算结构
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

//sm3_256摘要算法
void sch256_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//sm3_160摘要算法
void sch160_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//sm3_192摘要算法
void sch192_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//初始化SM3
void sm3_init(sm3_context *ctx, digalg_t alg);

//更新待处理数据
void sm3_update(sm3_context *ctx, unsigned char *input, int ilen);

//生成最终摘要结果
void sm3_final(sm3_context *ctx, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES]);

//计算指定缓冲区的摘要
void sch_digest(unsigned char *input, int ilen, digalg_t alg, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

//初始化HMAC_SM3
void sm3_hmac_init(sm3_context *ctx, unsigned char *key, int key_len, 
    digalg_t alg);

//更新待处理数据
void sm3_hmac_update(sm3_context *ctx, unsigned char *input, int ilen);

//生成最终结果
void sm3_hmac_final(sm3_context *ctx, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES]);

//hmac_sm3摘要算法
void sm3_hmac(unsigned char *key, unsigned int key_len, 
    unsigned char *data, unsigned int data_len, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len, digalg_t alg);

//hmac_sch256摘要算法
void sch256_hmac(unsigned char *key, unsigned int key_len, 
    unsigned char *data, unsigned int data_len, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len);

#ifdef __cplusplus
}
#endif

#endif //__SM3SCH_H__
