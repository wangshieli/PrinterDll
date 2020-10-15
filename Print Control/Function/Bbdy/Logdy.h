#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _log_xmxx
{
	CString st_sRybm;
	CString st_sXm;
	CString st_sCzsj;
	CString st_sCzmk;

	XM xmRybm;
	XM xmXm;
	XM xmCzsj;
	XM xmCzmk;
}LOG_XMXX;

typedef list<LOG_XMXX> LTLOG_XMXX;

typedef struct _log_bbxx
{
	_log_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sZbr = "";

		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lLogdBbxx.clear();
	}

	CString st_sTitle;
	CString st_sZbr;

	XM xmTitle;
	XM xmZbr;

	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;

	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;

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

	LTLOG_XMXX st_lLogdBbxx;
}LOG_BBXX;

class CLogdy :public CFpdyBase
{
public:
	CLogdy();
	~CLogdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString strFplxdm);

private:
	LOG_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(LOG_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(LOG_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

