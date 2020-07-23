#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _khbm_bbxm
{
	int st_nXh;
	CString st_sBm;
	CString st_sDz;
	CString st_sJm;
	CString st_sKzl;
	CString st_sMc;
	CString st_sNsrsbh;
	CString st_sYhzh;
	CString st_sYjdz;

	XM xmXh;
	XM xmBm;
	XM xmDz;
	XM xmJm;
	XM xmKzl;
	XM xmMc;
	XM xmNsrsbh;
	XM xmYhzh;
	XM xmYjdz;
}KHBM_BBXM;

typedef list<KHBM_BBXM> LTKHBM_BBXM;

typedef struct _khbm_bbxx
{
	int st_nBbxmCount;
	LTKHBM_BBXM st_lKhbmBbxm;

	void clear()
	{
		st_nBbxmCount = 0;
		st_lKhbmBbxm.clear();
	}
}KHBM_BBXX;

class CKhbmdy :public CFpdyBase
{
public:
	CKhbmdy();
	~CKhbmdy();

public:
	CString Dlfpdy(LPTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString strFplxdm);

private:
	KHBM_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(KHBM_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(KHBM_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

//	int m_nOrientation;
	int m_nPageSize;
};

