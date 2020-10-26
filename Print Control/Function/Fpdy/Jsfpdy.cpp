#include "../../pch.h"
#include "Jsfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/QRGenerator/QRGenerator.h"
#include "../../Helper/QRGenerator/Base64.h"

CJsfpdy::CJsfpdy():m_strFppy("")
{

}

CJsfpdy::~CJsfpdy()
{

}

CString CJsfpdy::Dlfpdy(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	JSFP_FPXX fpmx;
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

	fpmx = ParseFpmxFromXML(sInputInfo, fpdy);

	if (m_sPrintTaskNameFlag.Compare("1") == 0)
	{
		m_sPrintTaskName.Format("%s%s%s", fpdy.sFpdm.GetBuffer(0), "-", fpdy.sFphm.GetBuffer(0));
	}
	else
	{
		m_sPrintTaskName.Format("%s", "printer");
	}

	/*
	if(fpmx.sFpzt.CompareNoCase("02") == 0
		||fpmx.sFpzt.CompareNoCase("03") == 0
		||fpmx.sFpzt.CompareNoCase("04") == 0)
	{
		fpdy.sErrorCode = "-8";
		fpdy.sErrorInfo = "作废发票不能打印";
		return GenerateXMLFpdy(fpdy);
	}
*/

	fpmx.bMxFontSize = true;

	if (fpmx.sBz.IsEmpty())
	{
		if ((fpmx.sWidthRow7 > 22 && fpmx.sWidthRow8 > 19) || (fpmx.sNarrowRow7 > 17 && fpmx.sNarrowRow8 > 14))
		{
			fpdy.sErrorCode.Format("%d", -3);
			fpdy.sErrorInfo = "卷式发票明细行数超出限定行数，不能打印";
			return GenerateXMLFpdy(fpdy);
		}

		if ((fpmx.sWidthRow7 <= 22 && fpmx.sWidthRow8 > 19) || (fpmx.sNarrowRow7 <= 17 && fpmx.sNarrowRow8 > 14))
		{
			fpmx.bMxFontSize = false;
		}
	}
	else
	{
		if ((fpmx.sWidthRow7 > 20 && fpmx.sWidthRow8 > 17) || (fpmx.sNarrowRow7 > 14 && fpmx.sNarrowRow8 > 11))
		{
			fpdy.sErrorCode.Format("%d", -3);
			fpdy.sErrorInfo = "卷式发票明细行数超出限定行数，不能打印";
			return GenerateXMLFpdy(fpdy);
		}

		if ((fpmx.sWidthRow7 <= 20 && fpmx.sWidthRow8 > 17) || (fpmx.sNarrowRow7 <= 14 && fpmx.sNarrowRow8 > 11))
		{
			fpmx.bMxFontSize = false;
		}
	}

	printXml = GenerateFpdyXml(fpmx, fpdy.sDylx, fpdy);
	CString PrintData;
	xml.SetDoc(printXml.GetBuffer(0));
	if (xml.FindElem("Print")) PrintData = xml.GetData();
	xml.OutOfElem();

	if (PrintData.CompareNoCase("NO") == 0)
	{
		fpdy.sErrorCode.Format("%d", -100);
		fpdy.sErrorInfo.Format("%s", "不支持此种票样");
		return GenerateXMLFpdy(fpdy);
	}

	rtn = Print(printXml);

	return GenerateXMLFpdy(fpdy, rtn);
}

JSFP_FPXX CJsfpdy::ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy)
{
	CMarkup xml;
	JSFP_FPXX fpxx;
	JSFP_FYQDXX qdxx;
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
	if (xml.FindElem("fpdm"))  fpxx.sFpdm = xml.GetData();
	if (xml.FindElem("fphm"))  fpxx.sFphm = xml.GetData();
	if (xml.FindElem("fpzt"))  fpxx.sFpzt = xml.GetData();
	if (xml.FindElem("scbz"))  fpxx.sScbz = xml.GetData(); //上传标志
	if (xml.FindElem("kprq"))  fpxx.sKprq = xml.GetData();
	if (xml.FindElem("jqbh"))  fpxx.sSkpbh = xml.GetData();
	if (xml.FindElem("skm"))   fpxx.sJym = xml.GetData();
	//	if(xml.FindElem("jym"))      fpxx.sJym=xml.GetData();//校验码
	if (xml.FindElem("xhdwsbh"))  fpxx.sXhdwsbh = xml.GetData();//销货单位识别号
	if (xml.FindElem("xhdwmc"))   fpxx.sXhdwmc = xml.GetData();//销货单位名称
	if (xml.FindElem("ghdwsbh"))  fpxx.sGhdwsbh = xml.GetData();//购货单位识别号
	if (xml.FindElem("ghdwmc"))   fpxx.sGhdwmc = xml.GetData();//购货单位名称
	if (xml.FindElem("fppy"))     fpxx.sFppy = xml.GetData();
	m_strFppy = fpxx.sFppy;

	if (fpxx.sFpzt.CompareNoCase("02") == 0)
	{
		m_sFpzt = fpxx.sFpzt;
		return fpxx;
	}

	fpxx.sWidthRow7 = 0;
	fpxx.sWidthRow8 = 0;
	fpxx.sNarrowRow7 = 0;
	fpxx.sNarrowRow8 = 0;

	if (xml.FindElem("fyxm"))
	{
		fpxx.iFyxmCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxx.iFyxmCount; i++)
		{
			xml.FindElem("group");
			xml.IntoElem();
			if (xml.FindElem("spmc")) fpxx.fyxmxx[i].sSpmc = xml.GetData();
			if (m_strFppy.CompareNoCase("01") == 0 || m_strFppy.CompareNoCase("06") == 0)
			{
				if (fpxx.fyxmxx[i].sSpmc.GetLength() % 12 == 0)
				{
					fpxx.sWidthRow8 += fpxx.fyxmxx[i].sSpmc.GetLength() / 12;
				}
				else if (fpxx.fyxmxx[i].sSpmc.GetLength() % 12 != 0)
				{
					fpxx.sWidthRow8 += fpxx.fyxmxx[i].sSpmc.GetLength() / 12 + 1;
				}

				if (fpxx.fyxmxx[i].sSpmc.GetLength() % 14 == 0)
				{
					fpxx.sWidthRow7 += fpxx.fyxmxx[i].sSpmc.GetLength() / 14;
				}
				else if (fpxx.fyxmxx[i].sSpmc.GetLength() % 14 != 0)
				{
					fpxx.sWidthRow7 += fpxx.fyxmxx[i].sSpmc.GetLength() / 14 + 1;
				}
			}
			else if (m_strFppy.CompareNoCase("03") == 0 || m_strFppy.CompareNoCase("07") == 0)
			{
				if (fpxx.fyxmxx[i].sSpmc.GetLength() % 8 == 0)
				{
					fpxx.sNarrowRow8 += fpxx.fyxmxx[i].sSpmc.GetLength() / 8;
				}
				else if (fpxx.fyxmxx[i].sSpmc.GetLength() % 8 != 0)
				{
					fpxx.sNarrowRow8 += fpxx.fyxmxx[i].sSpmc.GetLength() / 8 + 1;
				}

				if (fpxx.fyxmxx[i].sSpmc.GetLength() % 10 == 0)
				{
					fpxx.sNarrowRow7 += fpxx.fyxmxx[i].sSpmc.GetLength() / 10;
				}
				else if (fpxx.fyxmxx[i].sSpmc.GetLength() % 10 != 0)
				{
					fpxx.sNarrowRow7 += fpxx.fyxmxx[i].sSpmc.GetLength() / 10 + 1;
				}
			}
			else
			{

			}
			if (xml.FindElem("spsl")) fpxx.fyxmxx[i].sSpsl = xml.GetData();
			if (xml.FindElem("hsdj")) fpxx.fyxmxx[i].sHsdj = xml.GetData();
			if (xml.FindElem("hsje")) fpxx.fyxmxx[i].sHsje = xml.GetData();
			if (xml.FindElem("dj"))   fpxx.fyxmxx[i].sDj = xml.GetData();
			if (xml.FindElem("je"))   fpxx.fyxmxx[i].sJe = xml.GetData();		
			if (xml.FindElem("sl"))   fpxx.fyxmxx[i].sSl = xml.GetData();
			if (xml.FindElem("se"))   fpxx.fyxmxx[i].sSe = xml.GetData();
			xml.OutOfElem();

			if (fpxx.fyxmxx[i].sSpsl.Find(".") != -1)
			{
				int leng_spsl = fpxx.fyxmxx[i].sSpsl.GetLength();
				if (fpxx.fyxmxx[i].sSpsl.Mid(leng_spsl - 4, 1).CompareNoCase(".") == 0)
				{
					fpxx.fyxmxx[i].sSpsl.Format("%.2f", atof(fpxx.fyxmxx[i].sSpsl.GetBuffer(0)));
				}
			}

			if (fpxx.fyxmxx[i].sJe.Find('.') != -1)  //有小数点
			{
				int leng_Hjje = fpxx.fyxmxx[i].sJe.GetLength();
				if (fpxx.fyxmxx[i].sJe.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
				{
					fpxx.fyxmxx[i].sJe += "0";
				}
			}
			else
			{
				if (fpxx.fyxmxx[i].sJe.GetLength() > 0)
					fpxx.fyxmxx[i].sJe += ".00";
				else
					fpxx.fyxmxx[i].sJe += "0.00";
			}

			if (fpxx.fyxmxx[i].sHsdj.Find('.') != -1)
			{
				int leng_hsdj = fpxx.fyxmxx[i].sHsdj.GetLength();
				if (fpxx.fyxmxx[i].sHsdj.Mid(leng_hsdj - 4, 1).CompareNoCase(".") == 0)
				{
					fpxx.fyxmxx[i].sHsdj.Format("%.2f", atof(fpxx.fyxmxx[i].sHsdj.GetBuffer(0)));
				}
				else if (fpxx.fyxmxx[i].sHsdj.Mid(leng_hsdj - 3, 1).CompareNoCase(".") != 0)
				{
					fpxx.fyxmxx[i].sHsdj += "0";
				}
			}
			else
			{
				if (fpxx.fyxmxx[i].sHsdj.GetLength() > 0)
					fpxx.fyxmxx[i].sHsdj += ".00";
			}

			if (fpxx.fyxmxx[i].sHsje.Find('.') != -1)
			{
				int leng_hsje = fpxx.fyxmxx[i].sHsje.GetLength();
				if (fpxx.fyxmxx[i].sHsje.Mid(leng_hsje - 3, 1).CompareNoCase(".") != 0)
				{
					fpxx.fyxmxx[i].sHsje += "0";
				}
			}
			else
			{
				if (fpxx.fyxmxx[i].sHsje.GetLength() > 0)
					fpxx.fyxmxx[i].sHsje += ".00";
			}

			if (fpxx.fyxmxx[i].sSe.CompareNoCase("-0.00") == 0)
			{
				fpxx.fyxmxx[i].sSe.Delete(0, 1);
			}
		}
		xml.OutOfElem();
	}

	if (xml.FindElem("hjje"))   fpxx.sHjje = xml.GetData();
	if (xml.FindElem("hjse"))   fpxx.sHjse = xml.GetData();

	if (fpxx.sHjse.Find('.') != -1)  //有小数点
	{
		int leng_Hjje = fpxx.sHjse.GetLength();
		if (fpxx.sHjse.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
		{
			fpxx.sHjse += "0";
		}
	}
	else
	{
		if (fpxx.sHjse.GetLength() > 0)
			fpxx.sHjse += ".00";
		else
			fpxx.sHjse += "0.00";
	}

	if (fpxx.sHjje.Find('.') != -1)  //有小数点
	{
		int leng_Hjje = fpxx.sHjje.GetLength();
		if (fpxx.sHjje.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
		{
			fpxx.sHjje += "0";
		}
	}
	else
	{
		if (fpxx.sHjje.GetLength() > 0)
			fpxx.sHjje += ".00";
		else
			fpxx.sHjje += "0.00";
	}

	if (xml.FindElem("jshj"))   fpxx.sJshj = xml.GetData();

	if (fpxx.sJshj.Find('.') != -1)  //有小数点
	{
		int leng_Hjje = fpxx.sJshj.GetLength();
		if (fpxx.sJshj.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
		{
			fpxx.sJshj += "0";
		}
	}
	else
	{
		if (fpxx.sJshj.GetLength() > 0)
			fpxx.sJshj += ".00";
		else
			fpxx.sJshj += "0.00";
	}
	if (xml.FindElem("bz"))     fpxx.sBz = xml.GetData();
	if (xml.FindElem("skr"))    fpxx.sSkr = xml.GetData();
	if (xml.FindElem("kpr"))    fpxx.sKpr = xml.GetData();
	if (xml.FindElem("yfpdm"))  fpxx.sYfpdm = xml.GetData();
	if (xml.FindElem("yfphm"))  fpxx.sYfphm = xml.GetData();
	if (xml.FindElem("zfrq"))   fpxx.sZfrq = xml.GetData();
	if (xml.FindElem("zfr"))    fpxx.sZfr = xml.GetData();
	if (xml.FindElem("qmcs"))   fpxx.sQmcs = xml.GetData();
	if (xml.FindElem("qmz"))    fpxx.sQmz = xml.GetData();
	if (xml.FindElem("ykfsje")) fpxx.sYkfsje = xml.GetData();

	fpxx.kprq.syear = fpxx.sKprq.Mid(0, 4);
	fpxx.kprq.smouth = fpxx.sKprq.Mid(4, 2);
	fpxx.kprq.sday = fpxx.sKprq.Mid(6, 2);

	fpxx.sJshj.TrimLeft("-");

	char strDxje[100];
	char strsc[100];
	strcpy(strsc, fpxx.sJshj.GetBuffer(0));
	PCLib_ChineseFee(strDxje, 100, strsc);//小写金额转换为大写金额
	fpxx.sJshjDx.Format("%s", strDxje);
	if (fpxx.sJshjDx.Mid(fpxx.sJshjDx.GetLength() - 2, 2).CompareNoCase("角") == 0)
	{
		fpxx.sJshjDx += "整";
	}

	if (fpxx.sFpzt == "01")
	{
		fpxx.sJshj = "-" + fpxx.sJshj;
		fpxx.sJshjDx = "负" + fpxx.sJshjDx;
		fpxx.sYfpdm = "原发票代码:" + fpxx.sYfpdm;
		fpxx.sYfphm = "原发票号码:" + fpxx.sYfphm;
	}

	fpxx.sJshj = "￥" + fpxx.sJshj;
	return fpxx;
}

CString CJsfpdy::GenerateFpdyXml(JSFP_FPXX fpmx, CString dylx, FPDY fpdy)
{
	CString xml;
	int nFppy = atoi(fpmx.sFppy.GetBuffer(0));
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";
	if (fpmx.sFpzt.CompareNoCase("02") != 0)
	{
		switch (nFppy) {
		case 1:
			xml += GenerateItemXmlB(fpmx, fpdy);
			break;
		case 2:
			xml += GenerateItemXmlA(fpmx, fpdy);
			break;
		case 3:
			xml += GenerateItemXmlD(fpmx, fpdy);
			break;
		case 4:
			xml += GenerateItemXmlC(fpmx, fpdy);
			break;
		case 5:
			xml += GenerateItemXmlE(fpmx, fpdy);
			break;
		case 6:
			xml += GenerateItemXmlF(fpmx, fpdy);
			break;
		case 7:
			xml += GenerateItemXmlG(fpmx, fpdy);
			break;
		case 8:
			xml += GenerateItemXmlH(fpmx, fpdy);
			break;
		default:
			xml += "No";
		}
	}
	xml += "</Print>";

	//	DEBUG_TRACELOG_STR("xml", xml.GetBuffer(0));

	return xml;
}

CString CJsfpdy::GenerateItemXmlA(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 435, LY + 230 - 5, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 80 - 25, LY + 230 - 5, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.kprq.year, LX + 70 - 25, LY + 385, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 140 - 25, LY + 385, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 160 - 25, LY + 385, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 195 - 25, LY + 385, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 215 - 25, LY + 385, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 80, LY + 425, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Ghdwsbh, LX + 80, LY + 515, LW, LH, LS_8, FS, ZL);

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{

		xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + -60, LY + 610 + i * 80, LW + 180, LH + 80, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 300, LY + 610 + i * 80, LW + 90, LH + 80, LS_8, FS, ZC);
		xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 150, LY + 610 + i * 80, LW + 90, LH + 80, LS_8, FS, ZR);
		xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 495, LY + 610 + i * 80, LW + 90, LH + 80, LS_8, FS, ZR);
	}

	xywhsf(fpmx.hjje, LX + 85, LY + 880, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.hjse, LX + 85, LY + 920 + 5, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.jshjDx, LX + 85, LY + 1000 + 5, LW, LH, LS_8, FS, ZL);   //大写价税合计
	xywhsf(fpmx.jshj, LX + 85, LY + 960 + 5, LW, LH, LS_8, FS, ZL);   //小写价税合计

	xywhsf(fpmx.Xhdwmc, LX + 80, LY + 260, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 80, LY + 350, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 400, LY + 385, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 10, LY + 1040 + 10, LW, LH, LS_8, FS, ZL);   //小写价税合计

	addxml(fpmx.sSkpbh, fpmx.Skpbh);
	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	addxml(fpmx.sHjje, fpmx.hjje);
	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlB(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	int nY = 30;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 430, LY + 235 - nY, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 80 - 25, LY + 235 - nY, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.kprq.year, LX + 70 - 25, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 140 - 25, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 160 - 25, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 195 - 25, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 215 - 25, LY + 380 - 10, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 80, LY + 410 - 5, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Ghdwsbh, LX + 80, LY + 500 - 5, LW, LH, LS_8, FS, ZL);

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{

		xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + -60, LY + 610 + i * 80 - 30, LW + 180, LH + 80, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 300, LY + 610 + i * 80 - 30, LW + 90, LH + 80, LS_8, FS, ZC);
		xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 150, LY + 610 + i * 80 - 30, LW + 90, LH + 80, LS_8, FS, ZR);
		xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 495, LY + 610 + i * 80 - 30, LW + 90, LH + 80, LS_8, FS, ZR);
	}

	xywhsf(fpmx.hjje, LX + 85, LY + 880 - 30 + 265, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.hjse, LX + 85, LY + 920 - 30 + 265, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.jshjDx, LX + 85, LY + 1000 - 30 + 265, LW, LH, LS_8, FS, ZL);   //大写价税合计
	xywhsf(fpmx.jshj, LX + 85, LY + 960 - 30 + 265, LW, LH, LS_8, FS, ZL);   //小写价税合计

	xywhsf(fpmx.Xhdwmc, LX + 80, LY + 270 - 30, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 80, LY + 340 - 10, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 400, LY + 380 - 10, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 10, LY + 1040 - 25 + 265, LW, LH, LS_8, FS, ZL);   //小写价税合计

	addxml(fpmx.sSkpbh, fpmx.Skpbh);
	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	addxml(fpmx.sHjje, fpmx.hjje);
	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlC(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 240, LY + 260, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 240, LY + 210 + 10, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.kprq.year, LX + 240, LY + 460, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 310, LY + 460, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 330, LY + 460, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 365, LY + 460, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 385, LY + 460, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 100, LY + 575, LW + 500, LH + 80, LS_8, FS, ZL);
	xywhsf(fpmx.Ghdwsbh, LX + 280, LY + 670, LW, LH, LS_8, FS, ZL);

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{

		xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 100, LY + 740 + i * 60, LW + 120, LH + 80, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 380, LY + 740 + i * 60, LW + 90, LH + 80, LS_8, FS, ZC);
		xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 270, LY + 740 + i * 60, LW + 90, LH + 80, LS_8, FS, ZR);
		xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 500, LY + 740 + i * 60, LW + 90, LH + 80, LS_8, FS, ZR);
	}

	xywhsf(fpmx.hjje, LX + 200, LY + 920, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.hjse, LX + 200, LY + 960, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.jshjDx, LX + 230, LY + 1040, LW, LH, LS_8, FS, ZL);   //大写价税合计
	xywhsf(fpmx.jshj, LX + 230, LY + 1000, LW, LH, LS_8, FS, ZL);   //小写价税合计

	xywhsf(fpmx.Xhdwmc, LX + 100, LY + 335, LW + 500, LH + 80, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 270, LY + 430, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 210, LY + 510, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 200, LY + 1080, LW, LH, LS_8, FS, ZL);

	addxml(fpmx.sSkpbh, fpmx.Skpbh);

	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	addxml(fpmx.sHjje, fpmx.hjje);
	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlD(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 240, LY + 260 - 15, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 240, LY + 260 - 55, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.kprq.year, LX + 240, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 310, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 330, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 365, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 385, LY + 460 - 15, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 100, LY + 560, LW + 500, LH + 80, LS_8, FS, ZL);
	xywhsf(fpmx.Ghdwsbh, LX + 270, LY + 650, LW + 330, LH + 60, LS_8, FS, ZL);

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{

		xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 100, LY + 730 + i * 60, LW + 120, LH + 80, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 380, LY + 730 + i * 60, LW + 90, LH + 80, LS_8, FS, ZC);
		xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 270, LY + 730 + i * 60, LW + 90, LH + 80, LS_8, FS, ZR);
		xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 500, LY + 730 + i * 60, LW + 90, LH + 80, LS_8, FS, ZR);
	}

	xywhsf(fpmx.hjje, LX + 200, LY + 920 + 225, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.hjse, LX + 200, LY + 960 + 225, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.jshjDx, LX + 230, LY + 1040 + 225, LW, LH, LS_8, FS, ZL);   //大写价税合计
	xywhsf(fpmx.jshj, LX + 230, LY + 1000 + 225, LW, LH, LS_8, FS, ZL);   //小写价税合计

	xywhsf(fpmx.Xhdwmc, LX + 100, LY + 315, LW + 500, LH + 80, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 270, LY + 410, LW + 330, LH + 60, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 210, LY + 490, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 210, LY + 1080 + 225, LW, LH, LS_8, FS, ZL);
	addxml(fpmx.sSkpbh, fpmx.Skpbh);

	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	addxml(fpmx.sHjje, fpmx.hjje);
	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlE(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 100, LY + 130, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 100, LY + 70 + 10, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.kprq.year, LX + 70 + 40, LY + 290, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 140 + 40, LY + 290, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 160 + 40, LY + 290, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 195 + 40, LY + 290, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 215 + 40, LY + 290, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 230, LY + 330, LW + 380, LH + 80, LS_8, FS, ZL);
	//	xywhsf(fpmx.Ghdwsbh       ,LX+ 100                         ,LY+ 500       ,LW         ,LH         ,LS_8          ,FT    ,ZL );

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{

		xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + -60, LY + 430 + i * 80, LW + 180, LH + 80, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 360, LY + 430 + i * 80, LW + 90, LH + 80, LS_8, FS, ZC);
		xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 210, LY + 430 + i * 80, LW + 90, LH + 80, LS_8, FS, ZR);
		xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 480, LY + 430 + i * 80, LW + 90, LH + 80, LS_8, FS, ZR);
	}

	//	xywhsf(fpmx.hjje						 ,LX+ 100               ,LY+ 880       ,LW    ,LH      ,LS_8        ,FT    ,ZL ); 	
	//	xywhsf(fpmx.hjse						 ,LX+ 100               ,LY+ 920       ,LW    ,LH      ,LS_8        ,FT    ,ZL );
	xywhsf(fpmx.jshjDx, LX + 150, LY + 720, LW, LH, LS_8, FS, ZL);   //大写价税合计
	xywhsf(fpmx.jshj, LX + 150, LY + 670, LW, LH, LS_8, FS, ZL);   //小写价税合计

	xywhsf(fpmx.Xhdwmc, LX + 100, LY + 170, LW + 500, LH + 80, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 160, LY + 250, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 500, LY + 290, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 100, LY + 770, LW, LH, LS_8, FS, ZL);   //小写价税合计

	addxml(fpmx.sSkpbh, fpmx.Skpbh);
	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	//	addxml(fpmx.sGhdwsbh,fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	//	addxml(fpmx.sHjje, fpmx.hjje);
	//	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlF(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	int nY = 30;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 390, LY + 235 - nY, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 35, LY + 235 - nY, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.kprq.year, LX + 70 - 30, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 140 - 30, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 160 - 30, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 200 - 5 - 30, LY + 380 - 10, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 220 - 5 - 30, LY + 380 - 10, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 70, LY + 410 - 5, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Ghdwsbh, LX + 100, LY + 500 - 5, LW, LH, LS_8, FS, ZL);

	int number = LY + 580;
	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{
		/*
		if(fpmx.fyxmxx[i].sSpmc.GetLength() < 13)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,number+i*25    ,LW+180        ,LH+1000     ,LS_8          ,FS    ,ZL );
		}
		else if(fpmx.fyxmxx[i].sSpmc.GetLength() > 12 && fpmx.fyxmxx[i].sSpmc.GetLength() < 15)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,number+i*25    ,LW+180        ,LH+1000     ,LS_7          ,FS    ,ZL );
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,number+i*25    ,LW+180        ,LH+1000     ,LS_5          ,FS    ,ZL );
		}
		*/
		//		xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,number    ,LW+180        ,LH+1000     ,LS_5          ,FS    ,ZL );
		//		xywhsf(fpmx.fyxmxx[i].ssSpsl         ,LX+ 305                   ,number+i*25    ,LW+110        ,LH+1000      ,LS_8          ,FS    ,ZL );
		//		xywhsf(fpmx.fyxmxx[i].ssHsdj         ,LX+ 135                   ,number+i*25    ,LW+150        ,LH+1000      ,LS_8          ,FS    ,ZL );
		//		xywhsf(fpmx.fyxmxx[i].ssHsje         ,LX+ 435                   ,number+i*25    ,LW+150        ,LH+1000      ,LS_8          ,FS    ,ZL );

		if (fpmx.bMxFontSize)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + -60, number, LW + 190, LH + 1000, LS_8, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 290, number, LW + 130, LH + 50, LS_8, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 135, number, LW + 150, LH + 50, LS_8, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 425, number, LW + 160, LH + 50, LS_8, FS, ZL);

		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + -60, number, LW + 192, LH + 1000, LS_7, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 290, number, LW + 130, LH + 50, LS_7, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 135, number, LW + 150, LH + 50, LS_7, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 425, number, LW + 160, LH + 50, LS_7, FS, ZL);

		}

		if (fpmx.bMxFontSize)
		{
			if (fpmx.fyxmxx[i].sSpmc.GetLength() % 12 == 0)
			{
				number += fpmx.fyxmxx[i].sSpmc.GetLength() / 12 * 30;
			}
			else
			{
				number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 12 + 1) * 30;
			}
		}
		else
		{
			if (fpmx.fyxmxx[i].sSpmc.GetLength() % 14 == 0)
			{
				number += fpmx.fyxmxx[i].sSpmc.GetLength() / 14 * 25;
			}
			else
			{
				number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 14 + 1) * 25;
			}
		}

		//		if(fpmx.fyxmxx[i].sSpmc.GetLength() == 0)
		//		{
		//			number += 30;	
		//		}
		//		else if(fpmx.fyxmxx[i].sSpmc.GetLength() % 20 == 0)
		//		{
		//			number += fpmx.fyxmxx[i].sSpmc.GetLength() / 12 * 30;
		//		}
		//		else
		//		{
		//			number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 12 + 1) * 30;
		//  		}
		//		if(fpmx.fyxmxx[i].sSpmc.GetLength() < 13)
		//		{
		//			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,number    ,LW+180        ,LH+1000     ,LS_8          ,FS    ,ZL );
		//		}
		//		else
		//		{
		//			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,number    ,LW+180        ,LH+1000     ,LS_5          ,FS    ,ZL );
		//		}
		//		xywhsf(fpmx.fyxmxx[i].ssSpsl         ,LX+ 305                   ,LY+ 610+i*80 - 30    ,LW+110        ,LH+ 80      ,LS_8          ,FS    ,ZC );
		//		xywhsf(fpmx.fyxmxx[i].ssHsdj         ,LX+ 135                   ,LY+ 610+i*80 - 30    ,LW+150        ,LH+ 80      ,LS_8          ,FS    ,ZR );
		//		xywhsf(fpmx.fyxmxx[i].ssHsje         ,LX+ 435                   ,LY+ 610+i*80 - 30    ,LW+150        ,LH+ 80      ,LS_8          ,FS    ,ZL );
	}
	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		xywhsf(fpmx.yfphm, LX + -60, LY + 960 - 30 + 265, LW, LH, LS_8, FS, ZL);
		xywhsf(fpmx.yfpdm, LX + -60, LY + 920 - 30 + 265, LW, LH, LS_8, FS, ZL);
	}

	if (fpmx.sJshjDx.GetLength() > 34)
	{
		xywhsf(fpmx.jshjDx, LX + 70, LY + 1040 - 30 + 265, LW, LH, LS_7, FS, ZL);   //大写价税合计
	}
	else
	{
		xywhsf(fpmx.jshjDx, LX + 70, LY + 1040 - 30 + 265, LW, LH, LS_8, FS, ZL);   //大写价税合计
	}
	xywhsf(fpmx.jshj, LX + 70, LY + 1000 - 30 + 265, LW, LH, LS_8, FS, ZL);   //小写价税合计

	xywhsf(fpmx.Xhdwmc, LX + 70, LY + 270 - 30, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 100, LY + 340 - 10, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 360, LY + 380 - 10, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 10, LY + 1080 - 30 + 265, LW, LH, LS_8, FS, ZL);   //小写价税合计
	if (fpmx.sBz.GetLength() < 85)
	{
		xywhsf(fpmx.bz, LX + -60, LY + 865 - 30 + 265, LW + 620, LH + 60, LS_8, FS, ZL);
	}
	else if (fpmx.sBz.GetLength() > 84 && fpmx.sBz.GetLength() < 97)
	{
		xywhsf(fpmx.bz, LX + -60, LY + 865 - 30 + 265, LW + 620, LH + 50, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.bz, LX + -60, LY + 865 - 30 + 265, LW + 620, LH + 60, LS_6, FS, ZL);
	}
	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		addxml(fpmx.sYfpdm, fpmx.yfpdm);
		addxml(fpmx.sYfphm, fpmx.yfphm);
	}
	addxml(fpmx.sBz, fpmx.bz);
	addxml(fpmx.sSkpbh, fpmx.Skpbh);
	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	char cstrTemp[2] = { 0 };
	CString strTemp;

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		if (fpmx.bMxFontSize)
		{
			for (int i = 12; i < fpmx.fyxmxx[j].sSpmc.GetLength(); i += 12)
			{
				strTemp = fpmx.fyxmxx[j].sSpmc.Mid(i, 1);
				strcpy(cstrTemp, strTemp.GetBuffer(0));
				if (!(cstrTemp[0] & 0x80))
				{
					fpmx.fyxmxx[j].sSpmc.Insert(i + 1, "\r\n");
					i += 2;
				}
			}
		}
		else
		{
			for (int i = 14; i < fpmx.fyxmxx[j].sSpmc.GetLength(); i += 14)
			{
				strTemp = fpmx.fyxmxx[j].sSpmc.Mid(i, 1);
				strcpy(cstrTemp, strTemp.GetBuffer(0));
				if (!(cstrTemp[0] & 0x80))
				{
					fpmx.fyxmxx[j].sSpmc.Insert(i + 1, "\r\n");
					i += 2;
				}
			}
		}
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	//	addxml(fpmx.sHjje, fpmx.hjje);
	//	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlG(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	//				            X                               Y              W	       H	         S		      F      Z
	xywhsf(fpmx.Skpbh, LX + 240, LY + 260 - 20, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 240, LY + 260 - 60, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.kprq.year, LX + 240 - 10, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + 310 - 10, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 330 - 10, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 365 - 10, LY + 460 - 15, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + 385 - 10, LY + 460 - 15, LW, LH, LS_9, FS, ZL);

	xywhsf(fpmx.Ghdwmc, LX + 100, LY + 560, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Ghdwsbh, LX + 290, LY + 650, LW, LH, LS_8, FS, ZL);

	int number = LY + 730;
	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{
		/*
		if(fpmx.fyxmxx[i].sSpmc.GetLength() < 16)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ 100                   ,number+i*30    ,LW+130        ,LH+1000     ,LS_8          ,FS    ,ZL );
		}
		else if(fpmx.fyxmxx[i].sSpmc.GetLength() > 16 && fpmx.fyxmxx[i].sSpmc.GetLength() < 21)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ 100                   ,number+i*30    ,LW+120        ,LH+1000     ,LS_6          ,FS    ,ZL );
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ 100                   ,number+i*30    ,LW+130        ,LH+1000     ,LS_5          ,FS    ,ZL );
		}
		*/
		//		xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ 100                   ,number    ,LW+120        ,LH+ 1000     ,LS_8          ,FS    ,ZL );
		//		xywhsf(fpmx.fyxmxx[i].ssSpsl         ,LX+ 370                   ,number+i*30    ,LW+90        ,LH+ 1000      ,LS_8          ,FS    ,ZC );
		//		xywhsf(fpmx.fyxmxx[i].ssHsdj         ,LX+ 235                   ,number+i*30    ,LW+120        ,LH+ 1000      ,LS_8          ,FS    ,ZR );
		//		xywhsf(fpmx.fyxmxx[i].ssHsje         ,LX+ 475                   ,number+i*30    ,LW+120        ,LH+ 1000      ,LS_8          ,FS    ,ZL );


		if (fpmx.bMxFontSize)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 100, number, LW + 135, LH + 1000, LS_8, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 370, number, LW + 90, LH + 30, LS_8, FS, ZC);
			xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 245, number, LW + 120, LH + 30, LS_8, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 465, number, LW + 130, LH + 30, LS_8, FS, ZR);
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 100, number, LW + 140, LH + 1000, LS_7, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 370, number, LW + 90, LH + 30, LS_7, FS, ZC);
			xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 245, number, LW + 120, LH + 30, LS_7, FS, ZL);
			xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 465, number, LW + 130, LH + 30, LS_7, FS, ZR);
		}

		if (fpmx.bMxFontSize)
		{
			if (fpmx.fyxmxx[i].sSpmc.GetLength() % 8 == 0)
			{
				number += fpmx.fyxmxx[i].sSpmc.GetLength() / 8 * 30;
			}
			else
			{
				number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 8 + 1) * 30;
			}
		}
		else
		{
			if (fpmx.fyxmxx[i].sSpmc.GetLength() % 10 == 0)
			{
				number += fpmx.fyxmxx[i].sSpmc.GetLength() / 10 * 25;
			}
			else
			{
				number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 10 + 1) * 25;
			}
		}

		//		if(fpmx.fyxmxx[i].sSpmc.GetLength() == 0)
		//		{
		//			number += 30;	
		//		}
		//		else if(fpmx.fyxmxx[i].sSpmc.GetLength() % 14 == 0)
		//		{
		//			number += fpmx.fyxmxx[i].sSpmc.GetLength() / 14 * 30;
		//		}
		//		else
		//		{
		//			number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 14 + 1) * 20;
		//  	}
	}

	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		xywhsf(fpmx.yfphm, LX + 100, LY + 975 + 225, LW, LH, LS_8, FS, ZL);
		xywhsf(fpmx.yfpdm, LX + 100, LY + 945 + 225, LW, LH, LS_8, FS, ZL);
	}
	xywhsf(fpmx.jshj, LX + 230, LY + 1010 + 225, LW, LH, LS_8, FS, ZL);   //小写价税合计
	if (fpmx.sJshjDx.GetLength() <= 26)
	{
		xywhsf(fpmx.jshjDx, LX + 230, LY + 1050 + 225, LW, LH, LS_8, FS, ZL);   //大写价税合计
	}
	else if (fpmx.sJshjDx.GetLength() > 26 && fpmx.sJshjDx.GetLength() <= 28)
	{
		xywhsf(fpmx.jshjDx, LX + 230, LY + 1050 + 225, LW, LH, LS_7, FS, ZL);   //大写价税合计
	}
	else
	{
		xywhsf(fpmx.jshjDx, LX + 230, LY + 1050 + 225, LW, LH, LS_6, FS, ZL);   //大写价税合计
	}
	xywhsf(fpmx.Xhdwmc, LX + 100, LY + 315, LW + 500, LH + 120, LS_8, FS, ZL);
	xywhsf(fpmx.Xhdwsbh, LX + 290, LY + 405, LW, LH, LS_8, FS, ZL);

	xywhsf(fpmx.skr, LX + 200, LY + 490, LW, LH, LS_8, FS, ZL);
	xywhsf(fpmx.Jym, LX + 200, LY + 1090 + 225, LW, LH, LS_8, FS, ZL);

	if (fpmx.sBz.GetLength() < 103)
	{
		xywhsf(fpmx.bz, LX + 100, LY + 850 - 30 + 265, LW + 500, LH + 120, LS_8, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.bz, LX + 100, LY + 850 - 30 + 265, LW + 510, LH + 70, LS_7, FS, ZL);
	}

	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		addxml(fpmx.sYfpdm, fpmx.yfpdm);
		addxml(fpmx.sYfphm, fpmx.yfphm);
	}
	addxml(fpmx.sBz, fpmx.bz);
	addxml(fpmx.sSkpbh, fpmx.Skpbh);

	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);

	char cstrTemp[2] = { 0 };
	CString strTemp;

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		if (fpmx.bMxFontSize)
		{
			for (int i = 8; i < fpmx.fyxmxx[j].sSpmc.GetLength(); i += 8)
			{
				strTemp = fpmx.fyxmxx[j].sSpmc.Mid(i, 1);
				strcpy(cstrTemp, strTemp.GetBuffer(0));
				if (!(cstrTemp[0] & 0x80))
				{
					fpmx.fyxmxx[j].sSpmc.Insert(i + 1, "\r\n");
					i += 2;
				}
			}
		}
		else
		{
			for (int i = 10; i < fpmx.fyxmxx[j].sSpmc.GetLength(); i += 10)
			{
				strTemp = fpmx.fyxmxx[j].sSpmc.Mid(i, 1);
				strcpy(cstrTemp, strTemp.GetBuffer(0));
				if (!(cstrTemp[0] & 0x80))
				{
					fpmx.fyxmxx[j].sSpmc.Insert(i + 1, "\r\n");
					i += 2;
				}
			}
		}
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	//	addxml(fpmx.sHjje, fpmx.hjje);
	//	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

CString CJsfpdy::GenerateItemXmlH(JSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;
	//				            X                               Y              W	       H	         S		      F      Z
	fpmx.sSkpbh = "机器编号:" + fpmx.sSkpbh;
	xywhsf(fpmx.Skpbh, LX + 210, LY + 290, LW, LH, LS_8, FS, ZL);
	fpmx.sFphm = "机打号码:" + fpmx.sFphm;
	xywhsf(fpmx.Fphm, LX + -60, LY + 290, LW, LH, LS_8, FS, ZL);

	int date = -60;
	fpmx.kprq.syear = "开票日期:" + fpmx.kprq.syear;
	xywhsf(fpmx.kprq.year, LX + date, LY + 440, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Year, LX + date + 70 + 140, LY + 440, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.mouth, LX + date + 90 + 140, LY + 440, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + date + 120 + 140, LY + 440, LW, LH, LS_9, FS, ZL);
	xywhsf(fpmx.kprq.day, LX + date + 140 + 140, LY + 440, LW, LH, LS_9, FS, ZL);

	fpmx.sGhdwmc = "                    " + fpmx.sGhdwmc;
	xywhsf(fpmx.Ghdwmc, LX + -60, LY + 170, LW + 600, LH + 120, LS_8, FS, ZL);
	fpmx.sGhdwsbh = "购货方税号:" + fpmx.sGhdwsbh;
	xywhsf(fpmx.Ghdwsbh, LX + -60, LY + 260, LW, LH, LS_8, FS, ZL);
	fpmx.sXmmc = "项目          含税单价    数量     含税金额";
	xywhsf(fpmx.xmmc, LX + -60, LY + 470, LW, LH, LS_8, FS, ZL);

	int number = LY + 500;
	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{
		xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + -60, number, LW + 180, LH + 1000, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 305, number, LW + 110, LH + 1000, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssHsdj, LX + 135, number, LW + 150, LH + 1000, LS_8, FS, ZL);
		xywhsf(fpmx.fyxmxx[i].ssHsje, LX + 435, number, LW + 150, LH + 1000, LS_8, FS, ZL);

		if (fpmx.fyxmxx[i].sSpmc.GetLength() == 0)
		{
			number += 30;
		}
		else if (fpmx.fyxmxx[i].sSpmc.GetLength() % 12 == 0)
		{
			number += fpmx.fyxmxx[i].sSpmc.GetLength() / 12 * 30;
		}
		else
		{
			number += (fpmx.fyxmxx[i].sSpmc.GetLength() / 12 + 1) * 30;
		}
		//		xywhsf(fpmx.fyxmxx[i].ssSpmc		 ,LX+ -60                   ,LY+ 610+i*80 - 30    ,LW+180        ,LH+ 80     ,LS_8          ,FS    ,ZL );
		//		xywhsf(fpmx.fyxmxx[i].ssSpsl         ,LX+ 305                   ,LY+ 610+i*80 - 30    ,LW+110        ,LH+ 80      ,LS_8          ,FS    ,ZC );
		//		xywhsf(fpmx.fyxmxx[i].ssHsdj         ,LX+ 135                   ,LY+ 610+i*80 - 30    ,LW+150        ,LH+ 80      ,LS_8          ,FS    ,ZR );
		//		xywhsf(fpmx.fyxmxx[i].ssHsje         ,LX+ 435                   ,LY+ 610+i*80 - 30    ,LW+150        ,LH+ 80      ,LS_8          ,FS    ,ZL );
	}
	fpmx.sHjje = "合计金额(小写):" + fpmx.sHjje;
	xywhsf(fpmx.hjje, LX + -60, LY + 880 - 70, LW, LH, LS_8, FS, ZL);
	fpmx.sHjse = "合计税额(小写):" + fpmx.sHjse;
	xywhsf(fpmx.hjse, LX + -60, LY + 910 - 70, LW, LH, LS_8, FS, ZL);
	fpmx.sJshjDx = "价税合计(大写):" + fpmx.sJshjDx;
	xywhsf(fpmx.jshjDx, LX + -60, LY + 970 - 70, LW, LH, LS_7, FS, ZL);   //大写价税合计
	fpmx.sJshj = "价税合计(小写):" + fpmx.sJshj;
	xywhsf(fpmx.jshj, LX + -60, LY + 940 - 70, LW, LH, LS_8, FS, ZL);   //小写价税合计

	fpmx.sXhdwmc = "销售方名称:" + fpmx.sXhdwmc;
	xywhsf(fpmx.Xhdwmc, LX + -60, LY + 320, LW + 560, LH + 80, LS_8, FS, ZL);
	fpmx.sXhdwsbh = "销售方税号:" + fpmx.sXhdwsbh;
	xywhsf(fpmx.Xhdwsbh, LX + -60, LY + 410, LW, LH, LS_8, FS, ZL);

	fpmx.sSkr = "收款员:" + fpmx.sSkr;
	xywhsf(fpmx.skr, LX + 270, LY + 440, LW, LH, LS_8, FS, ZL);
	fpmx.sJym = "校验码:" + fpmx.sJym;
	xywhsf(fpmx.Jym, LX + -60, LY + 1000 - 70, LW, LH, LS_8, FS, ZL);   //小写价税合计

	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		xywhsf(fpmx.yfphm, LX + -60, LY + 740, LW, LH, LS_8, FS, ZL);
		xywhsf(fpmx.yfpdm, LX + -60, LY + 770, LW, LH, LS_8, FS, ZL);
	}

	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		addxml(fpmx.sYfpdm, fpmx.yfpdm);
		addxml(fpmx.sYfphm, fpmx.yfphm);
	}
	addxml(fpmx.sSkpbh, fpmx.Skpbh);
	addxml(fpmx.sJym, fpmx.Jym);
	addxml(fpmx.sFphm, fpmx.Fphm);

	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);
	addxml(fpmx.sXmmc, fpmx.xmmc);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sHsdj, fpmx.fyxmxx[j].ssHsdj);
		addxml(fpmx.fyxmxx[j].sHsje, fpmx.fyxmxx[j].ssHsje);
	}

	addxml(fpmx.sHjje, fpmx.hjje);
	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);
	addxml(fpmx.sSkr, fpmx.skr);

	return xml.GetDoc();
}

LONG CJsfpdy::Print(LPCTSTR billXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	InitXYoff();

	do
	{
		CMarkup xml;
		if (!xml.SetDoc(billXml) || !xml.FindElem("Print"))
		{
			nrt = -5;// 打印内容解析失败
			break;
		}
		xml.IntoElem();

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

		nrt = ::StartPage(m_hPrinterDC);
		if (nrt <= 0)
		{
			::EndDoc(m_hPrinterDC);
			nrt = -4;// 发送打印内容失败
			break;
		}

		int n_x = 100;

		while (xml.FindElem("Item"))
		{
			/*RECT itemRect;

			int x = atoi(xml.GetAttrib("x"));
			int y = atoi(xml.GetAttrib("y"));
			int w = atoi(xml.GetAttrib("w"));
			int h = atoi(xml.GetAttrib("h"));
			int nFontSize = atoi(xml.GetAttrib("s"));
			CString strFontName = xml.GetAttrib("f");
			int z = atoi(xml.GetAttrib("z"));
			int fc = atoi(xml.GetAttrib("fc"));
			CString strText = xml.GetData();

			itemRect.left = x + nXoff + 100;
			itemRect.top = (-y - 5 - nYoff);
			itemRect.right = x + nXoff + 100 + w;
			itemRect.bottom = (-y + 5 - h - nYoff);


			if (w == 0 && h == 0)
			{
				CFont *pOldFont;
				CFont fontHeader;
				fontHeader.CreatePointFont(nFontSize, strFontName, CDC::FromHandle(m_hPrinterDC));
				pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));
				::TextOut(m_hPrinterDC, itemRect.left, itemRect.top, strText, strText.GetLength());
				::SelectObject(m_hPrinterDC, pOldFont);
				fontHeader.DeleteObject();
			}
			else
			{
				PaintTile(nFontSize, strFontName, itemRect, strText, z, fc);
			}*/
			RECT printRect;
			CFont ftPrint;
			CString strText = xml.GetData();
			int x = atoi(xml.GetAttrib("x"));
			int y = atoi(xml.GetAttrib("y"));
			int w = atoi(xml.GetAttrib("w"));
			int h = atoi(xml.GetAttrib("h"));
			int nFontSize = atoi(xml.GetAttrib("s"));
			CString strFontName = xml.GetAttrib("f");
			int z = atoi(xml.GetAttrib("z"));
			ftPrint.CreatePointFont(nFontSize, strFontName, CDC::FromHandle(m_hPrinterDC));
			::SelectObject(m_hPrinterDC, ftPrint);
			printRect.left = x + nXoff + 100;
			printRect.top = -(y - 5 + nYoff);

			if (w == 0 && h == 0)
			{
				::TextOut(m_hPrinterDC, printRect.left, printRect.top, strText, strText.GetLength());
			}
			else
			{
				printRect.right = x + nXoff + w + 100;
				printRect.bottom = -((y + 5 + nYoff) + h);

				RECT testRect = printRect;

				if (z == 0)
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &testRect,/*DT_NOCLIP*/DT_EDITCONTROL | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);
				}
				else if (z == 2)
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &testRect, DT_EDITCONTROL | DT_WORDBREAK | DT_CALCRECT | DT_CENTER | DT_NOPREFIX);
				}
				else
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &testRect,/*DT_NOCLIP*/DT_EDITCONTROL | DT_WORDBREAK | DT_CALCRECT | DT_RIGHT | DT_NOPREFIX);
				}

				if (printRect.right >= testRect.right)
				{
					if (z == 0)
					{
						::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect,/*DT_NOCLIP | DT_SINGLELINE*/DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX);
					}
					else if (z == 2)
					{
						::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect, DT_EDITCONTROL | DT_WORDBREAK | DT_CENTER | DT_NOPREFIX);
					}
					else
					{
						::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect,/*DT_NOCLIP | DT_SINGLELINE|*/ DT_EDITCONTROL | DT_WORDBREAK | DT_RIGHT | DT_NOPREFIX);
					}
				}
				else
				{
					if (z == 0)
					{
						::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect,/*DT_NOCLIP |*/DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX);
					}
					else if (z == 2)
					{
						::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect, DT_EDITCONTROL | DT_WORDBREAK | DT_CENTER | DT_NOPREFIX);
					}
					else
					{
						::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect,/*DT_NOCLIP |*/DT_EDITCONTROL | DT_WORDBREAK | DT_RIGHT | DT_NOPREFIX);
					}
				}
			}
			ftPrint.DeleteObject();
		}

		// 输出大写金额开头圈叉符号
		int x, y, tx, ty;
		bool zsfp = xml.FindElem("OX");//正数发票
		if (xml.FindElem("OX"))
		{
			x = atoi(xml.GetAttrib("x"));
			y = atoi(xml.GetAttrib("y"));
			tx = x + nXoff;
			ty = -(y + nYoff);

			::Ellipse(m_hPrinterDC, tx + n_x - 20, ty + 10, tx + 30 + n_x - 20, ty - 30 + 10);
			::MoveToEx(m_hPrinterDC, tx + 4 + n_x - 20, ty - 4 + 10, NULL);
			::LineTo(m_hPrinterDC, tx + 25 + n_x - 20, ty - 25 + 10);
			::MoveToEx(m_hPrinterDC, tx + 24 + n_x - 20, ty - 4 + 10, NULL);
			::LineTo(m_hPrinterDC, tx + 4 + n_x - 20, ty - 24 + 10);
		}
		::EndPage(m_hPrinterDC);
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}