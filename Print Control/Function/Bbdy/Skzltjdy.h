#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _skzltj_tjxx
{
	int st_nXh;
	CString st_sMc;
	CString st_sHj;
	CString st_s1;
	CString st_s2;
	CString st_s3;
	CString st_s4;
	CString st_sQt;

	XM xmXh;
	XM xmMc;
	XM xmHj;
	XM xmS1;
	XM xmS2;
	XM xmS3;
	XM xmS4;
	XM xmQt;
}SKZLTJ_TJXX;

typedef list<SKZLTJ_TJXX> LTSKZLTJ_TJXX;

typedef struct _skzltj_bbxx
{
	_skzltj_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sZbrq = "";
		st_sSsrq = "";
		st_sSm = "";
		st_sNsrsbh = "";
		st_sQymc = "";
		st_sDzdh = "";

		st_sTitle1 = "";
		st_sCqkc = "";
		st_sZsfp = "";
		st_sFsfp = "";
		st_sGjfp = "";
		st_sZffp = "";
		st_sFffp = "";
		st_sThfp = "";
		st_sQmkc = "";

		st_sTitle2 = "";
		st_sJedw = "";

		st_sXh = "";
		st_sMc = "";
		st_sHj = "";
		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sQt = "";

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lSkzltjTjxx.clear();
	}

	CString st_sTitle;
	CString st_sZbrq;
	CString st_sSsrq;
	CString st_sSm;
	CString st_sNsrsbh;
	CString st_sQymc;
	CString st_sDzdh;

	XM xmTitle;
	XM xmZbrq;
	XM xmSsrq;
	XM xmSm;
	XM xmNsrsbh;
	XM xmQymc;
	XM xmDzdh;

	CString st_sTitle1;
	CString st_sCqkc;
	CString st_sZsfp;
	CString st_sFsfp;
	CString st_sGjfp;
	CString st_sZffp;
	CString st_sFffp;
	CString st_sThfp;
	CString st_sQmkc;

	XM xmTitle1;
	XM xmCqkc;
	XM xmZsfp;
	XM xmFsfp;
	XM xmGjfp;
	XM xmZffp;
	XM xmFffp;
	XM xmThfp;
	XM xmQmkc;

	CString st_sTitle2;
	CString st_sJedw;

	XM xmTitle2;
	XM xmJedw;

	CString st_sXh;
	CString st_sMc;
	CString st_sHj;
	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;
	CString st_sQt;

	XM xmXh;
	XM xmMc;
	XM xmHj;
	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmQt;

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

	XM xm1;
	XM xm2;

	LTSKZLTJ_TJXX st_lSkzltjTjxx;
}SKZLTJ_BBXX;

class CSkzltjdy :public CFpdyBase
{
public:
	CSkzltjdy();
	~CSkzltjdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	SKZLTJ_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(SKZLTJ_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(SKZLTJ_BBXX bbxx);
};

