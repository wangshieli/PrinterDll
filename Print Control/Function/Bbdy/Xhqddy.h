#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _xhqd_qdxx
{
	_xhqd_qdxx()
	{
		clear();
	}

	void clear()
	{
		st_nXh = 0;
		st_sFphxz = "";
		st_sSpmc = "";
		st_sGgxh = "";
		st_sDw = "";
		st_sSpsl = "";
		st_sDj = "";
		st_sJe = "";
		st_sSlv = "";
		st_sSe = "";
		st_sHsbz = "";
		st_sLslbs = "";
	}

	int st_nXh;
	CString st_sFphxz;
	CString st_sSpmc;
	CString st_sGgxh;
	CString st_sDw;
	CString st_sSpsl;
	CString st_sDj;
	CString st_sJe;
	CString st_sSlv;
	CString st_sSe;
	CString st_sHsbz;
	CString st_sLslbs;

	XM xmXh;
	XM xmSpmc;
	XM xmGgxh;
	XM xmDw;
	XM xmSpsl;
	XM xmDj;
	XM xmJe;
	XM xmSlv;
	XM xmSe;
}XHQD_QDXX;

typedef std::list<XHQD_QDXX> LTXHQD_QDXX;

typedef struct _xhqd_bbxx
{
	_xhqd_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sGhdwmc = "";
		st_sXhdwmc = "";
		st_sFplxdm = "";
		st_sFpdm = "";
		st_sFphm = "";

		st_sBz = "";

		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sT5 = "";
		st_sT6 = "";
		st_sT7 = "";
		st_sT8 = "";
		st_sT9 = "";

		st_lXhqd_qdxx.clear();

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_sZ1 = "";
		st_sZ2 = "";
		st_sZ3 = "";
	}

	CString st_sTitle;
	CString st_sGhdwmc;
	CString st_sXhdwmc;
	CString st_sFpdm;
	CString st_sFphm;

	XM xmTitle;
	XM xmGhdwmc;
	XM xmXhdwmc;
	XM xmFpdm;
	XM xmFphm;

	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;
	CString st_sT5;
	CString st_sT6;
	CString st_sT7;
	CString st_sT8;
	CString st_sT9;

	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmT5;
	XM xmT6;
	XM xmT7;
	XM xmT8;
	XM xmT9;

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

	CString st_sFplxdm;

	CString st_sBz;

	LTXHQD_QDXX st_lXhqd_qdxx;

	CString st_sZ1;
	CString st_sZ2;
	CString st_sZ3;
}XHQD_BBXX;

class CXhqddy :public CFpdyBase
{
public:
	CXhqddy();
	~CXhqddy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	XHQD_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(XHQD_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(XHQD_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

