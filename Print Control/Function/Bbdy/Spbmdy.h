#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _spbm_bmxx
{
	CString st_sMc;
	CString st_sSsbm;
	CString st_sJm;
	CString st_sSlv;
	CString st_sGgxh;
	CString st_sDw;
	CString st_sDj;
	CString st_sHsbz;
	
	XM xmMc;
	XM xmSsbm;
	XM xmJm;
	XM xmSlv;
	XM xmGgxh;	
	XM xmDw;
	XM xmDj;
	XM xmHsbz;
}SPBM_BMXX;

typedef list<SPBM_BMXX> LTSPBM_BMXX;

typedef struct _spbm_bbxx
{
	_spbm_bbxx()
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

		st_lSpbmBmxx.clear();
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

	LTSPBM_BMXX st_lSpbmBmxx;
}SPBM_BBXX;

class CSpbmdy :public CFpdyBase
{
public:
	CSpbmdy();
	~CSpbmdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString strFplxdm);

private:
	SPBM_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(SPBM_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(SPBM_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

