/*
* �ļ�����: ZLib_Check.cpp
* �ļ�ժҪ: �ַ�����ʽУ��
* ��ǰ�汾: 1.0��2013-12-12
* ������Ϣ��zjg
*/

#include "../../pch.h"
#include "ZLib.h"

//��ʽ��д���ֶ�Ϊ�̶������󲹿ո�
CString ZLib_FormatLen(CString source, int len)
{
	CString strReturn="";

	strReturn=source+"                    ";
	return strReturn.Left(len);
}

// ��ʽ��˰����ش���
CString ZLib_FormatSwjgdm(IN CString csInput, IN int nEncode)
{
	DWORD dwLen;
	CString strReturn="";

	if(0 == nEncode) //���
	{
		dwLen=csInput.GetLength();
		if(dwLen==9) strReturn.Format("200%s",csInput);
		else if(dwLen==11) strReturn.Format("0%s",csInput);
		else if(dwLen==12) strReturn=csInput;
		else strReturn=""; //˰����ش��볤�ȴ���
	}
	else if(1 == nEncode) //ȥ�����
	{
		strReturn=csInput;
		dwLen=atoi(strReturn.Left(1));
		strReturn=strReturn.Mid(dwLen+1);
	}
	return strReturn;
}

//��ʽ����Ʊ���ʹ��룬2λʮ������ת��Ϊ3λʮ����
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

//��ʽ����Ʊ��ֹ���룬������ʼ����ͷ�������õ�
CString ZLib_FormatZzhm(CString qshm, int fpfs, int formatlen)
{
	CString strReturn="";

	long zzhm=atol((LPSTR)(LPCSTR)qshm)+fpfs-1;
	strReturn.Format("%010d",zzhm);

	return strReturn.Right(formatlen);
}

//��ʽ��д���ֶΣ��ж��Ƿ�ת����ʽ
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

//��ʽ�����ڸ�ʽ���ɡ�YYYYMMDD��ת���ɡ�YYYY-MM-DD��
CString ZLib_FormatDate(CString date)
{
	CString strReturn="";

	if(date.GetLength()!=8) return date;
	strReturn=date;
	strReturn.Insert(4,"-");
	strReturn.Insert(7,"-");
	return strReturn;
}

//��ʽ�����ڸ�ʽΪ��һ��
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

//��ʽ�����ڸ�ʽΪ�������һ��
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

//��ʽ����Ʊ���ţ�10������60���ƻ���ת��
CString ZLib_FormatKpjh(CString kpjh, int flag)
{
	CString strResult="";
	if(flag==0){ //10����ת60����
		int iKpjh=atoi(kpjh);
		strResult.Format("%02d%02d",iKpjh/60,iKpjh%60);
	}
	else{ //60����ת10����
		int a1,a2;
		sscanf_s(kpjh.GetBuffer(0),"%02d%02d",&a1,&a2);
		strResult.Format("%d",a1*60+a2);
	}
	return strResult;
}

// BCD��ת��Ϊʮ������
// ����˵����
//    bcd   Ҫת����bcd��
//    len   Ҫת����bcd����ֽ���
// ����ֵ�� ת�����ʮ������
DWORD64 Bcd2Int(LPBYTE bcd, DWORD len) 
{
	DWORD64 itemp=0;
	for(DWORD i=0; i<len; i++)
		itemp=(itemp<<8)+bcd[i];
	return itemp;
}


//���ݳ������У����
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
	strResult.Delete(0,2); //ȥ��2λ�汾��
	strResult.Delete(strResult.GetLength()-14,14);

	strcpy(ctmp,strResult);
	for(i=0;i<strlen(ctmp);i++){ //�����ַ��滻Ϊ��ĸ
		if(ctmp[i]>='0' && ctmp[i]<='9') continue;
		ctmp[i]=table1[ctmp[i]&0xf];
	}
	ZLib_Hex2Bcd(ctmp,&skmlen,bskm);

	for(i=0;i<8;i++){ //�������
		for(int j=1;j<skmlen/8;j++)
			bskm[i]^=bskm[i+j*8];
	}
	_ui64toa(Bcd2Int(bskm,8),ctmp,10);
	strResult.Format("00000000000000000000%s",ctmp);
	
	return strResult.Right(20);
}

/*
* ��������: ZLib_FormatFloat
* ��������: ��ʽ���������ַ���Ϊָ����ʽ
* ��    ��: 
*           csInput      - �������ַ���
*			iDotnum      - С��λ��
* �� �� ֵ: 
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
