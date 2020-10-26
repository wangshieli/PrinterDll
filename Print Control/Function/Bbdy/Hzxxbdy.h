#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _hzxxb_bbxm
{
	int st_nXh;
	CString st_sMc;
	CString st_sSl;
	CString st_sDj;
	CString st_sJe;
	CString st_sSlv;
	CString st_sSe;

	XM xmXh;
	XM xmMc;
	XM xmSl;
	XM xmDj;
	XM xmJe;
	XM xmSlv;
	XM xmSe;
}HZXXB_BBXM;

typedef list<HZXXB_BBXM> LTHZXXB_BBXM;

typedef struct _hzxxb_bbxx
{
	_hzxxb_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sSm = "";

		st_sGfmc = "";
		st_sGfsh = "";
		st_sXfmc = "";
		st_sXfsh = "";
		st_sFpdm = "";
		st_sFphm = "";
		st_sHjje = "";
		st_sHjse = "";
		st_sSqf = "";
		st_sSqyy = "";
		st_sXxbbh = "";

		st_nBbxmCount = 0;
		st_lHzxxbBbxm.clear();
	}

	CString st_sTitle;
	CString st_sSm;

	XM xmTitle;
	XM xmSm;

	CString st_sGfmc;
	CString st_sGfsh;
	CString st_sXfmc;
	CString st_sXfsh;
	CString st_sFpdm;
	CString st_sFphm;
	CString st_sHjje;
	CString st_sHjse;
	CString st_sSqf;// 0购方  1销方
	CString st_sSqyy; // 0已抵扣  1未抵扣
	CString st_sXxbbh;

	XM xmGfmc;
	XM xmGfsh;
	XM xmXfmc;
	XM xmXfsh;
	XM xmFpdm;
	XM xmFphm;
	XM xmHjje;
	XM xmHjse;
	XM xmXxbbh;

	int st_nBbxmCount;
	LTHZXXB_BBXM st_lHzxxbBbxm;
}HZXXB_BBXX;

class CHzxxbdy :public CFpdyBase
{
public:
	CHzxxbdy();
	~CHzxxbdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	HZXXB_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(HZXXB_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(HZXXB_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

