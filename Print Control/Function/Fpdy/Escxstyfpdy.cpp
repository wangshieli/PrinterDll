#include "../../pch.h"
#include "Escxstyfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"
#include "../../Helper/QRGenerator/QRGenerator.h"
#include "../../Helper/QRGenerator/Base64.h"

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
		rtn = Print(printXml, fpdy.sFplxdm);
	}

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CEscxstyfp::Print(LPCTSTR billXml, CString strFplxdm)
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
	nXoff = atoi(_sLeft);
	nYoff = atoi(_sTop);
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
				::StretchBlt(m_hPrinterDC, nXoff + 20 + 180 - nQRCodeSize + 20, -(nYoff + 40 + (180 - nQRCodeSize) - 50), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}
			else
			{
				int nScaledWidth = 160;	//GetDeviceCaps (m_hPrinterDC, HORZRES);
				int nScaledHeight = 160;	//GetDeviceCaps (m_hPrinterDC, VERTRES);
				::StretchBlt(m_hPrinterDC, nXoff + 20 + 180 - 160 + 20, -(nYoff + 40 + (180 - 160) - 50), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}

			::SelectObject(dcMem, hOldBmp);
			::DeleteDC(dcMem);
			::DeleteObject(hBitmap);

			int n_x_RMB1 = 1660, n_y_RMB1 = -950;

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
		}

		while (xml.FindElem("Item"))
		{
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
			printRect.right = x + nXoff + 100 + w;
			printRect.bottom = -(y - 5 + nYoff + h);

			if (w == 0 && h == 0)
			{
				::TextOut(m_hPrinterDC, printRect.left, printRect.top, strText, strText.GetLength());
			}
			else
			{
				printRect.right = x + nXoff + w + 100;
				printRect.bottom = -((y + 5 + nYoff) + h);

				if (z == 0)
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect, DT_WORDBREAK | DT_LEFT | DT_EDITCONTROL | DT_NOPREFIX);
				}
				else if (z == 2)
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect,/* DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX | */DT_SINGLELINE | DT_VCENTER);
				}
				else
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect, DT_WORDBREAK | DT_EDITCONTROL | DT_RIGHT | DT_NOPREFIX);
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
	ZLib_ChineseFee(strDxje, 100, strsc);//小写金额转换为大写金额
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

	if (fpdy.sDylx.CompareNoCase("0") == 0)
	{
		fpxx.sKprq = fpxx.sKprq.Left(8);
		CString strEwm = "01," + strFplx + "," + fpxx.sFpdm + "," + fpxx.sFphm + "," + fpxx.sCjhj + "," + fpxx.sKprq + "," + fpxx.sSkm;
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
	/*
		fpmx.sKprq = "2017-11-08";
		fpmx.sFpdm = "199311080703";
		fpmx.sFphm = "12345678";
		fpmx.sJqbh = "1234567890";
		fpmx.sGfmc = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六";
		fpmx.sGfsfzhm = "4104261993110810161022";
		fpmx.sGfdz = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sGfdh = "15515799921234567890";

		fpmx.sMfmc = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六";
		fpmx.sMfsfzhm = "4110252017072101041022";
		fpmx.sMfdz = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sMfdh = "18410107577234567890";

		fpmx.sCpzh = "一二三四五六七八九十";
		fpmx.sDjzh = "一二三四五六七八九十";
		fpmx.sCllx = "一二三四五六七八九十一二三四五六七八九十";
		fpmx.sCjhm = "12345678901234567890123";
		fpmx.sCpxh = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sZrdclglsmc = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sCjhjDx = "拾忆玖仟玖百玖拾玖万玖仟玖百玖拾玖圆整";
		fpmx.sCjhj = "1999999999.00";
		fpmx.sJypmdw = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sJypmdwdz = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sJypmdwnsrsbh = "1234567890123454567890";
		fpmx.sJypmdwyhzh = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sJypmdwdh = "12345678901234567890";

		fpmx.sEscsc = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";

		fpmx.sEscscnsrsbh = "12345678901234567890";
		fpmx.sEscscdz = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sEscscyhzh = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十";
		fpmx.sEscscdh = "12345678901234567890";
		fpmx.sBz = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二三四五";
		fpmx.sKpr = "一二三四五六七八九十";
	*/
	xywhsf(fpmx.Skm, LX + 1020, LY + 370, LW + 900, LH + 40, LS_12, FT, ZL);

	xywhsf(fpmx.Kprq, LX + 145, LY + 230, LW + 300, LH + 40, LS_12, FT, ZL);
	xywhsf(fpmx.Fpdm, LX + 200, LY + 300, LW + 600, LH + 40, LS_12, FT, ZL);
	xywhsf(fpmx.Fphm, LX + 200, LY + 370, LW + 600, LH + 40, LS_12, FT, ZL);
	xywhsf(fpmx.Jqbh, LX + 200, LY + 440, LW + 600, LH + 40, LS_12, FT, ZL);

	if (fpmx.sGfmc.GetLength() < 37)
	{
		xywhsf(fpmx.Gfmc, LX + 400, LY + 509, LW + 635, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sGfmc.GetLength() > 36 && fpmx.sGfmc.GetLength() < 39)
	{
		xywhsf(fpmx.Gfmc, LX + 400, LY + 509, LW + 635, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Gfmc, LX + 400, LY + 509, LW + 635, LH + 65, LS_9, FS, ZL);
	}


	xywhsf(fpmx.Gfsfzhm, LX + 1435, LY + 509, LW + 540, LH + 65, LS_11, FT, ZC);

	if (fpmx.sGfdz.GetLength() < 57)
	{
		xywhsf(fpmx.Gfdz, LX + 400, LY + 587, LW + 1010, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sGfdz.GetLength() > 56 && fpmx.sGfdz.GetLength() < 63)
	{
		xywhsf(fpmx.Gfdz, LX + 400, LY + 587, LW + 1010, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Gfdz, LX + 400, LY + 587, LW + 1010, LH + 65, LS_9, FS, ZL);
	}

	if (fpmx.sGfdh.GetLength() < 17)
	{
		xywhsf(fpmx.Gfdh, LX + 1552, LY + 587, LW + 420, LH + 65, LS_12, FT, ZC);
	}
	else if (fpmx.sGfdh.GetLength() > 16 && fpmx.sGfdh.GetLength() < 19)
	{
		xywhsf(fpmx.Gfdh, LX + 1552, LY + 587, LW + 420, LH + 65, LS_11, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Gfdh, LX + 1552, LY + 587, LW + 420, LH + 65, LS_10, FT, ZC);
	}



	if (fpmx.sMfmc.GetLength() < 37)
	{
		xywhsf(fpmx.Mfmc, LX + 400, LY + 665, LW + 635, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sMfmc.GetLength() > 36 && fpmx.sMfmc.GetLength() < 39)
	{
		xywhsf(fpmx.Mfmc, LX + 400, LY + 665, LW + 635, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Mfmc, LX + 400, LY + 665, LW + 635, LH + 65, LS_9, FS, ZL);
	}

	xywhsf(fpmx.Mfsfzhm, LX + 1435, LY + 665, LW + 540, LH + 65, LS_11, FT, ZC);

	if (fpmx.sMfdz.GetLength() < 57)
	{
		xywhsf(fpmx.Mfdz, LX + 400, LY + 743, LW + 1010, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sMfdz.GetLength() > 56 && fpmx.sMfdz.GetLength() < 63)
	{
		xywhsf(fpmx.Mfdz, LX + 400, LY + 743, LW + 1010, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Mfdz, LX + 400, LY + 743, LW + 1010, LH + 65, LS_9, FS, ZL);
	}

	if (fpmx.sMfdh.GetLength() < 17)
	{
		xywhsf(fpmx.Mfdh, LX + 1552, LY + 743, LW + 420, LH + 65, LS_12, FT, ZC);
	}
	else if (fpmx.sMfdh.GetLength() > 16 && fpmx.sMfdh.GetLength() < 19)
	{
		xywhsf(fpmx.Mfdh, LX + 1552, LY + 743, LW + 420, LH + 65, LS_11, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Mfdh, LX + 1552, LY + 743, LW + 420, LH + 65, LS_10, FT, ZC);
	}

	if (fpmx.sCpzh.GetLength() < 13)
	{
		xywhsf(fpmx.Cpzh, LX + 400, LY + 821, LW + 320, LH + 65, LS_12, FT, ZC);
	}
	else if (fpmx.sCpzh.GetLength() > 12 && fpmx.sCpzh.GetLength() < 15)
	{
		xywhsf(fpmx.Cpzh, LX + 400, LY + 821, LW + 320, LH + 65, LS_11, FT, ZC);
	}
	else if (fpmx.sCpzh.GetLength() > 14 && fpmx.sCpzh.GetLength() < 17)
	{
		xywhsf(fpmx.Cpzh, LX + 400, LY + 821, LW + 320, LH + 65, LS_9, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Cpzh, LX + 400, LY + 821, LW + 320, LH + 65, LS_8, FT, ZL);
	}

	if (fpmx.sDjzh.GetLength() < 17)
	{
		xywhsf(fpmx.Djzh, LX + 930, LY + 821, LW + 385, LH + 65, LS_11, FT, ZC);
	}
	else if (fpmx.sDjzh.GetLength() > 16 && fpmx.sDjzh.GetLength() < 19)
	{
		xywhsf(fpmx.Djzh, LX + 930, LY + 821, LW + 385, LH + 65, LS_10, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Djzh, LX + 930, LY + 821, LW + 385, LH + 65, LS_9, FT, ZC);
	}

	if (fpmx.sCllx.GetLength() < 23)
	{
		xywhsf(fpmx.Cllx, LX + 1552, LY + 821, LW + 420, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sCllx.GetLength() > 22 && fpmx.sCllx.GetLength() < 27)
	{
		xywhsf(fpmx.Cllx, LX + 1552, LY + 821, LW + 420, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Cllx, LX + 1552, LY + 821, LW + 420, LH + 65, LS_9, FS, ZL);
	}

	if (fpmx.sCjhm.GetLength() < 15)
	{
		xywhsf(fpmx.Cjhm, LX + 400, LY + 899, LW + 320, LH + 65, LS_11, FT, ZC);
	}
	else if (fpmx.sCjhm.GetLength() > 14 && fpmx.sCjhm.GetLength() < 17)
	{
		xywhsf(fpmx.Cjhm, LX + 400, LY + 899, LW + 320, LH + 65, LS_9, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Cjhm, LX + 400, LY + 899, LW + 320, LH + 65, LS_8, FT, ZL);
	}

	int nLengthOfCPXH = fpmx.sCpxh.GetLength();
	if (nLengthOfCPXH < 21)
	{
		xywhsf(fpmx.Cpxh, LX + 930, LY + 899, LW + 385, LH + 65, LS_10, FS, ZC);
	}
	else if (nLengthOfCPXH > 20 && nLengthOfCPXH < 23)
	{
		xywhsf(fpmx.Cpxh, LX + 930, LY + 899, LW + 385, LH + 65, LS_9, FS, ZC);
	}
	else if (nLengthOfCPXH > 22 && nLengthOfCPXH < 45)
	{
		xywhsf(fpmx.Cpxh, LX + 930, LY + 899, LW + 385, LH + 65, LS_8, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Cpxh, LX + 930, LY + 899, LW + 385, LH + 65, LS_7, FS, ZL);
	}

	int nLengthOfZRDCLGLSMC = fpmx.sZrdclglsmc.GetLength();
	if (nLengthOfZRDCLGLSMC < 23)
	{
		xywhsf(fpmx.Zrdclglsmc, LX + 1552, LY + 899, LW + 420, LH + 65, LS_10, FS, ZC);
	}
	else if (nLengthOfZRDCLGLSMC > 22 && nLengthOfZRDCLGLSMC < 27)
	{
		xywhsf(fpmx.Zrdclglsmc, LX + 1552, LY + 899, LW + 420, LH + 65, LS_9, FS, ZC);
	}
	else if (nLengthOfZRDCLGLSMC > 26 && nLengthOfZRDCLGLSMC < 53)
	{
		xywhsf(fpmx.Zrdclglsmc, LX + 1552, LY + 899, LW + 420, LH + 65, LS_8, FS, ZL);
	}
	else if (nLengthOfZRDCLGLSMC > 52 && nLengthOfZRDCLGLSMC < 61)
	{
		xywhsf(fpmx.Zrdclglsmc, LX + 1552, LY + 899, LW + 420, LH + 65, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Zrdclglsmc, LX + 1552, LY + 899, LW + 420, LH + 65, LS_6, FS, ZL);
	}

	xywhsf(fpmx.OX, LX + 550, LY + 997, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.CjhjDx, LX + 500, LY + 977, LW + 910, LH + 65, LS_10, FS, ZC);
	xywhsf(fpmx.Cjhj, LX + 1642, LY + 977, LW + 330, LH + 65, LS_12, FT, ZC);

	xywhsf(fpmx.Jypmdw, LX + 400, LY + 1055, LW + 1570, LH + 65, LS_10, FS, ZC);
	if (fpmx.sJypmdwdz.GetLength() < 47)
	{
		xywhsf(fpmx.Jypmdwdz, LX + 400, LY + 1133, LW + 830, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sJypmdwdz.GetLength() > 46 && fpmx.sJypmdwdz.GetLength() < 51)
	{
		xywhsf(fpmx.Jypmdwdz, LX + 400, LY + 1133, LW + 830, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Jypmdwdz, LX + 400, LY + 1133, LW + 830, LH + 65, LS_9, FS, ZL);
	}
	xywhsf(fpmx.Jypmdwnsrsbh, LX + 1435, LY + 1133, LW + 540, LH + 65, LS_11, FT, ZC);

	if (fpmx.sJypmdwyhzh.GetLength() < 65)
	{
		xywhsf(fpmx.Jypmdwyhzh, LX + 400, LY + 1211, LW + 1130, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sJypmdwyhzh.GetLength() > 64 && fpmx.sJypmdwyhzh.GetLength() < 71)
	{
		xywhsf(fpmx.Jypmdwyhzh, LX + 400, LY + 1211, LW + 1130, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Jypmdwyhzh, LX + 400, LY + 1211, LW + 1130, LH + 65, LS_9, FS, ZL);
	}

	if (fpmx.sJypmdwdh.GetLength() < 13)
	{
		xywhsf(fpmx.Jypmdwdh, LX + 1670, LY + 1211, LW + 300, LH + 65, LS_11, FT, ZC);
	}
	else if (fpmx.sJypmdwdh.GetLength() > 12 && fpmx.sJypmdwdh.GetLength() < 15)
	{
		xywhsf(fpmx.Jypmdwdh, LX + 1670, LY + 1211, LW + 300, LH + 65, LS_10, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Jypmdwdh, LX + 1670, LY + 1211, LW + 300, LH + 65, LS_8, FT, ZL);
	}

	int nLengthOfESCSC = fpmx.sEscsc.GetLength();
	if (nLengthOfESCSC < 33)
	{
		xywhsf(fpmx.Escsc, LX + 400, LY + 1289, LW + 590, LH + 130, LS_10, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Escsc, LX + 400, LY + 1289, LW + 590, LH + 130, LS_10, FS, ZL);
	}
	xywhsf(fpmx.Escscnsrsbh, LX + 1190, LY + 1289, LW + 780, LH + 65, LS_11, FT, ZC);

	if (fpmx.sEscscdz.GetLength() < 45)
	{
		xywhsf(fpmx.Escscdz, LX + 1190, LY + 1367, LW + 780, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sEscscdz.GetLength() > 44 && fpmx.sEscscdz.GetLength() < 49)
	{
		xywhsf(fpmx.Escscdz, LX + 1190, LY + 1367, LW + 780, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Escscdz, LX + 1190, LY + 1367, LW + 780, LH + 65, LS_9, FS, ZL);
	}

	if (fpmx.sEscscyhzh.GetLength() < 65)
	{
		xywhsf(fpmx.Escscyhzh, LX + 400, LY + 1445, LW + 1130, LH + 65, LS_10, FS, ZC);
	}
	else if (fpmx.sEscscyhzh.GetLength() > 64 && fpmx.sEscscyhzh.GetLength() < 71)
	{
		xywhsf(fpmx.Escscyhzh, LX + 400, LY + 1445, LW + 1130, LH + 65, LS_9, FS, ZC);
	}
	else
	{
		xywhsf(fpmx.Escscyhzh, LX + 400, LY + 1445, LW + 1130, LH + 65, LS_9, FS, ZL);
	}

	if (fpmx.sEscscdh.GetLength() < 13)
	{
		xywhsf(fpmx.Escscdh, LX + 1670, LY + 1445, LW + 300, LH + 65, LS_11, FT, ZC);
	}
	else if (fpmx.sEscscdh.GetLength() > 12 && fpmx.sEscscdh.GetLength() < 15)
	{
		xywhsf(fpmx.Escscdh, LX + 1670, LY + 1445, LW + 300, LH + 65, LS_10, FT, ZC);
	}
	else
	{
		xywhsf(fpmx.Escscdh, LX + 1670, LY + 1445, LW + 300, LH + 65, LS_8, FT, ZL);
	}

	int nLengthOfBZ = fpmx.sBz.GetLength();
	if (nLengthOfBZ < 101)
	{
		xywhsf(fpmx.Bz, LX + 170, LY + 1523, LW + 1790, LH + 65, LS_10, FS, ZC);
	}
	else if (nLengthOfBZ > 100 && nLengthOfBZ < 111)
	{
		xywhsf(fpmx.Bz, LX + 170, LY + 1523, LW + 1790, LH + 65, LS_9, FS, ZC);
	}
	else if (nLengthOfBZ > 110 && nLengthOfBZ < 221)
	{
		xywhsf(fpmx.Bz, LX + 170, LY + 1523, LW + 1790, LH + 65, LS_9, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Bz, LX + 170, LY + 1523, LW + 1790, LH + 65, LS_8, FS, ZL);
	}
	xywhsf(fpmx.Kpr, LX + 1435, LY + 1601, LW + 470, LH + 65, LS_10, FS, ZL);

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
	addxml(fpmx.sCjhj, fpmx.Cjhj)
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


	if (fpmx.sFpzt.Compare("0") == 0)
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
