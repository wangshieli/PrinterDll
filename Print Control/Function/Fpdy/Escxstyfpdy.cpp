#include "../../pch.h"
#include "Escxstyfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"

#include "../../Helper/QRGenerator/QRControl.h"

CEscxstyfp::CEscxstyfp()
{

}

CEscxstyfp::~CEscxstyfp()
{

}

CString CEscxstyfp::Dlfpdy(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	ESCFP_FPXX fpmx;
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
	//if (xml.FindElem("orientation")) m_nOrientation = atoi(xml.GetData());
	m_iPldy = atoi(fpdy.sDyfs.GetBuffer(0));

	m_sTempFplxdm = fpdy.sFplxdm;

	fpmx = ParseFpmxFromXML(sInputInfo, fpdy);

	if (fpdy.sDylx.CompareNoCase("1") == 0)
	{
		fpdy.sErrorCode = "-3";
		fpdy.sErrorInfo = "二手车销售统一发票没有清单，不能以清单形式打印";
		return GenerateXMLFpdy(fpdy);
	}

	if (m_sPrintTaskNameFlag.Compare("1") == 0)
	{
		m_sPrintTaskName.Format("%s%s%s", fpdy.sFpdm.GetBuffer(0), "-", fpdy.sFphm.GetBuffer(0));
	}
	else
	{
		m_sPrintTaskName.Format("%s", "printer");
	}

	printXml = GenerateFpdyXml(fpmx, fpdy.sDylx, fpdy);

	if (fpdy.sDylx.CompareNoCase("0") == 0)
	{
		rtn = Print(printXml);
	}

	return GenerateXMLFpdy(fpdy, rtn);
}

#include <atlimage.h>

LONG CEscxstyfp::Print(LPCTSTR billXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	InitXYoff();

	nrt = InitPrinter(FPWidth, 1778);
	if (0 != nrt)
		return nrt;

	DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };

	do
	{
		CMarkup xml;
		if (!xml.SetDoc(billXml) || !xml.FindElem("Print"))
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

		nrt = ::StartPage(m_hPrinterDC);
		if (nrt <= 0)
		{
			::EndDoc(m_hPrinterDC);
			nrt = -4;// 发送打印内容失败
			break;
		}

		if (m_sFpzt.Compare("02") != 0)
		{
			//char sTempPath[MAX_PATH];
			//strcpy(sTempPath, m_cQRcodePath);
			//strcat(sTempPath, "\\Ewm.bmp");

			//CxImage image;
			//image.Load(sTempPath);
			//image.Stretch(m_hPrinterDC, 225 + nXoff, -90 - nYoff, 160, -160, SRCCOPY);

			CQRControl cc;
			cc.funcc(m_sEwm, NULL, m_hPrinterDC, 225 + nXoff, -90 - nYoff, 160, -160, SRCCOPY);
			
			//HBITMAP hBitmap = (HBITMAP)::LoadImage(
			//	NULL,					// 模块实例句柄(要加载的图片在其他DLL中时)
			//	sTempPath,				// 位图路径
			//	IMAGE_BITMAP,			// 位图类型
			//	0,						// 指定图片的宽
			//	0,						// 指定图片的高
			//	LR_LOADFROMFILE | LR_CREATEDIBSECTION);		// 从路径处加载图片

			//BITMAP bitmap;
			//::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			//HDC dcMem;
			//dcMem = ::CreateCompatibleDC(m_hPrinterDC);
			//HBITMAP hOldBmp = (HBITMAP)::SelectObject(dcMem, hBitmap);

			//if (nQRCodeSize > 0) // isZero
			//{
			//	int nScaledWidth = nQRCodeSize;	//GetDeviceCaps (m_hPrinterDC, HORZRES);
			//	int nScaledHeight = nQRCodeSize;	//GetDeviceCaps (m_hPrinterDC, VERTRES);
			//	::StretchBlt(m_hPrinterDC, nXoff + 20 + 180 - nQRCodeSize + 20, -(nYoff + 40 + (180 - nQRCodeSize) - 50), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			//}
			//else
			//{
			//	int nScaledWidth = 160;	//GetDeviceCaps (m_hPrinterDC, HORZRES);
			//	int nScaledHeight = 160;	//GetDeviceCaps (m_hPrinterDC, VERTRES);
			//	::StretchBlt(m_hPrinterDC, 225 + nXoff, -90 - nYoff, nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			//}

			//::SelectObject(dcMem, hOldBmp);
			//::DeleteDC(dcMem);
			//::DeleteObject(hBitmap);
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
			int z1 = atoi(xml.GetAttrib("z"));
			int fc = atoi(xml.GetAttrib("fc"));
			CString strText = xml.GetData();

			itemRect.left = x + nXoff + 200;
			itemRect.top = (-y - nYoff - 320);
			itemRect.right = x + nXoff + 200 + w;
			itemRect.bottom = (-y - h - nYoff - 320);

			int z = z1 & 0x000000ff;
			int ls = z1 & 0xff00ff00;

			if (COIN_Y == (ls & 0xff000000) || COIN_O == (ls & 0xff000000))
			{
				PaintTile4(nFontSize, fc, strFontName, itemRect, strText, z1);
			}

			PaintTile(nFontSize, fc, strFontName, itemRect, strText, z);
		}

		::EndPage(m_hPrinterDC);
		::EndDoc(m_hPrinterDC);
	} while (--m_nCopies);

	return nrt;
}

ESCFP_FPXX CEscxstyfp::ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy)
{
	CMarkup xml;
	ESCFP_FPXX fpxx;
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
	if (xml.FindElem("fpdm"))	 fpxx.sFpdm = xml.GetData();
	if (xml.FindElem("fphm"))	 fpxx.sFphm = xml.GetData();
	if (xml.FindElem("fpzt"))	 fpxx.sFpzt = xml.GetData();
	if (xml.FindElem("kprq"))	 fpxx.sKprq = xml.GetData();
	if (xml.FindElem("jqbh"))	 fpxx.sJqbh = xml.GetData();
	if (xml.FindElem("skm"))		 fpxx.sSkm = xml.GetData();
	if (xml.FindElem("jym"))		 fpxx.sJym = xml.GetData();
	if (xml.FindElem("gfmc"))	 fpxx.sGfmc = xml.GetData();
	if (xml.FindElem("gfdm"))	 fpxx.sGfsfzhm = xml.GetData();
	if (xml.FindElem("gfdz"))	 fpxx.sGfdz = xml.GetData();
	if (xml.FindElem("gfdh"))	 fpxx.sGfdh = xml.GetData();
	if (xml.FindElem("xfmc"))	 fpxx.sMfmc = xml.GetData();
	if (xml.FindElem("xfdm"))	 fpxx.sMfsfzhm = xml.GetData();
	if (xml.FindElem("xfdz"))	 fpxx.sMfdz = xml.GetData();
	if (xml.FindElem("xfdh"))	 fpxx.sMfdh = xml.GetData();
	if (xml.FindElem("cpzh"))	 fpxx.sCpzh = xml.GetData();
	if (xml.FindElem("djzh"))	 fpxx.sDjzh = xml.GetData();
	if (xml.FindElem("cllx"))	 fpxx.sCllx = xml.GetData();
	if (xml.FindElem("clsbh"))	 fpxx.sCjhm = xml.GetData();
	if (xml.FindElem("cpxh"))	 fpxx.sCpxh = xml.GetData();
	if (xml.FindElem("zrdclglsmc")) fpxx.sZrdclglsmc = xml.GetData();
	if (xml.FindElem("jypmdwmc")) fpxx.sJypmdw = xml.GetData();
	if (xml.FindElem("jypmdwdz")) fpxx.sJypmdwdz = xml.GetData();
	if (xml.FindElem("jypmdwsbh")) fpxx.sJypmdwnsrsbh = xml.GetData();
	if (xml.FindElem("jypmdwyhzh")) fpxx.sJypmdwyhzh = xml.GetData();
	if (xml.FindElem("jypmdwdh")) fpxx.sJypmdwdh = xml.GetData();
	if (xml.FindElem("escscmc"))  fpxx.sEscsc = xml.GetData();
	if (xml.FindElem("escscsbh")) fpxx.sEscscnsrsbh = xml.GetData();
	if (xml.FindElem("escscdz"))  fpxx.sEscscdz = xml.GetData();
	if (xml.FindElem("escscyhzh"))fpxx.sEscscyhzh = xml.GetData();
	if (xml.FindElem("escscdh"))  fpxx.sEscscdh = xml.GetData();
	if (xml.FindElem("cjhj"))	 fpxx.sCjhj = xml.GetData();
	if (xml.FindElem("kpr"))		 fpxx.sKpr = xml.GetData();
	if (xml.FindElem("bz"))		 fpxx.sBz = xml.GetData();

	if (fpxx.sFpzt.CompareNoCase("02") == 0)
	{
		m_sFpzt = fpxx.sFpzt;
		return fpxx;
	}

	fpxx.sCjhj.TrimLeft("-");

	char strDxje[100];
	char strsc[100];

	strcpy(strsc, fpxx.sCjhj.GetBuffer(0));
	PCLib_ChineseFee(strDxje, 100, strsc);//小写金额转换为大写金额
	fpxx.sCjhjDx.Format("%s", strDxje);
	if (fpxx.sCjhjDx.Mid(fpxx.sCjhjDx.GetLength() - 2, 2).CompareNoCase("角") == 0)
	{
		fpxx.sCjhjDx += "整";
	}

	if (fpxx.sFpzt == "01")
	{
		fpxx.sCjhj = "-" + fpxx.sCjhj;
		fpxx.sCjhjDx = "负数:" + fpxx.sCjhjDx;
	}

	CString strFplx = "15";

	if (m_sFpzt.CompareNoCase("02") != 0)
	{
		fpxx.sKprq = fpxx.sKprq.Left(8);
		m_sEwm = "01," + strFplx + "," + fpxx.sFpdm + "," + fpxx.sFphm + "," + fpxx.sCjhj + "," + fpxx.sKprq + "," + fpxx.sSkm;
	}

	fpxx.kprq.syear = fpxx.sKprq.Mid(0, 4);
	fpxx.kprq.smouth = fpxx.sKprq.Mid(4, 2);
	fpxx.kprq.sday = fpxx.sKprq.Mid(6, 2);
	fpxx.sKprq = fpxx.kprq.syear + "-" + fpxx.kprq.smouth + "-" + fpxx.kprq.sday;

	return fpxx;
}

CString CEscxstyfp::GenerateFpdyXml(ESCFP_FPXX fpmx, CString dylx, FPDY fpdy)
{
	CString xml;

	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";
	if (fpmx.sFpzt.CompareNoCase("02") != 0)
	{
		if (dylx.CompareNoCase("0") == 0)
		{
			xml += GenerateItemXml(fpmx, fpdy);
		}
	}
	xml += "</Print>";

	return xml;
}

CString CEscxstyfp::GenerateItemXml(ESCFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;

	xywhsf(fpmx.Kprq, 180, -55, 270, 50, LS_12, FT, AM_VCL);

	xywhsf(fpmx.Fpdm, 220, 10, 680, 60, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Fphm, 220, 80, 680, 60, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Jqbh, 220, 150, 680, 60, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Skm, 1020, 0, 960, 220, LS_12, FT, AM_VCL);

	xywhsf_fc(fpmx.Gfmc, 420, 225, 630, 70, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Gfsfzhm, 1460, 225, 520, 70, LS_11, FT, AM_VCL);
	xywhsf_fc(fpmx.Gfdz, 420, 305, 1000, 70, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Gfdh, 1580, 305, 400, 70, LS_12, FT, AM_VCL, 10);

	xywhsf_fc(fpmx.Mfmc, 420, 380, 630, 70, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Mfsfzhm, 1460, 380, 520, 70, LS_11, FT, AM_VCL);
	xywhsf_fc(fpmx.Mfdz, 420, 460, 1000, 70, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Mfdh, 1580, 460, 400, 70, LS_12, FT, AM_VCL, 10);

	xywhsf_fc(fpmx.Cpzh, 420, 535, 320, 70, LS_12, FT, AM_VCL, 10);
	xywhsf_fc(fpmx.Djzh, 950, 535, 380, 70, LS_11, FT, AM_VCL, 10);
	xywhsf_fc(fpmx.Cllx, 1580, 535, 400, 70, LS_10, FS, AM_VCL, 10);

	xywhsf_fc(fpmx.Cjhm, 420, 615, 320, 70, LS_11, FT, AM_VCL, 10);
	xywhsf_fc(fpmx.Cpxh, 950, 615, 380, 70, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Zrdclglsmc, 1580, 615 + 5, 420, 70 - 7, LS_10, FS, AM_VCL);

	if (fpmx.sFpzt.Compare("00") == 0)
	{
		//xywhsf(fpmx.OX, 459, 720, 78, 70, LS_9, FT, ZVC);
		xywhsf(fpmx.CjhjDx, 480, 690, 940, 70, LS_10, FS, AM_VCL_S | COIN_O);
	}
	else
	{
		xywhsf(fpmx.CjhjDx, 420, 690, 1000, 70, LS_10, FS, AM_VCL);
	}
	//xywhsf(fpmx.OX, 459, 720, 78, 70, LS_9, FT, ZVC);
	//xywhsf(fpmx.CjhjDx, 480, 690, 940, 70, LS_10, FS, AM_VCL);
	xywhsf(fpmx.Cjhj, 1630, 690, 350, 70, LS_12, FT, AM_VCL_S | COIN_Y);

	xywhsf(fpmx.Jypmdw, 420, 770, 1560, 70, LS_10, FS, AM_VCL);
	xywhsf_fc(fpmx.Jypmdwdz, 420, 850, 830, 70, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Jypmdwnsrsbh, 1460, 850, 520, 70, LS_11, FT, AM_VCL);
	xywhsf_fc(fpmx.Jypmdwyhzh, 420, 925, 1130, 70, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Jypmdwdh, 1710, 925, 270, 70, LS_11, FT, AM_VCL, 10);
	
	xywhsf(fpmx.Escsc, 420, 1005, 580, 145, LS_10, FS, AM_VCL);
	xywhsf(fpmx.Escscnsrsbh, 1220, 1005, 760, 70, LS_11, FT, AM_VCL);
	xywhsf_fc(fpmx.Escscdz, 1220, 1080, 760, 70, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Escscyhzh, 420, 1160, 1130, 70, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Escscdh, 1710, 1160, 270, 70, LS_11, FT, AM_VCL, 10);

	xywhsf_fc(fpmx.Bz, 200, 1235, 1780, 70, LS_10, FS, AM_VCL, 10);
	
	xywhsf(fpmx.Kpr, 1440, 1320, 260, 60, LS_10, FS, AM_VCL);

	addxml(fpmx.sKprq, fpmx.Kprq);
	addxml(fpmx.sFpdm, fpmx.Fpdm);
	addxml(fpmx.sFphm, fpmx.Fphm);
	addxml(fpmx.sJqbh, fpmx.Jqbh);
	addxml(fpmx.sSkm, fpmx.Skm);

	addxml(fpmx.sGfmc, fpmx.Gfmc);
	addxml(fpmx.sGfsfzhm, fpmx.Gfsfzhm);
	addxml(fpmx.sGfdz, fpmx.Gfdz);
	addxml(fpmx.sGfdh, fpmx.Gfdh);

	addxml(fpmx.sMfmc, fpmx.Mfmc);
	addxml(fpmx.sMfsfzhm, fpmx.Mfsfzhm);
	addxml(fpmx.sMfdz, fpmx.Mfdz);
	addxml(fpmx.sMfdh, fpmx.Mfdh);

	addxml(fpmx.sCpzh, fpmx.Cpzh);
	addxml(fpmx.sDjzh, fpmx.Djzh);
	addxml(fpmx.sCllx, fpmx.Cllx);
	addxml(fpmx.sCjhm, fpmx.Cjhm);
	addxml(fpmx.sCpxh, fpmx.Cpxh);
	addxml(fpmx.sZrdclglsmc, fpmx.Zrdclglsmc);

	addxml(fpmx.sCjhjDx, fpmx.CjhjDx);
	addxml(fpmx.sCjhj, fpmx.Cjhj);
	addxml(fpmx.sJypmdw, fpmx.Jypmdw);
	addxml(fpmx.sJypmdwdz, fpmx.Jypmdwdz);
	addxml(fpmx.sJypmdwnsrsbh, fpmx.Jypmdwnsrsbh);
	addxml(fpmx.sJypmdwyhzh, fpmx.Jypmdwyhzh);
	addxml(fpmx.sJypmdwdh, fpmx.Jypmdwdh);
	addxml(fpmx.sEscsc, fpmx.Escsc);
	addxml(fpmx.sEscscdz, fpmx.Escscdz);
	addxml(fpmx.sEscscnsrsbh, fpmx.Escscnsrsbh);
	addxml(fpmx.sEscscyhzh, fpmx.Escscyhzh);
	addxml(fpmx.sEscscdh, fpmx.Escscdh);
	addxml(fpmx.sBz, fpmx.Bz);
	addxml(fpmx.sKpr, fpmx.Kpr);

	return xml.GetDoc();
}
