#include "../../pch.h"
#include "Hzxxbdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"

#define LINEFEED_P (22+4) //换行数，标识 竖向
#define LINEFEED_L (16) //换行数，标识 横向

#define XGF_W	225
#define XGFMCSH_W 300
#define XGFMCSH_W1 450

#define MC_W 450
#define SL_W 175
#define DJ_W 325
#define JE_W 360
#define SLV_W 90
#define SE_W 325

CHzxxbdy::CHzxxbdy() :m_nPageSize(LINEFEED_P)
{
}

CHzxxbdy::~CHzxxbdy()
{
}

CString CHzxxbdy::Dlfpdy(LPCTSTR sInputInfo)
{
	BBDY bbdy;
	CMarkup xml;
	HZXXB_BBXX bbxx;
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

	rtn = PrintQD(printXml, bbdy.sFplxdm);

	return GenerateXMLFpdy(bbdy, rtn);
}

LONG CHzxxbdy::PrintQD(LPCSTR billxml, CString bblx)
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

					if (z == -5)
					{
						CPen pen;
						pen.CreatePen(PS_SOLID, 7, RGB(0, 0, 0));
						CPen* pOld = (CPen*)(::SelectObject(m_hPrinterDC, pen));
						/*CFont fontHeader;
						fontHeader.CreatePointFont(180, "黑体", CDC::FromHandle(m_hPrinterDC));
						CFont *pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));*/
						MoveToEx(m_hPrinterDC, itemRect.left, itemRect.top, NULL);
						LineTo(m_hPrinterDC, itemRect.right, itemRect.bottom);
						::SelectObject(m_hPrinterDC, pOld);
						pen.DeleteObject();

						//::SelectObject(m_hPrinterDC, pOldFont);
						//fontHeader.DeleteObject();
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
					int z = atoi(xml.GetAttrib("z"));
					CString strText = xml.GetData();

					itemRect.left = x + nXoff + 10;
					itemRect.top = (-y - 10 - nYoff);
					itemRect.right = x + nXoff + 10 + w;
					itemRect.bottom = (-y - 10 - h - nYoff);

					if (w != 0) // 宽度为0 ，画竖线， 高度为零， 画横线
					{
						Rectangle(m_hPrinterDC, itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
						PaintTile(nFontSize, strFontName, itemRect, strText, z);
					}
					else
					{
						MoveToEx(m_hPrinterDC, itemRect.right, itemRect.top, NULL);
						LineTo(m_hPrinterDC, itemRect.right, itemRect.bottom);
					}

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

HZXXB_BBXX CHzxxbdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	HZXXB_BBXX bbxx;
	bbxx.clear();
	HZXXB_BBXM bbxm;
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
	if (xml.FindElem("gfmc")) bbxx.st_sGfmc = xml.GetData();
	if (xml.FindElem("gfsh")) bbxx.st_sGfsh = xml.GetData();
	if (xml.FindElem("xfmc")) bbxx.st_sXfmc = xml.GetData();
	if (xml.FindElem("xfsh")) bbxx.st_sXfsh = xml.GetData();
	if (xml.FindElem("fpdm")) bbxx.st_sFpdm = xml.GetData();
	if (xml.FindElem("fphm")) bbxx.st_sFphm = xml.GetData();
	if (xml.FindElem("hjje")) bbxx.st_sHjje = xml.GetData();
	if (xml.FindElem("hjse")) bbxx.st_sHjse = xml.GetData();
	if (xml.FindElem("sqf")) bbxx.st_sSqf = xml.GetData();
	if (xml.FindElem("sqyy")) bbxx.st_sSqyy = xml.GetData();
	if (xml.FindElem("xxbbh")) bbxx.st_sXxbbh = xml.GetData();

	if (xml.FindElem("bbxms"))
	{
		int xmCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < xmCount; i++)
		{
			xml.FindElem("bbxm");
			//bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("mc")) bbxm.st_sMc = xml.GetData();
			if (xml.FindElem("sl"))   bbxm.st_sSl = xml.GetData();
			if (xml.FindElem("dj"))   bbxm.st_sDj = xml.GetData();
			if (xml.FindElem("je"))   bbxm.st_sJe = xml.GetData();
			if (xml.FindElem("slv"))     bbxm.st_sSlv = xml.GetData();
			if (xml.FindElem("se"))   bbxm.st_sSe = xml.GetData();

			bbxx.st_lHzxxbBbxm.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CHzxxbdy::GenerateFpdyXml(HZXXB_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CHzxxbdy::GenerateItemMXXml(HZXXB_BBXX bbxx)
{
	CMarkup xml;
	//m_nPageSize = LINEFEED_L;
	m_nPageSize = 19;

	xml.AddElem("NewPage");
	xml.AddAttrib("pn", 1);
	xml.IntoElem();
	xml.AddElem("PageHeader");
	xml.IntoElem();
	

	int x0 = 0;

	int y = 0;
	xywhsf(bbxx.xmTitle, x0, y, 1990, 100, LS_16, FS, AM_ZC);
	y += 100;
	xywhsf(bbxx.xmSm, x0, y, 500, 50, LS_10, FS, AM_VCL);
	y += 50;
	addxml(bbxx.st_sTitle, bbxx.xmTitle);
	addxml(bbxx.st_sSm, bbxx.xmSm);
	xml.OutOfElem();

	xml.AddElem("PageData");
	xml.IntoElem();
	XM _xm;
	int x1 = x0 + XGF_W;
	int x2 = x1 + XGFMCSH_W;
	int x3 = x2 + XGFMCSH_W1;
	int x4 = x3 + XGF_W;
	int x5 = x4 + XGFMCSH_W;
	int x6 = x5 + XGFMCSH_W1;
	xywhsf(_xm, x0, y, XGF_W, 200, LS_16, FS, AM_ZC);
	addxml("销售方", _xm);
	xywhsf(_xm, x1, y, XGFMCSH_W, 110, LS_16, FS, AM_ZC);
	addxml("名称", _xm);
	xywhsf(_xm, x1, y + 110, XGFMCSH_W, 90, LS_16, FS, AM_ZC);
	addxml("纳税人识别号", _xm);
	xywhsf(_xm, x2, y, XGFMCSH_W1, 110, LS_16, FS, AM_ZC);
	addxml(bbxx.st_sXfmc, _xm);
	xywhsf(_xm, x2, y + 110, XGFMCSH_W1, 90, LS_16, FS, AM_ZC);
	addxml(bbxx.st_sXfsh, _xm);

	xywhsf(_xm, x3, y, XGF_W, 200, LS_16, FS, AM_ZC);
	addxml("购买方", _xm);
	xywhsf(_xm, x4, y, XGFMCSH_W, 110, LS_16, FS, AM_ZC);
	addxml("名称", _xm);
	xywhsf(_xm, x4, y + 110, XGFMCSH_W, 90, LS_16, FS, AM_ZC);
	addxml("纳税人识别号", _xm);
	xywhsf(_xm, x5, y, XGFMCSH_W1, 110, LS_16, FS, AM_ZC);
	addxml(bbxx.st_sGfmc, _xm);
	xywhsf(_xm, x5, y + 110, XGFMCSH_W1, 90, LS_16, FS, AM_ZC);
	addxml(bbxx.st_sGfsh, _xm);
	y += 200;

	// 处理数据项
	int nW = 70; // 标题项高度
	x0 = XGF_W;
	x1 = x0 + MC_W;
	x2 = x1 + SL_W;
	x3 = x2 + DJ_W;
	x4 = x3 + JE_W;
	x5 = x4 + SLV_W;
	x6 = x5 + SE_W;

	int _y = y;

	xywhsf(_xm, x0, y, MC_W, nW, LS_9, FS, AM_ZC);
	addxml("货物（劳务服务）名称", _xm);
	xywhsf(_xm, x1, y, SL_W, nW, LS_9, FS, AM_ZC);
	addxml("数量", _xm);
	xywhsf(_xm, x2, y, DJ_W, nW, LS_9, FS, AM_ZC);
	addxml("单价", _xm);
	xywhsf(_xm, x3, y, JE_W, nW, LS_9, FS, AM_ZC);
	addxml("金额", _xm);
	xywhsf(_xm, x4, y, SLV_W, nW, LS_9, FS, AM_ZC);
	addxml("税率", _xm);
	xywhsf(_xm, x5, y, SE_W, nW, LS_9, FS, AM_ZC);	
	addxml("税额", _xm);
	_y += nW;

	int nLY = 140; // 数据行高度
	m_nLineNum = 0;
	LTHZXXB_BBXM::iterator pos;
	for (pos = bbxx.st_lHzxxbBbxm.begin(); pos != bbxx.st_lHzxxbBbxm.end(); pos++)
	{
		xywhsf(pos->xmMc, x0, _y, MC_W, nLY, LS_9, FS, AM_ZC_CHEKC);
		xywhsf(pos->xmSl, x1, _y, SL_W, nLY, LS_9, FS, AM_VCR_S);
		xywhsf(pos->xmDj, x2, _y, DJ_W, nLY, LS_9, FS, AM_VCR_S);
		xywhsf(pos->xmJe, x3, _y, JE_W, nLY, LS_9, FS, AM_VCR_S);
		xywhsf(pos->xmSlv, x4, _y, SLV_W, nLY, LS_9, FS, AM_ZC);
		xywhsf(pos->xmSe, x5, _y, SE_W, nLY, LS_9, FS, AM_VCR_S);
		_y += nLY;
	}

	int nTemp = 9 - bbxx.st_lHzxxbBbxm.size();
	for (int i = 0; i < nTemp; i++)
	{
		xywhsf(_xm, x0, _y, MC_W, nLY, LS_9, FS, AM_ZC_CHEKC);
		addxml("", _xm);
		xywhsf(_xm, x1, _y, SL_W, nLY, LS_9, FS, AM_VCR_S);
		addxml("", _xm);
		xywhsf(_xm, x2, _y, DJ_W, nLY, LS_9, FS, AM_VCR_S);
		addxml("", _xm);
		xywhsf(_xm, x3, _y, JE_W, nLY, LS_9, FS, AM_VCR_S);
		addxml("", _xm);
		xywhsf(_xm, x4, _y, SLV_W, nLY, LS_9, FS, AM_ZC);
		addxml("", _xm);
		xywhsf(_xm, x5, _y, SE_W, nLY, LS_9, FS, AM_VCR_S);
		addxml("", _xm);
		_y += nLY;
	}

	int hj_w = 75;
	xywhsf(_xm, x0, _y, MC_W, hj_w, LS_9, FS, AM_ZC);
	addxml("合计", _xm);
	xywhsf(_xm, x1, _y, SL_W, hj_w, LS_9, FS, AM_ZC);
	addxml("", _xm);
	xywhsf(_xm, x2, _y, DJ_W, hj_w, LS_9, FS, AM_ZC);
	addxml("", _xm);
	xywhsf(_xm, x3, _y, JE_W, hj_w, LS_9, FS, AM_VCR_S);
	addxml(bbxx.st_sHjje, _xm);
	xywhsf(_xm, x4, _y, SLV_W, hj_w, LS_9, FS, AM_ZC);
	addxml("", _xm);
	xywhsf(_xm, x5, _y, SE_W, hj_w, LS_9, FS, AM_VCR_S);
	addxml(bbxx.st_sHjse, _xm);
	_y += hj_w;

	int num = 0;
	for (pos = bbxx.st_lHzxxbBbxm.begin(); pos != bbxx.st_lHzxxbBbxm.end(); pos++)
	{
		addxml(pos->st_sMc, pos->xmMc);
		addxml(pos->st_sSl, pos->xmSl);
		addxml(pos->st_sDj, pos->xmDj);
		addxml(pos->st_sJe, pos->xmJe);
		addxml(pos->st_sSlv, pos->xmSlv);
		addxml(pos->st_sSe, pos->xmSe);
	}

	x0 = 0;
	xywhsf(_xm, x0, y, XGF_W, _y - y, LS_16, FS, AM_ZC);
	addxml("开具\r\n红字\r\n专用\r\n发票\r\n内容", _xm);
	y = _y;

	int w = x6 - XGF_W;
	xywhsf(_xm, x0, y, XGF_W, 750, LS_16, FS, AM_ZC);
	addxml("说明", _xm);
	xywhsf(_xm, x6, y, 0, 750, LS_16, FS, AM_ZC); // 宽度为0 ，画竖线， 高度为零， 画横线
	addxml("", _xm);
	xml.OutOfElem();// 退出pagedata
	xml.OutOfElem();// 退出NewPage

	xml.SetDoc(xml.GetDoc());
	xml.FindElem("NewPage");
	xml.IntoElem();
	xml.FindElem("PageHeader");
	xml.IntoElem();// 进入pageheader // 把说明中的内容放到pageheader防止打印矩形框
	// 这里处理说明中的内容
	_y = y;
	_y += 85;
	xywhsf(_xm, XGF_W + 20, _y, w - 50, 80, LS_16, FH, AM_VCL_S);
	addxml("一、购买方      □", _xm);
	if (bbxx.st_sSqf.Compare("0") == 0)
	{
		xywhsf(_xm, XGF_W + 20 + 440, _y + 35, 20, 20, LS_16, FH, -5); // -5画√号的俩条线
		addxml("√", _xm);
		xywhsf(_xm, XGF_W + 20 + 440 + 20, _y + 35 + 20, 35, -40, LS_16, FH, -5);
		addxml("√", _xm);
	}
	_y += 80;
	xywhsf(_xm, XGF_W + 20, _y, w - 50, 60, LS_12, FS, AM_VCL_S);
	addxml("对应蓝字专用发票抵扣增值税销项税额情况：", _xm);
	_y += 60;
	xywhsf(_xm, XGF_W + 190, _y, w - 150, 80, LS_12, FH, AM_VCL_S);
	addxml("1、已抵扣    □", _xm);
	if (bbxx.st_sSqyy.Compare("0") == 0 && bbxx.st_sSqf.Compare("0") == 0)
	{
		xywhsf(_xm, XGF_W + 20 + 440, _y + 35, 20, 20, LS_16, FH, -5); // -5画√号的俩条线
		addxml("√", _xm);
		xywhsf(_xm, XGF_W + 20 + 440 + 20, _y + 35 + 20, 35, -40, LS_16, FH, -5);
		addxml("√", _xm);
	}
	_y += 80;
	xywhsf(_xm, XGF_W + 190, _y, w - 150, 80, LS_12, FH, AM_VCL_S);
	addxml("2、未抵扣    □", _xm);
	if (bbxx.st_sSqyy.Compare("1") == 0 && bbxx.st_sSqf.Compare("0") == 0)
	{
		xywhsf(_xm, XGF_W + 20 + 440, _y + 35, 20, 20, LS_16, FH, -5); // -5画√号的俩条线
		addxml("√", _xm);
		xywhsf(_xm, XGF_W + 20 + 440 + 20, _y + 35 + 20, 35, -40, LS_16, FH, -5);
		addxml("√", _xm);
	}
	_y += 80;
	_y += 20;
	xywhsf(_xm, XGF_W + 20, _y, w - 50, 60, LS_12, FS, AM_VCL_S);
	addxml("对应蓝字专用发票的代码：________________ 号码：________________", _xm);
	if (bbxx.st_sSqyy.Compare("1") == 0 && bbxx.st_sSqf.Compare("0") == 0)
	{
		xywhsf(_xm, XGF_W + 20 + 530, _y, 300, 40, LS_12, FS, AM_VCL_S);
		addxml(bbxx.st_sFpdm, _xm);
		xywhsf(_xm, XGF_W + 20 + 1020, _y, 300, 40, LS_12, FS, AM_VCL_S);
		addxml(bbxx.st_sFphm, _xm);
	}
	_y += 60;

	_y += 20;
	xywhsf(_xm, XGF_W + 20, _y, w - 50, 80, LS_16, FH, AM_VCL_S);
	addxml("一、销售方      □", _xm);
	if (bbxx.st_sSqf.Compare("1") == 0)
	{
		xywhsf(_xm, XGF_W + 20 + 440, _y + 35, 20, 20, LS_16, FH, -5); // -5画√号的俩条线
		addxml("√", _xm);
		xywhsf(_xm, XGF_W + 20 + 440 + 20, _y + 35 + 20, 35, -40, LS_16, FH, -5);
		addxml("√", _xm);
	}
	_y += 80;
	_y += 20;
	xywhsf(_xm, XGF_W + 20, _y, w - 50, 60, LS_12, FS, AM_VCL_S);
	addxml("对应蓝字专用发票的代码：________________ 号码：________________", _xm);
	if (bbxx.st_sSqf.Compare("1") == 0)
	{
		xywhsf(_xm, XGF_W + 20 + 530, _y, 300, 40, LS_12, FS, AM_VCL_S);
		addxml(bbxx.st_sFpdm, _xm);
		xywhsf(_xm, XGF_W + 20 + 1020, _y, 300, 40, LS_12, FS, AM_VCL_S);
		addxml(bbxx.st_sFphm, _xm);
	}
	_y += 60;
	xml.OutOfElem();// 退出pageheader
	y += 750;// 说明的数据处理完成

	xml.FindElem("PageData");
	xml.IntoElem();// 再次进入pagedata
	// 红字专\r\n用发票\r\n信息表\r\n编号
	xywhsf(_xm, x0, y, XGF_W, 250, LS_16, FS, AM_ZC);
	addxml("红字专\r\n用发票\r\n信息表\r\n编号", _xm);

	xywhsf(_xm, XGF_W, y, w, 250, LS_16, FS, AM_ZC);
	addxml(bbxx.st_sXxbbh, _xm);
	xml.OutOfElem();// 退出pagedata
	xml.OutOfElem();

	return xml.GetDoc();
}
