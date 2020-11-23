#include "../../pch.h"
#include "Khbmdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define LINEFEED_P (22+4) //换行数，标识 竖向
#define LINEFEED_L (16) //换行数，标识 横向

#define XU_W	100
#define MC_W	350
#define JM_W	250
#define SH_W	300
#define ZJ_W	250
#define DZ_W	250
#define YH_W	225
#define YJ_W	225

CKhbmdy::CKhbmdy()
{
}

CKhbmdy::~CKhbmdy()
{
}

CString CKhbmdy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	KHBM_BBXX bbxx;
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

KHBM_BBXX CKhbmdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	KHBM_BBXX bbxx;
	bbxx.clear();
	KHBM_BMXX bmxx;
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
	if (xml.FindElem("zbrq")) bbxx.st_sSm = xml.GetData();
	if (xml.FindElem("t1")) bbxx.st_sT1 = xml.GetData();
	if (xml.FindElem("t2")) bbxx.st_sT2 = xml.GetData();
	if (xml.FindElem("t3")) bbxx.st_sT3 = xml.GetData();
	if (xml.FindElem("t4")) bbxx.st_sT4 = xml.GetData();
	if (xml.FindElem("t5")) bbxx.st_sT5 = xml.GetData();
	if (xml.FindElem("t6")) bbxx.st_sT6 = xml.GetData();
	if (xml.FindElem("t7")) bbxx.st_sT7 = xml.GetData();
	if (xml.FindElem("t8")) bbxx.st_sT8 = xml.GetData();

	if (xml.FindElem("bmxxs"))
	{
		int nCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < nCount; i++)
		{
			xml.FindElem("bmxx");
			bmxx.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("mc")) bmxx.st_sMc = xml.GetData();
			if (xml.FindElem("jm"))   bmxx.st_sJm = xml.GetData();
			if (xml.FindElem("sh"))   bmxx.st_sNsrsbh = xml.GetData();
			if (xml.FindElem("sfzh"))   bmxx.st_sKzl = xml.GetData();
			if (xml.FindElem("dzdh"))     bmxx.st_sDz = xml.GetData();
			if (xml.FindElem("khyh"))   bmxx.st_sYhzh = xml.GetData();
			if (xml.FindElem("yjdz"))     bmxx.st_sYjdz = xml.GetData();
			xml.OutOfElem();
			
			bbxx.st_lKhbmBmxx.push_back(bmxx);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CKhbmdy::GenerateFpdyXml(KHBM_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CKhbmdy::GenerateItemMXXml(KHBM_BBXX bbxx)
{
	CMarkup xml;

	int x0 = 0;
	int x1 = x0 + XU_W;
	int x2 = x1 + MC_W;
	int x3 = x2 + JM_W;
	int x4 = x3 + SH_W;
	int x5 = x4 + ZJ_W;
	int x6 = x5 + DZ_W;
	int x7 = x6 + YH_W;
	int x8 = x7 + YJ_W;

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
	xywhsf(bbxx.xmT1, x0, y, XU_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT2, x1, y, MC_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT3, x2, y, JM_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT4, x3, y, SH_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT5, x4, y, ZJ_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT6, x5, y, DZ_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT7, x6, y, YH_W, nW, LS_9, FS, AM_ZC | LINE_STATE_LTB);
	xywhsf(bbxx.xmT8, x7, y, YJ_W, nW, LS_9, FS, AM_ZC | LINE_STATE_0);
	y += nW;

	int nLY = 90;// 数据行高度

	m_nLineNum = 0;
	m_nPageSize = 27;
	int _y = y;
	LTKHBM_BMXX::iterator pos;
	for (pos = bbxx.st_lKhbmBmxx.begin(); pos != bbxx.st_lKhbmBmxx.end(); pos++)
	{
		xywhsf(pos->xmXh, x0, _y, XU_W, nLY, LS_9, FS, AM_ZC | LINE_STATE_LB);
		xywhsf(pos->xmMc, x1, _y, MC_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmJm, x2, _y, JM_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmNsrsbh, x3, _y, SH_W, nLY, LS_9, FS, AM_ZC_S | LINE_STATE_LB);
		xywhsf(pos->xmKzl, x4, _y, ZJ_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmDz, x5, _y, DZ_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmYhzh, x6, _y, YH_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LB);
		xywhsf(pos->xmYjdz, x7, _y, YJ_W, nLY, LS_9, FS, AM_ZC_CHEKC | LINE_STATE_LBR);
		_y += nLY;

		m_nLineNum++;
		if (m_nLineNum%m_nPageSize == 0)
		{
			_y = y;
		}
		
	}

	m_nAllPageNum = m_nLineNum / m_nPageSize;
	if (0 != m_nLineNum % m_nPageSize || m_nAllPageNum == 0)
	{
		m_nAllPageNum++;
	}

	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;

	LTKHBM_BMXX::iterator posb = bbxx.st_lKhbmBmxx.begin();
	LTKHBM_BMXX::iterator pose = bbxx.st_lKhbmBmxx.end();

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
			addxml(bbxx.st_sT7, bbxx.xmT7);
			addxml(bbxx.st_sT8, bbxx.xmT8);
			bNewPage = FALSE;
		}

		for (; posb != pose; )
		{
			addxml(posb->st_sMc, posb->xmMc);
			addxml(posb->st_sJm, posb->xmJm);
			addxml(posb->st_sNsrsbh, posb->xmNsrsbh);
			addxml(posb->st_sKzl, posb->xmKzl);
			addxml(posb->st_sDz, posb->xmDz);
			addxml(posb->st_sYhzh, posb->xmYhzh);
			addxml(posb->st_sYjdz, posb->xmYjdz);
			addxml(posb->st_nXh, posb->xmXh);
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

	//for (pos = bbxx.st_lKhbmBmxx.begin(); pos != bbxx.st_lKhbmBmxx.end(); pos++)
	//{
	//	if (bNewPage)
	//	{
	//		xml.AddElem("NewPage");
	//		xml.AddAttrib("pn", nNewPageNum);
	//		xml.IntoElem();
	//		xml.AddElem("PageHeader");
	//		xml.IntoElem();
	//		addxml(bbxx.st_sTitle, bbxx.xmTitle);
	//		addxml(bbxx.st_sSm, bbxx.xmSm);
	//		addxml(bbxx.st_sDi, bbxx.xmDi);
	//		addxml(nNewPageNum++, bbxx.xmP1);
	//		addxml(bbxx.st_sYe1, bbxx.xmYe1);
	//		addxml(bbxx.st_sGong, bbxx.xmGong);
	//		addxml(m_nAllPageNum, bbxx.xmP2);
	//		addxml(bbxx.st_sYe2, bbxx.xmYe2);
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
	//	addxml(pos->st_sMc, pos->xmMc);
	//	addxml(pos->st_sJm, pos->xmJm);
	//	addxml(pos->st_sNsrsbh, pos->xmNsrsbh);
	//	addxml(pos->st_sKzl, pos->xmKzl);
	//	addxml(pos->st_sDz, pos->xmDz);
	//	addxml(pos->st_sYhzh, pos->xmYhzh);
	//	addxml(pos->st_sYjdz, pos->xmYjdz);
	//	addxml(pos->st_nXh, pos->xmXh);

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

LONG CKhbmdy::PrintQD(LPCSTR billxml)
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
