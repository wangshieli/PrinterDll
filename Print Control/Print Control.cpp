// Print Control.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "Print Control.h"
#include "Helper/XML/Markup.h"
#include "Function/Fpdy/Zzsfpdy.h"
#include "Function/Fpdy/Jdcfpdy.h"
#include "Function/Fpdy/Jsfpdy.h"
#include "Function/Fpdy/Escxstyfpdy.h"

#include "Function/Bbdy/Khbmdy.h"
#include "Function/Bbdy/Spbmdy.h"
#include "Function/Bbdy/Hzxxbdy.h"

#include "Helper/Log/TraceLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
			_tprintf(_T("错误: MFC 初始化失败\n"));
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        _tprintf(_T("错误: GetModuleHandle 失败\n"));
        nRetCode = 1;
    }

    return nRetCode;
}

void PostAndRecvEx(IN LPCTSTR pszPost, OUT LPSTR pszRecv)
{
	DEBUG_TRACELOG_STR("", "enter PostAndRecvEx");
	DEBUG_TRACELOG_STR("pszPost", pszPost);
	DEBUG_TRACELOG_STR("pszRecv", pszRecv);
	CMarkup xml;
	CString sFplxdm;
	CString sDylx;
	xml.SetDoc(pszPost);
	xml.FindElem("business");
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	if (xml.FindElem("dylx")) sDylx = xml.GetData();

	DEBUG_TRACELOG_STR("dylx", sDylx);
	if (sDylx.Compare("0") == 0) // 发票打印
	{
		DEBUG_TRACELOG_STR("", "enter fpdy");
		CString sFplxdm;
		if (xml.FindElem("fplxdm")) sFplxdm = xml.GetData();
		if (sFplxdm.Compare("004") == 0 || sFplxdm.Compare("007") == 0)
		{
			DEBUG_TRACELOG_STR("", "enter zzsfpdy");
			CZzsfpdy zzsfpdy;
			CString strResult = zzsfpdy.Dlfpdy(pszPost);
			strcpy(pszRecv, strResult);
			DEBUG_TRACELOG_STR("", "out zzsfpdy");
			return;
		}
		else if (sFplxdm.Compare("005") == 0)
		{
			CJdcfpdy jdcfpdy;
			jdcfpdy.Dlfpdy(pszPost);
			return;
		}
		else if (sFplxdm.Compare("025") == 0)
		{
			CJsfpdy jsfpdy;
			jsfpdy.Dlfpdy(pszPost);
			return;
		}
		else if (sFplxdm.Compare("006") == 0)
		{
			CEscxstyfp escfpdy;
			escfpdy.Dlfpdy(pszPost);
			return;
		}
		else
		{

		}
		DEBUG_TRACELOG_STR("", "out fpdy");
	}
	else if (sDylx.Compare("1") == 0) // 增值发票清单打印
	{
		CString sFplxdm;
		if (xml.FindChildElem("fplxdm")) sFplxdm = xml.GetData();
		//if(sFplxdm.Compare("004") == 0 || sFplxdm.Compare("007") == 0)
		//{
			CZzsfpdy zzsfpdy;
			zzsfpdy.Dlfpdy(pszPost);
			return;
		//}
	}
	else if (sDylx.Compare("2") == 0) //报表打印
	{
		CKhbmdy c;
		//CSpbmdy c;
		//CHzxxbdy c;
		c.Dlfpdy(pszPost);
		return;
	}
	else
	{

	}
}
