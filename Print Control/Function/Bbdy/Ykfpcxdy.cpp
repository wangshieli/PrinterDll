#include "../../pch.h"
#include "Ykfpcxdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"

#define FPLX_W	300
#define FPZT_W	160
#define FPDM_W	210
#define FPHM_W	210
#define SCZT_W	200
#define KHMC_W	790
#define ZYSPMC_W	790
#define SE_W	290
#define HJJE_W	290
#define JSHJ_W	290
#define YFPDM_W	290
#define YFPHM_W	290
#define TZDBH_W	400
#define KPR_W	270
#define KPRQ_W	270
#define ZFR_W	270
#define ZFRQ_W	270

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
	if (xml.FindElem("dyfs"))   bbdy.sDyfs = xml.GetData();
	if (xml.FindElem("printername"))	m_sPrinterName = xml.GetData();
	m_iPldy = atoi(bbdy.sDyfs.GetBuffer(0));

	m_sTempFplxdm = bbdy.sFplxdm;

	bbxx = ParseFpmxFromXML(sInputInfo, bbdy);

	printXml = GenerateFpdyXml(bbxx, bbdy.sDylx, bbdy);

	if (bbdy.sDyfs.Compare("100") == 0)
		return printXml;

	rtn = PrintQD(printXml, bbdy.sFplxdm);

	return GenerateXMLFpdy(bbdy, rtn);
}

LONG CYkfpcxdy::PrintQD(LPCSTR billxml, CString bblx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	int nXoff = 0;
	int nYoff = 0;

	// 添加读取配置文件功能
	CString _sTop = "";
	CString _sLeft = "";
	CString _sQRSize = "";
	CString _sItem = bblx;
	ZLib_GetIniYbjValue(_sItem, _sTop, _sLeft, _sQRSize);
	nXoff = atoi(_sLeft);
	nYoff = atoi(_sTop);

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
					int z = atoi(xml.GetAttrib("z"));
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					Rectangle(m_hPrinterDC, itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);

					PaintTile(nFontSize, strFontName, itemRect, strText, z);
					MoveToEx(m_hPrinterDC, itemRect.left, itemRect.bottom, NULL);
					LineTo(m_hPrinterDC, itemRect.right, itemRect.bottom);
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
	YKFPCX_FPXX bbxm;
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
	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	if (xml.FindElem("t6")) bbxx.st_sT6 = xml.GetData();
	if (xml.FindElem("t7")) bbxx.st_sT7 = xml.GetData();
	if (xml.FindElem("t8")) bbxx.st_sT8 = xml.GetData();
	if (xml.FindElem("t9")) bbxx.st_sT9 = xml.GetData();
	if (xml.FindElem("t10")) bbxx.st_sT10 = xml.GetData();
	if (xml.FindElem("t11")) bbxx.st_sT11 = xml.GetData();
	if (xml.FindElem("t12")) bbxx.st_sT12 = xml.GetData();
	if (xml.FindElem("t13")) bbxx.st_sT13 = xml.GetData();
	if (xml.FindElem("t14")) bbxx.st_sT14 = xml.GetData();
	if (xml.FindElem("t15")) bbxx.st_sT15 = xml.GetData();
	if (xml.FindElem("t16")) bbxx.st_sT16 = xml.GetData();
	if (xml.FindElem("t17")) bbxx.st_sT17 = xml.GetData();

	if (xml.FindElem("fpxxs"))
	{
		int fpxxCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxxCount; i++)
		{
			xml.FindElem("fpxx");
			xml.IntoElem();
			if (xml.FindElem("fplx")) bbxm.st_sFplx = xml.GetData();
			if (xml.FindElem("fpzt"))   bbxm.st_sFpzt = xml.GetData();
			if (xml.FindElem("fpdm"))  bbxm.st_sFpdm = xml.GetData();
			if (xml.FindElem("fphm"))  bbxm.st_sFphm = xml.GetData();
			if (xml.FindElem("sczt"))  bbxm.st_sSczt = xml.GetData();
			if (xml.FindElem("khmc"))  bbxm.st_sKhmc = xml.GetData();
			if (xml.FindElem("zyspmc"))  bbxm.st_sZyspmc = xml.GetData();
			if (xml.FindElem("se"))   bbxm.st_sSe = xml.GetData();
			if (xml.FindElem("hjje"))   bbxm.st_sHjje = xml.GetData();
			if (xml.FindElem("jshj"))   bbxm.st_sJshj = xml.GetData();
			if (xml.FindElem("yfpdm"))   bbxm.st_sYfpdm = xml.GetData();
			if (xml.FindElem("yfphm"))   bbxm.st_sYfphm = xml.GetData();
			if (xml.FindElem("tzdbh"))   bbxm.st_sTzdbh = xml.GetData();
			if (xml.FindElem("kpr"))   bbxm.st_sKpr = xml.GetData();
			if (xml.FindElem("kprq"))   bbxm.st_sKprq = xml.GetData();
			if (xml.FindElem("zfr"))   bbxm.st_sZfr = xml.GetData();
			if (xml.FindElem("zfrq"))   bbxm.st_sZfrq = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lYkfpcx_fpxx.push_back(bbxm);
		}
		xml.OutOfElem();
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
	//m_nPageSize = LINEFEED_L;
	m_nPageSize = 19;

	// 处理第一部分
	int x0 = 0;
	int x1 = x0 + FPLX_W;
	int x2 = x1 + FPZT_W;
	int x3 = x2 + FPDM_W;
	int x4 = x3 + FPHM_W;
	int x5 = x4 + SCZT_W;
	int x6 = x5 + KHMC_W;

	int y = 0;
	xywhsf(bbxx.xmTitle, x0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;
	xywhsf(bbxx.xmSm, x0, y, 500, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDi, 1350, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP1, 1430, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe1, 1520, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmGong, 1600, y, 80, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmP2, 1680, y, 90, 50, LS_10, FS, AM_ZC);
	xywhsf(bbxx.xmYe2, 1770, y, 80, 50, LS_10, FS, AM_ZC);
	y += 50;

	int nW = 70; // 标题项高度
	xywhsf(bbxx.xmT1, x0, y, FPLX_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT2, x1, y, FPZT_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT3, x2, y, FPDM_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT4, x3, y, FPHM_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT5, x4, y, SCZT_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT6, x5, y, KHMC_W, nW, LS_9, FS, AM_ZC);
	y += nW;

	int nLY = 140; // 数据行高度
	m_nLineNum = 0;
	LTYKFPCX_FPXX::iterator pos;
	int _y = y;
	for (pos = bbxx.st_lYkfpcx_fpxx.begin(); pos != bbxx.st_lYkfpcx_fpxx.end(); pos++)
	{
		xywhsf(pos->xmFplx, x0, _y, FPLX_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmFpzt, x1, _y, FPZT_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmFpdm, x2, _y, FPDM_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmFphm, x3, _y, FPHM_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmSczt, x4, _y, SCZT_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmKhmc, x5, _y, KHMC_W, nLY, LS_9, FS, AM_ZC_CHEKC);
		_y += nLY;

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			_y = y;
		}
	}

	m_nAllPageNum = m_nLineNum / m_nPageSize;
	if (0 != m_nLineNum % m_nPageSize)
	{
		m_nAllPageNum++;
	}

	m_nAllPageNum *= 3;

	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;
	for (pos = bbxx.st_lYkfpcx_fpxx.begin(); pos != bbxx.st_lYkfpcx_fpxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sSm, bbxx.xmSm);			
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
			bNewPage = FALSE;
		}
		addxml(pos->st_sFplx, pos->xmFplx);
		addxml(pos->st_sFpzt, pos->xmFpzt);
		addxml(pos->st_sFpdm, pos->xmFpdm);
		addxml(pos->st_sFphm, pos->xmFphm);
		addxml(pos->st_sSczt, pos->xmSczt);
		addxml(pos->st_sKhmc, pos->xmKhmc);

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

	// 处理第二部分

	x0 = 0;
	x1 = x0 + ZYSPMC_W;
	x2 = x1 + SE_W;
	x3 = x2 + HJJE_W;
	x4 = x3 + JSHJ_W;
	x5 = x4 + YFPDM_W;

	y -= nW;
	xywhsf(bbxx.xmT7, x0, y, ZYSPMC_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT8, x1, y, SE_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT9, x2, y, HJJE_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT10, x3, y, JSHJ_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT11, x4, y, YFPDM_W, nW, LS_9, FS, AM_ZC);
	y += nW;

	m_nLineNum = 0;
	_y = y;
	for (pos = bbxx.st_lYkfpcx_fpxx.begin(); pos != bbxx.st_lYkfpcx_fpxx.end(); pos++)
	{
		xywhsf(pos->xmZyspmc, x0, _y, ZYSPMC_W, nLY, LS_9, FS, AM_ZC_CHEKC);
		xywhsf(pos->xmSe, x1, _y, SE_W, nLY, LS_9, FS, AM_ZC_S);
		xywhsf(pos->xmHjje, x2, _y, HJJE_W, nLY, LS_9, FS, AM_ZC_S);
		xywhsf(pos->xmJshj, x3, _y, JSHJ_W, nLY, LS_9, FS, AM_ZC_S);
		xywhsf(pos->xmYfpdm, x4, _y, YFPDM_W, nLY, LS_9, FS, AM_ZC);
		_y += nLY;

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			_y = y;
		}
	}

	num = 0;
	bNewPage = TRUE;
	for (pos = bbxx.st_lYkfpcx_fpxx.begin(); pos != bbxx.st_lYkfpcx_fpxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sSm, bbxx.xmSm);
			addxml(bbxx.st_sDi, bbxx.xmDi);
			addxml(nNewPageNum++, bbxx.xmP1);
			addxml(bbxx.st_sYe1, bbxx.xmYe1);
			addxml(bbxx.st_sGong, bbxx.xmGong);
			addxml(m_nAllPageNum, bbxx.xmP2);
			addxml(bbxx.st_sYe2, bbxx.xmYe2);
			xml.OutOfElem();
			xml.AddElem("PageData");
			xml.IntoElem();
			addxml(bbxx.st_sT7, bbxx.xmT7);
			addxml(bbxx.st_sT8, bbxx.xmT8);
			addxml(bbxx.st_sT9, bbxx.xmT9);
			addxml(bbxx.st_sT10, bbxx.xmT10);
			addxml(bbxx.st_sT11, bbxx.xmT11);			
			bNewPage = FALSE;
		}
		addxml(pos->st_sZyspmc, pos->xmZyspmc);
		addxml(pos->st_sSe, pos->xmSe);
		addxml(pos->st_sHjje, pos->xmHjje);
		addxml(pos->st_sJshj, pos->xmJshj);
		addxml(pos->st_sYfpdm, pos->xmYfpdm);

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

	// 处理第三部分

	x0 = 0;
	x1 = x0 + YFPHM_W;
	x2 = x1 + TZDBH_W;
	x3 = x2 + KPR_W;
	x4 = x3 + KPRQ_W;
	x5 = x4 + ZFR_W;
	x6 = x5 + ZFRQ_W;

	y -= nW;
	xywhsf(bbxx.xmT12, x0, y, YFPHM_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT13, x1, y, TZDBH_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT14, x2, y, KPR_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT15, x3, y, KPRQ_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT16, x4, y, ZFR_W, nW, LS_9, FS, AM_ZC);
	xywhsf(bbxx.xmT17, x5, y, ZFRQ_W, nW, LS_9, FS, AM_ZC);
	y += nW;

	m_nLineNum = 0;
	_y = y;
	for (pos = bbxx.st_lYkfpcx_fpxx.begin(); pos != bbxx.st_lYkfpcx_fpxx.end(); pos++)
	{
		xywhsf(pos->xmYfphm, x0, _y, YFPHM_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmTzdbh, x1, _y, TZDBH_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmKpr, x2, _y, KPR_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmKprq, x3, _y, KPRQ_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmZfr, x4, _y, ZFR_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmZfrq, x5, _y, ZFRQ_W, nLY, LS_9, FS, AM_ZC);
		_y += nLY;

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			_y = y;
		}
	}

	num = 0;
	bNewPage = TRUE;
	for (pos = bbxx.st_lYkfpcx_fpxx.begin(); pos != bbxx.st_lYkfpcx_fpxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum);
			xml.IntoElem();
			xml.AddElem("PageHeader");
			xml.IntoElem();
			addxml(bbxx.st_sTitle, bbxx.xmTitle);
			addxml(bbxx.st_sSm, bbxx.xmSm);
			addxml(bbxx.st_sDi, bbxx.xmDi);
			addxml(nNewPageNum++, bbxx.xmP1);
			addxml(bbxx.st_sYe1, bbxx.xmYe1);
			addxml(bbxx.st_sGong, bbxx.xmGong);
			addxml(m_nAllPageNum, bbxx.xmP2);
			addxml(bbxx.st_sYe2, bbxx.xmYe2);
			xml.OutOfElem();
			xml.AddElem("PageData");
			xml.IntoElem();
			addxml(bbxx.st_sT12, bbxx.xmT12);
			addxml(bbxx.st_sT13, bbxx.xmT13);
			addxml(bbxx.st_sT14, bbxx.xmT14);
			addxml(bbxx.st_sT15, bbxx.xmT15);
			addxml(bbxx.st_sT16, bbxx.xmT16);
			addxml(bbxx.st_sT17, bbxx.xmT17);			
			bNewPage = FALSE;
		}
		addxml(pos->st_sYfphm, pos->xmYfphm);
		addxml(pos->st_sTzdbh, pos->xmTzdbh);
		addxml(pos->st_sKpr, pos->xmKpr);
		addxml(pos->st_sKprq, pos->xmKprq);
		addxml(pos->st_sZfr, pos->xmZfr);
		addxml(pos->st_sZfrq, pos->xmZfrq);

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
