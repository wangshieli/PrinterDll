#include "../../pch.h"
#include "Fpzlhz.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define XH_W	100
#define MC_W	235
#define XM_1	225
#define XM_2	225
#define XM_3	225
#define XM_4	225
#define XM_5	225
#define XM_6	225
#define XM_7	225

CFpzlhz::CFpzlhz()
{
}

CFpzlhz::~CFpzlhz()
{
}

CString CFpzlhz::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	FPZLHZ_BBXX bbxx;
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

LONG CFpzlhz::PrintQD(LPCSTR billxml)
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

			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
	} while (--m_nCopies);

	return nrt;
}

FPZLHZ_BBXX CFpzlhz::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	FPZLHZ_BBXX bbxx;
	bbxx.clear();
	FPZLHZ_TJXX bbxm;
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
	if (xml.FindElem("ssqj")) bbxx.st_sSsrq = xml.GetData();
	if (xml.FindElem("sm")) bbxx.st_sSm = xml.GetData();
	if (xml.FindElem("nsrsbh")) bbxx.st_sNsrsbh = xml.GetData();
	if (xml.FindElem("qymc")) bbxx.st_sQymc = xml.GetData();
	if (xml.FindElem("dzdh")) bbxx.st_sDzdh = xml.GetData();
	bbxx.st_sTitle1 = "★发票领用存情况★";
	if (xml.FindElem("cqkc")) bbxx.st_sCqkc = xml.GetData();
	if (xml.FindElem("zsfp")) bbxx.st_sZsfp = xml.GetData();
	if (xml.FindElem("fsfp")) bbxx.st_sFsfp = xml.GetData();
	if (xml.FindElem("gjfp")) bbxx.st_sGjfp = xml.GetData();
	if (xml.FindElem("zffp")) bbxx.st_sZffp = xml.GetData();
	if (xml.FindElem("fffp")) bbxx.st_sFffp = xml.GetData();
	if (xml.FindElem("thfp")) bbxx.st_sThfp = xml.GetData();
	if (xml.FindElem("qmkc")) bbxx.st_sQmkc = xml.GetData();
	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	bbxx.st_sXh = "序号";
	bbxx.st_sMc = "项目名称";
	bbxx.st_sHj = "合计";
	bbxx.st_sQt = "其他";

	bbxx.st_sTitle2 = "★销货情况★";
	bbxx.st_sJedw = "金额单位: 元";

	if (xml.FindElem("tjxx"))
	{
		int nCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < nCount; i++)
		{
			xml.FindElem("group");
			bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("xmmc")) bbxm.st_sMc = xml.GetData();
			if (xml.FindElem("hj"))   bbxm.st_sHj = xml.GetData();
			if (xml.FindElem("sl1"))  bbxm.st_s1 = xml.GetData();
			if (xml.FindElem("sl2"))  bbxm.st_s2 = xml.GetData();
			if (xml.FindElem("sl3"))  bbxm.st_s3 = xml.GetData();
			if (xml.FindElem("sl4"))  bbxm.st_s4 = xml.GetData();
			if (xml.FindElem("sl5"))  bbxm.st_s5 = xml.GetData();
			if (xml.FindElem("qt"))   bbxm.st_sQt = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lFpzlhzTjxx.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CFpzlhz::GenerateFpdyXml(FPZLHZ_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CFpzlhz::GenerateItemMXXml(FPZLHZ_BBXX bbxx)
{
	CMarkup xml;
	int i = 0;

	xywhsf(bbxx.xmTitle, 0, 0, 1990, 100, LS_16, FS, AM_ZC);
	xywhsf(bbxx.xmZbrq, 0, 100, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmSsrq, 0, 150, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmSm, 0, 200, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmNsrsbh, 0, 250, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmQymc, 0, 300, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmDzdh, 0, 350, 1990, 50, LS_10, FS, AM_VCL);

	xywhsf(bbxx.xmTitle1, 0, 440, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmCqkc, 0, 490, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmZsfp, 400, 490, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmFsfp, 800, 490, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmGjfp, 0, 540, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmZffp, 400, 540, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmFffp, 800, 540, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmThfp, 0, 590, 400, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmQmkc, 800, 590, 400, 50, LS_10, FS, AM_VCL);

	xywhsf(bbxx.xmTitle2, 0, 670, 1990, 50, LS_10, FS, AM_VCL);
	xywhsf(bbxx.xmJedw, 0, 720, 1990, 50, LS_10, FS, AM_VCL);

	int ny = 770;
	int nLY = 70;

	int nShiting = 0;
	xywhsf(bbxx.xmXh, nShiting, ny, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XH_W;
	xywhsf(bbxx.xmMc, nShiting, ny, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += MC_W;
	xywhsf(bbxx.xmHj, nShiting, ny, XM_1, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XM_1;
	xywhsf(bbxx.xmT1, nShiting, ny, XM_2, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XM_2;
	xywhsf(bbxx.xmT2, nShiting, ny, XM_3, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XM_3;
	xywhsf(bbxx.xmT3, nShiting, ny, XM_4, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XM_4;
	xywhsf(bbxx.xmT4, nShiting, ny, XM_5, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XM_5;
	xywhsf(bbxx.xmT5, nShiting, ny, XM_6, nLY, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	nShiting += XM_6;
	xywhsf(bbxx.xmQt, nShiting, ny, XM_7, nLY, LS_9, FS, AM_ZC | LINE_STATE_0);
	nShiting += XM_7;

	ny += 70;

	LTFPZLHZ_TJXX::iterator pos;
	for (pos = bbxx.st_lFpzlhzTjxx.begin(); pos != bbxx.st_lFpzlhzTjxx.end(); pos++)
	{
		int nShiting = 0;

		xywhsf(pos->xmXh, nShiting, i * nLY + ny, XH_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		nShiting += XH_W;

		xywhsf(pos->xmMc, nShiting, i * nLY + ny, MC_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		nShiting += MC_W;

		xywhsf(pos->xmHj, nShiting, i * nLY + ny, XM_1, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		nShiting += XM_1;

		xywhsf(pos->xmS1, nShiting, i * nLY + ny, XM_2, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		nShiting += XM_2;

		xywhsf(pos->xmS2, nShiting, i * nLY + ny, XM_3, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		nShiting += XM_3;

		xywhsf(pos->xmS3, nShiting, i * nLY + ny, XM_4, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		nShiting += XM_4;

		xywhsf(pos->xmS4, nShiting, i * nLY + ny, XM_5, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		nShiting += XM_5;

		xywhsf(pos->xmS5, nShiting, i * nLY + ny, XM_6, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		nShiting += XM_6;

		xywhsf(pos->xmQt, nShiting, i * nLY + ny, XM_7, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LBR);
		nShiting += XM_7;
		i++;
	}

	xml.AddElem("NewPage");
	xml.AddAttrib("pn", 1);
	xml.IntoElem();

	xml.AddElem("PageHeader");
	xml.IntoElem();
	addxml(bbxx.st_sTitle, bbxx.xmTitle);
	addxml(bbxx.st_sZbrq, bbxx.xmZbrq);
	addxml(bbxx.st_sSsrq, bbxx.xmSsrq);
	addxml(bbxx.st_sSm, bbxx.xmSm);
	addxml(bbxx.st_sNsrsbh, bbxx.xmNsrsbh);
	addxml(bbxx.st_sQymc, bbxx.xmQymc);
	addxml(bbxx.st_sDzdh, bbxx.xmDzdh);

	addxml(bbxx.st_sTitle1, bbxx.xmTitle1);
	addxml(bbxx.st_sCqkc, bbxx.xmCqkc);
	addxml(bbxx.st_sZsfp, bbxx.xmZsfp);
	addxml(bbxx.st_sFsfp, bbxx.xmFsfp);
	addxml(bbxx.st_sGjfp, bbxx.xmGjfp);
	addxml(bbxx.st_sZffp, bbxx.xmZffp);
	addxml(bbxx.st_sFffp, bbxx.xmFffp);
	addxml(bbxx.st_sThfp, bbxx.xmThfp);
	addxml(bbxx.st_sQmkc, bbxx.xmQmkc);

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
	addxml(bbxx.st_sT5, bbxx.xmT5);
	addxml(bbxx.st_sQt, bbxx.xmQt);

	for (pos = bbxx.st_lFpzlhzTjxx.begin(); pos != bbxx.st_lFpzlhzTjxx.end(); pos++)
	{
		addxml(pos->st_nXh, pos->xmXh);
		addxml(pos->st_sMc, pos->xmMc);
		addxml(pos->st_sHj, pos->xmHj);
		addxml(pos->st_s1, pos->xmS1);
		addxml(pos->st_s2, pos->xmS2);
		addxml(pos->st_s3, pos->xmS3);
		addxml(pos->st_s4, pos->xmS4);
		addxml(pos->st_s5, pos->xmS5);
		addxml(pos->st_sQt, pos->xmQt);
	}

	// 最后一页增加 页码
	xml.OutOfElem();
	xml.OutOfElem();

	return xml.GetDoc();
}
