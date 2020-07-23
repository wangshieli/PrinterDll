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

void PostAndRecvEx(IN LPTSTR pszPost, OUT LPTSTR pszRecv)
{
	CMarkup xml;
	CString sFplxdm;
	xml.SetDoc(pszPost);
	xml.FindElem("business");
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	if (xml.FindElem("fplxdm")) sFplxdm = xml.GetData();
	if (sFplxdm.Compare("004") == 0 || sFplxdm.Compare("007") == 0)
	{
		CZzsfpdy zzsfpdy;
		zzsfpdy.Dlfpdy(pszPost);
		//DEBUG_TRACELOG_STR("输出XML", strResult);
		return;
	}
	else if (sFplxdm.Compare("005") == 0)
	{
		CJdcfpdy jdcfpdy;
		jdcfpdy.Dlfpdy(pszPost);
		//DEBUG_TRACELOG_STR("输出XML", strResult);
		//strcpy(pszRecv, strResult);
		return;
	}
	else if (sFplxdm.Compare("025") == 0)
	{
		CJsfpdy jsfpdy;
		jsfpdy.Dlfpdy(pszPost);
		//DEBUG_TRACELOG_STR("输出XML", strResult);
		//strcpy(pszRecv, strResult);
		return;
	}
	else if (sFplxdm.Compare("006") == 0)
	{
		CEscxstyfp escfpdy;
		escfpdy.Dlfpdy(pszPost);
		//strcpy(pszRecv, strResult);
		return;
	}
	else
	{
		//CKhbmdy c;
		CSpbmdy c;
		c.Dlfpdy(pszPost);
		return;
	}
}
