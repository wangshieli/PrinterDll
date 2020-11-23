#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;
typedef struct _fpzlqd_qdxx
{
	int st_nXh;
	CString st_sFpzl;
	CString st_sFpdm;
	CString st_sFphm;
	CString st_sKprq;
	CString st_sGfsh;
	CString st_sHjje;
	CString st_sHjse;

	XM xmXh;
	XM xmFpzl;
	XM xmFpdm;
	XM xmFphm;
	XM xmKprq;
	XM xmGfsh;
	XM xmHjje;
	XM xmHjse;
}FPZLQD_QDXX;

typedef list<FPZLQD_QDXX> LTFPZLQD_QDXX;

typedef struct _bbpage_end
{
	XM xmTbr;
	XM xmCyr;
	XM xmLrr;
	XM xmFhr;
	XM xmSjr;
}BBPAGE_END;

typedef list<BBPAGE_END> LTBBPAGE_END;

typedef struct _fpzlqd_bbxx
{
	_fpzlqd_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sZbrq = "";
		st_sName = "";
		st_sSsq = "";
		st_sNsrsbh = "";
		st_sQymc = "";
		st_sDzdh = "";
		st_sDw = "";

		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sT5 = "";
		st_sT6 = "";
		st_sT7 = "";
		st_sT8 = "";

		st_sTbr = "";
		st_sCyr = "";
		st_sLrr = "";
		st_sFhr = "";
		st_sSjr = "";

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lFpzlqdBbxx.clear();
		st_lBbpage_end.clear();
	}

	CString st_sTitle;
	CString st_sZbrq;
	CString st_sName;
	CString st_sSsq;
	CString st_sNsrsbh;
	CString st_sQymc;
	CString st_sDzdh;
	CString st_sDw;

	XM xmTitle;
	XM xmZbrq;
	XM xmName;
	XM xmSsq;
	XM xmNsrsbh;
	XM xmQymc;
	XM xmDzdh;
	XM xmDw;

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

	CString st_sTbr;
	CString st_sCyr;
	CString st_sLrr;
	CString st_sFhr;
	CString st_sSjr;

	LTBBPAGE_END st_lBbpage_end;

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

	LTFPZLQD_QDXX st_lFpzlqdBbxx;
}FPZLQD_BBXX;

class CFpzlqddy :public CFpdyBase
{
public:
	CFpzlqddy();
	~CFpzlqddy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	FPZLQD_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(FPZLQD_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(FPZLQD_BBXX bbxx);
};

