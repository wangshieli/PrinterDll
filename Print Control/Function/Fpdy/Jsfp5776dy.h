#pragma once

#include "Fpdy.h"

#include <list>

using namespace std;

typedef struct _jsfp_fyxm_5776
{
	_jsfp_fyxm_5776()
	{
		clear();
	}

	void clear()
	{
		st_sSpmc = "";
		st_sSpsl = "";
		st_sHsdj = "";
		st_sHsje = "";
	}

	CString st_sSpmc;
	CString st_sSpsl;
	CString st_sHsdj;
	CString st_sHsje;

	XM xmSpmc;
	XM xmSpsl;
	XM xmHsdj;
	XM xmHsje;
}JSFP_FYXM_5776;

typedef list<JSFP_FYXM_5776> LTJSFP_FYXM_5776;

typedef struct _jsfp_fpxx_5776
{
	_jsfp_fpxx_5776()
	{
		clear();
	}

	void clear()
	{
		st_sFpdm = "";
		st_sFphm = "";
		st_sFpzt = "";
		st_sKprq = "";
		st_sJqbh = "";
		st_sSkm = "";
		st_sXhdwsbh = "";
		st_sXhdwmc = "";
		st_sGhdwsbh = "";
		st_sGhdwmc = "";
		st_sFppy = "";
		st_sJshj = "";
		st_sBz = "";
		st_sSkr = "";
		st_sYfpdm = "";
		st_sYfphm = "";
		st_sJshjDx = "";

		st_lJsfp_fyxm.clear();
	}

	CString st_sFpdm;
	CString st_sFphm;
	CString st_sFpzt;
	CString st_sKprq;
	CString st_sJqbh;
	CString st_sSkm;
	CString st_sXhdwsbh;
	CString st_sXhdwmc;
	CString st_sGhdwsbh;
	CString st_sGhdwmc;
	CString st_sFppy;

	XM xmFphm;
	XM xmKprq;
	XM xmJqbh;
	XM xmSkm;
	XM xmXhdwsbh;
	XM xmXhdwmc;
	XM xmGhdwsbh;
	XM xmGhdwmc;

	LTJSFP_FYXM_5776 st_lJsfp_fyxm;

	CString st_sJshj;
	CString st_sBz;
	CString st_sSkr;
	CString st_sYfpdm;
	CString st_sYfphm;
	CString st_sJshjDx;

	XM xmJshj;
	XM xmBz;
	XM xmSkr;
	XM xmYfpdm;
	XM xmYfphm;
	XM xmJshjDx;
}JSFP_FPXX_5776;

class CJsfp5776dy :public CFpdyBase
{
public:
	CJsfp5776dy();
	virtual ~CJsfp5776dy();

public:
	CString Dlfpdy_5776(LPCTSTR sInputInfo);
	LONG Print_5776(JSFP_FPXX_5776 fpmx);

private:
	JSFP_FPXX_5776 ParseFpmxFromXML_5776(LPCTSTR inXml, FPDY fpdy);
	CString GenerateFpdyXml_5776(JSFP_FPXX_5776 fpmx);
	CString GenerateItemXmlG_57(JSFP_FPXX_5776 fpmx);	// 总局新票种窄长
	CString GenerateItemXmlF_76(JSFP_FPXX_5776 fpmx, FPDY fpdy);	// 总局新票种宽长

private:
	CString m_strFppy;
};

