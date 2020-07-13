/*
* �ļ�����: ZLib_Regedit.cpp
* �ļ�ժҪ: ע����д
* ��ǰ�汾: 1.0��2013-12-19
* ������Ϣ��zjg
*/

#include "../../pch.h"
#include "ZLib.h"
#include "../../Helper/XML/Markup.h"
#if PATH
extern char sYbjszFile[MAX_PATH];
extern char sPrintConfigFile[MAX_PATH];
#endif

LPBYTE CString_To_LPBYTE(CString str)
{
	LPBYTE lpb=new BYTE[str.GetLength()+1];
	for(int i=0;i<str.GetLength();i++)
		lpb[i]=str[i];
	lpb[str.GetLength()]=0;
	return lpb;
}

/*
* ��������: ZLib_SetRegValue
* ��������: дע���
* ��    ��: 
*			pcInput   - ���ִ�������
*			dwLen     - ���ݳ���
* �� �� ֵ: true  У��ɹ�
*			false У��ʧ��
*/
int ZLib_SetRegValue(IN char* regPath, IN char* regItem, IN char* regValue){
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwDisposition;
	HKEY hKey; //�����йص�hKEY, �ڳ�������Ҫ�رա�

	//��·�����õ����hKEY��KEY_WRITE��ʾ��д�ķ�ʽ�򿪡� 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, KEY_WRITE, &hKey)); 
	if(ret0!=ERROR_SUCCESS)
	{
		::RegCreateKeyEx(HKEY_LOCAL_MACHINE,regPath,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition);
	}
	
	//�޸��й�����(�û����� owner_Set)��Ҫ�Ƚ�CString��ת��ΪLPBYTE�� 
	LPBYTE bKeyValue=CString_To_LPBYTE(regValue);
	DWORD dwType=REG_SZ;
	DWORD dwLen=strlen(regValue)+1; 
	//��RegQureyValueEx()���ƣ�hKEY��ʾ�Ѵ򿪵ļ��ľ����sKeyName��ʾҪ���ʵļ�ֵ����
	// bKeyValue��ʾ�µļ�ֵ��dwType��dwLen��ʾ��ֵ���������ͺ����ݳ��� 
	long ret1=::RegSetValueEx(hKey, regItem, NULL, dwType, bKeyValue, dwLen); 
	if(ret1!=ERROR_SUCCESS) 
	{
		return -1; //"����: �޷��޸��й�ע�����Ϣ!";
	}
	return 0; //"���óɹ�";
}

//��ע�����ȡ��ӡҳ�߾�
BSTR ZLib_GetRegValue(IN char* regPath, IN char* regItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ���ӵ��ȴ���������
	CString strResult="";
	HKEY hKey; //�����йص�HKEY, �ڲ�ѯ����ʱҪ�رա�

//	CString data_Set;
//	data_Set.Format(_T("Software\\FP_Print\\%s\\"),FP_PRINT); 
//	LPCTSTR data_Set="Software\\HYZZSFP_Print\\";

	//����·�� data_Set ��ص� hKEY����һ������Ϊ�������ƣ��ڶ��������� 
	//��ʾҪ���ʵļ���λ�ã���������������Ϊ0��KEY_READ��ʾ�Բ�ѯ�ķ�ʽ
	//����ע���hKEY�򱣴�˺������򿪵ļ��ľ���� 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey)); 
	if(ret0!=ERROR_SUCCESS) //����޷���hKEY������ֹ�����ִ��
	{
		return strResult.AllocSysString();
	}
	//��ѯ�йص����� (�û����� owner_Get)��
	LPBYTE bKeyValue=new BYTE[80];
	DWORD dwType=REG_SZ ;
	DWORD dwLen=80;
	//hKeyΪ�ղ�RegOpenKeyEx()�������򿪵ļ��ľ����sKeyName��
	//��ʾҪ��ѯ�ļ�ֵ����type_1��ʾ��ѯ���ݵ����ͣ�owner_Get��������
	//��ѯ�����ݣ�cbData_1��ʾԤ���õ����ݳ��ȡ�
	long ret1=::RegQueryValueEx(hKey, regItem, NULL, &dwType, bKeyValue, &dwLen);
	if(ret1!=ERROR_SUCCESS)
	{
		strResult="";
	}
	else
	{
		strResult=(CString)bKeyValue;
	}
	delete bKeyValue;

	//�������ǰ�ر��Ѿ��򿪵�hKey
	::RegCloseKey(hKey);

	return strResult.AllocSysString();
}


int ZLib_SetYbjValue(CString strTop, CString strLeft)
{
	CMarkup xml;

	//xml.SetDoc(XMLHEAD);
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gbk\"?>\r\n");
	xml.AddElem("business");
	xml.AddAttrib("id","20003");
	xml.AddAttrib("comment","ҳ�߾�����");
	xml.IntoElem();
	xml.AddElem("body");
	xml.IntoElem();
	xml.AddElem("top", strTop);
	xml.AddElem("left", strLeft);
	xml.OutOfElem();
	xml.OutOfElem();

	CString strInput = xml.GetDoc();

	char sTempPath[MAX_PATH];
#if PATH
	strcpy(sTempPath, sYbjszFile);
#else
	//strcpy(sTempPath, CONFIG_PATH);
#endif
	strcat(sTempPath, "\\ybjsz.xml");
	FILE *pFile = NULL;
	fopen_s(&pFile, sTempPath, "w+");
	if(pFile == NULL)
	{
		return -1;
	}
	else
	{
		int rtn = fwrite(strInput.GetBuffer(0), 1, strInput.GetLength(), pFile);
		if(rtn == 0)
		{
			return -2;
		}
	}
	fclose(pFile);

	return 0;
}

void ZLib_GetYbjValue(CString& strTop, CString& strLeft)
{
	CMarkup xml;

	char sTempPath[MAX_PATH];
#if PATH
	strcpy(sTempPath, sYbjszFile);
#else
//	strcpy(sTempPath, CONFIG_PATH);
#endif
	strcat(sTempPath, "\\ybjsz.xml");

	xml.Load(sTempPath);
	xml.FindElem("business");
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	if(xml.FindElem("top")) strTop = xml.GetData();
	if(xml.FindElem("left")) strLeft = xml.GetData();
	xml.OutOfElem();
}

int ZLib_GetPrinterName(CString& strPrinterName)
{
	CMarkup xml;

	char sTempPath[MAX_PATH];
#if PATH
	strcpy(sTempPath, sPrintConfigFile);
#else
//	strcpy(sTempPath, CONFIG_PATH);
#endif
	strcat(sTempPath, "\\printer.xml");

	xml.Load(sTempPath);
	xml.FindElem("business");
	if(xml.GetAttrib("id").CompareNoCase("2016")!=0)
	{
		return -1;
	}
	
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	if(xml.FindElem("PrinterName")) 
	{
		strPrinterName = xml.GetData();
	}
	else
	{
		return -2;
	}
	xml.OutOfElem();

	return 0;
}

int ZLib_SetIniYbjValue(CString strFplxdm, CString strTop, CString strLeft, CString strQRCodeSize)
{
	char sTempPath[MAX_PATH];
#if PATH
	strcpy(sTempPath, sYbjszFile);
#else
//	strcpy(sTempPath, CONFIG_PATH);
#endif
	strcat(sTempPath, "\\ybjsz.ini");

	BOOL brtn = WritePrivateProfileString(strFplxdm.GetBuffer(0), "top", strTop.GetBuffer(0), sTempPath);
	if(brtn != TRUE)
	{
		return -1;
	}
	brtn = WritePrivateProfileString(strFplxdm.GetBuffer(0), "left", strLeft.GetBuffer(0), sTempPath);
	if(brtn != TRUE)
	{
		return -1;
	}
	if(!strQRCodeSize.IsEmpty())
	{
		brtn = WritePrivateProfileString("QRCode", "QRCodeSize", strQRCodeSize.GetBuffer(0), sTempPath);
		if(brtn != TRUE)
		{
			return -1;
		}
	}
	return 0;
}

int ZLib_GetIniYbjValue(CString strFplxdm, CString& strTop, CString& strLeft, CString& strQRCodeSize)
{
	CMarkup xml;
	CString strTempTop, strTempLeft;
	int rtn[5];

	char sTempPath[MAX_PATH], sTempIniPath[MAX_PATH];
#if PATH
	strcpy(sTempPath, sYbjszFile);
#else
//	strcpy(sTempPath, CONFIG_PATH);
#endif
	strcpy(sTempIniPath, sTempPath);
	strcat(sTempIniPath, "\\ybjsz.ini");

	char cstr1[100], cstr2[100], cstr3[100];
	rtn[0] = GetPrivateProfileString(strFplxdm.GetBuffer(0), "top",	"", cstr1, sizeof(cstr1), sTempIniPath);
	rtn[1] = GetPrivateProfileString(strFplxdm.GetBuffer(0), "left",	"", cstr2, sizeof(cstr2), sTempIniPath);
	rtn[2] = GetPrivateProfileString("QRCode", "QRCodeSize",	"", cstr3, sizeof(cstr3), sTempIniPath);

	if(rtn[0] == 0 && rtn[1] == 0)
	{
		strcat_s(sTempPath, "\\ybjsz.xml");
		xml.Load(sTempPath);
		xml.FindElem("business");
		xml.IntoElem();
		xml.FindElem("body");
		xml.IntoElem();
		if(xml.FindElem("top")) strTempTop = xml.GetData();
		if(xml.FindElem("left")) strTempLeft = xml.GetData();
		xml.OutOfElem();

		WritePrivateProfileString(strFplxdm.GetBuffer(0), "top", strTempTop.GetBuffer(0), sTempIniPath);
		WritePrivateProfileString(strFplxdm.GetBuffer(0), "left", strTempLeft.GetBuffer(0), sTempIniPath);

		strTop = strTempTop;
		strLeft = strTempLeft;

		return 0;
	}
	strTop.Format("%s", cstr1);
	strLeft.Format("%s", cstr2);
	strQRCodeSize.Format("%s", cstr3);

	return 0;
}