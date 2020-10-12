#pragma once
#include "../Fpdy/Fpdy.h"
#include <list>

using namespace std;

typedef struct _fpkccx_fpxx
{
	int st_nXh;
	CString st_sFpdm;
	CString st_sFpfs;
	CString st_sFplx;
	CString st_sFpqshm;
	CString st_sFpzzhm;
	CString st_sKpxe;
	CString st_sLgrq;
	CString st_sSyfs;

	XM xmXh;
	XM xmFpdm;
	XM xmFpfs;
	XM xmFplx;
	XM xmFpqshm;
	XM xmFpzzhm;
	XM xmKpxe;
	XM xmLgrq;
	XM xmSyfs;
}FPKCCX_FPXX;

typedef list<FPKCCX_FPXX> LTFPKCCX_FPXX;

typedef struct _fpkccx_bbxx
{
public:
	_fpkccx_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sTitle = "";
		st_sSkpbh = "";
		st_lFpkccx_fpxx.clear();
	}

	CString st_sTitle;
	CString st_sSkpbh;

	XM xmTitle;
	XM xmSkpbh;

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

	LTFPKCCX_FPXX st_lFpkccx_fpxx;
}FPKCCX_BBXX;

class CFpkcdy :public CFpdyBase
{
public:
	CFpkcdy();
	~CFpkcdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml, CString bblx);

private:
	FPKCCX_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(FPKCCX_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(FPKCCX_BBXX bbxx);
};

