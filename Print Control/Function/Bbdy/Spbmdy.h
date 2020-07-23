#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _spbm_bbxm
{
	int st_nXh;
	CString st_sDj;
	CString st_sGgxh;
	CString st_sHsbz;
	CString st_sJldw;
	CString st_sJm;
	CString st_sMc;
	CString st_sSl;
	CString st_sSpflbm;

	XM xmXh;
	XM xmDj;
	XM xmGgxh;
	XM xmHsbz;
	XM xmJldw;
	XM xmJm;
	XM xmMc;
	XM xmSl;
	XM xmSpflbm;
}SPBM_BBXM;

typedef list<SPBM_BBXM> LTSPBM_BBXM;

typedef struct _spbm_bbxx
{
	int st_nBbxmCount;
	LTSPBM_BBXM st_lSpbmBbxm;

	void clear()
	{
		st_nBbxmCount = 0;
		st_lSpbmBbxm.clear();
	}
}SPBM_BBXX;

class CSpbmdy :public CFpdyBase
{
public:
	CSpbmdy();
	~CSpbmdy();

public:
	CString Dlfpdy(LPTSTR sInputInfo);

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

