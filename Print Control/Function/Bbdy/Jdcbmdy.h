#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _jdcbm_bmxx
{
	int st_nXh;
	CString st_sClmc;
	CString st_sCpxh;
	CString st_sCd;
	CString st_sDj;
	CString st_sSccj;

	XM xmXh;
	XM xmClmc;
	XM xmCpxh;
	XM xmCd;
	XM xmDj;
	XM xmSccj;
}JDCBM_BMXX;

typedef list<JDCBM_BMXX> LTJDCBM_BMXX;

typedef struct _jdcbm_bbxx
{
	_jdcbm_bbxx()
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

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lJdcbmBmxx.clear();
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

	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmT5;
	XM xmT6;

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

	LTJDCBM_BMXX st_lJdcbmBmxx;
}JDCBM_BBXX;

class CJdcbmdy :public CFpdyBase
{
public:
	CJdcbmdy();
	~CJdcbmdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	JDCBM_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(JDCBM_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(JDCBM_BBXX bbxx);
};

