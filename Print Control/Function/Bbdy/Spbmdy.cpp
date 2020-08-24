#include "../../pch.h"
#include "Spbmdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"

#define LINEFEED_P (22+4) //换行数，标识 竖向
#define LINEFEED_L (16) //换行数，标识 横向

#define MC_W	408
#define SM_W	306
#define JM_W	204
#define SL_W	204
#define GG_W	204
#define DW_W	204
#define DJ_W	204
#define HS_W	200

CSpbmdy::CSpbmdy() :m_nPageSize(LINEFEED_P)
{
}

CSpbmdy::~CSpbmdy()
{
}

CString CSpbmdy::Dlfpdy(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	SPBM_BBXX bmxx;
	CString printXml("");

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
		fpdy.sErrorCode.Format("%d", -3);
		fpdy.sErrorInfo = "输入XML格式中yylxdm不正确";
		return GenerateXMLFpdy(fpdy);
	}
	fpdy.iYylxdm = atoi(xml.GetAttrib("yylxdm"));
	xml.IntoElem();
	if (xml.FindElem("returncode"))		sCode = xml.GetData();
	if (xml.FindElem("returnmsg"))		sMsg = xml.GetData();
	if (sCode.Compare("0") != 0)
	{
		fpdy.sErrorCode = sCode;
		fpdy.sErrorInfo = sMsg;
		return GenerateXMLFpdy(fpdy);
	}

	bmxx = ParseFpmxFromXML(sInputInfo, fpdy);

	printXml = GenerateFpdyXml(bmxx, fpdy.sDylx, fpdy);


	rtn = PrintQD(printXml, fpdy.sFplxdm);

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CSpbmdy::PrintQD(LPCSTR billxml, CString strFplxdm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	int nXoff = 0;
	int nYoff = 0;

	// 添加读取配置文件功能
	CString _sTop = "";
	CString _sLeft = "";
	CString _sQRSize = "";
	CString _sItem = strFplxdm + "_QD";
	ZLib_GetIniYbjValue(_sItem, _sTop, _sLeft, _sQRSize);
	nXoff = atoi(_sLeft) * 10;
	nYoff = atoi(_sTop) * 10;

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

			CString strTemp;
			CFont *pOldFont;
			CFont fontHeader;

			CRect PaintRect;
			PaintRect.left = 0 + nXoff;
			PaintRect.top = 0 - nYoff;
			PaintRect.right = 2010 + nXoff;
			PaintRect.bottom = -1830 - 900 - nYoff;

			RECT rect;
			rect.left = PaintRect.left;
			rect.top = PaintRect.top;
			rect.right = PaintRect.right;
			rect.bottom = PaintRect.top - 100;

			PaintTile(150, "FixedSys", rect, "商品编码");

			fontHeader.CreatePointFont(120, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			TextOut(m_hPrinterDC, PaintRect.left + 40, PaintRect.top - 100, CString("制表日期："), strlen("制表日期：") + 1);

			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 600, PaintRect.top - 100, CString("共"), 2);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 680, PaintRect.top - 100, CString("页"), 2);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 740, PaintRect.top - 100, CString("第"), 2);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 830, PaintRect.top - 100, CString("页"), 2);
			char buffer[5] = { 0 };
			itoa(npn, buffer, 10);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 780, PaintRect.top - 100, buffer, strlen(buffer));
			itoa(m_nAllPageNum, buffer, 10);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 640, PaintRect.top - 100, buffer, strlen(buffer));

			//Rectangle 画矩形框
			Rectangle(m_hPrinterDC, PaintRect.left + 38, PaintRect.top - 165, PaintRect.right - 38, PaintRect.bottom);// 底部留了150
			MoveToEx(m_hPrinterDC, PaintRect.left + 38, PaintRect.top - 235, NULL);
			LineTo(m_hPrinterDC, PaintRect.right - 38, PaintRect.top - 235);	// 框顶 --> 标题底 = 70
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject(); // 框体打印完成

			fontHeader.CreatePointFont(95, "FixedSys", CDC::FromHandle(m_hPrinterDC));// 话竖线
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			int nShiting = 38;

			int fontSize = 95;
			LPCSTR fontType = FH;
			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + MC_W; // 1
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "名称");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + MC_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + MC_W, PaintRect.bottom);
			nShiting += MC_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + SM_W; // 4
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "税收编码");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + SM_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + SM_W, PaintRect.bottom);
			nShiting += SM_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + JM_W; // 1.5
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "简码");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + JM_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + JM_W, PaintRect.bottom);
			nShiting += JM_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + SL_W;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "税率");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + SL_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + SL_W, PaintRect.bottom);
			nShiting += SL_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + GG_W;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "规格型号");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + GG_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + GG_W, PaintRect.bottom);
			nShiting += GG_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + DW_W;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "单位");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + DW_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + DW_W, PaintRect.bottom);
			nShiting += DW_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + DJ_W;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "单价");
			MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + DJ_W, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + nShiting + DJ_W, PaintRect.bottom);
			nShiting += DJ_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + HS_W;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "含税标志");

			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

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

				itemRect.left = x + nXoff + 38;
				itemRect.top = (-y - 165 - nYoff);
				itemRect.right = x + nXoff + 38 + w;
				itemRect.bottom = (-y - 165 - h - nYoff);

				PaintTile(nFontSize, strFontName, itemRect, strText);
				MoveToEx(m_hPrinterDC, itemRect.left, itemRect.bottom, NULL);
				LineTo(m_hPrinterDC, itemRect.right, itemRect.bottom);
			}

			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

SPBM_BBXX CSpbmdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	SPBM_BBXX bbxx;
	bbxx.clear();
	SPBM_BBXM bbxm;
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

	if (xml.FindElem("bbxm"))
	{
		bbxx.st_nBbxmCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < bbxx.st_nBbxmCount; i++)
		{
			xml.FindElem("group");
			bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("dj")) bbxm.st_sDj = xml.GetData();
			if (xml.FindElem("ggxh"))   bbxm.st_sGgxh = xml.GetData();
			if (xml.FindElem("hsbz"))   bbxm.st_sHsbz = xml.GetData();
			if (xml.FindElem("jldw"))   bbxm.st_sJldw = xml.GetData();
			if (xml.FindElem("jm"))     bbxm.st_sJm = xml.GetData();
			if (xml.FindElem("mc"))   bbxm.st_sMc = xml.GetData();
			if (xml.FindElem("sl"))     bbxm.st_sSl = xml.GetData();
			if (xml.FindElem("spflbm"))     bbxm.st_sSpflbm = xml.GetData();
			xml.OutOfElem();

			bbxx.st_lSpbmBbxm.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CSpbmdy::GenerateFpdyXml(SPBM_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CSpbmdy::GenerateItemMXXml(SPBM_BBXX bbxx)
{
	CMarkup xml;
	int i = 0;
	int k = 0;

	int nTmp = 900;
	int nTmpKprq = 35;

	//if (m_nOrientation == DMORIENT_LANDSCAPE)
	//{
	//	m_nPageSize = LINEFEED_L;
	//	nTmp = 195;
	//	nTmpKprq = 20;
	//}

	int nLY = 70;

	LTSPBM_BBXM::iterator pos;
	for (pos = bbxx.st_lSpbmBbxm.begin(); pos != bbxx.st_lSpbmBbxm.end(); pos++)
	{
		int nShiting = 0;

		xywhsf(pos->xmMc, nShiting, nLY + i * nLY, MC_W, 70, LS_9, FS, ZC);
		nShiting += MC_W;

		xywhsf(pos->xmSpflbm, nShiting, nLY + i * nLY, SM_W, 70, LS_9, FS, ZC);
		nShiting += SM_W;

		xywhsf(pos->xmJm, nShiting, nLY + i * nLY, JM_W, 70, LS_9, FS, ZC);
		nShiting += JM_W;

		xywhsf(pos->xmSl, nShiting, nLY + i * nLY, SL_W, 70, LS_9, FS, ZC);
		nShiting += SL_W;

		xywhsf(pos->xmGgxh, nShiting, nLY + i * nLY, GG_W, 70, LS_9, FS, ZC);
		nShiting += GG_W;

		xywhsf(pos->xmJldw, nShiting, nLY + i * nLY, DW_W, 70, LS_9, FS, ZC);
		nShiting += DW_W;

		xywhsf(pos->xmDj, nShiting, nLY + i * nLY, DJ_W, 70, LS_9, FS, ZC);
		nShiting += DJ_W;

		xywhsf(pos->xmHsbz, nShiting, nLY + i * nLY, HS_W, 70, LS_9, FS, ZC);
		nShiting += HS_W;

		i++;
		if (i%m_nPageSize == 0)
		{
			i = 0;
		}
		m_nLineNum++;
	}

	m_nAllPageNum = m_nLineNum / m_nPageSize;
	if (0 != m_nLineNum % m_nPageSize)
	{
		m_nAllPageNum++;
	}

	int j = 0;
	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;
	for (pos = bbxx.st_lSpbmBbxm.begin(); pos != bbxx.st_lSpbmBbxm.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum++);
			xml.IntoElem();
			bNewPage = FALSE;
		}
		addxml(pos->st_sDj, pos->xmDj);
		addxml(pos->st_sGgxh, pos->xmGgxh);
		addxml(pos->st_sHsbz, pos->xmHsbz);
		addxml(pos->st_sJldw, pos->xmJldw);
		addxml(pos->st_sJm, pos->xmJm);
		addxml(pos->st_sMc, pos->xmMc);
		addxml(pos->st_sSl, pos->xmSl);
		addxml(pos->st_sSpflbm, pos->xmSpflbm);

		num++;
		if (num % m_nPageSize == 0)
		{
			// 够一页，增加页码
			xml.OutOfElem();
			bNewPage = TRUE;
		}
		j++;
	}

	// 最后一页增加 页码
	xml.OutOfElem();
	bNewPage = TRUE;

	return xml.GetDoc();
}
