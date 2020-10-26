#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _khbm_bmxx
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
}KHBM_BMXX;

typedef list<KHBM_BMXX> LTKHBM_BMXX;

typedef struct _khbm_bbxx
{
	_khbm_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sSm = "";

		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sT5 = "";
		st_sT6 = "";
		st_sT7 = "";
		st_sT8 = "";

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lKhbmBmxx.clear();
	}

	CString st_sTitle;
	CString st_sSm;

	XM xmTitle;
	XM xmSm;

	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;
	CString st_sT5;
	CString st_sT6;
	CString st_sT7;
	CString st_sT8;

	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmT5;
	XM xmT6;
	XM xmT7;
	XM xmT8;

	CString st_sDi;
	CString st_sYe1;
	CString st_sGong;
	CString st_sYe2;
	XM xmDi;
	XM xmYe1;
	XM xmGong;
	XM xmYe2;

	XM xmP1;
	XM xmP2;

	LTKHBM_BMXX st_lKhbmBmxx;
}KHBM_BBXX;

class CKhbmdy :public CFpdyBase
{
public:
	CKhbmdy();
	~CKhbmdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

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

