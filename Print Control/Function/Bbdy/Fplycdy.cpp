#include "../../pch.h"
#include "Fplycdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define DM_W	280
#define QC_W	280
#define LG_W	280
#define KJ_W	280
#define ZF_W	280
#define WS_W	280
#define QM_W	280

#define FS_W	100
#define HM_W	180

CFplycdy::CFplycdy()
{
}

CFplycdy::~CFplycdy()
{
}

CString CFplycdy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	FPLYC_BBXX bbxx;
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

LONG CFplycdy::PrintQD(LPCSTR billxml)
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

			if  (xml.FindElem("PageHeader"))
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
	} while (false);

	return nrt;
}

FPLYC_BBXX CFplycdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	FPLYC_BBXX bbxx;
	bbxx.clear();
	FPLYC_FPXX bmxx;
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
	if (xml.FindElem("ssq")) bbxx.st_sSsq = xml.GetData();
	if (xml.FindElem("zbrq")) bbxx.st_sZbrq = xml.GetData();
	if (xml.FindElem("dw")) bbxx.st_sDw = xml.GetData();
	if (xml.FindElem("qymc")) bbxx.st_sQymc = xml.GetData();

	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	if (xml.FindElem("t6")) bbxx.st_sT6 = xml.GetData();
	if (xml.FindElem("t7")) bbxx.st_sT7 = xml.GetData();

	if (xml.FindElem("z1")) bbxx.st_sZ1 = xml.GetData();
	if (xml.FindElem("z2")) bbxx.st_sZ2 = xml.GetData();
	if (xml.FindElem("z3")) bbxx.st_sZ3 = xml.GetData();

	if (xml.FindElem("fpxxs"))
	{
		int nCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < nCount; i++)
		{
			xml.FindElem("fpxx");
			xml.IntoElem();
			if (xml.FindElem("fpzl")) bmxx.st_sFpzl = xml.GetData();
			if (xml.FindElem("fplb"))   bmxx.st_sFplb = xml.GetData();
			if (xml.FindElem("qckcfs"))   bmxx.st_sQckcfs = xml.GetData();
			if (xml.FindElem("qckcfphm"))     bmxx.st_sQckcfphm = xml.GetData();
			if (xml.FindElem("bqlgfs"))   bmxx.st_sBqlgfs = xml.GetData();
			if (xml.FindElem("bqlggphm"))   bmxx.st_sBqlgfphm = xml.GetData();
			if (xml.FindElem("bqkjfs"))     bmxx.st_sBqkjfs = xml.GetData();
			if (xml.FindElem("bqkjfphm"))   bmxx.st_sBqkjfphm = xml.GetData();
			if (xml.FindElem("bqzffs"))     bmxx.st_sBqzffs = xml.GetData();
			if (xml.FindElem("bqzffphm"))   bmxx.st_sBqzffphm = xml.GetData();
			if (xml.FindElem("wsthfs"))     bmxx.st_sWsthfs = xml.GetData();
			if (xml.FindElem("wsthfphm"))   bmxx.st_sWsthfphm = xml.GetData();
			if (xml.FindElem("qmkcfs"))     bmxx.st_sQmkcfs = xml.GetData();
			if (xml.FindElem("qmkcfphm"))   bmxx.st_sQmkcfphm = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lFplycFpxx.push_back(bmxx);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CFplycdy::GenerateFpdyXml(FPLYC_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CFplycdy::GenerateItemMXXml(FPLYC_BBXX bbxx)
{
	CMarkup xml;

	int x0 = 0;
	int x1 = x0 + DM_W;
	int x2 = x1 + QC_W;
	int x3 = x2 + LG_W;
	int x4 = x3 + KJ_W;
	int x5 = x4 + ZF_W;
	int x6 = x5 + WS_W;
	int x7 = x6 + QM_W;

	int y = 0;
	xywhsf(bbxx.xmTitle, x0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;
	xywhsf(bbxx.xmSsq, x0, y, 500, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmZbrq, x0 + 600, y, 500, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDw, x0 + 1200, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;

	xywhsf(bbxx.xmQymc, x0, y, 500, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDi, 1350, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP1, 1430, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe1, 1520, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmGong, 1600, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP2, 1680, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe2, 1770, y, 80, 50, LS_10, FS, AM_ZC);
	y += 50;

	int nW = 70; // 标题项高度
	xywhsf(bbxx.xmT1, x0, y, DM_W, nW * 3, LS_9, FS, AM_ZC | LINE_STATE_LTB);

	xywhsf(bbxx.xmT2, x1, y, QC_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT2_FS, x1, y + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT2_HM, x1 + FS_W, y + nW , HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT2_FS_1, x1, y + nW + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT2_HM_1, x1 + FS_W, y + nW + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);

	xywhsf(bbxx.xmT3, x2, y, LG_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT3_FS, x2, y + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT3_HM, x2 + FS_W, y + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT3_FS_1, x2, y + nW + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT3_HM_1, x2 + FS_W, y + nW + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);

	xywhsf(bbxx.xmT4, x3, y, KJ_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT4_FS, x3, y + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT4_HM, x3 + FS_W, y + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT4_FS_1, x3, y + nW + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT4_HM_1, x3 + FS_W, y + nW + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);

	xywhsf(bbxx.xmT5, x4, y, ZF_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT5_FS, x4, y + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT5_HM, x4 + FS_W, y + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT5_FS_1, x4, y + nW + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT5_HM_1, x4 + FS_W, y + nW + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);

	xywhsf(bbxx.xmT6, x5, y, WS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT6_FS, x5, y + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT6_HM, x5 + FS_W, y + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT6_FS_1, x5, y + nW + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT6_HM_1, x5 + FS_W, y + nW + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);

	xywhsf(bbxx.xmT7, x6, y, QM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_0);
	xywhsf(bbxx.xmT7_FS, x6, y + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT7_HM, x6 + FS_W, y + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LBR);
	xywhsf(bbxx.xmT7_FS_1, x6, y + nW + nW, FS_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LB);
	xywhsf(bbxx.xmT7_HM_1, x6 + FS_W, y + nW + nW, HM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LBR);
	y += nW;
	y += nW;
	y += nW;


	x0 = 0;
	x1 = x0 + DM_W;
	x2 = x1 + FS_W;
	x3 = x2 + HM_W;
	x4 = x3 + FS_W;
	x5 = x4 + HM_W;
	x6 = x5 + FS_W;
	x7 = x6 + HM_W;
	int x8 = x7 + FS_W;
	int x9 = x8 + HM_W;
	int x10 = x9 + FS_W;
	int x11 = x10 + HM_W;
	int x12 = x11 + FS_W;
	int x13 = x12 + HM_W;

	int nLY = 70;// 数据行高度

	m_nLineNum = 0;
	m_nPageSize = 30;
	int _y = y;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;

	m_nAllPageNum = bbxx.st_lFplycFpxx.size() * 2 / m_nPageSize;
	if (0 != bbxx.st_lFplycFpxx.size() * 2 % m_nPageSize)
	{
		m_nAllPageNum++;
	}

	int _nQckchj = 0;
	int _nBqlghj = 0;
	int _nBqkjhj = 0;
	int _nBqzfhj = 0;
	int _nWsthhj = 0;
	int _nQmkchj = 0;

	LTFPLYC_FPXX::iterator pos;
	for (pos = bbxx.st_lFplycFpxx.begin(); pos != bbxx.st_lFplycFpxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sSsq, bbxx.xmSsq);
			addxml(bbxx.st_sZbrq, bbxx.xmZbrq);
			addxml(bbxx.st_sDw, bbxx.xmDw);

			addxml(bbxx.st_sQymc, bbxx.xmQymc);
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
			addxml("份数", bbxx.xmT2_FS);
			addxml("发票号码", bbxx.xmT2_HM);
			addxml(1, bbxx.xmT2_FS_1);
			addxml(2, bbxx.xmT2_HM_1);

			addxml(bbxx.st_sT3, bbxx.xmT3);
			addxml("份数", bbxx.xmT3_FS);
			addxml("发票号码", bbxx.xmT3_HM);
			addxml(3, bbxx.xmT3_FS_1);
			addxml(4, bbxx.xmT3_HM_1);

			addxml(bbxx.st_sT4, bbxx.xmT4);
			addxml("份数", bbxx.xmT4_FS);
			addxml("发票号码", bbxx.xmT4_HM);
			addxml(5, bbxx.xmT4_FS_1);
			addxml(6, bbxx.xmT4_HM_1);

			addxml(bbxx.st_sT5, bbxx.xmT5);
			addxml("份数", bbxx.xmT5_FS);
			addxml("发票号码", bbxx.xmT5_HM);
			addxml(7, bbxx.xmT5_FS_1);
			addxml(8, bbxx.xmT5_HM_1);

			addxml(bbxx.st_sT6, bbxx.xmT6);
			addxml("份数", bbxx.xmT6_FS);
			addxml("发票号码", bbxx.xmT6_HM);
			addxml(9, bbxx.xmT6_FS_1);
			addxml(10, bbxx.xmT6_HM_1);

			addxml(bbxx.st_sT7, bbxx.xmT7);
			addxml("份数", bbxx.xmT7_FS);
			addxml("发票号码", bbxx.xmT7_HM);
			addxml(11, bbxx.xmT7_FS_1);
			addxml(12, bbxx.xmT7_HM_1);
			bNewPage = FALSE;
		}
		xywhsf(pos->xmFpzl, x0, _y, DM_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		xywhsf(pos->xmQckcfs, x1, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmQckcfphm, x2, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmBqlgfs, x3, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmBqlgfphm, x4, _y, HM_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmBqkjfs, x5, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmBqkjfphm, x6, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmBqzffs, x7, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmBqzffphm, x8, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmWsthfs, x9, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmWsthfphm, x10, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmQmkcfs, x11, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmQmkcfphm, x12, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);

		addxml(pos->st_sFpzl + "-" + pos->st_sFplb, pos->xmFpzl);
		addxml(pos->st_sQckcfs, pos->xmQckcfs);
		addxml(pos->st_sQckcfphm, pos->xmQckcfphm);
		addxml(pos->st_sBqlgfs, pos->xmBqlgfs);
		addxml(pos->st_sBqlgfphm, pos->xmBqlgfphm);
		addxml(pos->st_sBqkjfs, pos->xmBqkjfs);
		addxml(pos->st_sBqkjfphm, pos->xmBqkjfphm);
		addxml(pos->st_sBqzffs, pos->xmBqzffs);
		addxml(pos->st_sBqzffphm, pos->xmBqzffphm);
		addxml(pos->st_sWsthfs, pos->xmWsthfs);
		addxml(pos->st_sWsthfphm, pos->xmWsthfphm);
		addxml(pos->st_sQmkcfs, pos->xmQmkcfs);
		addxml(pos->st_sQmkcfphm, pos->xmQmkcfphm);
		_y += nLY;

		XM _xm;
		xywhsf(_xm, x0, _y, DM_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		addxml("小计", _xm);
		xywhsf(_xm, x1, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(pos->st_sQckcfs, _xm);
		xywhsf(_xm, x2, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x3, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(pos->st_sBqlgfs, _xm);	
		xywhsf(_xm, x4, _y, HM_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x5, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(pos->st_sBqkjfs, _xm);	
		xywhsf(_xm, x6, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x7, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(pos->st_sBqzffs, _xm);	
		xywhsf(_xm, x8, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x9, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(pos->st_sWsthfs, _xm);		
		xywhsf(_xm, x10, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x11, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(pos->st_sQmkcfs, _xm);
		xywhsf(_xm, x12, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);
		addxml("", _xm);
		_y += nLY;

		_nQckchj += atoi(pos->st_sQckcfs);
		_nBqlghj += atoi(pos->st_sBqlgfs);
		_nBqkjhj += atoi(pos->st_sBqkjfs);
		_nBqzfhj += atoi(pos->st_sBqzffs);
		_nWsthhj += atoi(pos->st_sWsthfs);
		_nQmkchj += atoi(pos->st_sQmkcfs);
		
		m_nLineNum++;
		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			XM _xm;
			xywhsf(_xm, x0, _y, DM_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
			addxml("合计", _xm);
			xywhsf(_xm, x1, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml(_nQckchj, _xm);
			xywhsf(_xm, x2, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml("", _xm);
			xywhsf(_xm, x3, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml(_nBqlghj, _xm);
			xywhsf(_xm, x4, _y, HM_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
			addxml("", _xm);
			xywhsf(_xm, x5, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml(_nBqkjhj, _xm);
			xywhsf(_xm, x6, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml("", _xm);
			xywhsf(_xm, x7, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml(_nBqzfhj, _xm);
			xywhsf(_xm, x8, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml("", _xm);
			xywhsf(_xm, x9, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml(_nWsthhj, _xm);
			xywhsf(_xm, x10, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml("", _xm);
			xywhsf(_xm, x11, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
			addxml(_nQmkchj, _xm);
			xywhsf(_xm, x12, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);
			addxml("", _xm);	
			_y += nLY;
			xml.OutOfElem();
			xml.AddElem("PageEnd");
			xml.IntoElem();
			xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
			addxml(bbxx.st_sZ1, _xm);
			_y += nLY;
			xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
			addxml(bbxx.st_sZ2, _xm);
			_y += nLY;
			xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);		
			addxml(bbxx.st_sZ3, _xm);
			_y = y;
			xml.OutOfElem();
			xml.OutOfElem();
			bNewPage = TRUE;
		}

	}

	if (!bNewPage)
	{
		XM _xm;
		xywhsf(_xm, x0, _y, DM_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		addxml("合计", _xm);
		xywhsf(_xm, x1, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(_nQckchj, _xm);
		xywhsf(_xm, x2, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x3, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(_nBqlghj, _xm);
		xywhsf(_xm, x4, _y, HM_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x5, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(_nBqkjhj, _xm);
		xywhsf(_xm, x6, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x7, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(_nBqzfhj, _xm);
		xywhsf(_xm, x8, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x9, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(_nWsthhj, _xm);
		xywhsf(_xm, x10, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml("", _xm);
		xywhsf(_xm, x11, _y, FS_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		addxml(_nQmkchj, _xm);
		xywhsf(_xm, x12, _y, HM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);
		addxml("", _xm);
		_y += nLY;
		xml.OutOfElem();
		xml.AddElem("PageEnd");
		xml.IntoElem();
		xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sZ1, _xm);
		_y += nLY;
		xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sZ2, _xm);
		_y += nLY;
		xywhsf(_xm, 0, _y, 1500, nLY, LS_9, FS, AM_VCL);
		addxml(bbxx.st_sZ3, _xm);
		xml.OutOfElem();
		xml.OutOfElem();
		bNewPage = TRUE;
	}

	return xml.GetDoc();
}
