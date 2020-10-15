#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _fplyc_fpxx
{
	_fplyc_fpxx()
	{
		clear();
	}

	void clear()
	{
		st_sFpzl = "";
		st_sFplb = "";
		st_sQckcfs = "";
		st_sQckcfphm = "";
		st_sBqlgfs = "";
		st_sBqlgfphm = "";
		st_sBqkjfs = "";
		st_sBqkjfphm = "";
		st_sBqzffs = "";
		st_sBqzffphm = "";
		st_sWsthfs = "";
		st_sWsthfphm = "";
		st_sQmkcfs = "";
		st_sQmkcfphm = "";
	}

	CString st_sFpzl;
	CString st_sFplb;
	CString st_sQckcfs;
	CString st_sQckcfphm;
	CString st_sBqlgfs;
	CString st_sBqlgfphm;
	CString st_sBqkjfs;
	CString st_sBqkjfphm;
	CString st_sBqzffs;
	CString st_sBqzffphm;
	CString st_sWsthfs;
	CString st_sWsthfphm;
	CString st_sQmkcfs;
	CString st_sQmkcfphm;

	XM xmFpzl;
	XM xmQckcfs;
	XM xmQckcfphm;
	XM xmBqlgfs;
	XM xmBqlgfphm;
	XM xmBqkjfs;
	XM xmBqkjfphm;
	XM xmBqzffs;
	XM xmBqzffphm;
	XM xmWsthfs;
	XM xmWsthfphm;
	XM xmQmkcfs;
	XM xmQmkcfphm;
}FPLYC_FPXX;

typedef list<FPLYC_FPXX> LTFPLYC_FPXX;

typedef struct _fplyc_bbxx
{
	_fplyc_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sZbrq = "";
		st_sSsq = "";
		st_sQymc = "";
		st_sDw = "";

		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sT5 = "";
		st_sT6 = "";
		st_sT7 = "";

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";
		st_lFplycFpxx.clear();

		st_sZ1 = "";
		st_sZ2 = "";
		st_sZ3 = "";
	}

	CString st_sTitle;
	CString st_sSsq;
	CString st_sZbrq;
	CString st_sDw;
	CString st_sQymc;
	

	XM xmTitle;
	XM xmSsq;
	XM xmZbrq;
	XM xmDw;
	XM xmQymc;
	
	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;
	CString st_sT5;
	CString st_sT6;
	CString st_sT7;

	XM xmT1;

	XM xmT2;
	XM xmT2_FS;
	XM xmT2_HM;
	XM xmT2_FS_1;
	XM xmT2_HM_1;

	XM xmT3;
	XM xmT3_FS;
	XM xmT3_HM;
	XM xmT3_FS_1;
	XM xmT3_HM_1;

	XM xmT4;
	XM xmT4_FS;
	XM xmT4_HM;
	XM xmT4_FS_1;
	XM xmT4_HM_1;

	XM xmT5;
	XM xmT5_FS;
	XM xmT5_HM;
	XM xmT5_FS_1;
	XM xmT5_HM_1;

	XM xmT6;
	XM xmT6_FS;
	XM xmT6_HM;
	XM xmT6_FS_1;
	XM xmT6_HM_1;

	XM xmT7;
	XM xmT7_FS;
	XM xmT7_HM;
	XM xmT7_FS_1;
	XM xmT7_HM_1;

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

	LTFPLYC_FPXX st_lFplycFpxx;

	CString st_sZ1;
	CString st_sZ2;
	CString st_sZ3;
}FPLYC_BBXX;

class CFplycdy :public CFpdyBase
{
public:
	CFplycdy();
	~CFplycdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString strFplxdm);

private:
	FPLYC_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(FPLYC_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(FPLYC_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

