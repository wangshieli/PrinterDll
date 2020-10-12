#pragma once
#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _ykfpcx_fpxx
{
	_ykfpcx_fpxx()
	{
		clear();
	}

	void clear()
	{
		st_sFplx = "";
		st_sFpzt = "";
		st_sFpdm = "";
		st_sFphm = "";
		st_sSczt = "";
		st_sKhmc = "";
		st_sZyspmc = "";
		st_sSe = "";
		st_sHjje = "";
		st_sJshj = "";
		st_sYfpdm = "";
		st_sYfphm = "";
		st_sTzdbh = "";
		st_sKpr = "";
		st_sKprq = "";
		st_sZfr = "";
		st_sZfrq = "";
	}

	CString st_sFplx;
	CString st_sFpzt;
	CString st_sFpdm;
	CString st_sFphm;
	CString st_sSczt;
	CString st_sKhmc;
	CString st_sZyspmc;
	CString st_sSe;
	CString st_sHjje;
	CString st_sJshj;
	CString st_sYfpdm;
	CString st_sYfphm;
	CString st_sTzdbh;
	CString st_sKpr;
	CString st_sKprq;
	CString st_sZfr;
	CString st_sZfrq;

	XM xmFplx;
	XM xmFpzt;
	XM xmFpdm;
	XM xmFphm;
	XM xmSczt;
	XM xmKhmc;
	XM xmZyspmc;
	XM xmSe;
	XM xmHjje;
	XM xmJshj;
	XM xmYfpdm;
	XM xmYfphm;
	XM xmTzdbh;
	XM xmKpr;
	XM xmKprq;
	XM xmZfr;
	XM xmZfrq;
}YKFPCX_FPXX;

typedef std::list<YKFPCX_FPXX> LTYKFPCX_FPXX;

typedef struct _ykfpcx_bbxx
{
	_ykfpcx_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sT5 = "";
		st_sT6 = "";
		st_sT7 = "";
		st_sT8 = "";
		st_sT9 = "";
		st_sT10 = "";
		st_sT11 = "";
		st_sT12 = "";
		st_sT13 = "";
		st_sT14 = "";
		st_sT15 = "";
		st_sT16 = "";
		st_sT17 = "";

		st_sTitle = "";
		st_sSm = "";

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lYkfpcx_fpxx.clear();
	}

	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;
	CString st_sT5;
	CString st_sT6;
	CString st_sT7;
	CString st_sT8;
	CString st_sT9;
	CString st_sT10;
	CString st_sT11;
	CString st_sT12;
	CString st_sT13;
	CString st_sT14;
	CString st_sT15;
	CString st_sT16;
	CString st_sT17;

	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmT5;
	XM xmT6;
	XM xmT7;
	XM xmT8;
	XM xmT9;
	XM xmT10;
	XM xmT11;
	XM xmT12;
	XM xmT13;
	XM xmT14;
	XM xmT15;
	XM xmT16;
	XM xmT17;

	CString st_sTitle;
	XM xmTitle;

	CString st_sSm;
	XM xmSm;

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

	LTYKFPCX_FPXX st_lYkfpcx_fpxx;
}YKFPCX_BBXX;

class CYkfpcxdy :public CFpdyBase
{
public:
	CYkfpcxdy();
	~CYkfpcxdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString bblx);

private:
	YKFPCX_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(YKFPCX_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(YKFPCX_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;
	int m_nPageSize;
};

