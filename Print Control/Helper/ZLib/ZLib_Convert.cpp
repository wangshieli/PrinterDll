/*
* �ļ�����: ZLib_Check.cpp
* �ļ�ժҪ: �ַ�����ʽת��
* ��ǰ�汾: 1.0��2013-12-13
* ������Ϣ��zjg
*/

#include "../../pch.h"
#include "ZLib.h"

/*
* ��������: ZLib_Hexs2Hexb
* ��������: 16�����ַ���ת��Ϊ16�����ֽ���
* ��    ��:
*           pcHex        - �ַ�������
*           dwLen        - �ַ�����
*           pbBin        - �ֽ�������
* �� �� ֵ: NO
*/
void ZLib_Hexs2Hexb(IN const char *pcHex, IN DWORD dwLen, OUT BYTE *pbBin)
{
    BYTE bChar;

    dwLen = dwLen / 2 + dwLen % 2;

    while(dwLen--)
    {
        bChar = (((*pcHex) <= '9') && ((*pcHex) >= '0')) ? (*pcHex - '0') : (*pcHex - 'A' + 10);
        bChar <<= 4;
        pcHex++;
        bChar |= (((*pcHex <= '9') && (*pcHex >= '0')) ? (*pcHex - '0') : (*pcHex - 'A' + 10)) & 0x0F;
        pcHex++;
        *pbBin++ = bChar;
    }

    return;
}

/*
* ��������: ZLib_Hexb2Hexs
* ��������: 16�����ֽ���ת��Ϊ16�����ַ���
* ��    ��: 
*           pbBin        - �ֽ�������
*           dwLen        - ���ݳ���
*           pcHex        - �ַ�������
* �� �� ֵ: NO
*/
void ZLib_Hexb2Hexs(IN const BYTE *pbBin, IN DWORD dwLen, OUT char *pcHex)
{
    BYTE *pbP;
    BYTE *pbQ;

    pbP = (BYTE *)pcHex;
    pbQ = (BYTE *)pbBin;

    while(dwLen--)
    {
        *pbP = ((*pbQ) & 0xF0) >> 4;
        if (*pbP >= 10)
        {
            *pbP += ('A' - 10);
        }
        else
        {
            *pbP += '0';
        }

        pbP++;
        *pbP = (*pbQ) & 0x0F;
        if(*pbP >= 10)
        {
            *pbP += ('A' - 10);
        }
        else
        {
            *pbP += '0';
        }
        pbP++;
        pbQ++;
    }

    *pbP = 0;

    return;
}

/*
* ��������: ZLib_Hexb2Intn
* ��������: 16�����ֽ���ת��Ϊ10��������
* ��    ��:
*           pbHex        - �ֽ�������
*           dwLen        - ���ݳ���
* �� �� ֵ: 
*/
DWORD64 ZLib_Hexb2Intn(BYTE *pbHex, DWORD dwLen) 
{
	DWORD64 dwResult=0;

	for(DWORD i=0; i<dwLen; i++)
	{
		dwResult=(dwResult<<8)+pbHex[i];
	}

	return dwResult;
}

/*
* ��������: ZLib_Hexs2Intn
* ��������: 16�����ַ���ת��Ϊ10��������
* ��    ��:
*           pcHex        - �ַ�������
*           dwLen        - ���ݳ���
* �� �� ֵ: 
*/
DWORD64 ZLib_Hexs2Intn(IN const char *pcHex, DWORD dwLen) 
{
	BYTE abTemp[64];

	if(dwLen > strlen(pcHex))
	{
		dwLen = strlen(pcHex);
	}
	ZLib_Hexs2Hexb(pcHex, dwLen, abTemp);
	return ZLib_Hexb2Intn(abTemp,dwLen/2 + dwLen%2);
}

/*
* ��������: ZLib_Hexs2Ints
* ��������: 16�����ַ���ת����10���������ַ���
* ��    ��: 
*           pcHex        - �ַ�������
*			dwLen        - ���ݳ���
* �� �� ֵ: 
*/
CString ZLib_Hexs2Ints(IN const char *pcHex, DWORD dwLen) 
{
	CString strResult="";
	char acTempBuf[128];

	if(dwLen > strlen(pcHex))
	{
		dwLen = strlen(pcHex);
	}
	
	_ui64toa_s(ZLib_Hexs2Intn(pcHex, dwLen),acTempBuf, 128,10);
	strResult=acTempBuf;

	return strResult;
}

/*
* ��������: ZLib_Ints2Hexs
* ��������: 10���������ַ���ת����16�����ַ�����ǰ����0��
* ��    ��: 
*           csDec        - 10�����ַ�������
*			dwLen        - ��ʽ����16�������ݳ���
* �� �� ֵ: 
*/
CString ZLib_Ints2Hexs(IN CString csDec, IN DWORD dwLen)
{
	CString strReturn="";

	char ctmp[64];
	DWORD64 dwValue=(DWORD64)_atoi64(csDec);
	_ui64toa_s(dwValue,ctmp, 64,16);
	strReturn=ctmp;
	while(strReturn.GetLength()<(int)dwLen)
	{
		strReturn="0"+strReturn;
	}
	return strReturn;
}

/*
* ��������: ZLib_Hexs2Asc
* ��������: 16�����ַ���ת����ASC��
* ��    ��: 
*           pcHex        - 16�����ַ�������
*			dwLen        - ���ݳ���
* �� �� ֵ: ASC�봮
* ʾ    ��:
*/
CString ZLib_Hexs2Asc(IN char *pcHex, DWORD dwLen)
{
	CString strResult="";
	BYTE bcdb;

	if(dwLen > strlen(pcHex))
	{
		dwLen = strlen(pcHex);
	}
	if(0 == dwLen)
	{
		return "";
	}

	char* pcAsc=new char[dwLen];

	for(int i=0;i<(int)dwLen; i++)
	{
		if(pcHex[i]>='0' && pcHex[i]<='9') bcdb=pcHex[i]-'0';
		else if(pcHex[i]>='a' && pcHex[i]<='f') bcdb=pcHex[i]-'a'+10;
		else bcdb=pcHex[i]-'A'+10;
		pcAsc[i/2]=bcdb+(pcAsc[i/2]<<4);
	}
	pcAsc[(dwLen+1)/2]='\0';
	strResult=pcAsc;
	delete pcAsc;
	strResult.TrimRight();
	return strResult;
}

/*
* ��������: ZLib_Asc2Hexs
* ��������: ASC��ת����Xλʮ�������ַ������Ҳ���20��
* ��    ��: 
*           csAsc        - ASC���ַ���������
*           dwLen        - 16�����ַ�����ʽ������
* �� �� ֵ: ��
* ʾ    ��: Zlib_Asc2Hex("123",8)="31323320"
*/
CString ZLib_Asc2Hexs(IN CString csAsc, IN DWORD dwLen)
{
	CString strReturn="";

	BYTE chb;
	int clen=2*csAsc.GetLength();
	for(int i=0;i<clen;i++){
		if(i%2) chb=csAsc[i/2]&0x0f;
		else    chb=(csAsc[i/2]>>4)&0x0f;
		if(chb<=9) chb+='0';
		else       chb=chb-10+'a';
		strReturn+=chb;
	}
	while(strReturn.GetLength()<(int)dwLen) strReturn=strReturn+"20"; //���Ȳ��㲹�ո�
	
	return strReturn.Left(dwLen);
}

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
CString ZLib_Hexs2Floats(IN char *pcHex, IN DWORD dwLen, IN BYTE bDotNum)
{
	CString strResult="";

	if(dwLen > strlen(pcHex))
	{
		dwLen = strlen(pcHex);
	}
	strResult = ZLib_Hexs2Ints(pcHex, dwLen);
	while(strResult.GetLength() < (bDotNum + 1))
	{
		strResult = "0" + strResult;
	}

	if(0 == atoi(strResult.Right(bDotNum)))
	{
		strResult=strResult.Left(strResult.GetLength()-bDotNum);
	}
	else
	{
		strResult.Insert(strResult.GetLength()-bDotNum, ".");
	}

	return strResult;
}

//����������ַ���ת����ʮ�����������ַ�����ȡ����ֵ
CString ZLib_Floats2Hexs(IN CString csInput, IN BYTE bDotNum, IN DWORD dwLen)
{
	CString strReturn,price,value;
	price = csInput;
	price.TrimLeft();
	price.TrimRight();

	value="000000000000";
	if(price.Left(1)=='-') price.Delete(0,1); //ȡ����ֵ
	int PointLen = price.Find("."); //����߿�ʼ��һ��С����λ��
	if(PointLen==-1) price+=value.Left(bDotNum);
	else{
		int len=price.GetLength()-PointLen-1; //С��������λ��
		if(bDotNum>len) price+=value.Left(bDotNum-len);
		else price=price.Left(PointLen+bDotNum+1); //������С�������λ����ֹposλʱ���н�ȡ
		price.Delete(PointLen,1); //ɾ������߿�ʼ�ĵ�һ��С����
	}
	return ZLib_Ints2Hexs(price,dwLen);
}

/*
* ��������: ZLib_Floats2Intn
* ��������: �������ַ���ת����������ȡ����ֵ
* ��    ��: 
*           csInput        - Դ������
*			bDotNum        - С��λ��
* �� �� ֵ: 
* ʾ    ��: ZLib_Floats2Intn("255.01", 2) = 25501
*/
DWORD64 ZLib_Floats2Intn(IN CString csInput, IN BYTE bDotNum)
{
	int nDotPos = 0;
	CString csTempBuf = "";

	csTempBuf = csInput;
	csTempBuf.TrimLeft();
	csTempBuf.TrimRight();

	if(csTempBuf.Left(1) == '-')
	{
		csTempBuf.Delete(0, 1); //ȡ����ֵ
	}

	nDotPos = csTempBuf.Find("."); //����߿�ʼ��һ��С����λ��
	if(-1 == nDotPos)
	{
		for(int nLoop=0; nLoop<bDotNum; nLoop++)
		{
			csTempBuf += "0";
		}
	}
	else
	{
		while(csTempBuf.GetLength() - nDotPos - 1 < bDotNum) //С�������ʣ��λ������
		{
			csTempBuf += "0";
		}
		csTempBuf = csTempBuf.Left(nDotPos + bDotNum + 1); //С�������ʣ��λ������bDotNum
		csTempBuf.Delete(nDotPos, 1); //ɾ��С����
	}

	return _atoi64(csTempBuf.GetBuffer(0));
}


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
void ZLib_Hex2Bcd(IN LPSTR hex, OUT DWORD* len, OUT LPBYTE bcd) 
{
   BYTE bcdb;
   DWORD chlen=strlen(hex);
   char str[8912]="0";
   if(chlen%2){
	   strcat_s(str,hex);
	   strcpy_s(hex, chlen+1,str);
	   chlen++;
   }

   *len=chlen/2;
   for(DWORD i=0;i<chlen;i++){
	   if(hex[i]>='0' && hex[i]<='9'){
		  bcdb=hex[i]-'0';
	   }
	   else if(hex[i]>='a' && hex[i]<='f'){
		  bcdb=hex[i]-'a'+10;
	   }
	   else{
		  bcdb=hex[i]-'A'+10;
	   }
	   bcd[i/2]=bcdb+(bcd[i/2]<<4);
   }
}

// ��16��������ת��Ϊ16���Ʊ�ʾ���ַ���,SepCharΪ����ַ�
CString ZLib_Bcd2Hex(BYTE *Bin, int BinLen, char SepChar)
{
	CString RetValue="";
	for(int i=0;i<BinLen;i++){
		if((Bin[i]>>4) > 0x09) RetValue += (BYTE)('A'+ (Bin[i]>>4) -10);
		else RetValue += (BYTE)('0'+ (Bin[i]>>4));

		if((Bin[i]&0x0f) > 0x09) RetValue += (BYTE)('A'+ (Bin[i]&0x0f) -10);
		else RetValue += (BYTE)('0'+ (Bin[i]&0x0f));

		if(SepChar) RetValue += SepChar;
	}
	return RetValue;
}

CString ZLib_dbFen2Yuan(IN double fen)
{
	CString strResult="";
	char desc[64];

	int pos=2;
	sprintf_s(desc, 64,"%0.2f",fen/100.0);
	strResult=desc;
	return strResult;
}

/**
 * @brief ��Դ�ַ����е�Сд���ת��Ϊ��д��ʽ
 *
 * @param dest Ŀ���ַ���
 * @param src  Сд����ַ���
 * @return 
 * - NULL Դ�ַ����ĸ�ʽ���󣬷���NULL
 * - ��NULL Ŀ���ַ������׵�ַ
 * @note ת�����ݣ��й��������л��˾��д�����¡���ҵ��������ȷ����֧������
 *       ָ�ϡ��ĵ�114ҳ-��115ҳ
 */
char* ZLib_ChineseFee(char* dest, size_t destsize, char* src)
{
    enum
    {
        START,                 //��ʼ
        MINUS,                 //����
        ZEROINT,               //0����
        INTEGER,               //����
        DECIMAL,               //С����
        DECIMALfRACTION,       //С��λ
        END,                   //����
        ERROR_                  //����
    } status = START;
    struct
    {
        int minus;             //0Ϊ����1Ϊ��
        int sizeInt;
        int sizeDecimal;
        int integer[20];
        int decimal[20];
    } feeInfo;
    char* NumberChar[] = 
        { "��", "Ҽ", "��", "��", "��", "��", "½", "��", "��", "��" };
    char* UnitChar[] = 
        { "��",/*"Ԫ"*/"Բ", "ʰ", "��","Ǫ", "��", "ʰ", "��", "Ǫ", "��", 
          "ʰ", "��", "Ǫ", "����", "ʰ", "��", "Ǫ", "����", 
          "��", "��", "��", "�����" };
          
    int      i, j,size;             //ѭ������
    int      zeroTag        = 0,    //0��־
             decZeroTag     = 0;
      
    char*    pDest          = dest;
    char*    pSrc           = src;
    
    int*     pInt           = feeInfo.integer;
    int*     pDec           = feeInfo.decimal;
    
    //��ʼ��
    feeInfo.sizeInt        = 0;
    feeInfo.sizeDecimal    = 0;
    feeInfo.minus          = 0;
    
    //�����ַ���
    while( 1 )
    {
        switch ( *pSrc )
        {
            case '-' :
                status = ( status == START ) ? MINUS : ERROR_;
                feeInfo.minus = ( status == MINUS ) ? 1 : 0;
                break;
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
            case '0' :
                if ( *pSrc == '0' && status == ZEROINT )//|| status == START ) )
                {
                    status = ERROR_;
                    break;
                }
                if ( status == MINUS || status == START || status == INTEGER )
                {
                    if ( *pSrc == '0' && ( status == MINUS || status == START ) )
                        status = ZEROINT;
                    else
                        status = INTEGER;
                    *pInt = (*pSrc) - 48;
                    ++pInt;
                    ++feeInfo.sizeInt;
                }
                else if ( status == DECIMAL || status == DECIMALfRACTION )
                {
                    status = DECIMALfRACTION;
                    *pDec = (*pSrc) - 48;
                    ++pDec;
                    ++feeInfo.sizeDecimal;
                }
                else
                {
                    status =ERROR_;
                }
                break;
            case '.' :
                status = ( status == INTEGER || status == ZEROINT )? DECIMAL : ERROR_;
                break;
            case '\0' :
                status = ( status == INTEGER || status == DECIMALfRACTION || status == ZEROINT ) ? END : ERROR_;
                break;
            default :
                status = ERROR_;
        }
        if ( status == END ) 
            break;
        else if ( status == ERROR_ )
            return NULL;
        
        ++pSrc;
    }
    
    //ֻ��1λС��ʱ�����ðٷ�λΪ0��ʹ������벻��Ҫ�������������
    if ( feeInfo.sizeDecimal == 1 )
    {
        feeInfo.decimal[ 1 ] = 0;
        ++feeInfo.sizeDecimal;
    }
    //�ж��Ƿ���Ҫ��ӡС�����֣���С������ʮ��λ�Ͱٷ�λ����Ϊ0
    //��Ҫ��ӡС����ʱ��zeroTag��Ϊ0��������Ϊ1
    if ( feeInfo.sizeDecimal == 0                                 //û��С��
        || ( !feeInfo.decimal[ 0 ] && !feeInfo.decimal[ 1 ] ) )   //С������Ϊ0
        decZeroTag = 1;
    else
        decZeroTag = 0;
        
    //printf( "int size: %d    decimal size: %d\n", feeInfo.sizeInt, feeInfo.sizeDecimal );
    
    strcpy_s( pDest, destsize, "" );                  //��ʼ��Ŀ���ַ���-�����
    
    if ( feeInfo.minus ) 
		strcat_s( pDest, destsize, UnitChar[ 20 ] );    //����
        
    //������������
    size = feeInfo.sizeInt;

	int h = 0;

    for( i = 0; i < size; ++i )
    {
        j = size - i - 1 & 0x3;                              //j = 0ʱΪ��β

        if ( feeInfo.integer[ i ] == 0 && j )                //����Ƕ�β0
        {
            zeroTag = 1;
			++h;
        }
        else if ( feeInfo.integer[ i ] == 0 && !j )          //�����β0
        {
            if ( feeInfo.sizeInt == 1 && decZeroTag )        //���⴦���λ0
                strcat_s( pDest, destsize, NumberChar[ feeInfo.integer[ i ] ] );
			if(h != 3 || size - i == 1)
			{
				if ( feeInfo.sizeInt != 1 || decZeroTag )
					strcat_s( pDest, destsize, UnitChar[ size - i ] );
			}
            zeroTag = 0;
        }
        else                                                 //�����0
        {
            if ( zeroTag )
            {
                strcat_s( pDest, destsize, NumberChar[ 0 ] );
                zeroTag = 0;
            }
            strcat_s( pDest, destsize, NumberChar[ feeInfo.integer[ i ] ] );
            strcat_s( pDest, destsize, UnitChar[ size - i ] );
            if ( !j ) zeroTag = 0;                      //����Ƕ�β����Ϊ�Ǳ�־
        }

		if(!j)
			h = 0;
    }
    
    if ( decZeroTag )
    {
        strcat_s( pDest, destsize, UnitChar[ 0 ] );//û��С��������ӡ"��"�ַ�
    }
    else
    {
        //ʮ��λ
        if ( feeInfo.decimal[ 0 ] )
        {
            strcat_s( pDest, destsize, NumberChar[ feeInfo.decimal[ 0 ] ] );
            strcat_s( pDest, destsize, UnitChar[ 18 ] );
        }
        else if ( feeInfo.sizeInt != 1 || feeInfo.integer[ 0 ] )
        {
            strcat_s( pDest, destsize, NumberChar[ feeInfo.decimal[ 0 ] ] );
        }
        
        //�ٷ�λ��Ϊ0ʱ
        if ( feeInfo.decimal[ 1 ] )    
        {
            strcat_s( pDest, destsize, NumberChar[ feeInfo.decimal[ 1 ] ] );
            strcat_s( pDest, destsize, UnitChar[ 19 ] );
        }
    }
    return dest;
}
