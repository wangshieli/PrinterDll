/*
* 文件名称: ZLib.h
* 文件摘要: 基本函数库
* 当前版本: 1.0，2013-12-12
* 作者信息：zjg
*/

#ifndef _ZLIB_H_
#define _ZLIB_H_

/*
 * 函数名称: ZLib_MidChar
 * 函数功能: 截取指定长度字符串并去除末尾的空格
 * 参    数: 
 *		 	 pcInput    - 源缓冲区
 *		 	 dwPos      - 起始位置
 *           dwLen      - 截取长度
 * 返 回 值: 截取后的字符串
 */
CString ZLib_MidChar(IN LPSTR pcInput, IN DWORD dwPos, IN DWORD dwLen);

/*
 * 函数名称: ZLib_Trim
 * 函数功能: 去除字符串中的前补空格和后补空格
 * 参    数: 
 *		 	 source   - 原始字符串
 * 返 回 值: 清除串开头和串结尾空白字符后的字符串
 */
CString ZLib_Trim(IN CString source);

/*
 * 函数名称: ZLib_CheckNumber
 * 函数功能: 判断是否为指定长度的数字串，如果len为0则不控制长度
 * 参    数: 
 *		  	 csInput   - 数字串缓冲区
 *			 dwLen     - 数据长度
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckNumber(IN CString csInput, IN DWORD dwLen);

/*
 * 函数名称: ZLib_CheckFloat
 * 函数功能: 判断是否为浮点数，iDotnum为小数的最高位数
 * 参    数: 
 *			 csInput   - 传入缓冲区地址
 *			 iDotnum   - 小数位最高位数
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckFloat(IN CString &csInput, IN int iDotnum);
BOOL ZLib_CheckFloat(IN CString &csInput, IN int iIntnum, IN int iDotnum);

/*
 * 函数名称: ZLib_CheckDate
 * 函数功能: 判断是否为日期串，例如YYYYMMDD、YYYYMMDDHHMMSS
 * 参    数: 
 *			 csInput   - 传入缓冲区地址
 *			 csFormat  - 日期格式
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckDate(CString csInput, CString csFormat);

/*
 * 函数名称: ZLib_CheckLastDay
 * 函数功能: 判断日期是否为最后一天
 * 参    数: 
 *			 csInput   - 传入缓冲区地址
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckLastDay(CString csInput);

/*
 * 函数名称: ZLib_CheckIP
 * 函数功能: 判断IP地址格式是否合法，例如192.168.1.1
 * 参    数: 
 *			 csInput   - 传入缓冲区地址
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckIP(CString csInput);

/*
 * 函数名称: ZLib_CheckNsrsbh
 * 函数功能: 判断纳税人识别号是否合法，20位以内可见字符
 * 参    数: 
 *			 csInput - 输入纳税人识别号
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckNsrsbh(IN CString csInput);

/*
 * 函数名称: ZLib_CheckSwjgdm
 * 函数功能: 判断税务机关代码是否合法，9位或11位数字
 * 参    数: 
 *			 csInput - 输入纳税人识别号
 * 返 回 值: true  校验成功
 *			 false 校验失败
 */
BOOL ZLib_CheckSwjgdm(IN CString csInput);

//判断开票机号是否合法,0~255
BOOL ZLib_CheckKpjh(IN CString csInput);

/*
* 函数名称: ZLib_CheckZeroStr
* 函数功能: 校验是否全“0”字符串
* 参    数: 
*           csInput      - 源缓冲区
* 返 回 值: 
*           true         - 成功
*			false        - 失败
*/
BOOL ZLib_CheckZeroStr(IN CString csInput);

//------------------------------------------------------------------------------------------------------

/*
* 函数名称: ZLib_Hexs2Hexb
* 函数功能: 16进制字符串转化为16进制字节流
* 参    数:
*           pcHex        - 字符流缓存
*           dwLen        - 字符长度
*           pbBin        - 字节流缓存
* 返 回 值: NO
*/
void ZLib_Hexs2Hexb(IN const char *pcHex, IN DWORD dwLen, OUT BYTE *pbBin);

/*
* 函数名称: ZLib_Hexb2Hexs
* 函数功能: 16进制字节流转化为16进制字符串
* 参    数:
*           pbBin        - 字节流缓存
*           dwLen        - 数据长度
*           pcHex        - 字符流缓存
* 返 回 值: NO
*/
void ZLib_Hexb2Hexs(IN const BYTE *pbBin, IN DWORD dwLen, OUT char *pcHex);

/*
* 函数名称: ZLib_Hexb2Intn
* 函数功能: 16进制字节流转化为10进制整数
* 参    数:
*           pbHex        - 字节流缓存
*           dwLen        - 数据长度
* 返 回 值: 
*/
DWORD64 ZLib_Hexb2Intn(BYTE *pbHex, DWORD dwLen);

/*
* 函数名称: ZLib_Hexs2Ints
* 函数功能: 16进制字符串转换成10进制整数字符串
* 参    数: 
*           pcHex        - 字符流缓存
*			dwLen        - 数据长度
* 返 回 值: 
*/
CString ZLib_Hexs2Ints(IN const char *pcHex, DWORD dwLen);

/*
* 函数名称: ZLib_Hexs2Intn
* 函数功能: 16进制字符串转化为10进制整数
* 参    数:
*           pcHex        - 字符流缓存
*           dwLen        - 数据长度
* 返 回 值: 
*/
DWORD64 ZLib_Hexs2Intn(IN const char *pcHex, DWORD dwLen);

/*
* 函数名称: ZLib_Ints2Hexs
* 函数功能: 10进制整数字符串转换成16进制字符串，前补“0”
* 参    数: 
*           csDec        - 10进制字符流缓存
*			dwLen        - 格式化后16进制数据长度
* 返 回 值: 
*/
CString ZLib_Ints2Hexs(IN CString csDec, IN DWORD dwLen);

/*
* 函数名称: ZLib_Hexs2Asc
* 函数功能: 16进制字符串转换成ASC码
* 参    数: 
*           pcHex        - 16进制字符流缓存
*			dwLen        - 数据长度
* 返 回 值: ASC码串
* 示    例:
*/
CString ZLib_Hexs2Asc(IN char *pcHex, DWORD dwLen);

/*
* 函数名称: ZLib_Asc2Hexs
* 函数功能: ASC码转换成X位十六进制字符串，右补“20”
* 参    数: 
*           csAsc        - ASC码字符串缓冲区
*           dwLen        - 16进制字符串格式化长度
* 返 回 值: 无
* 示    例: Zlib_Asc2Hex("123",8)="31323320"
*/
CString ZLib_Asc2Hexs(IN CString csAsc, IN DWORD dwLen);

/*
* 函数名称: ZLib_Hexs2Floats
* 函数功能: 16进制字符串转换成浮点数字符串，指定小数位数
* 参    数: 
*           pcHex        - 16进制字符串缓冲区
*			dwLen        - 数据长度
*			bDotNum      - 小数位数
* 返 回 值: 浮点数字符串
* 示    例: ZLib_Hexs2Floats("ff0", 2, 2) = "2.55"
*/
CString ZLib_Hexs2Floats(IN char *pcHex, IN DWORD dwLen, IN BYTE bDotNum);

//浮点数金额字符串转化成十六进制整数字符串，取绝对值
CString ZLib_Floats2Hexs(IN CString csInput, IN BYTE bDotNum, IN DWORD dwLen);

/*
* 函数名称: ZLib_Floats2Intn
* 函数功能: 浮点数字符串转化成整数，取绝对值
* 参    数: 
*           csInput        - 源缓冲区
*			bDotNum        - 小数位数
* 返 回 值: 
* 示    例: ZLib_Floats2Intn("255.01", 2) = 25501
*/
DWORD64 ZLib_Floats2Intn(IN CString csInput, IN BYTE bDotNum);

/*
* 函数名称: ZLib_Hex2Bcd
* 函数功能: 十六进制字符串转换成BCD码
* 参    数: 
*			hex   要转换的十六进制字符串，如"a168fe"
*			len   转换得到的BCD码字节数
*			bcd   转换得到的BCD码，如0xa1,0x68,0xfe
* 返 回 值: 无
* 示    例:
*/
void ZLib_Hex2Bcd(IN LPSTR hex, OUT DWORD* len, OUT LPBYTE bcd);

// 将16进制数据转换为16进制表示的字符串,SepChar为间格字符
CString ZLib_Bcd2Hex(BYTE *Bin, int BinLen, char SepChar);

CString ZLib_dbFen2Yuan(IN double fen);

char* ZLib_ChineseFee(char* dest, size_t destsize, char* src);

//格式化写盘字段为固定长，后补空格
CString ZLib_FormatLen(CString source, int len);

//格式化写盘字段，判断是否转换格式
CString ZLib_FormatData(CString source, int flag);

//格式化日期格式，由“YYYYMMDD”转换成“YYYY-MM-DD”
CString ZLib_FormatDate(CString date);

//格式化日期格式为下一天
CString ZLib_FormatNextday(CString data);

//格式化日期格式为当月最后一天
CString ZLib_FormatLastday(CString data);

/*
* 函数名称: ZLib_FormatSwjgdm
* 函数功能: 格式化税务机关代码，前补“0”或“200”
* 参    数: 
*           csInput                         - 源缓冲区
*           nEncode                         - 编码类型（0 - 填充；1 - 去除填充）
* 返 回 值: 
*           true                            - 成功
*           false                           - 失败
*/
CString ZLib_FormatSwjgdm(CString csInput, int nEncode);


//格式化发票类型代码，2位十六进制转换为3位十进制
CString ZLib_FormatFplxdm(LPSTR fplxdm);

//格式化发票终止号码，根据起始号码和份数计算得到
CString ZLib_FormatZzhm(CString qshm, int fpfs, int formatlen);

//格式化开票机号，10进制与60进制互相转换
CString ZLib_FormatKpjh(CString kpjh, int flag);

//根据长码计算校验码
CString ZLib_FormatJym(CString skm, CString fplxdm);

/*
* 函数名称: ZLib_FormatFloat
* 函数功能: 格式化浮点数字符串为指定格式
* 参    数: 
*           csInput      - 浮点数字符串
*			iDotnum      - 小数位数
* 返 回 值: 
*/
CString ZLib_FormatFloat(IN CString &csInput, IN int iDotnum);

/*
 *	函数功能：ZLib_Sswr
 *  函数功能 把一个字符串四舍五入
 *  参数：
 *       sInput    - 浮点数字符串
 *       iDotnum   - 小数位数
 *  返回值：
 */
CString ZLib_Sswr(IN CString sInput);

int ZLib_SetRegValue(IN char* regPath, IN char* regItem, IN char* regValue);
BSTR ZLib_GetRegValue(IN char* regPath, IN char* regItem);

/*
* 函数名称: ZLib_CreatMulDir
* 函数功能: 创建多级目录
* 参    数: 
*           pcPath                          - 文件夹全路径
* 返 回 值: 
*           true                            - 成功
*           false                           - 失败
*/
BOOL ZLib_CreatMulDir(IN const char *pcPath);


/*
特殊字符的控制
*/
BOOL CheckString(CString Input);

BOOL Zlib_CheckString(CString& strInput, DWORD dwMinLen, DWORD dwMaxLen, CString &strError, BOOL blEnter=false);

BOOL ZLib_SetYbjValue(CString strTop, CString strLeft);

void ZLib_GetYbjValue(CString& strTop, CString& strLeft);

int ZLib_GetPrinterName(CString& strPrinterName);

int ZLib_SetIniYbjValue(CString strFplxdm, CString strTop, CString strLeft, CString strQRCodeSize);

int ZLib_GetIniYbjValue(CString strFplxdm, CString& strTop, CString& strLeft, CString& strQRCodeSize);

int ZLib_SetIniDyjName(CString strFplxdm, CString dyjName);
#endif

