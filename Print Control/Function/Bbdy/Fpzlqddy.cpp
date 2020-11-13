#include "../../pch.h"
#include "Fpzlqddy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define XU_W	100
#define ZL_W	350
#define DM_W	250
#define HM_W	250
#define RQ_W	225
#define SH_W	275
#define JE_W	225
#define SE_W	225

CFpzlqddy::CFpzlqddy()
{
}

CFpzlqddy::~CFpzlqddy()
{
}

CString CFpzlqddy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	FPZLQD_BBXX bbxx;
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

LONG CFpzlqddy::PrintQD(LPCSTR billxml)
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
			nrt = ::StartPage(m_hPrinterDC);
			if (nrt <= 0)
			{
				::EndDoc(m_hPrinterDC);
				nrt = -4;// 发送打印内容失败
				break;
			}

			int npn = atoi(xml.GetAttrib("pn"));
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
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					PaintTile(nFontSize, strFontName, itemRect, strText, z);
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
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					int z = z1 & 0x000000ff;
					int ls = z1 & 0x0000ff00;

					PaintLine(itemRect, ls);

					PaintTile(nFontSize, strFontName, itemRect, strText, z, 1, 0, 2);
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

FPZLQD_BBXX CFpzlqddy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	FPZLQD_BBXX bbxx;
	bbxx.clear();
	FPZLQD_QDXX bmxx;
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
	if (xml.FindElem("zbrq")) bbxx.st_sZbrq = xml.GetData();
	if (xml.FindElem("name")) bbxx.st_sName = xml.GetData();
	if (xml.FindElem("ssq")) bbxx.st_sSsq = xml.GetData();
	if (xml.FindElem("nsrsbh")) bbxx.st_sNsrsbh = xml.GetData();
	if (xml.FindElem("qymc")) bbxx.st_sQymc = xml.GetData();
	if (xml.FindElem("dzdh")) bbxx.st_sDzdh = xml.GetData();
	if (xml.FindElem("dw")) bbxx.st_sDw = xml.GetData();

	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	if (xml.FindElem("t6")) bbxx.st_sT6 = xml.GetData();
	if (xml.FindElem("t7")) bbxx.st_sT7 = xml.GetData();
	if (xml.FindElem("t8")) bbxx.st_sT8 = xml.GetData();

	if (xml.FindElem("tbr")) bbxx.st_sTbr = xml.GetData();
	if (xml.FindElem("cyr")) bbxx.st_sCyr = xml.GetData();
	if (xml.FindElem("lrr")) bbxx.st_sLrr = xml.GetData();
	if (xml.FindElem("fhr")) bbxx.st_sFhr = xml.GetData();
	if (xml.FindElem("sjr")) bbxx.st_sSjr = xml.GetData();

	if (xml.FindElem("qdxxs"))
	{
		int nCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < nCount; i++)
		{
			xml.FindElem("qdxx");
			bmxx.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("fpzl")) bmxx.st_sFpzl = xml.GetData();
			if (xml.FindElem("fpdm"))   bmxx.st_sFpdm = xml.GetData();
			if (xml.FindElem("fphm"))   bmxx.st_sFphm = xml.GetData();
			if (xml.FindElem("kprq"))   bmxx.st_sKprq = xml.GetData();
			if (xml.FindElem("gfsh"))     bmxx.st_sGfsh = xml.GetData();
			if (xml.FindElem("hjje"))   bmxx.st_sHjje = xml.GetData();
			if (xml.FindElem("hjse"))     bmxx.st_sHjse = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lFpzlqdBbxx.push_back(bmxx);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CFpzlqddy::GenerateFpdyXml(FPZLQD_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CFpzlqddy::GenerateItemMXXml(FPZLQD_BBXX bbxx)
{
	CMarkup xml;

	int x0 = 0;
	int x1 = x0 + XU_W;
	int x2 = x1 + ZL_W;
	int x3 = x2 + DM_W;
	int x4 = x3 + HM_W;
	int x5 = x4 + RQ_W;
	int x6 = x5 + SH_W;
	int x7 = x6 + JE_W;
	int x8 = x7 + SE_W;

	int y = 0;
	xywhsf(bbxx.xmTitle, x0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;
	xywhsf(bbxx.xmZbrq, x0, y, 500, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDi, 1350, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP1, 1430, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe1, 1520, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmGong, 1600, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP2, 1680, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe2, 1770, y, 80, 50, LS_10, FS, AM_ZC);
	y += 50;

	xywhsf(bbxx.xmName, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmSsq, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmNsrsbh, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmQymc, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmDzdh, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmDw, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;

	int nW = 70; // 标题项高度
	xywhsf(bbxx.xmT1, x0, y, XU_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT2, x1, y, ZL_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT3, x2, y, DM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT4, x3, y, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT5, x4, y, RQ_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT6, x5, y, SH_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT7, x6, y, JE_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT8, x7, y, SE_W, nW, LS_9, FS, AM_ZC | LINE_STATE_0);
	y += nW;

	int nLY = 90;// 数据行高度

	m_nLineNum = 0;
	m_nPageSize = 25;
	int _y = y;
	LTFPZLQD_QDXX::iterator pos;
	for (pos = bbxx.st_lFpzlqdBbxx.begin(); pos != bbxx.st_lFpzlqdBbxx.end(); pos++)
	{
		xywhsf(pos->xmXh, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmFpzl, x1, _y, ZL_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmFpdm, x2, _y, DM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmFphm, x3, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmKprq, x4, _y, RQ_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmGfsh, x5, _y, SH_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmHjje, x6, _y, JE_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmHjse, x7, _y, SE_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);
		_y += nLY;

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			int _w = 300;
			BBPAGE_END be;
			xywhsf(be.xmTbr, _w * 0, _y, _w, nLY, LS_9, FS, AM_VCL);
			xywhsf(be.xmCyr, _w * 1, _y, _w, nLY, LS_9, FS, AM_VCL);
			xywhsf(be.xmLrr, _w * 2, _y, _w, nLY, LS_9, FS, AM_VCL);
			xywhsf(be.xmFhr, _w * 3, _y, _w, nLY, LS_9, FS, AM_VCL);
			xywhsf(be.xmSjr, _w * 4, _y, _w, nLY, LS_9, FS, AM_VCL);
			bbxx.st_lBbpage_end.push_back(be);
			_y = y;
		}

	}

	if (m_nLineNum%m_nPageSize != 0 || m_nAllPageNum == 0)
	{
		int _w = 300;
		BBPAGE_END be;
		xywhsf(be.xmTbr, _w * 0, _y, _w, nLY, LS_9, FS, AM_VCL);
		xywhsf(be.xmCyr, _w * 1, _y, _w, nLY, LS_9, FS, AM_VCL);
		xywhsf(be.xmLrr, _w * 2, _y, _w, nLY, LS_9, FS, AM_VCL);
		xywhsf(be.xmFhr, _w * 3, _y, _w, nLY, LS_9, FS, AM_VCL);
		xywhsf(be.xmSjr, _w * 4, _y, _w, nLY, LS_9, FS, AM_VCL);
		bbxx.st_lBbpage_end.push_back(be);
	}

	m_nAllPageNum = m_nLineNum / m_nPageSize;
	if (0 != m_nLineNum % m_nPageSize || m_nAllPageNum == 0)
	{
		m_nAllPageNum++;
	}

	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;

	LTFPZLQD_QDXX::iterator posb = bbxx.st_lFpzlqdBbxx.begin();
	LTFPZLQD_QDXX::iterator pose = bbxx.st_lFpzlqdBbxx.end();

	do
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sZbrq, bbxx.xmZbrq);
			addxml(bbxx.st_sDi, bbxx.xmDi);
			addxml(nNewPageNum++, bbxx.xmP1);
			addxml(bbxx.st_sYe1, bbxx.xmYe1);
			addxml(bbxx.st_sGong, bbxx.xmGong);
			addxml(m_nAllPageNum, bbxx.xmP2);
			addxml(bbxx.st_sYe2, bbxx.xmYe2);
			addxml(bbxx.st_sName, bbxx.xmName);
			addxml(bbxx.st_sSsq, bbxx.xmSsq);
			addxml(bbxx.st_sNsrsbh, bbxx.xmNsrsbh);
			addxml(bbxx.st_sQymc, bbxx.xmQymc);
			addxml(bbxx.st_sDzdh, bbxx.xmDzdh);
			addxml(bbxx.st_sDw, bbxx.xmDw);

			BBPAGE_END be = bbxx.st_lBbpage_end.front();
			addxml(bbxx.st_sTbr, be.xmTbr);
			addxml(bbxx.st_sCyr, be.xmCyr);
			addxml(bbxx.st_sLrr, be.xmLrr);
			addxml(bbxx.st_sFhr, be.xmFhr);
			addxml(bbxx.st_sSjr, be.xmSjr);
			bbxx.st_lBbpage_end.pop_front();

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
			bNewPage = FALSE;
		}

		for (; posb != pose; )
		{
			addxml(posb->st_nXh, posb->xmXh);
			addxml(posb->st_sFpzl, posb->xmFpzl);
			addxml(posb->st_sFpdm, posb->xmFpdm);
			addxml(posb->st_sFphm, posb->xmFphm);
			addxml(posb->st_sKprq, posb->xmKprq);
			addxml(posb->st_sGfsh, posb->xmGfsh);
			addxml(posb->st_sHjje, posb->xmHjje);
			addxml(posb->st_sHjse, posb->xmHjse);
			posb++;

			num++;
			if (num % m_nPageSize == 0)
			{
				xml.OutOfElem();
				xml.OutOfElem();
				bNewPage = TRUE;
				break;
			}
		}
	} while (posb != pose);

	//for (pos = bbxx.st_lFpzlqdBbxx.begin(); pos != bbxx.st_lFpzlqdBbxx.end(); pos++)
	//{
	//	if (bNewPage)
	//	{
	//		xml.AddElem("NewPage");
	//		xml.AddAttrib("pn", nNewPageNum);
	//		xml.IntoElem();
	//		xml.AddElem("PageHeader");
	//		xml.IntoElem();
	//		addxml(bbxx.st_sTitle, bbxx.xmTitle);
	//		addxml(bbxx.st_sZbrq, bbxx.xmZbrq);
	//		addxml(bbxx.st_sDi, bbxx.xmDi);
	//		addxml(nNewPageNum++, bbxx.xmP1);
	//		addxml(bbxx.st_sYe1, bbxx.xmYe1);
	//		addxml(bbxx.st_sGong, bbxx.xmGong);
	//		addxml(m_nAllPageNum, bbxx.xmP2);
	//		addxml(bbxx.st_sYe2, bbxx.xmYe2);
	//		addxml(bbxx.st_sName, bbxx.xmName);
	//		addxml(bbxx.st_sSsq, bbxx.xmSsq);
	//		addxml(bbxx.st_sNsrsbh, bbxx.xmNsrsbh);
	//		addxml(bbxx.st_sQymc, bbxx.xmQymc);
	//		addxml(bbxx.st_sDzdh, bbxx.xmDzdh);
	//		addxml(bbxx.st_sDw, bbxx.xmDw);

	//		BBPAGE_END be = bbxx.st_lBbpage_end.front();
	//		addxml(bbxx.st_sTbr, be.xmTbr);
	//		addxml(bbxx.st_sCyr, be.xmCyr);
	//		addxml(bbxx.st_sLrr, be.xmLrr);
	//		addxml(bbxx.st_sFhr, be.xmFhr);
	//		addxml(bbxx.st_sSjr, be.xmSjr);
	//		bbxx.st_lBbpage_end.pop_front();

	//		xml.OutOfElem();
	//		xml.AddElem("PageData");
	//		xml.IntoElem();
	//		addxml(bbxx.st_sT1, bbxx.xmT1);
	//		addxml(bbxx.st_sT2, bbxx.xmT2);
	//		addxml(bbxx.st_sT3, bbxx.xmT3);
	//		addxml(bbxx.st_sT4, bbxx.xmT4);
	//		addxml(bbxx.st_sT5, bbxx.xmT5);
	//		addxml(bbxx.st_sT6, bbxx.xmT6);
	//		addxml(bbxx.st_sT7, bbxx.xmT7);
	//		addxml(bbxx.st_sT8, bbxx.xmT8);
	//		bNewPage = FALSE;
	//	}
	//	addxml(pos->st_nXh, pos->xmXh);
	//	addxml(pos->st_sFpzl, pos->xmFpzl);
	//	addxml(pos->st_sFpdm, pos->xmFpdm);
	//	addxml(pos->st_sFphm, pos->xmFphm);
	//	addxml(pos->st_sKprq, pos->xmKprq);
	//	addxml(pos->st_sGfsh, pos->xmGfsh);
	//	addxml(pos->st_sHjje, pos->xmHjje);
	//	addxml(pos->st_sHjse, pos->xmHjse);

	//	num++;
	//	if (num % m_nPageSize == 0)
	//	{
	//		xml.OutOfElem();
	//		xml.OutOfElem();
	//		bNewPage = TRUE;
	//	}
	//}

	if (!bNewPage)
	{
		xml.OutOfElem();
		xml.OutOfElem();
		bNewPage = TRUE;
	}

	return xml.GetDoc();
}
