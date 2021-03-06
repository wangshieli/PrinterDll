﻿// Print Control.cpp : 定义 DLL 的导出函数。
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
#include "Function/Bbdy/Fpzlhz.h"
#include "Function/Bbdy/Fpkcdy.h"
#include "Function/Bbdy/Ykfpcxdy.h"
#include "Function/Bbdy/Khbmdy.h"
#include "Function/Bbdy/Spbmdy.h"
#include "Function/Bbdy/Smbmdy.h"
#include "Function/Bbdy/Jdcbmdy.h"
#include "Function/Bbdy/Fpzlqddy.h"
#include "Function/Bbdy/Skzltjdy.h"
#include "Function/Bbdy/Fplycdy.h"
#include "Function/Bbdy/Logdy.h"
#include "Function/Bbdy/Xhqddy.h"

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

void PostAndRecvEx(IN LPCTSTR pszPost, OUT LPSTR* pszRecv)
{
	DEBUG_TRACELOG_STR("", "enter PostAndRecvEx");
	DEBUG_TRACELOG_STR("pszPost", pszPost);
	//DEBUG_TRACELOG_STR("pszRecv", pszRecv);
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
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			DEBUG_TRACELOG_STR("", "out zzsfpdy");
			return;
		}
		else if (sFplxdm.Compare("005") == 0)
		{
			CJdcfpdy jdcfpdy;
			CString strResult = jdcfpdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (sFplxdm.Compare("025") == 0)
		{
			CJsfpdy jsfpdy;
			CString strResult = jsfpdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (sFplxdm.Compare("006") == 0)
		{
			CEscxstyfp escfpdy;
			CString strResult = escfpdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else
		{

		}
		DEBUG_TRACELOG_STR("", "out fpdy");
	}
	else if (sDylx.Compare("2") == 0) //报表打印
	{
		CString bblx;
		if (xml.FindElem("bblx")) bblx = xml.GetData();
		if (bblx.Compare("FPZLHZ") == 0)
		{
			CFpzlhz zlhz;
			CString strResult = zlhz.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("FPKCCX") == 0)
		{
			CFpkcdy kcdy;
			CString strResult = kcdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("YKFPCX") == 0)
		{
			CYkfpcxdy ykfpcxdy;
			CString strResult = ykfpcxdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("HZXXB") == 0)
		{
			CHzxxbdy hzxxbdy;
			CString strResult = hzxxbdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("KHBM") == 0)
		{
			CKhbmdy khbmdy;
			CString strResult = khbmdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("SPBM") == 0)
		{
			CSpbmdy spbmdy;
			CString strResult = spbmdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("SMBM") == 0)
		{
			CSmbmdy smbmdy;
			CString strResult = smbmdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("JDCBM") == 0)
		{
			CJdcbmdy jdcbmdy;
			CString strResult = jdcbmdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("FPZLQD") == 0)
		{
			CFpzlqddy fpzlqddy;
			CString strResult = fpzlqddy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("SKZLTJ") == 0)
		{
			CSkzltjdy skzltjdy;
			CString strResult = skzltjdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("FPLYC") == 0)
		{
			CFplycdy fplycdy;
			CString strResult = fplycdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("LOGDY") == 0)
		{
			CLogdy logdy;
			CString strResult = logdy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
		else if (bblx.Compare("XHQD") == 0)
		{
			CXhqddy xhqddy;
			CString strResult = xhqddy.Dlfpdy(pszPost);
			*pszRecv = (char*)malloc(strResult.GetLength() + 1);
			strcpy(*pszRecv, strResult);
			return;
		}
	}
	else
	{

	}
}

void FreeMem(LPSTR pData)
{
	free((void*)pData);
}
