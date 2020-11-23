#include "../../pch.h"
#include "Skzltjdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define XH_W	100
#define MC_W	350
#define XM_1	250
#define XM_2	250
#define XM_3	250
#define XM_4	250
#define XM_5	250
#define XM_6	300

CSkzltjdy::CSkzltjdy()
{
}

CSkzltjdy::~CSkzltjdy()
{
}

CString CSkzltjdy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	SKZLTJ_BBXX bbxx;
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

LONG CSkzltjdy::PrintQD(LPCSTR billxml)
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
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					if (h == 0)
					{
						CPen pen;
						pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
						CPen* pOld = (CPen*)(::SelectObject(m_hPrinterDC, pen));

						MoveToEx(m_hPrinterDC, itemRect.left, itemRect.top, NULL);
						LineTo(m_hPrinterDC, itemRect.right, itemRect.top);
						::SelectObject(m_hPrinterDC, pOld);
						pen.DeleteObject();
					}
					else
					{
						PaintTile(nFontSize, strFontName, itemRect, strText, z);
					}
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

SKZLTJ_BBXX CSkzltjdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	SKZLTJ_BBXX bbxx;
	bbxx.clear();
	SKZLTJ_TJXX bbxm;
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
	if (xml.FindElem("ssq")) bbxx.st_sSsrq = xml.GetData();
	if (xml.FindElem("sm")) bbxx.st_sSm = xml.GetData();
	if (xml.FindElem("nsrsbh")) bbxx.st_sNsrsbh = xml.GetData();
	if (xml.FindElem("qymc")) bbxx.st_sQymc = xml.GetData();
	if (xml.FindElem("dzdh")) bbxx.st_sDzdh = xml.GetData();
	
	if (xml.FindElem("title1")) bbxx.st_sTitle1 = xml.GetData();
	if (xml.FindElem("cqkc")) bbxx.st_sCqkc = xml.GetData();
	if (xml.FindElem("zsfp")) bbxx.st_sZsfp = xml.GetData();
	if (xml.FindElem("fsfp")) bbxx.st_sFsfp = xml.GetData();
	if (xml.FindElem("gjfp")) bbxx.st_sGjfp = xml.GetData();
	if (xml.FindElem("zffp")) bbxx.st_sZffp = xml.GetData();
	if (xml.FindElem("fffp")) bbxx.st_sFffp = xml.GetData();
	if (xml.FindElem("thfp")) bbxx.st_sThfp = xml.GetData();
	if (xml.FindElem("qmkc")) bbxx.st_sQmkc = xml.GetData();
	if (xml.FindElem("title2")) bbxx.st_sTitle2 = xml.GetData();
	if (xml.FindElem("dw")) bbxx.st_sJedw = xml.GetData();

	if (xml.FindElem("xh")) bbxx.st_sXh = xml.GetData();
	if (xml.FindElem("mc")) bbxx.st_sMc = xml.GetData();
	if (xml.FindElem("hj")) bbxx.st_sHj = xml.GetData();
	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("qt")) bbxx.st_sQt = xml.GetData();

	if (xml.FindElem("tjxxs"))
	{
		int nCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < nCount; i++)
		{
			xml.FindElem("tjxx");
			bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("xmmc")) bbxm.st_sMc = xml.GetData();
			if (xml.FindElem("hj"))   bbxm.st_sHj = xml.GetData();
			if (xml.FindElem("sl1"))  bbxm.st_s1 = xml.GetData();
			if (xml.FindElem("sl2"))  bbxm.st_s2 = xml.GetData();
			if (xml.FindElem("sl3"))  bbxm.st_s3 = xml.GetData();
			if (xml.FindElem("sl4"))  bbxm.st_s4 = xml.GetData();
			if (xml.FindElem("qt"))   bbxm.st_sQt = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lSkzltjTjxx.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CSkzltjdy::GenerateFpdyXml(SKZLTJ_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CSkzltjdy::GenerateItemMXXml(SKZLTJ_BBXX bbxx)
{
	CMarkup xml;

	int x0 = 0;
	int x1 = x0 + XH_W;
	int x2 = x1 + MC_W;
	int x3 = x2 + XM_1;
	int x4 = x3 + XM_2;
	int x5 = x4 + XM_3;
	int x6 = x5 + XM_4;
	int x7 = x6 + XM_5;
	int x8 = x7 + XM_6;

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

	xywhsf(bbxx.xmSsrq, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmSm, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmNsrsbh, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmQymc, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmDzdh, x0, y, 1990, 50, LS_10, FS, AM_VCL);
	y += 50;

	y += 25;
	xywhsf(bbxx.xm1, x0, y, 1800, 0, LS_16, FS, AM_ZC);
	y += 25;

	xywhsf(bbxx.xmTitle1, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;

	int x_ = 0;
	xywhsf(bbxx.xmCqkc, x_, y, 400, 50, LS_10, FS, AM_VCL);
	x_ += 400;
	xywhsf(bbxx.xmZsfp, x_, y, 400, 50, LS_10, FS, AM_VCL);
	x_ += 400;
	xywhsf(bbxx.xmFsfp, x_, y, 400, 50, LS_10, FS, AM_VCL);
	y += 50;

	x_ = 0;
	xywhsf(bbxx.xmGjfp, x_, y, 400, 50, LS_10, FS, AM_VCL);
	x_ += 400;
	xywhsf(bbxx.xmZffp, x_, y, 400, 50, LS_10, FS, AM_VCL);
	x_ += 400;
	xywhsf(bbxx.xmFffp, x_, y, 400, 50, LS_10, FS, AM_VCL);
	y += 50;

	x_ = 0;
	xywhsf(bbxx.xmThfp, x_, y, 400, 50, LS_10, FS, AM_VCL);
	x_ += 400;
	x_ += 400;
	xywhsf(bbxx.xmQmkc, x_, y, 400, 50, LS_10, FS, AM_VCL);
	y += 50;

	y += 25;
	xywhsf(bbxx.xm2, x0, y, 1800, 0, LS_16, FS, AM_ZC);
	y += 25;
	
	xywhsf(bbxx.xmTitle2, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;
	xywhsf(bbxx.xmJedw, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;

	int nW = 70; // 标题项高度
	xywhsf(bbxx.xmXh, x0, y, XH_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmMc, x1, y, MC_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmHj, x2, y, XM_1, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT1, x3, y, XM_2, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT2, x4, y, XM_3, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT3, x5, y, XM_4, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT4, x6, y, XM_5, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmQt, x7, y, XM_6, nW, LS_9, FS, AM_ZC | LINE_STATE_0);
	y += nW;

	int nLY = 90;// 数据行高度

	m_nLineNum = 0;
	m_nPageSize = 28;
	int _y = y;
	LTSKZLTJ_TJXX::iterator pos;
	for (pos = bbxx.st_lSkzltjTjxx.begin(); pos != bbxx.st_lSkzltjTjxx.end(); pos++)
	{
		xywhsf(pos->xmXh, x0, _y, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		xywhsf(pos->xmMc, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmHj, x2, _y, XM_1, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmS1, x3, _y, XM_2, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmS2, x4, _y, XM_3, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmS3, x5, _y, XM_4, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmS4, x6, _y, XM_5, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmQt, x7, _y, XM_6, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);
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

	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;
	for (pos = bbxx.st_lSkzltjTjxx.begin(); pos != bbxx.st_lSkzltjTjxx.end(); pos++)
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

			addxml(bbxx.st_sSsrq, bbxx.xmSsrq);
			addxml(bbxx.st_sSm, bbxx.xmSm);
			addxml(bbxx.st_sNsrsbh, bbxx.xmNsrsbh);
			addxml(bbxx.st_sQymc, bbxx.xmQymc);
			addxml(bbxx.st_sDzdh, bbxx.xmDzdh);

			addxml("", bbxx.xm1);

			addxml(bbxx.st_sTitle1, bbxx.xmTitle1);
			addxml(bbxx.st_sCqkc, bbxx.xmCqkc);
			addxml(bbxx.st_sZsfp, bbxx.xmZsfp);
			addxml(bbxx.st_sFsfp, bbxx.xmFsfp);
			addxml(bbxx.st_sGjfp, bbxx.xmGjfp);
			addxml(bbxx.st_sZffp, bbxx.xmZffp);
			addxml(bbxx.st_sFffp, bbxx.xmFffp);
			addxml(bbxx.st_sThfp, bbxx.xmThfp);
			addxml(bbxx.st_sQmkc, bbxx.xmQmkc);

			addxml("", bbxx.xm2);

			addxml(bbxx.st_sTitle2, bbxx.xmTitle2);
			addxml(bbxx.st_sJedw, bbxx.xmJedw);

			xml.OutOfElem();
			xml.AddElem("PageData");
			xml.IntoElem();
			addxml(bbxx.st_sXh, bbxx.xmXh);
			addxml(bbxx.st_sMc, bbxx.xmMc);
			addxml(bbxx.st_sHj, bbxx.xmHj);
			addxml(bbxx.st_sT1, bbxx.xmT1);
			addxml(bbxx.st_sT2, bbxx.xmT2);
			addxml(bbxx.st_sT3, bbxx.xmT3);
			addxml(bbxx.st_sT4, bbxx.xmT4);
			addxml(bbxx.st_sQt, bbxx.xmQt);
			bNewPage = FALSE;
		}
		addxml(pos->st_nXh, pos->xmXh);
		addxml(pos->st_sMc, pos->xmMc);
		addxml(pos->st_sHj, pos->xmHj);
		addxml(pos->st_s1, pos->xmS1);
		addxml(pos->st_s2, pos->xmS2);
		addxml(pos->st_s3, pos->xmS3);
		addxml(pos->st_s4, pos->xmS4);
		addxml(pos->st_sQt, pos->xmQt);

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
