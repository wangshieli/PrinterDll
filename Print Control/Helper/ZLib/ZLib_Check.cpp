/*
* 文件名称: ZLib_Check.cpp
* 文件摘要: 字符串格式校验
* 当前版本: 1.0，2013-12-12
* 作者信息：zjg
*/

#include "../../pch.h"
#include "ZLib.h"

/*
* 函数名称: ZLib_CheckNumber
* 函数功能: 判断是否为指定长度的数字串，如果len为0则不控制长度
* 参    数: 
*			pcInput   - 数字串缓冲区
*			dwLen     - 数据长度
* 返 回 值: true  校验成功
*			false 校验失败
*/
BOOL ZLib_CheckNumber(IN CString csInput, IN DWORD dwLen)
{
	if(dwLen>0 && csInput.GetLength()!=(int)dwLen) return false; //长度不合法
	char *pStr=csInput.GetBuffer(0);
	while((*pStr>='0') && (*pStr<='9')) pStr++;
	if(*pStr) return false;
	return true;
}

/*
* 函数名称: ZLib_CheckFloat
* 函数功能: 判断是否为浮点数，iDotnum为小数的最高位数
* 参    数: 
*			csInput   - 传入缓冲区地址
*			iDotnum   - 小数位最高位数
* 返 回 值: true  校验成功
*			false 校验失败
*/
BOOL ZLib_CheckFloat(IN CString& csInput, IN int iDotnum)
{
	int booll = 0;
	csInput.TrimLeft();
	csInput.TrimRight();
	if(csInput.IsEmpty())
		return true;

	if(csInput.Left(1) == '-')
	{
		csInput.Delete(0, 1); //取绝对值
		booll = 1;
	}

	int len = csInput.GetLength();
	char buffer[MAX_PATH + 1] = {0};
	int len1, len2;

	//判断是否有小数点
	if(csInput.Find('.') != -1)
	{
		char bufferDot[MAX_PATH + 1] = {0};
		//匹配数字和小数点
		sscanf_s(csInput, "%[0-9].%[0-9]", buffer, bufferDot);
		//判断buffer和bufferDot是否为空且长度之和是否为总长度-1
		len1= strlen(buffer);
		len2 = strlen(bufferDot);
		if(len1 == 0 || len2 == 0)
			return false;
		if(len != len1 + len2 + 1)
			return false;
		if(len2>iDotnum)
			return false;
		//去掉整数部分最左边的0
		csInput.Format("%s.%s", buffer, bufferDot);
		int length_csInput = csInput.GetLength();
		for (int i = length_csInput-1; i >= 0; i--)
		{
			if (csInput.Mid(i,1).CompareNoCase(".")==0)
			{
				csInput.Delete(i,1);
				break;
			}
			if (csInput.Mid(i,1).CompareNoCase("0")==0)
			{
				csInput.Delete(i,1);
			}
			else
				break;
		}
	}
	else
	{
		//匹配数字
		sscanf_s(csInput, "%[0-9]", buffer);
		len1 = strlen(buffer);
		if(len1 == 0)
			return false;
		if(len1 != len)
			return false;
		//去掉整数部分最左边的0
		//csInput.Format("%d", atoi(buffer));
		//csInput.Format("%d", _tcstol(buffer, NULL, 10));	//tcstol函数最大能出了力7FFFFFFF，对于99亿就会溢出
	}

	if (booll == 1)
	{
		csInput.Insert(0,"-");
	}

	return true;
}

/*
* 函数名称: ZLib_CheckFloat
* 函数功能: 判断是否为浮点数，iDotnum为小数的最高位数
* 参    数: 
*           csInput   - 传入缓冲区地址
*           iIntnum   - 整数位最高位数
*           iDotnum   - 小数位最高位数
* 返 回 值: true  校验成功
*			false 校验失败
*/
BOOL ZLib_CheckFloat(IN CString &csInput, IN int iIntnum, IN int iDotnum)
{
	csInput.TrimLeft();
	csInput.TrimRight();
	if(csInput.IsEmpty())
		return true;
	int len = csInput.GetLength();
	char buffer[MAX_PATH + 1] = {0};
	int len1, len2;
	//判断是否有小数点
	if(csInput.Find('.') != -1)
	{
		char bufferDot[MAX_PATH + 1] = {0};
		//匹配数字和小数点
		sscanf_s(csInput, "%[0-9].%[0-9]", buffer, bufferDot);
		//判断buffer和bufferDot是否为空且长度之和是否为总长度-1
		len1= strlen(buffer);
		len2 = strlen(bufferDot);
		if(len1 == 0 || len2 == 0)
			return false;
		if(len != len1 + len2 + 1)
			return false;
		if(len1>iIntnum) return false;
		if(len2>iDotnum) return false;
		//去掉整数部分最左边的0
		csInput.Format("%s.%s", buffer, bufferDot);
	}
	else
	{
		//匹配数字
		sscanf_s(csInput, "%[0-9]", buffer);
		len1 = strlen(buffer);
		if(len1 == 0)
			return false;
		if(len1 != len)
			return false;
		if(len1>iIntnum)
			return false;
		//去掉整数部分最左边的0
		//csInput.Format("%d", atoi(buffer));
	}
	return true;
}

//判断是否为日期串，例如YYYYMMDD、YYYYMMDDHHMMSS
BOOL ZLib_CheckDate(CString csInput, CString csFormat)
{
	CString csTemp=csInput;

	if(csFormat.CompareNoCase("YYYYMMDDHHMMSS")==0){
		if(csTemp.GetLength()!=14) return false;
		csTemp.Insert(4, '-');
        csTemp.Insert(7, '-');
		csTemp.Insert(10, ' ');
		csTemp.Insert(13, ':');
		csTemp.Insert(16, ':');
	}
	else if(csFormat.CompareNoCase("YYYYMMDD")==0){
		if(csTemp.GetLength()!=8) return false;
		csTemp.Insert(4, '-');
        csTemp.Insert(7, '-');
	}
	else{
		return false;
	}
	
	COleDateTime dt;
	return dt.ParseDateTime(csTemp, VAR_DATEVALUEONLY); //ParseDateTime()只能判断间隔符为“-”的日期字符串
}

//判断日期是否为最后一天
BOOL ZLib_CheckLastDay(CString csInput)
{
	if(ZLib_CheckDate(csInput,"YYYYMMDD")==false) return false;

	CString lastday=ZLib_FormatLastday(csInput);
	if(lastday.CompareNoCase(csInput)==0) return true;
	return false;
}

//判断IP地址格式是否合法
BOOL ZLib_CheckIP(CString csInput)
{
	int pos,loop=0;
	CString strTemp,strValue;
	
	strTemp=csInput;
	while((pos=strTemp.Find(".")) != -1)
	{
		loop++;
		strTemp.Delete(0,pos+1);
	}
	if(loop != 3)
	{
		return FALSE;
	}
	strTemp = csInput+".";
	for(int j=0; j<=loop; j++)
	{
		pos=strTemp.Find(".");
		strValue=strTemp.Left(pos);
		if(strValue.GetLength()>3 || ZLib_CheckNumber(strValue,0)==false) return FALSE;
		int iValue = atoi(strValue);
		strTemp.Delete(0,pos+1);
		if(iValue<0 || iValue>255)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//判断纳税人识别号是否合法，20位以内可见字符
BOOL ZLib_CheckNsrsbh(IN CString csInput)
{
	if(csInput.GetLength()<15 || csInput.GetLength()>20) return false; //长度不合法
	char *pStr=csInput.GetBuffer(0);
	//while((*pStr>=0x21) && (*pStr<=0x80)) pStr++;
	while ((*pStr>='0' && *pStr<='9')
		|| (*pStr>='a' && *pStr<='z')
		|| (*pStr>='A' && *pStr<='Z'))
	pStr++;
	if(*pStr) return false;
	return true;
}

//判断税务机关代码是否合法，9位或11位数字
BOOL ZLib_CheckSwjgdm(IN CString csInput)
{
	if(csInput.GetLength()!=9 && csInput.GetLength()!=11) return false; //长度不合法
	return ZLib_CheckNumber(csInput,0);
}

//判断开票机号是否合法,0~255
BOOL ZLib_CheckKpjh(IN CString csInput)
{
	if(csInput.GetLength()<1 || csInput.GetLength()>3) return false; //长度不合法
	if(ZLib_CheckNumber(csInput,0)==false) return false;
	if(atoi(csInput)>255) return false;
	return true;
}

/*
* 函数名称: ZLib_CheckZeroStr
* 函数功能: 校验是否全“0”字符串
* 参    数: 
*           csInput      - 源缓冲区
* 返 回 值: 
*           true         - 成功
*			false        - 失败
*/
BOOL ZLib_CheckZeroStr(IN CString csInput)
{
	if(0 == csInput.GetLength())
	{
		return true;
	}
	char *pcTempBuf = csInput.GetBuffer(0);
	while('0' == *pcTempBuf)
	{
		pcTempBuf++;
	}
	if(*pcTempBuf)
	{
		return false;
	}
	return true;
}

/*
* 函数名称: GenCheckCode
* 函数功能: 生成第十一位检测码
* 参    数: 
*   pszPartSN -前11位SN号
* 返 回 值: char
*/ 
char GenCheckCode(char *pszPartSN)
{
	static unsigned char Wi[11]={2,1,6,3,7,9,10,5,8,4,2};
	unsigned char ZWi = 0,i;

	for(i=0;i<11;i++)
	{
		ZWi = (ZWi+Wi[i]*(*(pszPartSN+i)-0x30))%11;
	}
	ZWi = (11-ZWi)%10;
	return (char)('0'+ ZWi);
}

BOOL CheckString(CString Input)
{
	CString strHex = "";
	strHex = ZLib_Asc2Hexs(Input, Input.GetLength()*2);

	for (int i = 0; i < strHex.GetLength(); i+=2)
	{
		if (strHex.Mid(i,2).CompareNoCase("3f")==0 ||
			strHex.Mid(i,2).CompareNoCase("0d")==0 ||
			strHex.Mid(i,2).CompareNoCase("0a")==0 )
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*
strInput:输入的参数
dwMinLen最小长度
dwMaxLen最大长度
strError错误信息 
blEnter是否允许有回车符
  */
BOOL Zlib_CheckString(CString& strInput, DWORD dwMinLen, DWORD dwMaxLen, CString &strError, BOOL blEnter)
{
	strInput.TrimLeft();
	strInput.TrimRight();

	CString strInput2 = "";
	strInput2 = strInput;
	CString strHex = "";

	DWORD dwTempLen = strInput2.GetLength();
	if(dwTempLen < dwMinLen)
	{
		if(dwMinLen == 1)
		{
			 strError = "不可为空";
		}
		else
		{
			strError.Format("长度不能小于%d", dwMinLen);
		}
		return false;
	}

	if(dwTempLen > dwMaxLen)
	{
		strError.Format("长度不能大于%d", dwMaxLen);
		return false;
	}

	strHex = ZLib_Asc2Hexs(strInput2, dwTempLen*2);
	for(int i = 0; i < strHex.GetLength(); i+=2)
	{
		if(strHex.Mid(i,2).CompareNoCase("3f")==0 || strHex.Mid(i,2).CompareNoCase("80")==0)
		{
			strError = "包含特殊字符";
			return false;
		}
		if(blEnter==false) //不能包含回车换行符
		{
			if(strHex.Mid(i,2).CompareNoCase("0d")==0 || strHex.Mid(i,2).CompareNoCase("0a")==0)
			{
				strError = "包含回车换行字符";	
				return false;
			}
		}
	}

	return true;
}
