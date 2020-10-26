#include "../../pch.h"
#include "Jdcfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/QRGenerator/QRGenerator.h"
#include "../../Helper/QRGenerator/Base64.h"

CJdcfpdy::CJdcfpdy()
{
}

CJdcfpdy::~CJdcfpdy()
{
}

CString CJdcfpdy::Dlfpdy(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	JDCFP_FPXX fpmx;
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
		fpdy.sErrorInfo = "机动车销售统一发票没有清单，不能以清单形式打印";
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
		rtn = Print(printXml, fpmx.sZzsse);
	}

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CJdcfpdy::Print(LPCTSTR billXml, CString zzzse)
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

		nrt = InitPrinter(FPWidth, 1778);
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

		if (m_sFpzt.Compare("02") != 0)
		{
			char sTempPath[MAX_PATH];
			strcpy(sTempPath, m_cQRcodePath);
			strcat(sTempPath, "\\Ewm.bmp");
			HBITMAP hBitmap = (HBITMAP)::LoadImage(
				NULL,					// 模块实例句柄(要加载的图片在其他DLL中时)
				sTempPath,				// 位图路径
				IMAGE_BITMAP,			// 位图类型
				0,						// 指定图片的宽
				0,						// 指定图片的高
				LR_LOADFROMFILE | LR_CREATEDIBSECTION);		// 从路径处加载图片

			BITMAP bitmap;
			::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			HDC dcMem;
			dcMem = ::CreateCompatibleDC(m_hPrinterDC);
			HBITMAP hOldBmp = (HBITMAP)::SelectObject(dcMem, hBitmap);

			if (nQRCodeSize > 0) // isZero
			{
				int nScaledWidth = nQRCodeSize;	//GetDeviceCaps (m_hPrinterDC, HORZRES);
				int nScaledHeight = nQRCodeSize;	//GetDeviceCaps (m_hPrinterDC, VERTRES);
				::StretchBlt(m_hPrinterDC, nXoff + 20 + 180 - nQRCodeSize, -(nYoff + 40 + (180 - nQRCodeSize)), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}
			else
			{
				int nScaledWidth = 160;	//GetDeviceCaps (m_hPrinterDC, HORZRES);
				int nScaledHeight = 160;	//GetDeviceCaps (m_hPrinterDC, VERTRES);
				::StretchBlt(m_hPrinterDC, 220 + nXoff, -100 - nYoff, nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}

			::SelectObject(dcMem, hOldBmp);
			::DeleteDC(dcMem);
			::DeleteObject(hBitmap);

			int n_x_RMB1 = 1590 + 190, n_y_RMB1 = -745 - 330;
			int n_x_RMB2 = 700 + 190, n_y_RMB2 = -1125 - 330;
			int n_x_RMB3 = 390 + 190, n_y_RMB3 = -1230 - 330;

			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB1, n_y_RMB1 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB1 + 10, n_y_RMB1 - 13 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB1 + 20, n_y_RMB1 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB1 + 10, n_y_RMB1 - 13 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB1 + 10, n_y_RMB1 - 13 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB1 + 10, n_y_RMB1 - 31 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB1, n_y_RMB1 - 13 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB1 + 20, n_y_RMB1 - 13 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB1, n_y_RMB1 - 22 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB1 + 20, n_y_RMB1 - 22 - nYoff);
			if (zzzse.CompareNoCase("***") != 0)
			{
				MoveToEx(m_hPrinterDC, nXoff + n_x_RMB2, n_y_RMB2 - nYoff, NULL);
				LineTo(m_hPrinterDC, nXoff + n_x_RMB2 + 10, n_y_RMB2 - 13 - nYoff);
				MoveToEx(m_hPrinterDC, nXoff + n_x_RMB2 + 20, n_y_RMB2 - nYoff, NULL);
				LineTo(m_hPrinterDC, nXoff + n_x_RMB2 + 10, n_y_RMB2 - 13 - nYoff);
				MoveToEx(m_hPrinterDC, nXoff + n_x_RMB2 + 10, n_y_RMB2 - 13 - nYoff, NULL);
				LineTo(m_hPrinterDC, nXoff + n_x_RMB2 + 10, n_y_RMB2 - 31 - nYoff);
				MoveToEx(m_hPrinterDC, nXoff + n_x_RMB2, n_y_RMB2 - 13 - nYoff, NULL);
				LineTo(m_hPrinterDC, nXoff + n_x_RMB2 + 20, n_y_RMB2 - 13 - nYoff);
				MoveToEx(m_hPrinterDC, nXoff + n_x_RMB2, n_y_RMB2 - 22 - nYoff, NULL);
				LineTo(m_hPrinterDC, nXoff + n_x_RMB2 + 20, n_y_RMB2 - 22 - nYoff);
			}

			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB3, n_y_RMB3 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB3 + 10, n_y_RMB3 - 13 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB3 + 20, n_y_RMB3 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB3 + 10, n_y_RMB3 - 13 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB3 + 10, n_y_RMB3 - 13 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB3 + 10, n_y_RMB3 - 31 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB3, n_y_RMB3 - 13 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB3 + 20, n_y_RMB3 - 13 - nYoff);
			MoveToEx(m_hPrinterDC, nXoff + n_x_RMB3, n_y_RMB3 - 22 - nYoff, NULL);
			LineTo(m_hPrinterDC, nXoff + n_x_RMB3 + 20, n_y_RMB3 - 22 - nYoff);
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
			itemRect.bottom = (-y - h - nYoff- 330);

			PaintTile(nFontSize, strFontName, itemRect, strText, z, fc);
		}

		// 输出大写金额开头圈叉符号
		int x, y, tx, ty;
		if (xml.FindElem("OX"))
		{
			x = atoi(xml.GetAttrib("x"));
			y = atoi(xml.GetAttrib("y"));
			tx = x + nXoff + 190;
			ty = -(y + nYoff) - 330;

			::Ellipse(m_hPrinterDC, tx - 20, ty + 10, tx + 10, ty - 20);
			::MoveToEx(m_hPrinterDC, tx - 16, ty + 6, NULL);
			::LineTo(m_hPrinterDC, tx + 5, ty - 15);
			::MoveToEx(m_hPrinterDC, tx + 4, ty + 6, NULL);
			::LineTo(m_hPrinterDC, tx - 16, ty - 14);
		}
		::EndPage(m_hPrinterDC);
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

JDCFP_FPXX CJdcfpdy::ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy)
{
	CMarkup xml;
	JDCFP_FPXX fpxx;
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
	if (xml.FindElem("fpdm"))	 fpxx.sFpdm = xml.GetData();
	if (xml.FindElem("fphm"))	 fpxx.sFphm = xml.GetData();
	if (xml.FindElem("fpzt"))	 fpxx.sFpzt = xml.GetData();
	if (xml.FindElem("scbz"))	 fpxx.sScbz = xml.GetData();
	if (xml.FindElem("kprq"))	 fpxx.sKprq = xml.GetData();
	if (xml.FindElem("jqbh"))	 fpxx.sJqbh = xml.GetData();
	if (xml.FindElem("skm"))		 fpxx.sSkm = xml.GetData();
	if (xml.FindElem("ghfmc"))    fpxx.sGhfmc = xml.GetData();
	if (xml.FindElem("sfzhm"))    fpxx.sSfzhm = xml.GetData();
	if (xml.FindElem("ghfsbh"))	 fpxx.sGhfsbh = xml.GetData();
	if (xml.FindElem("scqymc"))   fpxx.sScqymc = xml.GetData();
	if (xml.FindElem("cllx"))	 fpxx.sCllx = xml.GetData();
	if (xml.FindElem("cpxh"))	 fpxx.sCpxh = xml.GetData();
	if (xml.FindElem("cd"))		 fpxx.sCd = xml.GetData();
	if (xml.FindElem("hgzh"))	 fpxx.sHgzh = xml.GetData();
	if (xml.FindElem("jkzmsh"))	 fpxx.sJkzmsh = xml.GetData();
	if (xml.FindElem("sjdh"))	 fpxx.sSjdh = xml.GetData();
	if (xml.FindElem("fdjhm"))	 fpxx.sFdjhm = xml.GetData();
	if (xml.FindElem("cjhm"))	 fpxx.sCjhm = xml.GetData();
	if (xml.FindElem("jshj"))	 fpxx.sJshj = xml.GetData();
	if (xml.FindElem("xhdwmc"))   fpxx.sXhdwmc = xml.GetData();
	if (xml.FindElem("xhdwsbh"))  fpxx.sXhdwsbh = xml.GetData();
	if (xml.FindElem("dh"))		 fpxx.sDh = xml.GetData();
	if (xml.FindElem("zh"))		 fpxx.sZh = xml.GetData();
	if (xml.FindElem("dz"))		 fpxx.sDz = xml.GetData();
	if (xml.FindElem("khyh"))	 fpxx.sKhyh = xml.GetData();
	if (xml.FindElem("zzssl"))	 fpxx.sZzssl = xml.GetData();
	if (xml.FindElem("zzsse"))	 fpxx.sZzsse = xml.GetData();
	if (xml.FindElem("swjgdm"))	 fpxx.sSwjgdm = xml.GetData();
	if (xml.FindElem("swjgmc"))	 fpxx.sSwjgmc = xml.GetData();
	if (xml.FindElem("bhsj"))	 fpxx.sBhsj = xml.GetData();
	if (xml.FindElem("wspzhm"))	 fpxx.sWspzhm = xml.GetData();
	if (xml.FindElem("dw"))		 fpxx.sDw = xml.GetData();
	if (xml.FindElem("xcrs"))	 fpxx.sXcrs = xml.GetData();
	if (xml.FindElem("lslbs"))	 fpxx.sLslbs = xml.GetData();
	if (xml.FindElem("bz"))		 fpxx.sBz = xml.GetData();
	if (xml.FindElem("kpr"))		 fpxx.sKpr = xml.GetData();
	if (xml.FindElem("yfpdm"))	 fpxx.sYfpdm = xml.GetData();
	if (xml.FindElem("yfphm"))	 fpxx.sYfphm = xml.GetData();
	if (xml.FindElem("zfrq"))	 fpxx.sZfrq = xml.GetData();
	if (xml.FindElem("zfr"))		 fpxx.sZfr = xml.GetData();
	if (xml.FindElem("ykfsje"))	 fpxx.sYkfsje = xml.GetData();
	xml.OutOfElem();
	xml.OutOfElem();
	xml.OutOfElem();
	xml.OutOfElem();
	xml.OutOfElem();

	if (fpxx.sFpzt.CompareNoCase("02") == 0)
	{
		m_sFpzt = fpxx.sFpzt;
		return fpxx;
	}

	if (fpxx.sZzssl.CompareNoCase("0") != 0 &&
		fpxx.sZzssl.CompareNoCase("0.0") != 0 &&
		fpxx.sZzssl.CompareNoCase("0.00") != 0 &&
		fpxx.sZzssl.CompareNoCase("0.000") != 0)
	{
		if (fpxx.sZzssl.GetLength() > 0)
		{
			double sTempSl = atof(fpxx.sZzssl.GetBuffer(0)) * 100;
			fpxx.sZzssl.Format("%g%s", sTempSl, "%");
		}

		if (fpxx.sZzsse.Find('.') != -1)  //有小数点
		{
			int leng_Hjje = fpxx.sZzsse.GetLength();
			if (fpxx.sZzsse.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
			{
				fpxx.sZzsse += "0";
			}
		}
		else
		{
			if (fpxx.sZzsse.GetLength() > 0)
				fpxx.sZzsse += ".00";
			else
				fpxx.sZzsse += "0.00";
		}
	}
	else
	{
		if (fpxx.sLslbs.CompareNoCase("1") == 0)
		{
			fpxx.sZzssl.Format("%s", "免税");
		}
		else if (fpxx.sLslbs.CompareNoCase("2") == 0)
		{
			fpxx.sZzssl.Format("%s", "不征税");
		}
		else if (fpxx.sLslbs.CompareNoCase("3") == 0)
		{
			fpxx.sZzssl.Format("%s", "0%");
		}
		else if (fpxx.sLslbs.CompareNoCase("0.015") == 0)
		{
			fpxx.sZzssl.Format("%s", "***");
		}
		else
		{
			fpxx.sZzssl.Format("%s", "0%");
		}

		if (fpxx.sZzsse.CompareNoCase("0") == 0 ||
			fpxx.sZzsse.CompareNoCase("0.0") == 0 ||
			fpxx.sZzsse.CompareNoCase("0.00") == 0)
		{
			fpxx.sZzsse.Format("***");
		}
	}

	fpxx.sSkm1 = fpxx.sSkm.Mid(0, 38);
	fpxx.sSkm2 = fpxx.sSkm.Mid(38, 38);
	fpxx.sSkm3 = fpxx.sSkm.Mid(76, 38);
	fpxx.sSkm4 = fpxx.sSkm.Mid(114, 38);
	fpxx.sSkm5 = fpxx.sSkm.Mid(152, 38);

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
		fpxx.sJshjDx = "负数:" + fpxx.sJshjDx;
	}

	CString strFplx = "03";

	if (fpdy.sDylx.CompareNoCase("0") == 0)
	{
		fpxx.sKprq = fpxx.sKprq.Left(8);
		CString strEwm = "01," + strFplx + "," + fpxx.sFpdm + "," + fpxx.sFphm + "," + fpxx.sBhsj + "," + fpxx.sKprq + ",";
		int size = 0;
		unsigned int num = 0;
		getSize(strEwm.GetBuffer(0), &num);
		unsigned char* fpEwm = new unsigned char[num];
		QRGeneratorBit(strEwm.GetBuffer(0), fpEwm, &size);
		Base64 base;
		int outLen = 0;
		string str64 = base.Decode((const char*)fpEwm, size, outLen);
		delete[]fpEwm;
		FILE * pFile = NULL;
		char sTempPath[MAX_PATH];
		strcpy(sTempPath, m_cQRcodePath);
		strcat(sTempPath, "\\Ewm.bmp");
		if ((pFile = fopen(sTempPath, "wb")) != NULL)
		{
			fwrite(str64.c_str(), 1, outLen, pFile);
			fclose(pFile);
		}
		else
		{
			//
		}
	}

	fpxx.kprq.syear = fpxx.sKprq.Mid(0, 4);
	fpxx.kprq.smouth = fpxx.sKprq.Mid(4, 2);
	fpxx.kprq.sday = fpxx.sKprq.Mid(6, 2);
	fpxx.sKprq = fpxx.kprq.syear + "-" + fpxx.kprq.smouth + "-" + fpxx.kprq.sday;
	return fpxx;
}

CString CJdcfpdy::GenerateFpdyXml(JDCFP_FPXX fpmx, CString dylx, FPDY fpdy)
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

CString CJdcfpdy::GenerateItemXml(JDCFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;

	//计算出销货方名称的长度
	xywhsf(fpmx.OX, LX + 380, LY + 1085, LW, LH, LS_9, FT, ZL);

	xywhsf(fpmx.Kprq, 190, -60, 310, 50, LS_12, FT, AM_VCL);

	xywhsf(fpmx.Fpdm, 290, 50, 580, 60, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Fphm, 290, 120, 580, 60, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Jqbh, 290, 190, 580, 60, LS_12, FT, AM_VCL);

	xywhsf(fpmx.Skm1, 980, 20, 1020, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Skm2, 980, 70, 1020, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Skm3, 980, 120, 1020, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Skm4, 980, 170, 1020, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Skm5, 980, 220, 1020, 50, LS_12, FT, AM_VCL);

	xywhsf_fc(fpmx.Ghfmc, 290, 310, 650, 60, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Sfzhm, 290, 370, 650, 60, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Ghfsbh, 1420, 310, 650, 120, LS_12, FT, AM_VCL);
	
	xywhsf_fc(fpmx.Cllx, 290, 440, 400, 80, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Cpxh, 900, 440, 660, 80, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Cd, 1690, 440, 310, 80, LS_10, FS, AM_VCL, 10);

	xywhsf(fpmx.Hgzh, 290, 535, 400, 80, LS_10, FS, AM_VCL);
	xywhsf(fpmx.Jkzmsh, 970, 535, 530, 80, LS_10, FS, AM_VCL);
	xywhsf(fpmx.Sjdh, 1690, 535, 310, 80, LS_10, FS, AM_VCL);

	xywhsf(fpmx.Fdjhm, 290, 628, 650, 80, LS_10, FS, AM_VCL);
	xywhsf(fpmx.Cjhm, 1380, 628, 650, 80, LS_12, FT, AM_VCL);

	if (fpmx.sFpzt.CompareNoCase("00") == 0)//发票状态标志为0 正数发票有圈叉符号
	{
		xywhsf(fpmx.OX, 330, 755, 80, 80, LS_10, FS, AM_VCL);
		xywhsf(fpmx.JshjDx, 350, 720, 1100, 80, LS_10, FS, AM_VCL);
	}
	else
	{
		xywhsf(fpmx.JshjDx, 290, 720, 1100, 80, LS_10, FS, AM_VCL);
	}

	xywhsf(fpmx.Jshj, 1620, 720, 380, 80, LS_12, FT, AM_VCL);

	xywhsf_fc(fpmx.Xhdwmc, 290, 815, 1020, 80, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Dh, 1440, 815, 560, 80, LS_10, FS, AM_VCL);

	xywhsf(fpmx.Xhdwsbh, 290, 905, 1020, 80, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Zh, 1440, 905, 560, 80, LS_10, FS, AM_VCL);

	xywhsf_fc(fpmx.Dz, 290, 1000, 760, 80, LS_10, FS, AM_VCL, 10);
	xywhsf_fc(fpmx.Khyh, 1240, 1000, 760, 80, LS_10, FS, AM_VCL, 10);

	xywhsf(fpmx.Zzssl, 290, 1090, 225, 110, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Zzsse, 730, 1090, 355, 110, LS_12, FT, AM_VCL_S);
	xywhsf_fc(fpmx.Swjgmc, 1335, 1090, 665, 55, LS_10, FS, AM_VCL, 10);
	xywhsf(fpmx.Swjgdm, 1335, 1145, 665, 55, LS_12, FT, AM_VCL);

	xywhsf(fpmx.Bhsj, 420, 1210, 310, 80, LS_12, FT, AM_VCL_S);
	xywhsf(fpmx.Wspzhm, 1010, 1210, 460, 80, LS_9, FT, AM_VCL);
	xywhsf(fpmx.Dw, 1590, 1210, 80, 80, LS_10, FS, AM_ZL);
	xywhsf(fpmx.Xcrs, 1860, 1210, 140, 80, LS_10, FS, AM_VCL);
	xywhsf(fpmx.Kpr, 960, 1310, 190, 60, LS_10, FS, AM_VCL);

	addxml(fpmx.sKprq, fpmx.Kprq);
	addxml(fpmx.sFpdm, fpmx.Fpdm);
	addxml(fpmx.sFphm, fpmx.Fphm);
	addxml(fpmx.sJqbh, fpmx.Jqbh);
	addxml(fpmx.sGhfmc, fpmx.Ghfmc);
	addxml(fpmx.sSfzhm, fpmx.Sfzhm);
	addxml(fpmx.sCllx, fpmx.Cllx);
	addxml(fpmx.sHgzh, fpmx.Hgzh);
	addxml(fpmx.sFdjhm, fpmx.Fdjhm);
	addxml(fpmx.sJshjDx, fpmx.JshjDx);
	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);
	addxml(fpmx.sDz, fpmx.Dz);
	addxml(fpmx.sZzssl, fpmx.Zzssl);
	addxml(fpmx.sBhsj, fpmx.Bhsj);
	addxml(fpmx.sSkm1, fpmx.Skm1);
	addxml(fpmx.sSkm2, fpmx.Skm2);
	addxml(fpmx.sSkm3, fpmx.Skm3);
	addxml(fpmx.sSkm4, fpmx.Skm4);
	addxml(fpmx.sSkm5, fpmx.Skm5);
	addxml(fpmx.sGhfsbh, fpmx.Ghfsbh);
	addxml(fpmx.sCpxh, fpmx.Cpxh);
	addxml(fpmx.sCd, fpmx.Cd);
	addxml(fpmx.sJkzmsh, fpmx.Jkzmsh);
	addxml(fpmx.sSjdh, fpmx.Sjdh);
	addxml(fpmx.sCjhm, fpmx.Cjhm);
	addxml(fpmx.sJshj, fpmx.Jshj);
	addxml(fpmx.sDh, fpmx.Dh);
	addxml(fpmx.sZh, fpmx.Zh);
	addxml(fpmx.sKhyh, fpmx.Khyh);
	addxml(fpmx.sZzsse, fpmx.Zzsse);
	addxml(fpmx.sSwjgmc, fpmx.Swjgmc);
	addxml(fpmx.sSwjgdm, fpmx.Swjgdm);
	addxml(fpmx.sWspzhm, fpmx.Wspzhm);
	if (fpmx.sDw.GetLength() > 4)
	{
		fpmx.sDw.Insert(4, "\r\n");
	}
	addxml(fpmx.sDw, fpmx.Dw);
	addxml(fpmx.sXcrs, fpmx.Xcrs);
	addxml(fpmx.sKpr, fpmx.Kpr);

	if (fpmx.sFpzt.CompareNoCase("00") == 0)//发票状态标志为0 正数发票有圈叉符号
	{
		xml.AddElem("OX", "1");
		xml.AddAttrib("x", fpmx.OX.x);
		xml.AddAttrib("y", fpmx.OX.y);
		xml.AddAttrib("w", fpmx.OX.w);
		xml.AddAttrib("h", fpmx.OX.h);
		xml.AddAttrib("s", fpmx.OX.s);
		xml.AddAttrib("f", fpmx.OX.f);
		xml.AddAttrib("fc", fpmx.OX.fc);
		xml.IntoElem();
		xml.OutOfElem();
	}

	return xml.GetDoc();
}
