#include "../../pch.h"
#include "Jsfp5776dy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#define SP57_W	130
#define DJ57_W	110
#define SL57_W	110
#define JE57_W	130

#define SP76_W	10
#define DJ76_W	10
#define SL76_W	10
#define JE76_W	10

CJsfp5776dy::CJsfp5776dy()
{
}

CJsfp5776dy::~CJsfp5776dy()
{
}

CString CJsfp5776dy::Dlfpdy_5776(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	JSFP_FPXX_5776 fpmx;
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
	if (xml.FindElem("dylx"))   fpdy.sDylx = xml.GetData();
	if (xml.FindElem("fplxdm")) fpdy.sFplxdm = xml.GetData();
	if (xml.FindElem("dyfs"))   fpdy.sDyfs = xml.GetData();
	if (xml.FindElem("printername"))	m_sPrinterName = xml.GetData();
	if (xml.FindElem("printtasknameflag")) m_sPrintTaskNameFlag = xml.GetData();
	//	if (xml.FindElem("orientation")) m_nOrientation = atoi(xml.GetData());
	m_iPldy = atoi(fpdy.sDyfs.GetBuffer(0));

	if (fpdy.sDylx.CompareNoCase("1") == 0)
	{
		fpdy.sErrorCode = "-3";
		fpdy.sErrorInfo = "卷式发票打印不支持清单打印";
		return GenerateXMLFpdy(fpdy);
	}

	m_sTempFplxdm = fpdy.sFplxdm;

	fpmx = ParseFpmxFromXML_5776(sInputInfo, fpdy);

	if (m_sPrintTaskNameFlag.Compare("1") == 0)
	{
		m_sPrintTaskName.Format("%s%s%s", fpdy.sFpdm.GetBuffer(0), "-", fpdy.sFphm.GetBuffer(0));
	}
	else
	{
		m_sPrintTaskName.Format("%s", "printer");
	}

	rtn = Print_5776(fpmx);

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CJsfp5776dy::Print_5776(JSFP_FPXX_5776 fpmx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	InitXYoff();

	do
	{
		nrt = InitPrinter(760, (short)1770.8);
		if (0 != nrt)
			break;

		DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };
		nrt = ::StartDoc(m_hPrinterDC, &di);
		if (nrt <= 0)
		{
			nrt = -3;// 启动打印任务失败
			break;
		}

		CString billXml = GenerateFpdyXml_5776(fpmx);

		CMarkup xml;
		if (!xml.SetDoc(billXml) || !xml.FindElem("Print"))
		{
			nrt = -5;// 打印内容解析失败
			break;
		}
		CString PrintData = xml.GetData();
		if (PrintData.CompareNoCase("NO") == 0)
		{
			nrt = -100;
			break;
		}

		xml.IntoElem();

		nrt = ::StartPage(m_hPrinterDC);
		if (nrt <= 0)
		{
			::EndDoc(m_hPrinterDC);
			nrt = -4;// 发送打印内容失败
			break;
		}

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

			itemRect.left = x + nXoff + 190;
			itemRect.top = (-y - nYoff - 330);
			itemRect.right = x + nXoff + 190 + w;
			itemRect.bottom = (-y - h - nYoff - 330);

			PaintTile(nFontSize, strFontName, itemRect, strText, z, fc);
		}

		::EndPage(m_hPrinterDC);
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

JSFP_FPXX_5776 CJsfp5776dy::ParseFpmxFromXML_5776(LPCTSTR inXml, FPDY fpdy)
{
	CMarkup xml;
	JSFP_FPXX_5776 fpxx;
	JSFP_FYXM_5776 fyxm;
	int booltrue = false;
	CString sl = "";

	xml.SetDoc(inXml);
	xml.FindElem("business");
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	xml.FindElem("returndata");
	xml.IntoElem();
	xml.FindElem("kpxx");
	xml.IntoElem();
	xml.FindElem("group");
	xml.IntoElem();
	//if (xml.FindElem("fpdm"))  fpxx.sFpdm = xml.GetData();
	if (xml.FindElem("fphm"))  fpxx.st_sFphm = xml.GetData();
	if (xml.FindElem("fpzt"))  fpxx.st_sFpzt = xml.GetData();
	//if (xml.FindElem("scbz"))  fpxx.sScbz = xml.GetData(); //上传标志
	if (xml.FindElem("kprq"))  fpxx.st_sKprq = xml.GetData();
	if (xml.FindElem("jqbh"))  fpxx.st_sJqbh = xml.GetData();
	if (xml.FindElem("skm"))   fpxx.st_sSkm = xml.GetData();
	if (xml.FindElem("xhdwsbh"))  fpxx.st_sXhdwsbh = xml.GetData();//销货单位识别号
	if (xml.FindElem("xhdwmc"))   fpxx.st_sXhdwmc = xml.GetData();//销货单位名称
	if (xml.FindElem("ghdwsbh"))  fpxx.st_sGhdwsbh = xml.GetData();//购货单位识别号
	if (xml.FindElem("ghdwmc"))   fpxx.st_sGhdwmc = xml.GetData();//购货单位名称
	if (xml.FindElem("fppy"))     fpxx.st_sFppy = xml.GetData();
	m_strFppy = fpxx.st_sFppy;

	if (fpxx.st_sFpzt.CompareNoCase("02") == 0)
	{
		m_sFpzt = fpxx.st_sFpzt;
		return fpxx;
	}

	if (xml.FindElem("fyxm"))
	{
		int n = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < n; i++)
		{
			xml.FindElem("group");
			xml.IntoElem();
			if (xml.FindElem("spmc")) fyxm.st_sSpmc = xml.GetData();
			if (xml.FindElem("spsl")) fyxm.st_sSpsl = xml.GetData();
			if (xml.FindElem("hsdj")) fyxm.st_sHsdj = xml.GetData();
			if (xml.FindElem("hsje")) fyxm.st_sHsje = xml.GetData();
			xml.OutOfElem();

			if (fyxm.st_sSpsl.Find(".") != -1)
			{
				int leng_spsl = fyxm.st_sSpsl.GetLength();
				if (fyxm.st_sSpsl.Mid(leng_spsl - 4, 1).CompareNoCase(".") == 0)
				{
					fyxm.st_sSpsl.Format("%.2f", atof(fyxm.st_sSpsl.GetBuffer(0)));
				}
			}

			if (fyxm.st_sHsdj.Find('.') != -1)
			{
				int leng_hsdj = fyxm.st_sHsdj.GetLength();
				if (fyxm.st_sHsdj.Mid(leng_hsdj - 4, 1).CompareNoCase(".") == 0)
				{
					fyxm.st_sHsdj.Format("%.2f", atof(fyxm.st_sHsdj.GetBuffer(0)));
				}
				else if (fyxm.st_sHsdj.Mid(leng_hsdj - 3, 1).CompareNoCase(".") != 0)
				{
					fyxm.st_sHsdj += "0";
				}
			}
			else
			{
				if (fyxm.st_sHsdj.GetLength() > 0)
					fyxm.st_sHsdj += ".00";
			}

			if (fyxm.st_sHsje.Find('.') != -1)
			{
				int leng_hsje = fyxm.st_sHsje.GetLength();
				if (fyxm.st_sHsje.Mid(leng_hsje - 3, 1).CompareNoCase(".") != 0)
				{
					fyxm.st_sHsje += "0";
				}
			}
			else
			{
				if (fyxm.st_sHsje.GetLength() > 0)
					fyxm.st_sHsje += ".00";
			}

			fpxx.st_lJsfp_fyxm.push_back(fyxm);
		}
		xml.OutOfElem();
	}

	if (xml.FindElem("jshj"))   fpxx.st_sJshj = xml.GetData();

	if (fpxx.st_sJshj.Find('.') != -1)  //有小数点
	{
		int leng_Hjje = fpxx.st_sJshj.GetLength();
		if (fpxx.st_sJshj.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
		{
			fpxx.st_sJshj += "0";
		}
	}
	else
	{
		if (fpxx.st_sJshj.GetLength() > 0)
			fpxx.st_sJshj += ".00";
		else
			fpxx.st_sJshj += "0.00";
	}
	if (xml.FindElem("bz"))     fpxx.st_sBz = xml.GetData();
	if (xml.FindElem("skr"))    fpxx.st_sSkr = xml.GetData();
	if (xml.FindElem("yfpdm"))  fpxx.st_sYfpdm = xml.GetData();
	if (xml.FindElem("yfphm"))  fpxx.st_sYfphm = xml.GetData();

	//fpxx.kprq.syear = fpxx.sKprq.Mid(0, 4);
	//fpxx.kprq.smouth = fpxx.sKprq.Mid(4, 2);
	//fpxx.kprq.sday = fpxx.sKprq.Mid(6, 2);

	fpxx.st_sJshj.TrimLeft("-");

	char strDxje[100];
	char strsc[100];
	strcpy(strsc, fpxx.st_sJshj.GetBuffer(0));
	PCLib_ChineseFee(strDxje, 100, strsc);//小写金额转换为大写金额
	fpxx.st_sJshjDx.Format("%s", strDxje);
	if (fpxx.st_sJshjDx.Mid(fpxx.st_sJshjDx.GetLength() - 2, 2).CompareNoCase("角") == 0)
	{
		fpxx.st_sJshjDx += "整";
	}

	if (fpxx.st_sFpzt == "01")
	{
		fpxx.st_sJshj = "-" + fpxx.st_sJshj;
		fpxx.st_sJshjDx = "负" + fpxx.st_sJshjDx;
		fpxx.st_sYfpdm = "原发票代码:" + fpxx.st_sYfpdm;
		fpxx.st_sYfphm = "原发票号码:" + fpxx.st_sYfphm;
	}

	fpxx.st_sJshj = "￥" + fpxx.st_sJshj;
	return fpxx;
}

CString CJsfp5776dy::GenerateFpdyXml_5776(JSFP_FPXX_5776 fpmx)
{
	CString xml;
	int nFppy = atoi(fpmx.st_sFppy.GetBuffer(0));
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";
	if (fpmx.st_sFpzt.CompareNoCase("02") != 0)
	{
		switch (nFppy) {
		case 1:
			//xml += GenerateItemXmlB(fpmx, fpdy);
			break;
		case 2:
			//xml += GenerateItemXmlA(fpmx, fpdy);
			break;
		case 3:
			//xml += GenerateItemXmlD(fpmx, fpdy);
			break;
		case 4:
			//xml += GenerateItemXmlC(fpmx, fpdy);
			break;
		case 5:
			//xml += GenerateItemXmlE(fpmx, fpdy);
			break;
		case 6:
			//xml += GenerateItemXmlF(fpmx, fpdy);
			break;
		case 7:
			xml += GenerateItemXmlG_57(fpmx);
			break;
		case 8:
			//xml += GenerateItemXmlH(fpmx, fpdy);
			break;
		default:
			xml += "No";
		}
	}
	xml += "</Print>";

	//	DEBUG_TRACELOG_STR("xml", xml.GetBuffer(0));

	return xml;
}

CString CJsfp5776dy::GenerateItemXmlG_57(JSFP_FPXX_5776 fpmx)
{
	CMarkup xml;

	int h = 0;
	xywhsf(fpmx.xmFphm, 130, h, 300, 30, LS_8, FS, AM_VCL);
	h += 30;
	h += 15;
	xywhsf(fpmx.xmJqbh, 130, h, 300, 30, LS_8, FS, AM_VCL);
	h += 30;
	h += 40;
	xywhsf(fpmx.xmXhdwmc, 0, h, 470, 90, LS_8, FS, AM_ZL_L);
	h += 90;
	xywhsf(fpmx.xmXhdwsbh, 155, h, 300, 30, LS_8, FS, AM_VCL);
	h += 30;
	h += 10;
	xywhsf(fpmx.xmKprq, 130, h, 300, 30, LS_8, FS, AM_VCL);
	h += 30;
	h += 10;
	xywhsf(fpmx.xmSkr, 90, h, 300, 30, LS_8, FS, AM_VCL);
	h += 30;
	h += 40;
	xywhsf(fpmx.xmGhdwmc, 0, h, 470, 90, LS_8, FS, AM_ZL_L);
	h += 90;
	xywhsf(fpmx.xmGhdwsbh, 155, h, 300, 30, LS_8, FS, AM_VCL);
	h += 30;
	h += 40;

	int x0 = 0;
	int x1 = x0 + SP57_W;
	int x2 = x1 + DJ57_W;
	int x3 = x2 + SL57_W;
	int x4 = x3 + JE57_W;

	int b = -500;
	if (!fpmx.st_sBz.IsEmpty())
		b = -400;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = SP57_W;
	rect.bottom = b;// 商品名称最高限制50

	LTJSFP_FYXM_5776::iterator pos;
	CString mc = "";
	for (pos = fpmx.st_lJsfp_fyxm.begin(); pos != fpmx.st_lJsfp_fyxm.end(); pos++)
	{
		pos->st_sSpmc = pos->st_sSpmc.Left(DataPrintMaxLen(pos->st_sSpmc, 92));
		mc += pos->st_sSpmc;
		mc += "\n";
	}

	LONG f = PaintTile2(3, LS_8, FS, rect, mc, AM_ZL_L);

	int _h = h;
	for (pos = fpmx.st_lJsfp_fyxm.begin(); pos != fpmx.st_lJsfp_fyxm.end(); pos++)
	{
		//pos->st_sSpmc = pos->st_sSpmc.Left(DataPrintMaxLen(pos->st_sSpmc, 40));
		LONG l = PaintTile2(2, f, FS, rect, pos->st_sSpmc, AM_ZL_L);
		xywhsf(pos->xmSpmc, x0, h, SP57_W, l, f, FS, AM_ZL_L);
		xywhsf(pos->xmHsdj, x1, h, DJ57_W, l, f, FS, AM_ZR_S);
		xywhsf(pos->xmSpsl, x2, h, SL57_W, l, f, FS, AM_ZR_S);
		xywhsf(pos->xmHsje, x3, h, JE57_W, l, f, FS, AM_ZR_S);
		h += l;
	}

	int th = h - _h;// 数据项高
	int _l = 500 - th - 100;// 数据项与备注之间的间隔
	h += _l;

	if (fpmx.st_sFpzt.CompareNoCase("01") == 0)
	{
		h += 65;
		xywhsf(fpmx.xmYfpdm, 0, h, 250, 35, LS_8, FS, AM_VCL_S);
		xywhsf(fpmx.xmYfphm, 255, h, 200, 35, LS_8, FS, AM_VCL_S);
		h -= 65;
	}
	else
	{
		h += 35;
	}

	if (!fpmx.st_sBz.IsEmpty())
	{
		rect.left = 0;
		rect.top = 0;
		rect.right = 470;
		rect.bottom = -65;
		LONG l = PaintTile2(2, LS_9, FS, rect, fpmx.st_sBz, AM_ZL_L);
		h += (65 - l);
		xywhsf(fpmx.xmBz, 0, h, 470, l, LS_8, FS, AM_ZL_L);
		h += l;
		if (fpmx.st_sFpzt.CompareNoCase("01") == 0)
		{
			h += 35;
		}
	}
	else
	{
		h += 65;
	}

	xywhsf(fpmx.xmJshj, 130, h, 300, 40, LS_8, FS, AM_VCL_S);   //小写价税合计
	h += 40;
	xywhsf(fpmx.xmJshjDx, 130, h, 300, 40, LS_8, FS, AM_VCL_S);   //大写价税合计
	h += 40;

	xywhsf(fpmx.xmSkm, 120, h, 350, 40, LS_8, FS, AM_VCL_S);

	addxml(fpmx.st_sFphm, fpmx.xmFphm);
	addxml(fpmx.st_sJqbh, fpmx.xmJqbh);
	addxml(fpmx.st_sXhdwmc, fpmx.xmXhdwmc);
	addxml(fpmx.st_sXhdwsbh, fpmx.xmXhdwsbh);
	addxml(fpmx.st_sKprq, fpmx.xmKprq);
	addxml(fpmx.st_sSkr, fpmx.xmSkr);
	addxml(fpmx.st_sGhdwmc, fpmx.xmGhdwmc);
	addxml(fpmx.st_sGhdwsbh, fpmx.xmGhdwsbh);

	for (pos = fpmx.st_lJsfp_fyxm.begin(); pos != fpmx.st_lJsfp_fyxm.end(); pos++)
	{
		addxml(pos->st_sSpmc, pos->xmSpmc);
		addxml(pos->st_sHsdj, pos->xmHsdj);
		addxml(pos->st_sSpsl, pos->xmSpsl);
		addxml(pos->st_sHsje, pos->xmHsje);
	}

	addxml(fpmx.st_sBz, fpmx.xmBz);
	if (fpmx.st_sFpzt.CompareNoCase("01") == 0)
	{
		addxml(fpmx.st_sYfpdm, fpmx.xmYfpdm);
		addxml(fpmx.st_sYfphm, fpmx.xmYfphm);
	}

	addxml(fpmx.st_sJshj, fpmx.xmJshj);
	addxml(fpmx.st_sJshjDx, fpmx.xmJshjDx);
	addxml(fpmx.st_sSkm, fpmx.xmSkm);

	return xml.GetDoc();
}

CString CJsfp5776dy::GenerateItemXmlF_76(JSFP_FPXX_5776 fpmx, FPDY fpdy)
{
	return CString();
}
