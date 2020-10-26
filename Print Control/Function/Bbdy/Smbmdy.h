#pragma once

#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _smbm_bmxx
{
	int st_nXh;
	CString st_sBm;
	CString st_sMc;
	CString st_sSlv;

	XM xmXh;
	XM xmBm;
	XM xmMc;
	XM xmSlv;
}SMBM_BMXX;

typedef list<SMBM_BMXX> LTSMBM_BMXX;

typedef struct _smbm_bbxx
{
	_smbm_bbxx()
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

		st_sDi = "µÚ";
		st_sYe1 = "Ò³";
		st_sGong = "¹²";
		st_sYe2 = "Ò³";

		st_lSmbmBmxx.clear();
	}

	CString st_sTitle;
	CString st_sSm;

	XM xmTitle;
	XM xmSm;

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

	LTSMBM_BMXX st_lSmbmBmxx;
}SMBM_BBXX;

class CSmbmdy :public CFpdyBase
{
public:
	CSmbmdy();
	~CSmbmdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	SMBM_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(SMBM_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(SMBM_BBXX bbxx);

private:
	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;
};

