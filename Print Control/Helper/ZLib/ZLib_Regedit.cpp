/*
* 文件名称: ZLib_Regedit.cpp
* 文件摘要: 注册表读写
* 当前版本: 1.0，2013-12-19
* 作者信息：zjg
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
* 函数名称: ZLib_SetRegValue
* 函数功能: 写注册表
* 参    数: 
*			pcInput   - 数字串缓冲区
*			dwLen     - 数据长度
* 返 回 值: true  校验成功
*			false 校验失败
*/
int ZLib_SetRegValue(IN char* regPath, IN char* regItem, IN char* regValue){
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwDisposition;
	HKEY hKey; //定义有关的hKEY, 在程序的最后要关闭。

	//打开路径，得到句柄hKEY，KEY_WRITE表示以写的方式打开。 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, KEY_WRITE, &hKey)); 
	if(ret0!=ERROR_SUCCESS)
	{
		::RegCreateKeyEx(HKEY_LOCAL_MACHINE,regPath,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition);
	}
	
	//修改有关数据(用户姓名 owner_Set)，要先将CString型转换为LPBYTE。 
	LPBYTE bKeyValue=CString_To_LPBYTE(regValue);
	DWORD dwType=REG_SZ;
	DWORD dwLen=strlen(regValue)+1; 
	//与RegQureyValueEx()类似，hKEY表示已打开的键的句柄，sKeyName表示要访问的键值名，
	// bKeyValue表示新的键值，dwType和dwLen表示新值的数据类型和数据长度 
	long ret1=::RegSetValueEx(hKey, regItem, NULL, dwType, bKeyValue, dwLen); 
	if(ret1!=ERROR_SUCCESS) 
	{
		return -1; //"错误: 无法修改有关注册表信息!";
	}
	return 0; //"设置成功";
}

//从注册表中取打印页边距
BSTR ZLib_GetRegValue(IN char* regPath, IN char* regItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CString strResult="";
	HKEY hKey; //定义有关的HKEY, 在查询结束时要关闭。

//	CString data_Set;
//	data_Set.Format(_T("Software\\FP_Print\\%s\\"),FP_PRINT); 
//	LPCTSTR data_Set="Software\\HYZZSFP_Print\\";

	//打开与路径 data_Set 相关的 hKEY，第一个参数为根键名称，第二个参数表。 
	//表示要访问的键的位置，第三个参数必须为0，KEY_READ表示以查询的方式
	//访问注册表，hKEY则保存此函数所打开的键的句柄。 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey)); 
	if(ret0!=ERROR_SUCCESS) //如果无法打开hKEY，则终止程序的执行
	{
		return strResult.AllocSysString();
	}
	//查询有关的数据 (用户姓名 owner_Get)。
	LPBYTE bKeyValue=new BYTE[80];
	DWORD dwType=REG_SZ ;
	DWORD dwLen=80;
	//hKey为刚才RegOpenKeyEx()函数所打开的键的句柄，sKeyName。
	//表示要查询的键值名，type_1表示查询数据的类型，owner_Get保存所。
	//查询的数据，cbData_1表示预设置的数据长度。
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

	//程序结束前关闭已经打开的hKey
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
	xml.AddAttrib("comment","页边距设置");
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