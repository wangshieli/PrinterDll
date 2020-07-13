/*
* 文件名称: ZLib_Check.cpp
* 文件摘要: 字符串格式转换
* 当前版本: 1.0，2013-12-13
* 作者信息：zjg
*/

#include "../../pch.h"
#include "ZLib.h"

/*
* 函数名称: ZLib_Hexs2Hexb
* 函数功能: 16进制字符串转化为16进制字节流
* 参    数:
*           pcHex        - 字符流缓存
*           dwLen        - 字符长度
*           pbBin        - 字节流缓存
* 返 回 值: NO
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
* 函数名称: ZLib_Hexb2Hexs
* 函数功能: 16进制字节流转化为16进制字符串
* 参    数: 
*           pbBin        - 字节流缓存
*           dwLen        - 数据长度
*           pcHex        - 字符流缓存
* 返 回 值: NO
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
* 函数名称: ZLib_Hexb2Intn
* 函数功能: 16进制字节流转化为10进制整数
* 参    数:
*           pbHex        - 字节流缓存
*           dwLen        - 数据长度
* 返 回 值: 
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
* 函数名称: ZLib_Hexs2Intn
* 函数功能: 16进制字符串转化为10进制整数
* 参    数:
*           pcHex        - 字符流缓存
*           dwLen        - 数据长度
* 返 回 值: 
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
* 函数名称: ZLib_Hexs2Ints
* 函数功能: 16进制字符串转换成10进制整数字符串
* 参    数: 
*           pcHex        - 字符流缓存
*			dwLen        - 数据长度
* 返 回 值: 
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
* 函数名称: ZLib_Ints2Hexs
* 函数功能: 10进制整数字符串转换成16进制字符串，前补“0”
* 参    数: 
*           csDec        - 10进制字符流缓存
*			dwLen        - 格式化后16进制数据长度
* 返 回 值: 
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
* 函数名称: ZLib_Hexs2Asc
* 函数功能: 16进制字符串转换成ASC码
* 参    数: 
*           pcHex        - 16进制字符流缓存
*			dwLen        - 数据长度
* 返 回 值: ASC码串
* 示    例:
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
* 函数名称: ZLib_Asc2Hexs
* 函数功能: ASC码转换成X位十六进制字符串，右补“20”
* 参    数: 
*           csAsc        - ASC码字符串缓冲区
*           dwLen        - 16进制字符串格式化长度
* 返 回 值: 无
* 示    例: Zlib_Asc2Hex("123",8)="31323320"
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
	while(strReturn.GetLength()<(int)dwLen) strReturn=strReturn+"20"; //长度不足补空格
	
	return strReturn.Left(dwLen);
}

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

//浮点数金额字符串转化成十六进制整数字符串，取绝对值
CString ZLib_Floats2Hexs(IN CString csInput, IN BYTE bDotNum, IN DWORD dwLen)
{
	CString strReturn,price,value;
	price = csInput;
	price.TrimLeft();
	price.TrimRight();

	value="000000000000";
	if(price.Left(1)=='-') price.Delete(0,1); //取绝对值
	int PointLen = price.Find("."); //从左边开始第一个小数点位置
	if(PointLen==-1) price+=value.Left(bDotNum);
	else{
		int len=price.GetLength()-PointLen-1; //小数点后面的位数
		if(bDotNum>len) price+=value.Left(bDotNum-len);
		else price=price.Left(PointLen+bDotNum+1); //当出现小数点后面位数不止pos位时进行截取
		price.Delete(PointLen,1); //删除从左边开始的第一个小数点
	}
	return ZLib_Ints2Hexs(price,dwLen);
}

/*
* 函数名称: ZLib_Floats2Intn
* 函数功能: 浮点数字符串转化成整数，取绝对值
* 参    数: 
*           csInput        - 源缓冲区
*			bDotNum        - 小数位数
* 返 回 值: 
* 示    例: ZLib_Floats2Intn("255.01", 2) = 25501
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
		csTempBuf.Delete(0, 1); //取绝对值
	}

	nDotPos = csTempBuf.Find("."); //从左边开始第一个小数点位置
	if(-1 == nDotPos)
	{
		for(int nLoop=0; nLoop<bDotNum; nLoop++)
		{
			csTempBuf += "0";
		}
	}
	else
	{
		while(csTempBuf.GetLength() - nDotPos - 1 < bDotNum) //小数点后面剩余位数不足
		{
			csTempBuf += "0";
		}
		csTempBuf = csTempBuf.Left(nDotPos + bDotNum + 1); //小数点后面剩余位数多余bDotNum
		csTempBuf.Delete(nDotPos, 1); //删除小数点
	}

	return _atoi64(csTempBuf.GetBuffer(0));
}


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

// 将16进制数据转换为16进制表示的字符串,SepChar为间格字符
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
 * @brief 将源字符串中的小写金额转换为大写格式
 *
 * @param dest 目的字符串
 * @param src  小写金额字符串
 * @return 
 * - NULL 源字符串的格式错误，返回NULL
 * - 非NULL 目的字符串的首地址
 * @note 转换根据：中国人民银行会计司编写的最新《企业、银行正确办理支付结算
 *       指南》的第114页-第115页
 */
char* ZLib_ChineseFee(char* dest, size_t destsize, char* src)
{
    enum
    {
        START,                 //开始
        MINUS,                 //负号
        ZEROINT,               //0整数
        INTEGER,               //整数
        DECIMAL,               //小数点
        DECIMALfRACTION,       //小数位
        END,                   //结束
        ERROR_                  //错误
    } status = START;
    struct
    {
        int minus;             //0为正，1为负
        int sizeInt;
        int sizeDecimal;
        int integer[20];
        int decimal[20];
    } feeInfo;
    char* NumberChar[] = 
        { "零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖" };
    char* UnitChar[] = 
        { "整",/*"元"*/"圆", "拾", "佰","仟", "万", "拾", "佰", "仟", "亿", 
          "拾", "佰", "仟", "万亿", "拾", "佰", "仟", "亿亿", 
          "角", "分", "负", "人民币" };
          
    int      i, j,size;             //循环变量
    int      zeroTag        = 0,    //0标志
             decZeroTag     = 0;
      
    char*    pDest          = dest;
    char*    pSrc           = src;
    
    int*     pInt           = feeInfo.integer;
    int*     pDec           = feeInfo.decimal;
    
    //初始化
    feeInfo.sizeInt        = 0;
    feeInfo.sizeDecimal    = 0;
    feeInfo.minus          = 0;
    
    //分析字符串
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
    
    //只有1位小数时，设置百分位为0，使下面代码不需要区分这两种情况
    if ( feeInfo.sizeDecimal == 1 )
    {
        feeInfo.decimal[ 1 ] = 0;
        ++feeInfo.sizeDecimal;
    }
    //判断是否需要打印小数部分，有小数部且十分位和百分位不都为0
    //需要打印小数部时，zeroTag设为0，否则设为1
    if ( feeInfo.sizeDecimal == 0                                 //没有小数
        || ( !feeInfo.decimal[ 0 ] && !feeInfo.decimal[ 1 ] ) )   //小数部都为0
        decZeroTag = 1;
    else
        decZeroTag = 0;
        
    //printf( "int size: %d    decimal size: %d\n", feeInfo.sizeInt, feeInfo.sizeDecimal );
    
    strcpy_s( pDest, destsize, "" );                  //初始化目标字符串-人民币
    
    if ( feeInfo.minus ) 
		strcat_s( pDest, destsize, UnitChar[ 20 ] );    //负号
        
    //处理整数部分
    size = feeInfo.sizeInt;

	int h = 0;

    for( i = 0; i < size; ++i )
    {
        j = size - i - 1 & 0x3;                              //j = 0时为段尾

        if ( feeInfo.integer[ i ] == 0 && j )                //处理非段尾0
        {
            zeroTag = 1;
			++h;
        }
        else if ( feeInfo.integer[ i ] == 0 && !j )          //处理段尾0
        {
            if ( feeInfo.sizeInt == 1 && decZeroTag )        //特殊处理个位0
                strcat_s( pDest, destsize, NumberChar[ feeInfo.integer[ i ] ] );
			if(h != 3 || size - i == 1)
			{
				if ( feeInfo.sizeInt != 1 || decZeroTag )
					strcat_s( pDest, destsize, UnitChar[ size - i ] );
			}
            zeroTag = 0;
        }
        else                                                 //处理非0
        {
            if ( zeroTag )
            {
                strcat_s( pDest, destsize, NumberChar[ 0 ] );
                zeroTag = 0;
            }
            strcat_s( pDest, destsize, NumberChar[ feeInfo.integer[ i ] ] );
            strcat_s( pDest, destsize, UnitChar[ size - i ] );
            if ( !j ) zeroTag = 0;                      //如果是段尾，设为非标志
        }

		if(!j)
			h = 0;
    }
    
    if ( decZeroTag )
    {
        strcat_s( pDest, destsize, UnitChar[ 0 ] );//没有小数部，打印"整"字符
    }
    else
    {
        //十分位
        if ( feeInfo.decimal[ 0 ] )
        {
            strcat_s( pDest, destsize, NumberChar[ feeInfo.decimal[ 0 ] ] );
            strcat_s( pDest, destsize, UnitChar[ 18 ] );
        }
        else if ( feeInfo.sizeInt != 1 || feeInfo.integer[ 0 ] )
        {
            strcat_s( pDest, destsize, NumberChar[ feeInfo.decimal[ 0 ] ] );
        }
        
        //百分位不为0时
        if ( feeInfo.decimal[ 1 ] )    
        {
            strcat_s( pDest, destsize, NumberChar[ feeInfo.decimal[ 1 ] ] );
            strcat_s( pDest, destsize, UnitChar[ 19 ] );
        }
    }
    return dest;
}
