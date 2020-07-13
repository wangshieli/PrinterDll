/*
* �ļ�����: ZLib_Check.cpp
* �ļ�ժҪ: �ַ�����ʽУ��
* ��ǰ�汾: 1.0��2013-12-12
* ������Ϣ��zjg
*/

#include "../../pch.h"
#include "ZLib.h"

/*
* ��������: ZLib_CheckNumber
* ��������: �ж��Ƿ�Ϊָ�����ȵ����ִ������lenΪ0�򲻿��Ƴ���
* ��    ��: 
*			pcInput   - ���ִ�������
*			dwLen     - ���ݳ���
* �� �� ֵ: true  У��ɹ�
*			false У��ʧ��
*/
BOOL ZLib_CheckNumber(IN CString csInput, IN DWORD dwLen)
{
	if(dwLen>0 && csInput.GetLength()!=(int)dwLen) return false; //���Ȳ��Ϸ�
	char *pStr=csInput.GetBuffer(0);
	while((*pStr>='0') && (*pStr<='9')) pStr++;
	if(*pStr) return false;
	return true;
}

/*
* ��������: ZLib_CheckFloat
* ��������: �ж��Ƿ�Ϊ��������iDotnumΪС�������λ��
* ��    ��: 
*			csInput   - ���뻺������ַ
*			iDotnum   - С��λ���λ��
* �� �� ֵ: true  У��ɹ�
*			false У��ʧ��
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
		csInput.Delete(0, 1); //ȡ����ֵ
		booll = 1;
	}

	int len = csInput.GetLength();
	char buffer[MAX_PATH + 1] = {0};
	int len1, len2;

	//�ж��Ƿ���С����
	if(csInput.Find('.') != -1)
	{
		char bufferDot[MAX_PATH + 1] = {0};
		//ƥ�����ֺ�С����
		sscanf_s(csInput, "%[0-9].%[0-9]", buffer, bufferDot);
		//�ж�buffer��bufferDot�Ƿ�Ϊ���ҳ���֮���Ƿ�Ϊ�ܳ���-1
		len1= strlen(buffer);
		len2 = strlen(bufferDot);
		if(len1 == 0 || len2 == 0)
			return false;
		if(len != len1 + len2 + 1)
			return false;
		if(len2>iDotnum)
			return false;
		//ȥ��������������ߵ�0
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
		//ƥ������
		sscanf_s(csInput, "%[0-9]", buffer);
		len1 = strlen(buffer);
		if(len1 == 0)
			return false;
		if(len1 != len)
			return false;
		//ȥ��������������ߵ�0
		//csInput.Format("%d", atoi(buffer));
		//csInput.Format("%d", _tcstol(buffer, NULL, 10));	//tcstol��������ܳ�����7FFFFFFF������99�ھͻ����
	}

	if (booll == 1)
	{
		csInput.Insert(0,"-");
	}

	return true;
}

/*
* ��������: ZLib_CheckFloat
* ��������: �ж��Ƿ�Ϊ��������iDotnumΪС�������λ��
* ��    ��: 
*           csInput   - ���뻺������ַ
*           iIntnum   - ����λ���λ��
*           iDotnum   - С��λ���λ��
* �� �� ֵ: true  У��ɹ�
*			false У��ʧ��
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
	//�ж��Ƿ���С����
	if(csInput.Find('.') != -1)
	{
		char bufferDot[MAX_PATH + 1] = {0};
		//ƥ�����ֺ�С����
		sscanf_s(csInput, "%[0-9].%[0-9]", buffer, bufferDot);
		//�ж�buffer��bufferDot�Ƿ�Ϊ���ҳ���֮���Ƿ�Ϊ�ܳ���-1
		len1= strlen(buffer);
		len2 = strlen(bufferDot);
		if(len1 == 0 || len2 == 0)
			return false;
		if(len != len1 + len2 + 1)
			return false;
		if(len1>iIntnum) return false;
		if(len2>iDotnum) return false;
		//ȥ��������������ߵ�0
		csInput.Format("%s.%s", buffer, bufferDot);
	}
	else
	{
		//ƥ������
		sscanf_s(csInput, "%[0-9]", buffer);
		len1 = strlen(buffer);
		if(len1 == 0)
			return false;
		if(len1 != len)
			return false;
		if(len1>iIntnum)
			return false;
		//ȥ��������������ߵ�0
		//csInput.Format("%d", atoi(buffer));
	}
	return true;
}

//�ж��Ƿ�Ϊ���ڴ�������YYYYMMDD��YYYYMMDDHHMMSS
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
	return dt.ParseDateTime(csTemp, VAR_DATEVALUEONLY); //ParseDateTime()ֻ���жϼ����Ϊ��-���������ַ���
}

//�ж������Ƿ�Ϊ���һ��
BOOL ZLib_CheckLastDay(CString csInput)
{
	if(ZLib_CheckDate(csInput,"YYYYMMDD")==false) return false;

	CString lastday=ZLib_FormatLastday(csInput);
	if(lastday.CompareNoCase(csInput)==0) return true;
	return false;
}

//�ж�IP��ַ��ʽ�Ƿ�Ϸ�
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

//�ж���˰��ʶ����Ƿ�Ϸ���20λ���ڿɼ��ַ�
BOOL ZLib_CheckNsrsbh(IN CString csInput)
{
	if(csInput.GetLength()<15 || csInput.GetLength()>20) return false; //���Ȳ��Ϸ�
	char *pStr=csInput.GetBuffer(0);
	//while((*pStr>=0x21) && (*pStr<=0x80)) pStr++;
	while ((*pStr>='0' && *pStr<='9')
		|| (*pStr>='a' && *pStr<='z')
		|| (*pStr>='A' && *pStr<='Z'))
	pStr++;
	if(*pStr) return false;
	return true;
}

//�ж�˰����ش����Ƿ�Ϸ���9λ��11λ����
BOOL ZLib_CheckSwjgdm(IN CString csInput)
{
	if(csInput.GetLength()!=9 && csInput.GetLength()!=11) return false; //���Ȳ��Ϸ�
	return ZLib_CheckNumber(csInput,0);
}

//�жϿ�Ʊ�����Ƿ�Ϸ�,0~255
BOOL ZLib_CheckKpjh(IN CString csInput)
{
	if(csInput.GetLength()<1 || csInput.GetLength()>3) return false; //���Ȳ��Ϸ�
	if(ZLib_CheckNumber(csInput,0)==false) return false;
	if(atoi(csInput)>255) return false;
	return true;
}

/*
* ��������: ZLib_CheckZeroStr
* ��������: У���Ƿ�ȫ��0���ַ���
* ��    ��: 
*           csInput      - Դ������
* �� �� ֵ: 
*           true         - �ɹ�
*			false        - ʧ��
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
* ��������: GenCheckCode
* ��������: ���ɵ�ʮһλ�����
* ��    ��: 
*   pszPartSN -ǰ11λSN��
* �� �� ֵ: char
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
strInput:����Ĳ���
dwMinLen��С����
dwMaxLen��󳤶�
strError������Ϣ 
blEnter�Ƿ������лس���
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
			 strError = "����Ϊ��";
		}
		else
		{
			strError.Format("���Ȳ���С��%d", dwMinLen);
		}
		return false;
	}

	if(dwTempLen > dwMaxLen)
	{
		strError.Format("���Ȳ��ܴ���%d", dwMaxLen);
		return false;
	}

	strHex = ZLib_Asc2Hexs(strInput2, dwTempLen*2);
	for(int i = 0; i < strHex.GetLength(); i+=2)
	{
		if(strHex.Mid(i,2).CompareNoCase("3f")==0 || strHex.Mid(i,2).CompareNoCase("80")==0)
		{
			strError = "���������ַ�";
			return false;
		}
		if(blEnter==false) //���ܰ����س����з�
		{
			if(strHex.Mid(i,2).CompareNoCase("0d")==0 || strHex.Mid(i,2).CompareNoCase("0a")==0)
			{
				strError = "�����س������ַ�";	
				return false;
			}
		}
	}

	return true;
}
