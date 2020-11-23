#include "../../pch.h"
#include "Ykfpcxdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

CYkfpcxdy::CYkfpcxdy()
{
}

CYkfpcxdy::~CYkfpcxdy()
{
}

CString CYkfpcxdy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	YKFPCX_BBXX bbxx;
	CString printXml("");
	m_sPrinterName.Empty();

	int rtn = 0;
	CString sCode = "", sMsg = "";

	xml.SetDoc(sInputInfo);
	if (xml.FindElem("business"))
	{
		//得到操作码
		CString id = xml.GetAttrib("id");
		if (0 == id.Mid(0, 1).Compare("3"))
			m_bStatus = TRUE;
		else
			m_bStatus = FALSE;
	}
	xml.IntoElem();
	xml.FindElem("body");
	if (xml.GetAttrib("yylxdm").CompareNoCase("1") != 0 && xml.GetAttrib("yylxdm").CompareNoCase("2") != 0)
	{
		bbdy.sErrorCode.Format("%d", -3);
		bbdy.sErrorInfo = "输入XML格式中yylxdm不正确";
		return GenerateXMLFpdy(bbdy);
	}
	bbdy.iYylxdm = atoi(xml.GetAttrib("yylxdm"));
	xml.IntoElem();
	if (xml.FindElem("returncode"))		sCode = xml.GetData();
	if (xml.FindElem("returnmsg"))		sMsg = xml.GetData();
	if (sCode.Compare("0") != 0)
	{
		bbdy.sErrorCode = sCode;
		bbdy.sErrorInfo = sMsg;
		return GenerateXMLFpdy(bbdy);
	}

	if (xml.FindElem("dylx"))   bbdy.sDylx = xml.GetData();
	if (xml.FindElem("bblx"))	bbdy.sFplxdm = xml.GetData();
	if (xml.FindElem("dyfs"))   bbdy.sDyfs = xml.GetData();
	if (xml.FindElem("printername"))	m_sPrinterName = xml.GetData();
	m_iPldy = atoi(bbdy.sDyfs.GetBuffer(0));

	m_sTempFplxdm = bbdy.sFplxdm;

	bbxx = ParseFpmxFromXML(sInputInfo, bbdy);

	printXml = GenerateFpdyXml(bbxx, bbdy.sDylx, bbdy);

	if (bbdy.sDyfs.Compare("100") == 0)
		return printXml;

	rtn = PrintQD(printXml);

	return GenerateXMLFpdy(bbdy, rtn);
}

LONG CYkfpcxdy::PrintQD(LPCSTR billxml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	InitXYoff();

	do
	{
		CMarkup xml;
		if (!xml.SetDoc(billxml) || !xml.FindElem("Print"))
		{
			nrt = -5;// 打印内容解析失败
			break;
		}
		xml.IntoElem();

		nrt = InitPrinter(A4_W, A4_H);
		if (0 != nrt)
			break;

		DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };
		nrt = ::StartDoc(m_hPrinterDC, &di);
		if (nrt <= 0)
		{
			nrt = -3;// 启动打印任务失败
			break;
		}

		while (xml.FindElem("NewPage"))
		{
			int npn = atoi(xml.GetAttrib("pn"));
			if (npn > m_nToPage || npn < m_nFromPage)
				continue;

			nrt = ::StartPage(m_hPrinterDC);
			if (nrt <= 0)
			{
				::EndDoc(m_hPrinterDC);
				nrt = -4;// 发送打印内容失败
				break;
			}

			xml.IntoElem();

			CRect PaintRect;
			PaintRect.left = 10 + nXoff;
			PaintRect.top = 10 - nYoff;
			PaintRect.right = 2000 + nXoff;
			PaintRect.bottom = -2870 - nYoff;

			if (xml.FindElem("PageHeader"))
			{
				xml.IntoElem();
				while (xml.FindElem("Item"))
				{
					RECT itemRect;

					int x = atoi(xml.GetAttrib("x"));
					int y = atoi(xml.GetAttrib("y"));
					int w = atoi(xml.GetAttrib("w"));
					int h = atoi(xml.GetAttrib("h"));
					int nFontSize = atoi(xml.GetAttrib("s"));
					CString strFontName = xml.GetAttrib("f");
					int z = atoi(xml.GetAttrib("z"));
					int fc = atoi(xml.GetAttrib("fc"));
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					PaintTile(nFontSize, fc, strFontName, itemRect, strText, z);
				}
				xml.OutOfElem();
			}

			if (xml.FindElem("PageData"))
			{
				xml.IntoElem();
				CFont fontHeader;
				fontHeader.CreatePointFont(120, "FixedSys", CDC::FromHandle(m_hPrinterDC));
				CFont *pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

				while (xml.FindElem("Item"))
				{
					RECT itemRect;

					int x = atoi(xml.GetAttrib("x"));
					int y = atoi(xml.GetAttrib("y"));
					int w = atoi(xml.GetAttrib("w"));
					int h = atoi(xml.GetAttrib("h"));
					int nFontSize = atoi(xml.GetAttrib("s"));
					CString strFontName = xml.GetAttrib("f");
					int z1 = atoi(xml.GetAttrib("z"));
					int fc = atoi(xml.GetAttrib("fc"));
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					int z = z1 & 0x000000ff;
					int ls = z1 & 0x0000ff00;

					PaintLine(itemRect, ls);

					PaintTile(nFontSize, fc, strFontName, itemRect, strText, z, { 2, 2, 1, 1 });
				}
				xml.OutOfElem();
			}

			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

YKFPCX_BBXX CYkfpcxdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	YKFPCX_BBXX bbxx;
	bbxx.clear();
	int booltrue = false;
	CString sl = "";

	xml.SetDoc(inXml);
	xml.FindElem("business");
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	xml.FindElem("returndata");
	xml.IntoElem();
	xml.FindElem("bbxx");
	xml.IntoElem();
	xml.FindElem("group");
	xml.IntoElem();

	if (xml.FindElem("title")) bbxx.st_sTitle = xml.GetData();
	if (xml.FindElem("sm")) bbxx.st_sSm = xml.GetData();
	int tc = 0;
	if (xml.FindElem("tc")) bbxx.st_nTc = atoi(xml.GetData());
	for (int i = 0; i < bbxx.st_nTc; i++)
	{
		YKFPCX_ITEM im;
		CString key = "";
		key.Format("t%d", i + 1);
		if (xml.FindElem(key))
		{
			im.st_sName = xml.GetData();
			im.st_sKey = getChineseSpell(im.st_sName);
			im.st_nWide = GetWideByItemName(im.st_sKey);
			bbxx.st_lItem.push_back(im);
		}
	}

	if (xml.FindElem("fpxxs"))
	{
		int fpxxCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxxCount; i++)
		{
			xml.FindElem("fpxx");
			xml.IntoElem();
			vector<YKFPCX_ITEM_DATA_XX> v_ix;
			YKFPCX_ITEM_DATA_XX ix;
			LTYKFPCX_ITEM::iterator pos;
			for (pos = bbxx.st_lItem.begin(); pos != bbxx.st_lItem.end(); pos++)
			{
				if (xml.FindElem(pos->st_sKey))
				{
					ix.item_data = xml.GetData();
					ix.xmItem_data.w = pos->st_nWide;
					GetFlagsByName(pos->st_sKey, ix.xmItem_data);
					v_ix.push_back(ix);
				}
			}
			bbxx.st_vData.push_back(v_ix);
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;
	do
	{
		nrt = InitPrinter1(A4_W, A4_H);
		if (0 != nrt)
			break;// 如果打印机选择失败所有行使用默认高度

		DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };

		vector<vector<YKFPCX_ITEM_DATA_XX>>::iterator pos;
		for (pos = bbxx.st_vData.begin(); pos != bbxx.st_vData.end(); pos++)
		{
			int _Line_h = YKFP_LINE_H_MIN;
			int _h = 0;
			for (vector<YKFPCX_ITEM_DATA_XX>::iterator v_pos = pos->begin(); v_pos != pos->end(); v_pos++)
			{
				RECT rect;
				rect.left = 0;
				rect.top = 0;
				rect.right = v_pos->xmItem_data.w;
				rect.bottom = -210;
				_h = PaintTile3(v_pos->xmItem_data.s, 1, v_pos->xmItem_data.f, rect, v_pos->item_data, v_pos->xmItem_data.z);
				_Line_h = _h > _Line_h ? _h : _Line_h;
			}

			for (vector<YKFPCX_ITEM_DATA_XX>::iterator v_pos = pos->begin(); v_pos != pos->end(); v_pos++)
			{
				v_pos->xmItem_data.h = _Line_h;
			}
		}
	} while (false);

	if (NULL != m_hPrinterDC)
	{
		::DeleteDC(m_hPrinterDC);
		m_hPrinterDC = NULL;

		if (NULL != m_pDlg)
			delete m_pDlg;

		m_pDlg = new CPrintDialog(FALSE, PD_ALLPAGES);
	}		

	return bbxx;
}

CString CYkfpcxdy::GenerateFpdyXml(YKFPCX_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CYkfpcxdy::GenerateItemMXXml(YKFPCX_BBXX bbxx)
{
	CMarkup xml;
	m_nPageSize = 19;

	int _nItemPageCount = 0; // 标题需要的分页数
	int _nPageWide = 0; // 当前页的宽度（记录）
	int _nPageItemNums = 0; // 每一页的标题数量
	int _nItemNums = 0; // 当前页的标题数量（记录）
	LTYKFPCX_ITEM::iterator pos_item;
	for (pos_item = bbxx.st_lItem.begin(); pos_item != bbxx.st_lItem.end(); pos_item++)
	{	
		_nItemNums++;
		_nPageWide += pos_item->st_nWide;
		if (_nPageWide > 2000)
		{
			_nPageItemNums = (((--_nItemNums) << (_nItemPageCount) * 8) | _nPageItemNums);
			_nItemPageCount++;
			_nPageWide = pos_item->st_nWide;
			_nItemNums = 1;
		}
	}

	if (_nPageWide <= 2000)
	{
		_nPageItemNums = ((_nItemNums << (_nItemPageCount) * 8) | _nPageItemNums);
		_nItemPageCount++;
	}

	m_nAllPageNum = bbxx.st_vData.size() / m_nPageSize;
	if (0 != bbxx.st_vData.size() % m_nPageSize)
	{
		m_nAllPageNum++;
	}

	m_nAllPageNum *= _nItemPageCount;


	int y = 0;
	xywhsf(bbxx.xmTitle, 0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;
	xywhsf(bbxx.xmSm, 0, y, 500, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDi, 1350, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP1, 1430, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe1, 1520, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmGong, 1600, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP2, 1680, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe2, 1770, y, 80, 50, LS_10, FS, AM_ZC);
	y += 50;

	int nNewPageNum = 1;
	int _nTemp = 0;
	int _nIndexOfItem = 0;
	int _nPageSize = 0;
	while (_nTemp < 4 && (_nPageSize = ((_nPageItemNums >> (8 * _nTemp++)) & 0x000000ff)))
	{	
		int nW = 70; // 标题项高度
		int _TempSize = _nPageSize;
		int _nPostion = 0;
		pos_item = bbxx.st_lItem.begin();
		for (int i = 0; i < _nIndexOfItem; i++)
		{
			pos_item++;
		}
		for (; pos_item != bbxx.st_lItem.end() && _TempSize-- > 0; pos_item++)
		{
			UINT flags = (_TempSize == 0 ? AM_ZC | LINE_STATE_0 : AM_ZC | LINE_STATE_LTB);
			xywhsf(pos_item->st_xmName, _nPostion, y, pos_item->st_nWide, nW, LS_9, FS, flags);
			_nPostion += pos_item->st_nWide;
		}

		BOOL bNewPage = TRUE;
		int _y = y;		

		vector<vector<YKFPCX_ITEM_DATA_XX>>::iterator pos;
		_TempSize = _nPageSize;
		for (pos = bbxx.st_vData.begin(); pos != bbxx.st_vData.end(); pos++)
		{
			vector<YKFPCX_ITEM_DATA_XX>::iterator v_pos = pos->begin() + _nIndexOfItem;
			if (_y + v_pos->xmItem_data.h - y > 2550)
			{
				xml.OutOfElem();
				xml.OutOfElem();
				bNewPage = TRUE;
				_y = y;
			}
			_nPostion = 0;
			if (bNewPage)
			{
				_y += nW;
				xml.AddElem("NewPage");
				xml.AddAttrib("pn", nNewPageNum);
				xml.IntoElem();
				xml.AddElem("PageHeader");
				xml.IntoElem();
				addxml(bbxx.st_sTitle, bbxx.xmTitle);
				addxml(bbxx.st_sDi, bbxx.xmDi);
				addxml(nNewPageNum++, bbxx.xmP1);
				addxml(bbxx.st_sYe1, bbxx.xmYe1);
				addxml(bbxx.st_sGong, bbxx.xmGong);
				addxml(m_nAllPageNum, bbxx.xmP2);
				addxml(bbxx.st_sYe2, bbxx.xmYe2);

				xml.OutOfElem();
				xml.AddElem("PageData");
				xml.IntoElem();
				_TempSize = _nPageSize;
				pos_item = bbxx.st_lItem.begin();
				for (int i = 0; i < _nIndexOfItem; i++)
				{
					pos_item++;
				}
				for (; pos_item != bbxx.st_lItem.end() && _TempSize-- > 0; pos_item++)
				{
					addxml(pos_item->st_sName, pos_item->st_xmName);
				}
				bNewPage = FALSE;
			}

			_TempSize = _nPageSize;
			int yTemp = _y;
			for (; v_pos != pos->end() && _TempSize != 0; v_pos++)
			{
				UINT flags = v_pos->xmItem_data.z;
				flags = (--_TempSize == 0 ? (_y += v_pos->xmItem_data.h, (flags | LINE_STATE_LBR)) : flags | LINE_STATE_LB);
				xywhsf(v_pos->xmItem_data, _nPostion, yTemp, v_pos->xmItem_data.w, v_pos->xmItem_data.h, v_pos->xmItem_data.s, v_pos->xmItem_data.f, flags);
				addxml(v_pos->item_data, v_pos->xmItem_data);
				_nPostion += v_pos->xmItem_data.w;
			}
		}

		if (!bNewPage)
		{
			xml.OutOfElem();
			xml.OutOfElem();
			bNewPage = TRUE;
			_y = y;
		}

		_nIndexOfItem += _nPageSize;
	}

	return xml.GetDoc();
}

int CYkfpcxdy::GetWideByItemName(CString & itemName)
{
	if (getChineseSpell("发票类型").Compare(itemName) == 0) { return 290; }
	else if (getChineseSpell("发票状态").Compare(itemName) == 0) { return 160; }
	else if (getChineseSpell("发票代码").Compare(itemName) == 0) { return 210; }
	else if (getChineseSpell("发票号码").Compare(itemName) == 0) { return 180; }
	else if (getChineseSpell("上传状态").Compare(itemName) == 0) { return 160; }
	 // 004
		else if (getChineseSpell("客户名称").Compare(itemName) == 0) { return 490; }
		else if (getChineseSpell("主要商品名称").Compare(itemName) == 0) { return 490; }
		else if (getChineseSpell("税额").Compare(itemName) == 0) { return 200; }
		else if (getChineseSpell("合计金额").Compare(itemName) == 0) { return 200; }
	

	 // 005 006
		else if (getChineseSpell("车辆类型").Compare(itemName) == 0) { return 160; }
		else if (getChineseSpell("厂牌型号").Compare(itemName) == 0) { return 300; }
	

	 // 005
		else if (getChineseSpell("产地").Compare(itemName) == 0) { return 300; }
		else if (getChineseSpell("车架号码").Compare(itemName) == 0) { return 300; }
		else if (getChineseSpell("购货单位").Compare(itemName) == 0) { return 490; }
		else if (getChineseSpell("销货单位").Compare(itemName) == 0) { return 490; }
		else if (getChineseSpell("不含税价").Compare(itemName) == 0) { return 200; }
	

	 // 006
		else if (getChineseSpell("车牌照号").Compare(itemName) == 0) { return 160; }
		else if (getChineseSpell("车架号").Compare(itemName) == 0) { return 300; }
		else if (getChineseSpell("买方单位").Compare(itemName) == 0) { return 490; }
		else if (getChineseSpell("卖方单位").Compare(itemName) == 0) { return 490; }
		else if (getChineseSpell("车价合计").Compare(itemName) == 0) { return 200; }
	

	 // 004 005
		else if (getChineseSpell("价税合计").Compare(itemName) == 0) { return 200; }
	
	else if (getChineseSpell("原发票代码").Compare(itemName) == 0) { return 210; }
	else if (getChineseSpell("原发票号码").Compare(itemName) == 0) { return 180; }
	 // 004
		else if (getChineseSpell("信息表编号").Compare(itemName) == 0) { return 300; }
	
	else if (getChineseSpell("开票人").Compare(itemName) == 0) { return 150; }
	else if (getChineseSpell("开票日期").Compare(itemName) == 0) { return 210; }
	else if (getChineseSpell("作废人").Compare(itemName) == 0) { return 150; }
	else if (getChineseSpell("作废日期").Compare(itemName) == 0) { return 210; }
	 // 004
		else if (getChineseSpell("客户识别号").Compare(itemName) == 0) { return 300; }
		else if (getChineseSpell("清单标识").Compare(itemName) == 0) { return 160; }
	

	return 200;
}

void CYkfpcxdy::GetFlagsByName(CString & itemName, XM& _xm)
{
	int s = 0;
	CString f = "";
	UINT z = 0;
	if (getChineseSpell("发票类型").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("发票状态").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("发票代码").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("发票号码").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("上传状态").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	 // 004
	else if (getChineseSpell("客户名称").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
	else if (getChineseSpell("主要商品名称").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
	else if (getChineseSpell("税额").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("合计金额").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	

	 // 005 006
		else if (getChineseSpell("车辆类型").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC; }
		else if (getChineseSpell("厂牌型号").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
	

	 // 005
		else if (getChineseSpell("产地").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("车架号码").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("购货单位").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("销货单位").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("不含税价").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	

	 // 006
		else if (getChineseSpell("车牌照号").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("车架号").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("买方单位").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("卖方单位").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_CHEKC; }
		else if (getChineseSpell("车价合计").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	

	 // 004 005
		else if (getChineseSpell("价税合计").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	
	else if (getChineseSpell("原发票代码").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("原发票号码").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	 // 004
		else if (getChineseSpell("信息表编号").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC; }
	
	else if (getChineseSpell("开票人").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC; }
	else if (getChineseSpell("开票日期").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	else if (getChineseSpell("作废人").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC; }
	else if (getChineseSpell("作废日期").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
	 // 004
		else if (getChineseSpell("客户识别号").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC_S; }
		else if (getChineseSpell("清单标识").Compare(itemName) == 0) { s = LS_9; f = FS; z = AM_ZC; }

	_xm.s = s;
	_xm.f = f;
	_xm.z = z;
}

bool CYkfpcxdy::In(wchar_t start, wchar_t end, wchar_t code)
{
	if (code >= start && code <= end)
	{
		return   true;
	}
	return   false;
}

char CYkfpcxdy::convert(wchar_t n)
{
	if (In(0xB0A1, 0xB0C4, n))   return   'a';
	if (In(0XB0C5, 0XB2C0, n))   return   'b';
	if (In(0xB2C1, 0xB4ED, n))   return   'c';
	if (In(0xB4EE, 0xB6E9, n))   return   'd';
	if (In(0xB6EA, 0xB7A1, n))   return   'e';
	if (In(0xB7A2, 0xB8c0, n))   return   'f';
	if (In(0xB8C1, 0xB9FD, n))   return   'g';
	if (In(0xB9FE, 0xBBF6, n))   return   'h';
	if (In(0xBBF7, 0xBFA5, n))   return   'j';
	if (In(0xBFA6, 0xC0AB, n))   return   'k';
	if (In(0xC0AC, 0xC2E7, n))   return   'l';
	if (In(0xC2E8, 0xC4C2, n))   return   'm';
	if (In(0xC4C3, 0xC5B5, n))   return   'n';
	if (In(0xC5B6, 0xC5BD, n))   return   'o';
	if (In(0xC5BE, 0xC6D9, n))   return   'p';
	if (In(0xC6DA, 0xC8BA, n))   return   'q';
	if (In(0xC8BB, 0xC8F5, n))   return   'r';
	if (In(0xC8F6, 0xCBF0, n))   return   's';
	if (In(0xCBFA, 0xCDD9, n))   return   't';
	if (In(0xCDDA, 0xCEF3, n))   return   'w';
	if (In(0xCEF4, 0xD188, n))   return   'x';
	if (In(0xD1B9, 0xD4D0, n))   return   'y';
	if (In(0xD4D1, 0xD7F9, n))   return   'z';
	return   '\0';
}

CString CYkfpcxdy::getChineseSpell(CString src)
{
	string   sChinese = src.GetBuffer(0); 
	char   chr[3];
	wchar_t   wchr = 0;
	int len = sChinese.length() / 2;
	char*   buff = new   char[len + 1];
	memset(buff, 0x00, sizeof(char)*sChinese.length() / 2 + 1);

	for (size_t i = 0, j = 0; i < (sChinese.length() / 2); ++i)
	{
		memset(chr, 0x00, sizeof(chr));
		chr[0] = sChinese[j++];
		chr[1] = sChinese[j++];
		chr[2] = '\0';

		wchr = 0;
		wchr = (chr[0] & 0xff) << 8;
		wchr |= (chr[1] & 0xff);
		buff[i] = convert(wchr);
	}

	CString rdata(buff);
	if (buff != NULL)
		delete buff;

	return  rdata;
}
