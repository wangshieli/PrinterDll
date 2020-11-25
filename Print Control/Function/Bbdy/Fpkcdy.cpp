#include "../../pch.h"
#include "Fpkcdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define T1_W	120
#define T2_W	330
#define T3_W	200
#define T4_W	250
#define T5_W	200
#define T6_W	200
#define T7_W	200
#define T8_W	200
#define T9_W	200

CFpkcdy::CFpkcdy()
{

}

CFpkcdy::~CFpkcdy()
{

}

CString CFpkcdy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	FPKCCX_BBXX bbxx;
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

LONG CFpkcdy::PrintQD(LPCSTR billxml)
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

FPKCCX_BBXX CFpkcdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	FPKCCX_BBXX bbxx;
	bbxx.clear();
	FPKCCX_FPXX bbxm;
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
	if (xml.FindElem("sm")) bbxx.st_sSkpbh = xml.GetData();
	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	if (xml.FindElem("t6")) bbxx.st_sT6 = xml.GetData();
	if (xml.FindElem("t7")) bbxx.st_sT7 = xml.GetData();
	if (xml.FindElem("t8")) bbxx.st_sT8 = xml.GetData();
	if (xml.FindElem("t9")) bbxx.st_sT9 = xml.GetData();

	if (xml.FindElem("fpxxs"))
	{
		int fpxxCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxxCount; i++)
		{
			xml.FindElem("fpxx");
			bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("fpdm")) bbxm.st_sFpdm = xml.GetData();
			if (xml.FindElem("fpfs"))   bbxm.st_sFpfs = xml.GetData();
			if (xml.FindElem("fplx"))  bbxm.st_sFplx = xml.GetData();
			if (xml.FindElem("fpqshm"))  bbxm.st_sFpqshm = xml.GetData();
			if (xml.FindElem("fpzzhm"))  bbxm.st_sFpzzhm = xml.GetData();
			if (xml.FindElem("kpxe"))  bbxm.st_sKpxe = xml.GetData();
			if (xml.FindElem("lgrq"))  bbxm.st_sLgrq = xml.GetData();
			if (xml.FindElem("syfs"))   bbxm.st_sSyfs = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lFpkccx_fpxx.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CFpkcdy::GenerateFpdyXml(FPKCCX_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CFpkcdy::GenerateItemMXXml(FPKCCX_BBXX bbxx)
{
	CMarkup xml;

	int x0 = 0;
	int x1 = x0 + T1_W;
	int x2 = x1 + T2_W;
	int x3 = x2 + T3_W;
	int x4 = x3 + T4_W;
	int x5 = x4 + T5_W;
	int x6 = x5 + T6_W;
	int x7 = x6 + T7_W;
	int x8 = x7 + T8_W;
	int x9 = x8 + T9_W;

	int y = 0;
	xywhsf(bbxx.xmTitle, x0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;
	xywhsf(bbxx.xmSkpbh, 0, 100, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDi, 1350, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP1, 1430, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe1, 1520, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmGong, 1600, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP2, 1680, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe2, 1770, y, 80, 50, LS_10, FS, AM_ZC);
	y += 50;

	int nW = 70; // 标题项高度
	xywhsf(bbxx.xmT1, x0, y, T1_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT2, x1, y, T2_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT3, x2, y, T3_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT4, x3, y, T4_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT5, x4, y, T5_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT6, x5, y, T6_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT7, x6, y, T7_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT8, x7, y, T8_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT9, x8, y, T9_W, nW, LS_9, FS, AM_ZC | LINE_STATE_0);
	y += nW;

	int nLY = 90;// 数据行高度

	m_nLineNum = 0;
	m_nPageSize = 27;
	int _y = y;
	LTFPKCCX_FPXX::iterator pos;
	for (pos = bbxx.st_lFpkccx_fpxx.begin(); pos != bbxx.st_lFpkccx_fpxx.end(); pos++)
	{
		xywhsf(pos->xmXh, x0, _y, T1_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		xywhsf(pos->xmFplx, x1, _y, T2_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmFpdm, x2, _y, T3_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmKpxe, x3, _y, T4_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmFpqshm, x4, _y, T5_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmFpzzhm, x5, _y, T6_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmFpfs, x6, _y, T7_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmSyfs, x7, _y, T8_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmLgrq, x8, _y, T9_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LBR);
		_y += nLY;

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			_y = y;
		}
	}

	m_nAllPageNum = m_nLineNum / m_nPageSize;
	if (0 != m_nLineNum % m_nPageSize)
		m_nAllPageNum++;

	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;
	for (pos = bbxx.st_lFpkccx_fpxx.begin(); pos != bbxx.st_lFpkccx_fpxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sSkpbh, bbxx.xmSkpbh);
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
		addxml(pos->st_nXh, pos->xmXh);
		addxml(pos->st_sFplx, pos->xmFplx);
		addxml(pos->st_sFpdm, pos->xmFpdm);
		addxml(pos->st_sKpxe, pos->xmKpxe);
		addxml(pos->st_sFpqshm, pos->xmFpqshm);
		addxml(pos->st_sFpzzhm, pos->xmFpzzhm);
		addxml(pos->st_sFpfs, pos->xmFpfs);
		addxml(pos->st_sSyfs, pos->xmSyfs);
		addxml(pos->st_sLgrq, pos->xmLgrq);

		num++;
		if (num % m_nPageSize == 0)
		{
			xml.OutOfElem();
			xml.OutOfElem();
			bNewPage = TRUE;
		}
	}

	if (!bNewPage)
	{
		xml.OutOfElem();
		xml.OutOfElem();
		bNewPage = TRUE;
	}

	return xml.GetDoc();
}
