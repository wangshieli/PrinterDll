/*
* �ļ�����: ZLib.h
* �ļ�ժҪ: ����������
* ��ǰ�汾: 1.0��2013-12-12
* ������Ϣ��zjg
*/

#ifndef _ZLIB_H_
#define _ZLIB_H_

/*
 * ��������: ZLib_MidChar
 * ��������: ��ȡָ�������ַ�����ȥ��ĩβ�Ŀո�
 * ��    ��: 
 *		 	 pcInput    - Դ������
 *		 	 dwPos      - ��ʼλ��
 *           dwLen      - ��ȡ����
 * �� �� ֵ: ��ȡ����ַ���
 */
CString ZLib_MidChar(IN LPSTR pcInput, IN DWORD dwPos, IN DWORD dwLen);

/*
 * ��������: ZLib_Trim
 * ��������: ȥ���ַ����е�ǰ���ո�ͺ󲹿ո�
 * ��    ��: 
 *		 	 source   - ԭʼ�ַ���
 * �� �� ֵ: �������ͷ�ʹ���β�հ��ַ�����ַ���
 */
CString ZLib_Trim(IN CString source);

/*
 * ��������: ZLib_CheckNumber
 * ��������: �ж��Ƿ�Ϊָ�����ȵ����ִ������lenΪ0�򲻿��Ƴ���
 * ��    ��: 
 *		  	 csInput   - ���ִ�������
 *			 dwLen     - ���ݳ���
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckNumber(IN CString csInput, IN DWORD dwLen);

/*
 * ��������: ZLib_CheckFloat
 * ��������: �ж��Ƿ�Ϊ��������iDotnumΪС�������λ��
 * ��    ��: 
 *			 csInput   - ���뻺������ַ
 *			 iDotnum   - С��λ���λ��
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckFloat(IN CString &csInput, IN int iDotnum);
BOOL ZLib_CheckFloat(IN CString &csInput, IN int iIntnum, IN int iDotnum);

/*
 * ��������: ZLib_CheckDate
 * ��������: �ж��Ƿ�Ϊ���ڴ�������YYYYMMDD��YYYYMMDDHHMMSS
 * ��    ��: 
 *			 csInput   - ���뻺������ַ
 *			 csFormat  - ���ڸ�ʽ
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckDate(CString csInput, CString csFormat);

/*
 * ��������: ZLib_CheckLastDay
 * ��������: �ж������Ƿ�Ϊ���һ��
 * ��    ��: 
 *			 csInput   - ���뻺������ַ
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckLastDay(CString csInput);

/*
 * ��������: ZLib_CheckIP
 * ��������: �ж�IP��ַ��ʽ�Ƿ�Ϸ�������192.168.1.1
 * ��    ��: 
 *			 csInput   - ���뻺������ַ
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckIP(CString csInput);

/*
 * ��������: ZLib_CheckNsrsbh
 * ��������: �ж���˰��ʶ����Ƿ�Ϸ���20λ���ڿɼ��ַ�
 * ��    ��: 
 *			 csInput - ������˰��ʶ���
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckNsrsbh(IN CString csInput);

/*
 * ��������: ZLib_CheckSwjgdm
 * ��������: �ж�˰����ش����Ƿ�Ϸ���9λ��11λ����
 * ��    ��: 
 *			 csInput - ������˰��ʶ���
 * �� �� ֵ: true  У��ɹ�
 *			 false У��ʧ��
 */
BOOL ZLib_CheckSwjgdm(IN CString csInput);

//�жϿ�Ʊ�����Ƿ�Ϸ�,0~255
BOOL ZLib_CheckKpjh(IN CString csInput);

/*
* ��������: ZLib_CheckZeroStr
* ��������: У���Ƿ�ȫ��0���ַ���
* ��    ��: 
*           csInput      - Դ������
* �� �� ֵ: 
*           true         - �ɹ�
*			false        - ʧ��
*/
BOOL ZLib_CheckZeroStr(IN CString csInput);

//------------------------------------------------------------------------------------------------------

/*
* ��������: ZLib_Hexs2Hexb
* ��������: 16�����ַ���ת��Ϊ16�����ֽ���
* ��    ��:
*           pcHex        - �ַ�������
*           dwLen        - �ַ�����
*           pbBin        - �ֽ�������
* �� �� ֵ: NO
*/
void ZLib_Hexs2Hexb(IN const char *pcHex, IN DWORD dwLen, OUT BYTE *pbBin);

/*
* ��������: ZLib_Hexb2Hexs
* ��������: 16�����ֽ���ת��Ϊ16�����ַ���
* ��    ��:
*           pbBin        - �ֽ�������
*           dwLen        - ���ݳ���
*           pcHex        - �ַ�������
* �� �� ֵ: NO
*/
void ZLib_Hexb2Hexs(IN const BYTE *pbBin, IN DWORD dwLen, OUT char *pcHex);

/*
* ��������: ZLib_Hexb2Intn
* ��������: 16�����ֽ���ת��Ϊ10��������
* ��    ��:
*           pbHex        - �ֽ�������
*           dwLen        - ���ݳ���
* �� �� ֵ: 
*/
DWORD64 ZLib_Hexb2Intn(BYTE *pbHex, DWORD dwLen);

/*
* ��������: ZLib_Hexs2Ints
* ��������: 16�����ַ���ת����10���������ַ���
* ��    ��: 
*           pcHex        - �ַ�������
*			dwLen        - ���ݳ���
* �� �� ֵ: 
*/
CString ZLib_Hexs2Ints(IN const char *pcHex, DWORD dwLen);

/*
* ��������: ZLib_Hexs2Intn
* ��������: 16�����ַ���ת��Ϊ10��������
* ��    ��:
*           pcHex        - �ַ�������
*           dwLen        - ���ݳ���
* �� �� ֵ: 
*/
DWORD64 ZLib_Hexs2Intn(IN const char *pcHex, DWORD dwLen);

/*
* ��������: ZLib_Ints2Hexs
* ��������: 10���������ַ���ת����16�����ַ�����ǰ����0��
* ��    ��: 
*           csDec        - 10�����ַ�������
*			dwLen        - ��ʽ����16�������ݳ���
* �� �� ֵ: 
*/
CString ZLib_Ints2Hexs(IN CString csDec, IN DWORD dwLen);

/*
* ��������: ZLib_Hexs2Asc
* ��������: 16�����ַ���ת����ASC��
* ��    ��: 
*           pcHex        - 16�����ַ�������
*			dwLen        - ���ݳ���
* �� �� ֵ: ASC�봮
* ʾ    ��:
*/
CString ZLib_Hexs2Asc(IN char *pcHex, DWORD dwLen);

/*
* ��������: ZLib_Asc2Hexs
* ��������: ASC��ת����Xλʮ�������ַ������Ҳ���20��
* ��    ��: 
*           csAsc        - ASC���ַ���������
*           dwLen        - 16�����ַ�����ʽ������
* �� �� ֵ: ��
* ʾ    ��: Zlib_Asc2Hex("123",8)="31323320"
*/
CString ZLib_Asc2Hexs(IN CString csAsc, IN DWORD dwLen);

/*
* ��������: ZLib_Hexs2Floats
* ��������: 16�����ַ���ת���ɸ������ַ�����ָ��С��λ��
* ��    ��: 
*           pcHex        - 16�����ַ���������
*			dwLen        - ���ݳ���
*			bDotNum      - С��λ��
* �� �� ֵ: �������ַ���
* ʾ    ��: ZLib_Hexs2Floats("ff0", 2, 2) = "2.55"
*/
CString ZLib_Hexs2Floats(IN char *pcHex, IN DWORD dwLen, IN BYTE bDotNum);

//����������ַ���ת����ʮ�����������ַ�����ȡ����ֵ
CString ZLib_Floats2Hexs(IN CString csInput, IN BYTE bDotNum, IN DWORD dwLen);

/*
* ��������: ZLib_Floats2Intn
* ��������: �������ַ���ת����������ȡ����ֵ
* ��    ��: 
*           csInput        - Դ������
*			bDotNum        - С��λ��
* �� �� ֵ: 
* ʾ    ��: ZLib_Floats2Intn("255.01", 2) = 25501
*/
DWORD64 ZLib_Floats2Intn(IN CString csInput, IN BYTE bDotNum);

/*
* ��������: ZLib_Hex2Bcd
* ��������: ʮ�������ַ���ת����BCD��
* ��    ��: 
*			hex   Ҫת����ʮ�������ַ�������"a168fe"
*			len   ת���õ���BCD���ֽ���
*			bcd   ת���õ���BCD�룬��0xa1,0x68,0xfe
* �� �� ֵ: ��
* ʾ    ��:
*/
void ZLib_Hex2Bcd(IN LPSTR hex, OUT DWORD* len, OUT LPBYTE bcd);

// ��16��������ת��Ϊ16���Ʊ�ʾ���ַ���,SepCharΪ����ַ�
CString ZLib_Bcd2Hex(BYTE *Bin, int BinLen, char SepChar);

CString ZLib_dbFen2Yuan(IN double fen);

char* ZLib_ChineseFee(char* dest, size_t destsize, char* src);

//��ʽ��д���ֶ�Ϊ�̶������󲹿ո�
CString ZLib_FormatLen(CString source, int len);

//��ʽ��д���ֶΣ��ж��Ƿ�ת����ʽ
CString ZLib_FormatData(CString source, int flag);

//��ʽ�����ڸ�ʽ���ɡ�YYYYMMDD��ת���ɡ�YYYY-MM-DD��
CString ZLib_FormatDate(CString date);

//��ʽ�����ڸ�ʽΪ��һ��
CString ZLib_FormatNextday(CString data);

//��ʽ�����ڸ�ʽΪ�������һ��
CString ZLib_FormatLastday(CString data);

/*
* ��������: ZLib_FormatSwjgdm
* ��������: ��ʽ��˰����ش��룬ǰ����0����200��
* ��    ��: 
*           csInput                         - Դ������
*           nEncode                         - �������ͣ�0 - ��䣻1 - ȥ����䣩
* �� �� ֵ: 
*           true                            - �ɹ�
*           false                           - ʧ��
*/
CString ZLib_FormatSwjgdm(CString csInput, int nEncode);


//��ʽ����Ʊ���ʹ��룬2λʮ������ת��Ϊ3λʮ����
CString ZLib_FormatFplxdm(LPSTR fplxdm);

//��ʽ����Ʊ��ֹ���룬������ʼ����ͷ�������õ�
CString ZLib_FormatZzhm(CString qshm, int fpfs, int formatlen);

//��ʽ����Ʊ���ţ�10������60���ƻ���ת��
CString ZLib_FormatKpjh(CString kpjh, int flag);

//���ݳ������У����
CString ZLib_FormatJym(CString skm, CString fplxdm);

/*
* ��������: ZLib_FormatFloat
* ��������: ��ʽ���������ַ���Ϊָ����ʽ
* ��    ��: 
*           csInput      - �������ַ���
*			iDotnum      - С��λ��
* �� �� ֵ: 
*/
CString ZLib_FormatFloat(IN CString &csInput, IN int iDotnum);

/*
 *	�������ܣ�ZLib_Sswr
 *  �������� ��һ���ַ�����������
 *  ������
 *       sInput    - �������ַ���
 *       iDotnum   - С��λ��
 *  ����ֵ��
 */
CString ZLib_Sswr(IN CString sInput);

int ZLib_SetRegValue(IN char* regPath, IN char* regItem, IN char* regValue);
BSTR ZLib_GetRegValue(IN char* regPath, IN char* regItem);

/*
* ��������: ZLib_CreatMulDir
* ��������: �����༶Ŀ¼
* ��    ��: 
*           pcPath                          - �ļ���ȫ·��
* �� �� ֵ: 
*           true                            - �ɹ�
*           false                           - ʧ��
*/
BOOL ZLib_CreatMulDir(IN const char *pcPath);


/*
�����ַ��Ŀ���
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

