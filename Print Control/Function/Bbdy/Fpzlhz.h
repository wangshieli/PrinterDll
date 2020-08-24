#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _fpzlhz_tjxx
{
	int st_nXh;
	CString st_sMc;
	CString st_sHj;
	CString st_s1;
	CString st_s2;
	CString st_s3;
	CString st_s4;
	CString st_s5;
	CString st_sQt;

	XM xmXh;
	XM xmMc;
	XM xmHj;
	XM xmS1;
	XM xmS2;
	XM xmS3;
	XM xmS4;
	XM xmS5;
	XM xmQt;
}FPZLHZ_TJXX;

typedef list<FPZLHZ_TJXX> LTFPZLHZ_TJXX;

typedef struct _fpzlhz_bbxx
{
	_fpzlhz_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_lFpzlhzTjxx.clear();
		st_nTjxxCount = 0;
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
	CString st_sT5;
	CString st_sQt;

	XM xmXh;
	XM xmMc;
	XM xmHj;
	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmT5;
	XM xmQt;

	LTFPZLHZ_TJXX st_lFpzlhzTjxx;
	int st_nTjxxCount;
}FPZLHZ_BBXX;

class CFpzlhz :public CFpdyBase
{
public:
	CFpzlhz();
	~CFpzlhz();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString bblx);

private:
	FPZLHZ_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(FPZLHZ_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(FPZLHZ_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

