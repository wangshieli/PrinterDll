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

	int st_nBbxmCount;
	LTHZXXB_BBXM st_lHzxxbBbxm;

	void clear()
	{
		st_nBbxmCount = 0;
		st_lHzxxbBbxm.clear();
	}
}HZXXB_BBXX;

class CHzxxbdy :public CFpdyBase
{
public:
	CHzxxbdy();
	~CHzxxbdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString strFplxdm);

private:
	HZXXB_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(HZXXB_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(HZXXB_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;

	CString m_sGfmc;
	CString m_sGfsh;
	CString m_sXfmc;
	CString m_sXfsh;
	CString m_sFpdm;
	CString m_sFphm;
	CString m_sHjje;
	CString m_sHjse;
	CString m_sSqf;// 0购方  1销方
	CString m_sSqyy; // 0已抵扣  1未抵扣
	CString m_sSqdbh;
};

