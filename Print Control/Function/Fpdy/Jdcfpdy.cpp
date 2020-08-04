#include "../../pch.h"
#include "Jdcfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"
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
		rtn = Print(printXml, fpdy.sFplxdm, fpmx.sZzsse);
	}

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CJdcfpdy::Print(LPCTSTR billXml, CString strFplxdm, CString zzzse)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;
	int nXoff = 0;
	int nYoff = 0;
	int nQRCodeSize = 0;

	// 添加读取配置文件功能
	CString _sTop = "";
	CString _sLeft = "";
	CString _sQRSize = "";
	ZLib_GetIniYbjValue(strFplxdm, _sTop, _sLeft, _sQRSize);
	nXoff = atoi(_sLeft) * 10;
	nYoff = atoi(_sTop) * 10;
	nQRCodeSize = atoi(_sQRSize) * 10;

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
				::StretchBlt(m_hPrinterDC, nXoff + 20 + 180 - 160, -(nYoff + 40 + (180 - 160)), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}

			::SelectObject(dcMem, hOldBmp);
			::DeleteDC(dcMem);
			::DeleteObject(hBitmap);

			int n_x_RMB1 = 1570, n_y_RMB1 = -1040;
			int n_x_RMB2 = 720, n_y_RMB2 = -1425;
			int n_x_RMB3 = 370, n_y_RMB3 = -1530;

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
				PaintTile(nFontSize, strFontName, itemRect, strText, z, 0);
			}
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
	ZLib_ChineseFee(strDxje, 100, strsc);//小写金额转换为大写金额
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
	int LengOfXhfmc = fpmx.sXhdwmc.GetLength();
	xywhsf(fpmx.OX, LX + 380, LY + 1085, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.Kprq, LX + 110, LY + 290, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Fpdm, LX + 220, LY + 400, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Fphm, LX + 220, LY + 470, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Jqbh, LX + 220, LY + 540, LW, LH, LS_12, FT, ZL);
	//计算出购货方名称的长度	
	int LengOfGhfmc = fpmx.sGhfmc.GetLength();
	if (LengOfGhfmc < 37)
	{
		xywhsf(fpmx.Ghfmc, LX + 220, LY + 690, LW + 660, LH + 150, LS_10, FS, ZL);
	}
	else if (LengOfGhfmc > 36 && LengOfGhfmc < 43)
	{
		xywhsf(fpmx.Ghfmc, LX + 220, LY + 690, LW + 660, LH + 150, LS_9, FS, ZL);
	}
	else if (LengOfGhfmc > 42 && LengOfGhfmc < 47)
	{
		xywhsf(fpmx.Ghfmc, LX + 220, LY + 690, LW + 660, LH + 150, LS_8, FS, ZL);
	}
	else if (LengOfGhfmc > 46 && LengOfGhfmc < 53)
	{
		xywhsf(fpmx.Ghfmc, LX + 220, LY + 690, LW + 660, LH + 150, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Ghfmc, LX + 220, LY + 680, LW + 660, LH + 150, LS_7, FS, ZL);
	}
	xywhsf(fpmx.Sfzhm, LX + 220, LY + 730, LW, LH, LS_12, FT, ZL);
	// 计算车辆类型的长度
	int LengOfCllx = fpmx.sCllx.GetLength();
	if (LengOfCllx < 23)
	{
		xywhsf(fpmx.Cllx, LX + 220, LY + 805, LW + 420, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Cllx, LX + 220, LY + 805, LW + 420, LH + 150, LS_9, FS, ZL);
	}
	// 计算合格证号的长度
	int LengOfHgzh = fpmx.sHgzh.GetLength();
	if (LengOfHgzh < 25)
	{
		xywhsf(fpmx.Hgzh, LX + 220, LY + 890, LW + 420, LH + 150, LS_10, FS, ZL);
	}
	else if (LengOfHgzh > 24 && LengOfHgzh < 49)
	{
		xywhsf(fpmx.Hgzh, LX + 220, LY + 880, LW + 420, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Hgzh, LX + 220, LY + 880, LW + 420, LH + 150, LS_9, FS, ZL);
	}
	// 计算发动机号码的长度
	int LengOfFdjhm = fpmx.sFdjhm.GetLength();
	if (LengOfFdjhm < 37)
	{
		xywhsf(fpmx.Fdjhm, LX + 220, LY + 985, LW + 660, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Fdjhm, LX + 220, LY + 975, LW + 660, LH + 150, LS_10, FS, ZL);
	}
	xywhsf(fpmx.JshjDx, LX + 300, LY + 1080, LW, LH, LS_10, FS, ZL);
	// 计算销货单位名称的长度
	int LengOfXhdwmc = fpmx.sXhdwmc.GetLength();
	if (LengOfXhdwmc < 59)
	{
		xywhsf(fpmx.Xhdwmc, LX + 220, LY + 1170, LW + 1050, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Xhdwmc, LX + 220, LY + 1160, LW + 1050, LH + 150, LS_9, FS, ZL);
	}
	xywhsf(fpmx.Xhdwsbh, LX + 220, LY + 1260, LW, LH, LS_12, FT, ZL);
	// 计算地址的长度
	int LengOfDz = fpmx.sDz.GetLength();
	if (LengOfDz < 43)
	{
		xywhsf(fpmx.Dz, LX + 220, LY + 1365, LW + 750, LH + 150, LS_10, FS, ZL);
	}
	else if (LengOfDz > 42 && LengOfDz < 49)
	{
		xywhsf(fpmx.Dz, LX + 220, LY + 1365, LW + 750, LH + 150, LS_9, FS, ZL);
	}
	else if (LengOfDz > 48 && LengOfDz < 53)
	{
		xywhsf(fpmx.Dz, LX + 220, LY + 1365, LW + 750, LH + 150, LS_8, FS, ZL);
	}
	else if (LengOfDz > 52 && LengOfDz < 61)
	{
		xywhsf(fpmx.Dz, LX + 220, LY + 1365, LW + 750, LH + 150, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Dz, LX + 220, LY + 1355, LW + 750, LH + 150, LS_7, FS, ZL);
	}
	xywhsf(fpmx.Zzssl, LX + 280, LY + 1455, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Bhsj, LX + 350, LY + 1570, LW, LH, LS_12, FT, ZL);

	xywhsf(fpmx.Skm1, LX + 930, LY + 390 - 20, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Skm2, LX + 930, LY + 430 - 15, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Skm3, LX + 930, LY + 470 - 10, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Skm4, LX + 930, LY + 510 - 5, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Skm5, LX + 930, LY + 550, LW, LH, LS_12, FT, ZL);

	xywhsf(fpmx.Ghfsbh, LX + 1350, LY + 690, LW, LH, LS_12, FT, ZL);
	// 计算厂牌型号的长度
	int LengOfCpxh = fpmx.sCpxh.GetLength();
	if (LengOfCpxh < 39)
	{
		xywhsf(fpmx.Cpxh, LX + 820, LY + 805, LW + 690, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Cpxh, LX + 820, LY + 805, LW + 690, LH + 150, LS_9, FS, ZL);
	}
	// 计算产地的长度
	int LengOfCd = fpmx.sCd.GetLength();
	if (LengOfCd < 19)
	{
		xywhsf(fpmx.Cd, LX + 1610, LY + 805, LW + 340, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Cd, LX + 1610, LY + 805, LW + 340, LH + 150, LS_9, FS, ZL);
	}
	// 计算证明书号的长度
	int LengOfJkzmsh = fpmx.sJkzmsh.GetLength();
	if (LengOfJkzmsh < 31)
	{
		xywhsf(fpmx.Jkzmsh, LX + 900, LY + 895, LW + 550, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Jkzmsh, LX + 900, LY + 885, LW + 550, LH + 150, LS_10, FS, ZL);
	}
	// 计算商检单号的长度
	int LengOfSjdh = fpmx.sSjdh.GetLength();
	if (LengOfSjdh < 19)
	{
		xywhsf(fpmx.Sjdh, LX + 1610, LY + 895, LW + 340, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Sjdh, LX + 1610, LY + 885, LW + 340, LH + 150, LS_10, FS, ZL);
	}
	xywhsf(fpmx.Cjhm, LX + 1320, LY + 985, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.Jshj, LX + 1550, LY + 1080, LW, LH, LS_12, FT, ZL);
	// 计算电话的长度
	int LengOfDh = fpmx.sDh.GetLength();
	if (LengOfDh < 33)
	{
		xywhsf(fpmx.Dh, LX + 1370, LY + 1170, LW + 550, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Dh, LX + 1370, LY + 1160, LW + 550, LH + 150, LS_10, FS, ZL);
	}
	// 计算账户的长度
	int LengOfZh = fpmx.sZh.GetLength();
	if (LengOfZh < 33)
	{
		xywhsf(fpmx.Zh, LX + 1370, LY + 1260, LW + 550, LH + 150, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Zh, LX + 1370, LY + 1250, LW + 550, LH + 150, LS_10, FS, ZL);
	}
	// 计算开户银行的长度
	int LengOfKhyh = fpmx.sKhyh.GetLength();
	if (LengOfKhyh < 43)
	{
		xywhsf(fpmx.Khyh, LX + 1170, LY + 1365, LW + 750, LH + 150, LS_10, FS, ZL);
	}
	else if (LengOfKhyh > 42 && LengOfKhyh < 49)
	{
		xywhsf(fpmx.Khyh, LX + 1170, LY + 1365, LW + 750, LH + 150, LS_9, FS, ZL);
	}
	else if (LengOfKhyh > 48 && LengOfKhyh < 53)
	{
		xywhsf(fpmx.Khyh, LX + 1170, LY + 1365, LW + 750, LH + 150, LS_8, FS, ZL);
	}
	else if (LengOfKhyh > 52 && LengOfKhyh < 61)
	{
		xywhsf(fpmx.Khyh, LX + 1170, LY + 1365, LW + 750, LH + 150, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Khyh, LX + 1170, LY + 1355, LW + 750, LH + 150, LS_7, FS, ZL);
	}
	xywhsf(fpmx.Zzsse, LX + 700, LY + 1465, LW, LH, LS_12, FT, ZL);
	// 计算税务机关名称的长度
	int LengOfSwjgmc = fpmx.sSwjgmc.GetLength();
	if (LengOfSwjgmc < 37)
	{
		xywhsf(fpmx.Swjgmc, LX + 1255, LY + 1465, LW + 660, LH + 120, LS_10, FS, ZL);
	}
	else if (LengOfSwjgmc > 36 && LengOfSwjgmc < 41)
	{
		xywhsf(fpmx.Swjgmc, LX + 1255, LY + 1465, LW + 660, LH + 120, LS_9, FS, ZL);
	}
	else if (LengOfSwjgmc > 40 && LengOfSwjgmc < 47)
	{
		xywhsf(fpmx.Swjgmc, LX + 1255, LY + 1465, LW + 660, LH + 120, LS_8, FS, ZL);
	}
	else if (LengOfSwjgmc > 46 && LengOfSwjgmc < 53)
	{
		xywhsf(fpmx.Swjgmc, LX + 1255, LY + 1450, LW + 660, LH + 120, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Swjgmc, LX + 1255, LY + 1450, LW + 660, LH + 120, LS_7, FS, ZL);
	}
	xywhsf(fpmx.Swjgdm, LX + 1255, LY + 1500, LW, LH, LS_12, FT, ZL);
	// 计算完税凭证的长度
	int LengOfWspzhm = fpmx.sWspzhm.GetLength();
	if (LengOfWspzhm < 26)
	{
		xywhsf(fpmx.Wspzhm, LX + 930, LY + 1575, LW + 470, LH + 120, LS_9, FT, ZL);
	}
	else
	{
		xywhsf(fpmx.Wspzhm, LX + 930, LY + 1555, LW + 470, LH + 120, LS_9, FT, ZL);
	}
	// 计算吨位的长度
	int LengOfDw = fpmx.sDw.GetLength();
	if (LengOfDw < 6)
	{
		xywhsf(fpmx.Dw, LX + 1505, LY + 1575, LW + 100, LH + 120, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Dw, LX + 1505, LY + 1555, LW + 100, LH + 120, LS_10, FS, ZL);
	}
	// 计算限乘人数的长度
	int LengOfXcrs = fpmx.sXcrs.GetLength();
	if (LengOfXcrs < 9)
	{
		xywhsf(fpmx.Xcrs, LX + 1785, LY + 1575, LW + 150, LH + 120, LS_10, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Xcrs, LX + 1785, LY + 1555, LW + 150, LH + 120, LS_10, FS, ZL);
	}
	xywhsf(fpmx.Kpr, LX + 900, LY + 1655, LW, LH, LS_10, FS, ZL);



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
		xml.IntoElem();
		xml.OutOfElem();
	}

	return xml.GetDoc();
}
