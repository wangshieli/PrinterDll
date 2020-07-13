#include "stdafx.h"
#include "sm3sch.h"
#include <string.h>

//获取big-endian序缓冲区的数值
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}

//将数值按big-endian序存储
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}

#define SET_SM3_BITS(ctx, alg) do \
{ \
    switch (alg) \
    { \
    default: \
    case DIGALG_SM3SCH256: \
        ctx -> hlen = SM3SCH_256_DIGEST_BYTES; \
        ctx -> alg = DIGALG_SM3SCH256; \
        break; \
 \
    case DIGALG_SM3SCH160: \
        ctx -> hlen = SM3SCH_160_DIGEST_BYTES; \
        ctx -> alg = DIGALG_SM3SCH160; \
        break; \
 \
    case DIGALG_SM3SCH192: \
        ctx -> hlen = SM3SCH_192_DIGEST_BYTES; \
        ctx -> alg = DIGALG_SM3SCH192; \
        break; \
    } \
} \
while (0)

//初始化SM3
void sm3_init(sm3_context *ctx, digalg_t alg)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x7380166F;
    ctx->state[1] = 0x4914B2B9;
    ctx->state[2] = 0x172442D7;
    ctx->state[3] = 0xDA8A0600;
    ctx->state[4] = 0xA96F30BC;
    ctx->state[5] = 0x163138AA;
    ctx->state[6] = 0xE38DEE4D;
    ctx->state[7] = 0xB0FB0E4E;

    SET_SM3_BITS(ctx, alg);
}

//进行一轮SM3计算
static void sm3_process(sm3_context *ctx, unsigned char data[64])
{
    unsigned long temp1, temp2, W[68], tt1, tt2;
    unsigned long A, B, C, D, E, F, G, H;

//循环左移
#define ROTL(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P0(x) (x ^ ROTL(x, 9) ^ ROTL(x, 17))

#define P1(x) (x ^ ROTL(x, 15) ^ ROTL(x, 23))

#define R(t)                                            \
{                                                       \
    temp1 = W[t - 16] ^ W[t - 9] ^ ROTL(W[t - 3], 15);  \
    W[t] = P1(temp1) ^ ROTL(W[t - 13], 7) ^ W[t - 6];   \
}

#define FF0(x, y, z) (x ^ y ^ z)
#define FF1(x, y, z) ((x & y) | (x & z) | (y & z))

#define GG0(x, y, z) (x ^ y ^ z)
#define GG1(x, y, z) ((x & y) | ((~x) & z))

#define CF0(a, b, c, d, e, f, g, h, j, t)                   \
{                                                           \
    temp1 = ROTL(a, 12) + e + t;                            \
    temp1 = ROTL(temp1, 7);                                 \
    temp2 = temp1 ^ ROTL(a, 12);                            \
    tt1 = FF0(a, b, c) + d + temp2 + (W[j] ^ W[j + 4]);     \
    tt2 = GG0(e, f, g) + h + temp1 + W[j];                  \
    b = ROTL(b, 9); f = ROTL(f, 19); h = tt1; d = P0(tt2);  \
}

#define CF1(a, b, c, d, e, f, g, h, j, t)                   \
{                                                           \
    temp1 = ROTL(a, 12) + e + t;                            \
    temp1 = ROTL(temp1, 7);                                 \
    temp2 = temp1 ^ ROTL(a, 12);                            \
    tt1 = FF1(a, b, c) + d + temp2 + (W[j] ^ W[j + 4]);     \
    tt2 = GG1(e, f, g) + h + temp1 + W[j];                  \
    b = ROTL(b, 9); f = ROTL(f, 19); h = tt1; d = P0(tt2);  \
}

    GET_ULONG_BE( W[ 0], data,  0 );
    GET_ULONG_BE( W[ 1], data,  4 );
    GET_ULONG_BE( W[ 2], data,  8 );
    GET_ULONG_BE( W[ 3], data, 12 );
    GET_ULONG_BE( W[ 4], data, 16 );
    GET_ULONG_BE( W[ 5], data, 20 );
    GET_ULONG_BE( W[ 6], data, 24 );
    GET_ULONG_BE( W[ 7], data, 28 );
    GET_ULONG_BE( W[ 8], data, 32 );
    GET_ULONG_BE( W[ 9], data, 36 );
    GET_ULONG_BE( W[10], data, 40 );
    GET_ULONG_BE( W[11], data, 44 );
    GET_ULONG_BE( W[12], data, 48 );
    GET_ULONG_BE( W[13], data, 52 );
    GET_ULONG_BE( W[14], data, 56 );
    GET_ULONG_BE( W[15], data, 60 );

    R(16); R(17); R(18); R(19); R(20); R(21); R(22); R(23); R(24); R(25);
    R(26); R(27); R(28); R(29); R(30); R(31); R(32); R(33); R(34); R(35);
    R(36); R(37); R(38); R(39); R(40); R(41); R(42); R(43); R(44); R(45);
    R(46); R(47); R(48); R(49); R(50); R(51); R(52); R(53); R(54); R(55);
    R(56); R(57); R(58); R(59); R(60); R(61); R(62); R(63); R(64); R(65);
    R(66); R(67);

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];

    CF0( A, B, C, D, E, F, G, H,  0, 0x79cc4519 );
    CF0( H, A, B, C, D, E, F, G,  1, 0xf3988a32 );
    CF0( G, H, A, B, C, D, E, F,  2, 0xe7311465 );
    CF0( F, G, H, A, B, C, D, E,  3, 0xce6228cb );
    CF0( E, F, G, H, A, B, C, D,  4, 0x9cc45197 );
    CF0( D, E, F, G, H, A, B, C,  5, 0x3988a32f );
    CF0( C, D, E, F, G, H, A, B,  6, 0x7311465e );
    CF0( B, C, D, E, F, G, H, A,  7, 0xe6228cbc );
    CF0( A, B, C, D, E, F, G, H,  8, 0xcc451979 );
    CF0( H, A, B, C, D, E, F, G,  9, 0x988a32f3 );
    CF0( G, H, A, B, C, D, E, F, 10, 0x311465e7 );
    CF0( F, G, H, A, B, C, D, E, 11, 0x6228cbce );
    CF0( E, F, G, H, A, B, C, D, 12, 0xc451979c );
    CF0( D, E, F, G, H, A, B, C, 13, 0x88a32f39 );
    CF0( C, D, E, F, G, H, A, B, 14, 0x11465e73 );
    CF0( B, C, D, E, F, G, H, A, 15, 0x228cbce6 );
    CF1( A, B, C, D, E, F, G, H, 16, 0x9d8a7a87 );
    CF1( H, A, B, C, D, E, F, G, 17, 0x3b14f50f );
    CF1( G, H, A, B, C, D, E, F, 18, 0x7629ea1e );
    CF1( F, G, H, A, B, C, D, E, 19, 0xec53d43c );
    CF1( E, F, G, H, A, B, C, D, 20, 0xd8a7a879 );
    CF1( D, E, F, G, H, A, B, C, 21, 0xb14f50f3 );
    CF1( C, D, E, F, G, H, A, B, 22, 0x629ea1e7 );
    CF1( B, C, D, E, F, G, H, A, 23, 0xc53d43ce );
    CF1( A, B, C, D, E, F, G, H, 24, 0x8a7a879d );
    CF1( H, A, B, C, D, E, F, G, 25, 0x14f50f3b );
    CF1( G, H, A, B, C, D, E, F, 26, 0x29ea1e76 );
    CF1( F, G, H, A, B, C, D, E, 27, 0x53d43cec );
    CF1( E, F, G, H, A, B, C, D, 28, 0xa7a879d8 );
    CF1( D, E, F, G, H, A, B, C, 29, 0x4f50f3b1 );
    CF1( C, D, E, F, G, H, A, B, 30, 0x9ea1e762 );
    CF1( B, C, D, E, F, G, H, A, 31, 0x3d43cec5 );
    CF1( A, B, C, D, E, F, G, H, 32, 0x7a879d8a );
    CF1( H, A, B, C, D, E, F, G, 33, 0xf50f3b14 );
    CF1( G, H, A, B, C, D, E, F, 34, 0xea1e7629 );
    CF1( F, G, H, A, B, C, D, E, 35, 0xd43cec53 );
    CF1( E, F, G, H, A, B, C, D, 36, 0xa879d8a7 );
    CF1( D, E, F, G, H, A, B, C, 37, 0x50f3b14f );
    CF1( C, D, E, F, G, H, A, B, 38, 0xa1e7629e );
    CF1( B, C, D, E, F, G, H, A, 39, 0x43cec53d );
    CF1( A, B, C, D, E, F, G, H, 40, 0x879d8a7a );
    CF1( H, A, B, C, D, E, F, G, 41, 0x0f3b14f5 );
    CF1( G, H, A, B, C, D, E, F, 42, 0x1e7629ea );
    CF1( F, G, H, A, B, C, D, E, 43, 0x3cec53d4 );
    CF1( E, F, G, H, A, B, C, D, 44, 0x79d8a7a8 );
    CF1( D, E, F, G, H, A, B, C, 45, 0xf3b14f50 );
    CF1( C, D, E, F, G, H, A, B, 46, 0xe7629ea1 );
    CF1( B, C, D, E, F, G, H, A, 47, 0xcec53d43 );
    CF1( A, B, C, D, E, F, G, H, 48, 0x9d8a7a87 );
    CF1( H, A, B, C, D, E, F, G, 49, 0x3b14f50f );
    CF1( G, H, A, B, C, D, E, F, 50, 0x7629ea1e );
    CF1( F, G, H, A, B, C, D, E, 51, 0xec53d43c );
    CF1( E, F, G, H, A, B, C, D, 52, 0xd8a7a879 );
    CF1( D, E, F, G, H, A, B, C, 53, 0xb14f50f3 );
    CF1( C, D, E, F, G, H, A, B, 54, 0x629ea1e7 );
    CF1( B, C, D, E, F, G, H, A, 55, 0xc53d43ce );
    CF1( A, B, C, D, E, F, G, H, 56, 0x8a7a879d );
    CF1( H, A, B, C, D, E, F, G, 57, 0x14f50f3b );
    CF1( G, H, A, B, C, D, E, F, 58, 0x29ea1e76 );
    CF1( F, G, H, A, B, C, D, E, 59, 0x53d43cec );
    CF1( E, F, G, H, A, B, C, D, 60, 0xa7a879d8 );
    CF1( D, E, F, G, H, A, B, C, 61, 0x4f50f3b1 );
    CF1( C, D, E, F, G, H, A, B, 62, 0x9ea1e762 );
    CF1( B, C, D, E, F, G, H, A, 63, 0x3d43cec5 );

    ctx->state[0] ^= A;
    ctx->state[1] ^= B;
    ctx->state[2] ^= C;
    ctx->state[3] ^= D;
    ctx->state[4] ^= E;
    ctx->state[5] ^= F;
    ctx->state[6] ^= G;
    ctx->state[7] ^= H;
}

//更新待处理数据
void sm3_update(sm3_context *ctx, unsigned char *input, int ilen)
{
    int fill;
    unsigned long left;

    if (ilen <= 0)
        return;

    //当前已被占用缓冲区的字节数
    left = ctx->total[0] & 0x3F;

    //缓冲区剩余可用字节数
    fill = 64 - left;

    //将待处理数据长度累加到总长度中
    ctx->total[0] += ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    //如果长度超出0x0xFFFFFFFF, 则向前(total[1])进位
    if (ctx->total[0] < (unsigned long) ilen)
        ctx->total[1]++;

    if (left && ilen >= fill)
    {
        //待处理数据长度大于剩余长度
        memcpy((void *) (ctx->buffer + left), (void *) input, fill);
        sm3_process(ctx, ctx->buffer);
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= 64)
    {
        //将未处理完的数据按照64字节进行SM3计算
        sm3_process(ctx, input);
        input += 64;
        ilen  -= 64;
    }

    if (ilen > 0)
    {
        //剩余不足64字节的数据
        memcpy((void *) (ctx->buffer + left), (void *) input, ilen);
    }
}

static const unsigned char sm3_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//生成最终摘要结果
void sm3_final(sm3_context *ctx, unsigned char output[SM3SCH_DIGEST_MAX_BYTES])
{
    unsigned long last, padn;
    unsigned long high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_ULONG_BE( high, msglen, 0 );
    PUT_ULONG_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    sm3_update(ctx, (unsigned char *) sm3_padding, padn);
    sm3_update(ctx, msglen, 8);

    if (ctx -> alg == DIGALG_SM3SCH160)
    {
        ctx->state[0] = ctx->state[0] ^ ctx->state[1] ^ ctx->state[4];
        ctx->state[1] = ctx->state[1] ^ ctx->state[5] ^ ctx->state[2];
        ctx->state[2] = ctx->state[2] ^ ctx->state[6];
        ctx->state[3] = ctx->state[3] ^ ctx->state[7];
        ctx->state[4] = ctx->state[3] ^ ctx->state[7] ^ ctx->state[6];

        PUT_ULONG_BE(ctx->state[0], output,  0);
        PUT_ULONG_BE(ctx->state[1], output,  4);
        PUT_ULONG_BE(ctx->state[2], output,  8);
        PUT_ULONG_BE(ctx->state[3], output, 12);
        PUT_ULONG_BE(ctx->state[4], output, 16);
    }
    else if (ctx -> alg == DIGALG_SM3SCH192)
    {
        ctx->state[0] = ctx->state[0] ^ ctx->state[1] ^ ctx->state[4];
        ctx->state[1] = ctx->state[1] ^ ctx->state[5];
        ctx->state[2] = ctx->state[2] ^ ctx->state[6];
        ctx->state[3] = ctx->state[3] ^ ctx->state[7];
        ctx->state[4] = ctx->state[5] ^ ctx->state[2] ^ ctx->state[6];
        ctx->state[5] = ctx->state[3] ^ ctx->state[7] ^ ctx->state[6];

        PUT_ULONG_BE(ctx->state[0], output,  0);
        PUT_ULONG_BE(ctx->state[1], output,  4);
        PUT_ULONG_BE(ctx->state[2], output,  8);
        PUT_ULONG_BE(ctx->state[3], output, 12);
        PUT_ULONG_BE(ctx->state[4], output, 16);
        PUT_ULONG_BE(ctx->state[5], output, 20);
    }
    else
    {
        PUT_ULONG_BE(ctx->state[0], output,  0);
        PUT_ULONG_BE(ctx->state[1], output,  4);
        PUT_ULONG_BE(ctx->state[2], output,  8);
        PUT_ULONG_BE(ctx->state[3], output, 12);
        PUT_ULONG_BE(ctx->state[4], output, 16);
        PUT_ULONG_BE(ctx->state[5], output, 20);
        PUT_ULONG_BE(ctx->state[6], output, 24);
        PUT_ULONG_BE(ctx->state[7], output, 28);
    }
}

//计算指定缓冲区的摘要
void sch_digest(unsigned char *input, int ilen, digalg_t alg,  
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len)
{
    sm3_context ctx;

    sm3_init(&ctx, alg);
    sm3_update(&ctx, input, ilen);
    sm3_final(&ctx, output);

    if (dig_len)
    {
        *dig_len = ctx.hlen;
    }
}

//sm3_256摘要算法
void sch256_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len)
{
    sm3_context ctx;

    sm3_init(&ctx, DIGALG_SM3SCH256);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, digest);

    if (dig_len)
    {
        *dig_len = ctx.hlen;
    }
}

//sm3_160摘要算法
void sch160_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len)
{
    sm3_context ctx;

    sm3_init(&ctx, DIGALG_SM3SCH160);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, digest);

    if (dig_len)
    {
        *dig_len = ctx.hlen;
    }
}

//sm3_192摘要算法
void sch192_digest(unsigned char *data, unsigned int data_len, 
    unsigned char digest[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len)
{
    sm3_context ctx;

    sm3_init(&ctx, DIGALG_SM3SCH192);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, digest);

    if (dig_len)
    {
        *dig_len = ctx.hlen;
    }
}

/********************************************************************
 HMAC_SM3相关
 *******************************************************************/

void sch256_hmac(unsigned char *key, unsigned int key_len, 
    unsigned char *data, unsigned int data_len, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len)
{
    sm3_hmac(key, key_len, data, data_len, output, dig_len, DIGALG_SM3SCH256);
}

void sm3_hmac_init(sm3_context *ctx, unsigned char *key, int key_len, 
    digalg_t alg)
{
    int i;
    unsigned char sum[SM3SCH_DIGEST_MAX_BYTES];

    if (key_len > 64)
    {
        sch_digest(key, key_len, alg, sum, NULL);
        key_len = (alg == DIGALG_SM3SCH256) ? SM3SCH_256_DIGEST_BYTES : 
                (alg == DIGALG_SM3SCH160 ? SM3SCH_160_DIGEST_BYTES : 
                (alg == DIGALG_SM3SCH192 ? SM3SCH_192_DIGEST_BYTES : 
                SM3SCH_256_DIGEST_BYTES));
        key = sum;
    }

    memset(ctx -> ipad, 0x36, 64);
    memset(ctx -> opad, 0x5C, 64);

    for (i = 0; i < key_len; i++)
    {
        ctx -> ipad[i] = (unsigned char) (ctx -> ipad[i] ^ key[i]);
        ctx -> opad[i] = (unsigned char) (ctx -> opad[i] ^ key[i]);
    }

    sm3_init(ctx, alg);
    sm3_update(ctx, ctx -> ipad, 64);
}

//更新待处理数据
void sm3_hmac_update(sm3_context *ctx, unsigned char *input, int ilen)
{
    sm3_update(ctx, input, ilen);
}

//生成最终结果
void sm3_hmac_final(sm3_context *ctx, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES])
{
    unsigned char tmpbuf[SM3SCH_DIGEST_MAX_BYTES];

    sm3_final(ctx, tmpbuf);
    sm3_init(ctx, ctx -> alg);
    sm3_update(ctx, ctx -> opad, 64);
    sm3_update(ctx, tmpbuf, ctx -> hlen);
    sm3_final(ctx, output);
}

//hmac_sm3摘要算法
void sm3_hmac(unsigned char *key, unsigned int key_len, 
    unsigned char *data, unsigned int data_len, 
    unsigned char output[SM3SCH_DIGEST_MAX_BYTES], 
    unsigned int *dig_len, digalg_t alg)
{
    sm3_context ctx;

    sm3_hmac_init(&ctx, key, key_len, alg);
    sm3_hmac_update(&ctx, data, data_len);
    sm3_hmac_final(&ctx, output);

    if (dig_len)
    {
        *dig_len = ctx.hlen;
    }
}
