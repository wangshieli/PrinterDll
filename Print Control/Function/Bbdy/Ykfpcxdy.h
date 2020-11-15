#pragma once
#include "../Fpdy/Fpdy.h"
#include <list>
#include <vector>

using namespace std;

typedef struct _ykpfcx_item
{
	_ykpfcx_item()
	{
		clear();
	}

	void clear()
	{
		st_sName = "";
		st_sKey = "";
		st_nWide = 0;
	}
	CString st_sName;
	XM st_xmName;
	CString st_sKey;
	int st_nWide;
}YKFPCX_ITEM;

typedef std::list<YKFPCX_ITEM> LTYKFPCX_ITEM;

typedef struct _ykfpcx_item_data_xx
{
	_ykfpcx_item_data_xx()
	{
		clear();
	}
	void clear()
	{
		item_data = "";
		xmItem_data.h = YKFP_LINE_H_MAX;// 最大高度210

	}
	CString item_data;
	XM xmItem_data;
}YKFPCX_ITEM_DATA_XX;

typedef struct _ykfpcx_bbxx
{
	_ykfpcx_bbxx()
	{
		clear();
	}

	void clear()
	{
		st_sT1 = "";
		st_sT2 = "";
		st_sT3 = "";
		st_sT4 = "";
		st_sT5 = "";
		st_sT6 = "";
		st_sT7 = "";
		st_sT8 = "";
		st_sT9 = "";
		st_sT10 = "";
		st_sT11 = "";
		st_sT12 = "";
		st_sT13 = "";
		st_sT14 = "";
		st_sT15 = "";
		st_sT16 = "";
		st_sT17 = "";

		st_sTitle = "";
		st_sSm = "";

		st_nTc = 0;

		st_lItem.clear();

		st_vData.clear();

		st_sDi = "第";
		st_sYe1 = "页";
		st_sGong = "共";
		st_sYe2 = "页";
	}

	CString st_sT1;
	CString st_sT2;
	CString st_sT3;
	CString st_sT4;
	CString st_sT5;
	CString st_sT6;
	CString st_sT7;
	CString st_sT8;
	CString st_sT9;
	CString st_sT10;
	CString st_sT11;
	CString st_sT12;
	CString st_sT13;
	CString st_sT14;
	CString st_sT15;
	CString st_sT16;
	CString st_sT17;

	XM xmT1;
	XM xmT2;
	XM xmT3;
	XM xmT4;
	XM xmT5;
	XM xmT6;
	XM xmT7;
	XM xmT8;
	XM xmT9;
	XM xmT10;
	XM xmT11;
	XM xmT12;
	XM xmT13;
	XM xmT14;
	XM xmT15;
	XM xmT16;
	XM xmT17;

	CString st_sTitle;
	XM xmTitle;

	CString st_sSm;
	XM xmSm;

	int st_nTc;

	LTYKFPCX_ITEM st_lItem;

	vector<vector<YKFPCX_ITEM_DATA_XX>> st_vData;

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
}YKFPCX_BBXX;

class CYkfpcxdy :public CFpdyBase
{
public:
	CYkfpcxdy();
	~CYkfpcxdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

public:
	LONG PrintQD(LPCSTR billxml);

private:
	YKFPCX_BBXX ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy);
	CString GenerateFpdyXml(YKFPCX_BBXX bbxx, CString dylx, BBDY bbdy);
	CString GenerateItemMXXml(YKFPCX_BBXX bbxx);

	int GetWideByItemName(CString& itemName);// 根据发票类型，和具体表头名称设置宽度
	void GetFlagsByName(CString& name, XM& _xm);

	bool In(wchar_t start, wchar_t end, wchar_t code);
	char convert(wchar_t n);
	CString getChineseSpell(CString src);

private:
	int m_nLineNum;
	int m_nAllPageNum;
	int m_nPageSize;
};

