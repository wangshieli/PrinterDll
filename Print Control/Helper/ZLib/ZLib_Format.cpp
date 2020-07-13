/*
* 文件名称: ZLib_Check.cpp
* 文件摘要: 字符串格式校验
* 当前版本: 1.0，2013-12-12
* 作者信息：zjg
*/

#include "../../pch.h"
#include "ZLib.h"

//格式化写盘字段为固定长，后补空格
CString ZLib_FormatLen(CString source, int len)
{
	CString strReturn="";

	strReturn=source+"                    ";
	return strReturn.Left(len);
}

// 格式化税务机关代码
CString ZLib_FormatSwjgdm(IN CString csInput, IN int nEncode)
{
	DWORD dwLen;
	CString strReturn="";

	if(0 == nEncode) //填充
	{
		dwLen=csInput.GetLength();
		if(dwLen==9) strReturn.Format("200%s",csInput);
		else if(dwLen==11) strReturn.Format("0%s",csInput);
		else if(dwLen==12) strReturn=csInput;
		else strReturn=""; //税务机关代码长度错误
	}
	else if(1 == nEncode) //去除填充
	{
		strReturn=csInput;
		dwLen=atoi(strReturn.Left(1));
		strReturn=strReturn.Mid(dwLen+1);
	}
	return strReturn;
}

//格式化发票类型代码，2位十六进制转换为3位十进制
CString ZLib_FormatFplxdm(LPSTR fplxdm)
{
	CString strResult="";
	int invotype;
	char ctmp[16];

	for(int i=0;i<(int)strlen(fplxdm);i+=2) {
		sscanf_s(fplxdm+i,"%02x",&invotype);
		sprintf_s(ctmp, 16,"%03d",invotype);
		strResult+=ctmp;
	}
	return strResult;
}

//格式化发票终止号码，根据起始号码和份数计算得到
CString ZLib_FormatZzhm(CString qshm, int fpfs, int formatlen)
{
	CString strReturn="";

	long zzhm=atol((LPSTR)(LPCSTR)qshm)+fpfs-1;
	strReturn.Format("%010d",zzhm);

	return strReturn.Right(formatlen);
}

//格式化写盘字段，判断是否转换格式
CString ZLib_FormatData(CString source, int flag)
{
	CString strReturn="";

	int length=source.GetLength();
	if(flag==1)
		strReturn.Format("%02x%s",length,ZLib_Asc2Hexs(source,length*2));
	else
		strReturn.Format("%02x%s",length/2,source);

	return strReturn;
}

//格式化日期格式，由“YYYYMMDD”转换成“YYYY-MM-DD”
CString ZLib_FormatDate(CString date)
{
	CString strReturn="";

	if(date.GetLength()!=8) return date;
	strReturn=date;
	strReturn.Insert(4,"-");
	strReturn.Insert(7,"-");
	return strReturn;
}

//格式化日期格式为下一天
CString ZLib_FormatNextday(CString data)
{
	int year,month,day;

	if(data.GetLength()!=8) return data;
	sscanf_s(data,"%04d%02d%02d",&year,&month,&day);
	CTime t1(year,month,day,0,0,0);
	CTimeSpan ts(1,0,0,0);
	t1+=ts;
	return t1.Format("%Y%m%d");
}

//格式化日期格式为当月最后一天
CString ZLib_FormatLastday(CString data)
{
	int year,month,day;

	if(data.GetLength()!=8) return data;
	sscanf_s(data,"%04d%02d%02d",&year,&month,&day);
	CTime t1(year+month/12,month%12+1,1,0,0,0);
	CTimeSpan ts(-1,0,0,0);
	t1+=ts;
	return t1.Format("%Y%m%d");
}

//格式化开票机号，10进制与60进制互相转换
CString ZLib_FormatKpjh(CString kpjh, int flag)
{
	CString strResult="";
	if(flag==0){ //10进制转60进制
		int iKpjh=atoi(kpjh);
		strResult.Format("%02d%02d",iKpjh/60,iKpjh%60);
	}
	else{ //60进制转10进制
		int a1,a2;
		sscanf_s(kpjh.GetBuffer(0),"%02d%02d",&a1,&a2);
		strResult.Format("%d",a1*60+a2);
	}
	return strResult;
}

// BCD码转换为十进制数
// 参数说明：
//    bcd   要转换的bcd码
//    len   要转换的bcd码的字节数
// 返回值： 转换后的十进制数
DWORD64 Bcd2Int(LPBYTE bcd, DWORD len) 
{
	DWORD64 itemp=0;
	for(DWORD i=0; i<len; i++)
		itemp=(itemp<<8)+bcd[i];
	return itemp;
}


//根据长码计算校验码
CString ZLib_FormatJym(CString skm, CString fplxdm)
{
	char table1[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	CString strResult="00000000000000000000";
	
	DWORD skmlen;
	char ctmp[1024];
	BYTE bskm[256];

	if(skm.IsEmpty()) return "";
	strResult=skm;
	for(int i=0,pos=0;i<4;i++){
		pos+=24;
		strResult.Delete(pos,4);
	}
	strResult.Delete(0,2); //去除2位版本号
	strResult.Delete(strResult.GetLength()-14,14);

	strcpy(ctmp,strResult);
	for(i=0;i<strlen(ctmp);i++){ //特殊字符替换为字母
		if(ctmp[i]>='0' && ctmp[i]<='9') continue;
		ctmp[i]=table1[ctmp[i]&0xf];
	}
	ZLib_Hex2Bcd(ctmp,&skmlen,bskm);

	for(i=0;i<8;i++){ //分组异或
		for(int j=1;j<skmlen/8;j++)
			bskm[i]^=bskm[i+j*8];
	}
	_ui64toa(Bcd2Int(bskm,8),ctmp,10);
	strResult.Format("00000000000000000000%s",ctmp);
	
	return strResult.Right(20);
}

/*
* 函数名称: ZLib_FormatFloat
* 函数功能: 格式化浮点数字符串为指定格式
* 参    数: 
*           csInput      - 浮点数字符串
*			iDotnum      - 小数位数
* 返 回 值: 
*/
CString ZLib_FormatFloat(IN CString &csInput, IN int iDotnum)
{
	CString strReturn = "";

	strReturn = csInput;
	strReturn.TrimRight();
	strReturn.TrimLeft();
	if(strReturn.IsEmpty())
	{
		strReturn = "0";
	}
	if(iDotnum < 0)
	{
		return "";
	}
	if(strReturn.Left(1).Compare(".") == 0)
	{
		strReturn = "0" + strReturn;
	}
	int iLen = strReturn.GetLength();
	int iDotPos = strReturn.Find('.');
	if(iDotnum == 0)
	{
		if(iDotPos < 0)
		{
			iDotPos = iLen;
		}
		strReturn = strReturn.Left(iDotPos);
		return strReturn;
	}

	if(iDotPos < 0)
	{
		strReturn += ".";
		for(int index = 0; index < iDotnum; index++)
		{
			strReturn += "0";
		}
	}
	else
	{
		int icount = iLen - iDotPos - 1;
		if(icount > iDotnum)
		{
			if(iDotnum == 8)
			{
				strReturn = strReturn.Left(iLen + iDotnum - icount + 1);

				strReturn = ZLib_Sswr(strReturn);
				return strReturn;
			}
			strReturn = strReturn.Left(iLen + iDotnum - icount);
		}
		else
		{
			icount = iDotnum - icount;
			for(int index = 0; index < icount; index++)
			{
				strReturn += "0";
			}
		}
	}
	return strReturn;
}

CString ZLib_Sswr(CString sInput)
{
	int iLen = sInput.GetLength();
	int iDotPos = sInput.Find('.');

	BOOL NeedToAdd = TRUE;
	for(int i = iLen - 1; i >= 0; i--)
	{
		if(i == iLen - 1)
		{
			char sTemp = sInput.GetAt(i);
			if(sTemp > '4')
			{
				continue;
			}
			else
			{
				sInput = sInput.Left(sInput.GetLength() - 1);
				return sInput;
			}
		}
		else if(sInput.GetAt(i) == '.')
		{
			continue;
		}
		else
		{
			if(NeedToAdd)
			{
				char Temp = sInput.GetAt(i);
				Temp++;
				if(Temp > '9')
				{
					Temp = '0';
					sInput.SetAt(i, Temp);
					NeedToAdd = TRUE;
				}
				else
				{
					NeedToAdd = FALSE;
					sInput.SetAt(i, Temp);
					break;
				}
			}
		}
	}
	if(NeedToAdd)
	{
		sInput = "1" + sInput;
	}

	sInput = sInput.Left(sInput.GetLength() - 1);
	return sInput;
}
