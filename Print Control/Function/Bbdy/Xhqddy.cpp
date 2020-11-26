#include "../../pch.h"
#include "Xhqddy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define XU_W	100
#define MC_W	400
#define XH_W	250
#define DW_W	192
#define SL_W	192
#define DJ_W	200
#define JE_W	250
#define SLV_W	100
#define SE_W	250

CXhqddy::CXhqddy()
{
}

CXhqddy::~CXhqddy()
{
}

CString CXhqddy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	XHQD_BBXX bbxx;
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

LONG CXhqddy::PrintQD(LPCSTR billxml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	InitXYoff();

	nrt = InitPrinter(A4_W, A4_H);
	if (0 != nrt)
		return nrt;

	DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };

	do
	{
		CMarkup xml;
		if (!xml.SetDoc(billxml) || !xml.FindElem("Print"))
		{
			nrt = -5;// 打印内容解析失败
			break;
		}
		xml.IntoElem();
	
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

			if (xml.FindElem("PageEnd"))
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

			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
	} while (--m_nCopies);

	return nrt;
}

XHQD_BBXX CXhqddy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	XHQD_BBXX bbxx;
	bbxx.clear();
	XHQD_QDXX bmxx;
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
	if (xml.FindElem("ghdwmc")) bbxx.st_sGhdwmc = xml.GetData();
	if (xml.FindElem("xhdwmc")) bbxx.st_sXhdwmc = xml.GetData();
	if (xml.FindElem("fpdm")) bbxx.st_sFpdm = xml.GetData();
	if (xml.FindElem("fphm")) bbxx.st_sFphm = xml.GetData();

	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	if (xml.FindElem("t6")) bbxx.st_sT6 = xml.GetData();
	if (xml.FindElem("t7")) bbxx.st_sT7 = xml.GetData();
	if (xml.FindElem("t8")) bbxx.st_sT8 = xml.GetData();
	if (xml.FindElem("t9")) bbxx.st_sT9 = xml.GetData();

	if (xml.FindElem("z1")) bbxx.st_sZ1 = xml.GetData();
	if (xml.FindElem("z2")) bbxx.st_sZ2 = xml.GetData();
	if (xml.FindElem("z3")) bbxx.st_sZ3 = xml.GetData();

	if (xml.FindElem("fplxdm")) bbxx.st_sFplxdm = xml.GetData();
	if (xml.FindElem("bz")) bbxx.st_sBz = xml.GetData();

	if (xml.FindElem("qdxxs"))
	{
		int nCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < nCount; i++)
		{
			xml.FindElem("qdxx");
			bmxx.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("fphxz")) bmxx.st_sFphxz = xml.GetData();
			if (xml.FindElem("spmc"))   bmxx.st_sSpmc = xml.GetData();
			if (xml.FindElem("ggxh"))     bmxx.st_sGgxh = xml.GetData();
			if (xml.FindElem("dw"))   bmxx.st_sDw = xml.GetData();
			if (xml.FindElem("spsl"))   bmxx.st_sSpsl = xml.GetData();
			if (xml.FindElem("dj"))     bmxx.st_sDj = xml.GetData();
			if (xml.FindElem("je"))   bmxx.st_sJe = xml.GetData();
			if (xml.FindElem("sl"))     bmxx.st_sSlv = xml.GetData();
			if (xml.FindElem("se"))   bmxx.st_sSe = xml.GetData();
			if (xml.FindElem("hsbz"))     bmxx.st_sHsbz = xml.GetData();
			if (xml.FindElem("lslbs"))   bmxx.st_sLslbs = xml.GetData();
			xml.OutOfElem();

			CString sTempSl = bmxx.st_sSlv;
			sTempSl.TrimLeft(_T("0."));
			sTempSl = sTempSl.Left(2);
			if (sTempSl.GetLength() > 0)
			{
				double dTempSl = atof(bmxx.st_sSlv.GetBuffer(0)) * 100;
				bmxx.st_sSlv.Format("%g%s", dTempSl, "%");
				if (bmxx.st_sSe.Find('.') != -1)  //有小数点
				{
					int leng_Hjje = bmxx.st_sSe.GetLength();
					if (bmxx.st_sSe.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
					{
						bmxx.st_sSe += "0";
					}
				}
				else
				{
					if (bmxx.st_sSe.GetLength() > 0)
						bmxx.st_sSe += ".00";
					else
						bmxx.st_sSe += "0.00";
				}
			}
			else
			{
				if (bmxx.st_sLslbs.CompareNoCase("1") == 0)
				{
					bmxx.st_sSlv.Format("%s", "免税");
				}
				else if (bmxx.st_sLslbs.CompareNoCase("2") == 0)
				{
					bmxx.st_sSlv.Format("%s", "不征税");
				}
				else if (bmxx.st_sLslbs.CompareNoCase("3") == 0)
				{
					bmxx.st_sSlv.Format("%s", "0%");
				}
				else
				{
					bmxx.st_sSlv.Format("%s", "0%");
				}
			}
			if (bbxx.st_sFplxdm.CompareNoCase("004") != 0)
			{
				if (bmxx.st_sSe.CompareNoCase("0") == 0 ||
					bmxx.st_sSe.CompareNoCase("0.0") == 0 ||
					bmxx.st_sSe.CompareNoCase("0.00") == 0)
				{
					bmxx.st_sSe.Format("***");
				}
			}
			if (bmxx.st_sJe.Find('.') != -1)  //有小数点
			{
				int leng_Hjje = bmxx.st_sJe.GetLength();
				if (bmxx.st_sJe.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
				{
					bmxx.st_sJe += "0";
				}
			}
			else
			{
				if (bmxx.st_sJe.GetLength() > 0)
					bmxx.st_sJe += ".00";
				else
					bmxx.st_sJe += "0.00";
			}

			if (bmxx.st_sSe.CompareNoCase("-0.00") == 0)
			{
				bmxx.st_sSe.Delete(0, 1);
			}

			bbxx.st_lXhqd_qdxx.push_back(bmxx);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CXhqddy::GenerateFpdyXml(XHQD_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CXhqddy::GenerateItemMXXml(XHQD_BBXX bbxx)
{
	CMarkup xml;

	double nJE = 0.00;
	double nSE = 0.00;
	double nHjje = 0.00;
	double nHjse = 0.00;
	double dZKJE = 0.00;
	double dZKSE = 0.00;

	int x0 = 0;
	int x1 = x0 + XU_W;
	int x2 = x1 + MC_W;
	int x3 = x2 + XH_W;
	int x4 = x3 + DW_W;
	int x5 = x4 + SL_W;
	int x6 = x5 + DJ_W;
	int x7 = x6 + JE_W;
	int x8 = x7 + SLV_W;
	int x9 = x8 + SE_W;

	int y = 0;
	xywhsf(bbxx.xmTitle, x0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;

	xywhsf(bbxx.xmGhdwmc, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;

	xywhsf(bbxx.xmXhdwmc, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;

	xywhsf(bbxx.xmFpdm, x0, y, 700, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmFphm, x0 + 700, y, 500, 50, LS_10, FS, AM_VCL);

	xywhsf(bbxx.xmDi, 1350, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP1, 1430, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe1, 1520, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmGong, 1600, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP2, 1680, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe2, 1770, y, 80, 50, LS_10, FS, AM_ZC);
	y += 50;

	int nW = 70; // 标题项高度
	xywhsf(bbxx.xmT1, x0, y, XU_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT2, x1, y, MC_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT3, x2, y, XH_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT4, x3, y, DW_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT5, x4, y, SL_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT6, x5, y, DJ_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT7, x6, y, JE_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT8, x7, y, SLV_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT9, x8, y, SE_W, nW, LS_9, FS, AM_ZC | LINE_STATE_0);
	y += nW;

	int nLY = 70;// 数据行高度

	m_nLineNum = 0;
	m_nPageSize = 26;
	int _y = y;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;

	m_nAllPageNum = bbxx.st_lXhqd_qdxx.size() / m_nPageSize;
	if (0 != bbxx.st_lXhqd_qdxx.size() % m_nPageSize)
	{
		m_nAllPageNum++;
	}

	LTXHQD_QDXX::iterator pos;
	for (pos = bbxx.st_lXhqd_qdxx.begin(); pos != bbxx.st_lXhqd_qdxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sGhdwmc, bbxx.xmGhdwmc);
			addxml(bbxx.st_sXhdwmc, bbxx.xmXhdwmc);
			addxml(bbxx.st_sFpdm, bbxx.xmFpdm);
			addxml(bbxx.st_sFphm, bbxx.xmFphm);

			addxml(bbxx.st_sDi, bbxx.xmDi);
			addxml(nNewPageNum++, bbxx.xmP1);
			addxml(bbxx.st_sYe1, bbxx.xmYe1);
			addxml(bbxx.st_sGong, bbxx.xmGong);
			addxml(m_nAllPageNum, bbxx.xmP2);
			addxml(bbxx.st_sYe2, bbxx.xmYe2);

			xml.OutOfElem();
			xml.AddElem("PageData");
			xml.IntoElem();
			addxml(bbxx.st_sT1, bbxx.xmT1);
			addxml(bbxx.st_sT2, bbxx.xmT2);
			addxml(bbxx.st_sT3, bbxx.xmT3);
			addxml(bbxx.st_sT4, bbxx.xmT4);
			addxml(bbxx.st_sT5, bbxx.xmT5);
			addxml(bbxx.st_sT6, bbxx.xmT6);
			addxml(bbxx.st_sT7, bbxx.xmT7);
			addxml(bbxx.st_sT8, bbxx.xmT8);
			addxml(bbxx.st_sT9, bbxx.xmT9);
			bNewPage = FALSE;
		}
		xywhsf(pos->xmXh, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		xywhsf(pos->xmSpmc, x1, _y, MC_W, nLY, LS_9, FS, AM_VCL | LINE_STATE_L);
		xywhsf(pos->xmGgxh, x2, _y, XH_W, nLY, LS_9, FS, AM_VCL | LINE_STATE_L);
		xywhsf(pos->xmDw, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_L);
		xywhsf(pos->xmSpsl, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_L);
		xywhsf(pos->xmDj, x5, _y, DJ_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
		xywhsf(pos->xmJe, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
		xywhsf(pos->xmSlv, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_L);
		xywhsf(pos->xmSe, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);

		addxml(pos->st_nXh, pos->xmXh);
		addxml(pos->st_sSpmc, pos->xmSpmc);
		addxml(pos->st_sGgxh, pos->xmGgxh);
		addxml(pos->st_sDw, pos->xmDw);
		addxml(pos->st_sSpsl, pos->xmSpsl);
		addxml(pos->st_sDj, pos->xmDj);
		addxml(pos->st_sJe, pos->xmJe);
		addxml(pos->st_sSlv, pos->xmSlv);
		addxml(pos->st_sSe, pos->xmSe);
		_y += nLY;

		if (pos->st_sFphxz.CompareNoCase("1") == 0)
		{
			dZKJE += atof(pos->st_sJe.GetBuffer(0));
			dZKSE += atof(pos->st_sSe.GetBuffer(0));
		}
		else
		{
			nJE += atof(pos->st_sJe.GetBuffer(0));
			nSE += atof(pos->st_sSe.GetBuffer(0));
		}

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			for (int k = 0; k < 2; k++)
			{
				XM _xm;
				xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
				addxml("", _xm);
				xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
				addxml("", _xm);
				_y += nLY;
			}
			CString xjje;
			CString xjse;
			CString zkje;
			CString zkse;
			CString hjje;
			CString hjse;

			xjje.Format("%.2f", (nJE + dZKJE));
			xjse.Format("%.2f", (nSE + dZKSE));
			zkje.Format("%.2f", dZKJE);
			zkse.Format("%.2f", dZKSE);
			nHjje += (nJE + dZKJE);
			nHjse += (nSE + dZKSE);
			hjje.Format("%.2f", nHjje);
			hjse.Format("%.2f", nHjse);

			XM _xm;
			xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_L);
			addxml("小计", _xm);
			xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
			addxml(xjje, _xm);
			xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
			addxml(xjse, _xm);
			_y += nLY;

			xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_L);
			addxml("折扣", _xm);
			xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
			addxml(zkje, _xm);
			xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
			addxml(zkse, _xm);
			_y += nLY;

			xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml("总计", _xm);
			xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
			addxml(hjje, _xm);
			xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
			addxml(hjse, _xm);
			_y += nLY;
			xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml("备注", _xm);
			xywhsf(_xm, x1, _y, x9 - x1, nLY, LS_9, FS, AM_ZC_CHEKC);
			addxml(bbxx.st_sBz, _xm);
			_y += nLY;

			xml.OutOfElem();
			xml.AddElem("PageEnd");
			xml.IntoElem();
			xywhsf(_xm, x0, _y, 500, nLY, LS_9, FS, AM_VCL);
			addxml(bbxx.st_sZ1, _xm);
			xywhsf(_xm, x0 + 1300, _y, 1500, nLY, LS_9, FS, AM_VCL);
			addxml(bbxx.st_sZ2, _xm);
			_y += nLY;
			xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
			addxml(bbxx.st_sZ3, _xm);
			_y = y;
			xml.OutOfElem();
			xml.OutOfElem();
			nJE = 0.00;
			nSE = 0.00;
			dZKJE = 0.00;
			dZKSE = 0.00;
			bNewPage = TRUE;
		}

	}

	if (!bNewPage)
	{
		for (int k = 0; k < 2; k++)
		{
			XM _xm;
			xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
			addxml("", _xm);
			xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
			addxml("", _xm);
			_y += nLY;
		}

		CString xjje;
		CString xjse;
		CString zkje;
		CString zkse;
		CString hjje;
		CString hjse;

		xjje.Format("%.2f", (nJE + dZKJE));
		xjse.Format("%.2f", (nSE + dZKSE));
		zkje.Format("%.2f", dZKJE);
		zkse.Format("%.2f", dZKSE);
		nHjje += (nJE + dZKJE);
		nHjse += (nSE + dZKSE);
		hjje.Format("%.2f", nHjje);
		hjse.Format("%.2f", nHjse);

		XM _xm;
		xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_L);
		addxml("小计", _xm);
		xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
		addxml(xjje, _xm);
		xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
		addxml(xjse, _xm);
		_y += nLY;

		xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_L);
		addxml("折扣", _xm);
		xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
		addxml(zkje, _xm);
		xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
		addxml(zkse, _xm);
		_y += nLY;

		xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("总计", _xm);
		xywhsf(_xm, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x2, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x3, _y, DW_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x4, _y, SL_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x5, _y, DJ_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x6, _y, JE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_L);
		addxml(hjje, _xm);
		xywhsf(_xm, x7, _y, SLV_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_L);
		addxml("", _xm);
		xywhsf(_xm, x8, _y, SE_W, nLY, LS_9, FS, AM_VCR_S | LINE_STATE_LR);
		addxml(hjse, _xm);
		_y += nLY;
		xywhsf(_xm, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("备注", _xm);
		xywhsf(_xm, x1, _y, x9 - x1, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sBz, _xm);
		_y += nLY;

		xml.OutOfElem();
		xml.AddElem("PageEnd");
		xml.IntoElem();
		xywhsf(_xm, x0, _y, 500, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sZ1, _xm);
		xywhsf(_xm, x0 + 1300, _y, 1500, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sZ2, _xm);
		_y += nLY;
		xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sZ3, _xm);
		_y = y;
		xml.OutOfElem();
		xml.OutOfElem();
		nJE = 0.00;
		nSE = 0.00;
		nHjje = 0.00;
		nHjse = 0.00;
		dZKJE = 0.00;
		dZKSE = 0.00;
		bNewPage = TRUE;
	}

	return xml.GetDoc();
}
