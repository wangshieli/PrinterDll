#include "../../pch.h"
#include "Zzsfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/QRGenerator/QRGenerator.h"
#include "../../Helper/QRGenerator/Base64.h"

#define LINEFEED_P (22+4) //换行数，标识 竖向
#define LINEFEED_L (16) //换行数，标识 横向

#define XU_W	100
#define MC_W	400
#define XH_W	250
#define DW_W	192
#define SL_W	192
#define DJ_W	200
#define JE_W	250
#define SLV_W	100
#define SE_W	250

CZzsfpdy::CZzsfpdy() :m_nOrientation(DMORIENT_PORTRAIT),
m_nPageSize(LINEFEED_P),
m_sHx("")
{
}

CZzsfpdy::~CZzsfpdy()
{
}

CString CZzsfpdy::Dlfpdy(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	ZZSFP_FPXX fpmx;
	CString printXml("");
	m_sPrinterName.Empty();
	TRACE("wsl:");
	OutputDebugString(sInputInfo);

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
	if (xml.FindElem("hx")) m_sHx = xml.GetData();
	if (xml.FindElem("printtasknameflag")) m_sPrintTaskNameFlag = xml.GetData();
	if (xml.FindElem("kjfs")) fpdy.sKJFS = xml.GetData();
	if (xml.FindElem("orientation")) m_nOrientation = atoi(xml.GetData());
	m_iPldy = atoi(fpdy.sDyfs.GetBuffer(0));

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

	if (fpdy.sDylx.CompareNoCase("1") == 0 && fpmx.sQdbz.CompareNoCase("0") == 0)
	{
		fpdy.sErrorCode = "-3";
		fpdy.sErrorInfo = "本张发票没有清单，不能以清单形式打印";
		return GenerateXMLFpdy(fpdy);
	}

	printXml = GenerateFpdyXml(fpmx, fpdy.sDylx, fpdy);

	if (fpdy.sDylx.CompareNoCase("0") == 0)
	{
		rtn = Print(printXml, fpmx.sHjje, fpmx.sHjse);
	}

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CZzsfpdy::Print(LPCTSTR billXml, CString hjje, CString hjse)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	InitXYoff();

	int _nHjjeLen = strlen(hjje);
	int _nHjseLen = strlen(hjse);

	do
	{
		CMarkup xml;
		if (!xml.SetDoc(billXml) || !xml.FindElem("Print"))
		{
			nrt = -5;// 打印内容解析失败
			break;
		}
		xml.IntoElem();

		nrt = InitPrinter(FPWidth, FPLength);
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

		double nHjjeFontSize = 21, nHjseFontSize = 21;
		if (_nHjjeLen > 8)
		{
			nHjjeFontSize = 18.5;
		}

		if (_nHjseLen > 8)
		{
			nHjseFontSize = 18.5;
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
				::StretchBlt(m_hPrinterDC, nXoff + 80 + 180 - nQRCodeSize, -(nYoff + 30 + (180 - nQRCodeSize)), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}
			else
			{
				int nScaledWidth = 160;	//GetDeviceCaps (m_hPrinterDC, HORZRES);
				int nScaledHeight = 160;	//GetDeviceCaps (m_hPrinterDC, VERTRES);
				::StretchBlt(m_hPrinterDC, 190 + nXoff, -110 - nYoff, nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}

			::SelectObject(dcMem, hOldBmp);
			::DeleteDC(dcMem);
			::DeleteObject(hBitmap);

			RECT itemRect;

			int x = 1320 + 30;
			int y = 610;
			int w = 280 - 30;
			int h = 50;
			int nFontSize = LS_10;
			CString strFontName = FT;
			int z = AM_VCR;
			CString strText = hjje;

			itemRect.left = x + nXoff + 190;
			itemRect.top = (-y - nYoff - 300);
			itemRect.right = x + nXoff + 190 + w;
			itemRect.bottom = (-y - h - nYoff - 300);

			LONG r = PaintTile2(1, nFontSize, strFontName, itemRect, strText, z);
			int n_x_RMB1 = itemRect.right - (r - itemRect.left) - 30, n_y_RMB1 = itemRect.top - 5;
			
			MoveToEx(m_hPrinterDC, n_x_RMB1, n_y_RMB1, NULL);
			LineTo(m_hPrinterDC, n_x_RMB1 + 10, n_y_RMB1 - 13);
			MoveToEx(m_hPrinterDC, n_x_RMB1 + 20, n_y_RMB1, NULL);
			LineTo(m_hPrinterDC, n_x_RMB1 + 10, n_y_RMB1 - 13);
			MoveToEx(m_hPrinterDC, n_x_RMB1 + 10, n_y_RMB1 - 13, NULL);
			LineTo(m_hPrinterDC, n_x_RMB1 + 10, n_y_RMB1 - 31);
			MoveToEx(m_hPrinterDC, n_x_RMB1, n_y_RMB1 - 13, NULL);
			LineTo(m_hPrinterDC, n_x_RMB1 + 20, n_y_RMB1 - 13);
			MoveToEx(m_hPrinterDC, n_x_RMB1, n_y_RMB1 - 22, NULL);
			LineTo(m_hPrinterDC, n_x_RMB1 + 20, n_y_RMB1 - 22);
			if (hjse.CompareNoCase("***") != 0)
			{
				//xywhsf(fpmx.hjse, 1730, 610, 270, 50, LS_10, FT, AM_VCR);
				x = 1730 + 30;
				y = 610;
				w = 270 - 30;
				h = 50;
				nFontSize = LS_10;
				strFontName = FT;
				z = AM_VCR;
				strText = hjse;

				itemRect.left = x + nXoff + 190;
				itemRect.top = (-y - nYoff - 300);
				itemRect.right = x + nXoff + 190 + w;
				itemRect.bottom = (-y - h - nYoff - 300);

				r = PaintTile2(1, nFontSize, strFontName, itemRect, strText, z);
				int n_x_RMB2 = itemRect.right - (r - itemRect.left) - 30, n_y_RMB2 = itemRect.top - 5;

				MoveToEx(m_hPrinterDC, n_x_RMB2, n_y_RMB2, NULL);
				LineTo(m_hPrinterDC, n_x_RMB2 + 10, n_y_RMB2 - 13);
				MoveToEx(m_hPrinterDC, n_x_RMB2 + 20, n_y_RMB2, NULL);
				LineTo(m_hPrinterDC, n_x_RMB2 + 10, n_y_RMB2 - 13);
				MoveToEx(m_hPrinterDC, n_x_RMB2 + 10, n_y_RMB2 - 13, NULL);
				LineTo(m_hPrinterDC, n_x_RMB2 + 10, n_y_RMB2 - 31);
				MoveToEx(m_hPrinterDC, n_x_RMB2, n_y_RMB2 - 13, NULL);
				LineTo(m_hPrinterDC, n_x_RMB2 + 20, n_y_RMB2 - 13);
				MoveToEx(m_hPrinterDC, n_x_RMB2, n_y_RMB2 - 22, NULL);
				LineTo(m_hPrinterDC, n_x_RMB2 + 20, n_y_RMB2 - 22);
			}

			int n_x_RMB3 = 1600 + 190, n_y_RMB3 = -685 - 300;
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
			itemRect.top = (-y - nYoff - 300);
			itemRect.right = x + nXoff + 190 + w;
			itemRect.bottom = (-y - h - nYoff - 300);

			PaintTile(nFontSize, strFontName, itemRect, strText, z, fc);
		}

		// 输出大写金额开头圈叉符号
		int x, y, tx, ty;
		if (xml.FindElem("OX"))
		{
			x = atoi(xml.GetAttrib("x"));
			y = atoi(xml.GetAttrib("y"));
			tx = x + nXoff + 190;
			ty = -(y + nYoff) - 300;

			::Ellipse(m_hPrinterDC, tx - 20, ty + 10, tx + 30 - 20, ty - 30 + 10);
			::MoveToEx(m_hPrinterDC, tx + 4 - 20, ty - 4 + 10, NULL);
			::LineTo(m_hPrinterDC, tx + 25 - 20, ty - 25 + 10);
			::MoveToEx(m_hPrinterDC, tx + 24 - 20, ty - 4 + 10, NULL);
			::LineTo(m_hPrinterDC, tx + 4 - 20, ty - 24 + 10);
		}
		::EndPage(m_hPrinterDC);
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

ZZSFP_FPXX CZzsfpdy::ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy)
{
	CMarkup xml;
	ZZSFP_FPXX fpxx;
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
	if (xml.FindElem("skm"))   fpxx.sSkm = xml.GetData();
	if (xml.FindElem("jym"))      fpxx.sJym = xml.GetData();//校验码
	if (xml.FindElem("tspz"))	 fpxx.sTspz = xml.GetData();
	if (xml.FindElem("xhdwsbh"))  fpxx.sXhdwsbh = xml.GetData();//销货单位识别号
	if (xml.FindElem("xhdwmc"))   fpxx.sXhdwmc = xml.GetData();//销货单位名称
	if (xml.FindElem("xhdwdzdh")) fpxx.sXhdwdzdh = xml.GetData();//销货单位地址电话
	if (xml.FindElem("xhdwyhzh")) fpxx.sXhdwyhzh = xml.GetData();//销货单位银行账号
	if (xml.FindElem("ghdwsbh"))  fpxx.sGhdwsbh = xml.GetData();//购货单位识别号
	if (xml.FindElem("ghdwmc"))   fpxx.sGhdwmc = xml.GetData();//购货单位名称
	if (xml.FindElem("ghdwdzdh")) fpxx.sGhdwdzdh = xml.GetData();//购货单位地址电话
	if (xml.FindElem("ghdwyhzh")) fpxx.sGhdwyhzh = xml.GetData();//购货单位银行账号
	if (xml.FindElem("zsfs"))	 fpxx.sZsfs = xml.GetData();	// 征税方式

	fpxx.sKprq.Remove('-');

	if (fpxx.sFpzt.CompareNoCase("02") == 0)
	{
		m_sFpzt = fpxx.sFpzt;
		return fpxx;
	}

	if (xml.FindElem("fyxm"))
	{
		fpxx.iFyxmCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxx.iFyxmCount; i++)
		{
			xml.FindElem("group");
			xml.IntoElem();
			if (xml.FindElem("fphxz"))fpxx.fyxmxx[i].sFphxz = xml.GetData();
			if (xml.FindElem("spmc")) fpxx.fyxmxx[i].sSpmc = xml.GetData();
			if (xml.FindElem("spsm")) fpxx.fyxmxx[i].sSpsm = xml.GetData();
			if (xml.FindElem("ggxh")) fpxx.fyxmxx[i].sGgxh = xml.GetData();
			if (xml.FindElem("dw"))   fpxx.fyxmxx[i].sDw = xml.GetData();
			if (xml.FindElem("spsl")) fpxx.fyxmxx[i].sSpsl = xml.GetData();
			if (xml.FindElem("dj"))   fpxx.fyxmxx[i].sDj = xml.GetData();
			if (xml.FindElem("je"))   fpxx.fyxmxx[i].sJe = xml.GetData();
			if (xml.FindElem("sl"))   fpxx.fyxmxx[i].sSl = xml.GetData();
			if (xml.FindElem("se"))   fpxx.fyxmxx[i].sSe = xml.GetData();
			if (xml.FindElem("hsbz")) fpxx.fyxmxx[i].sHsbz = xml.GetData();
			if (xml.FindElem("lslbs"))fpxx.fyxmxx[i].sLslbs = xml.GetData();
			xml.OutOfElem();

			if (fpxx.fyxmxx[i].sSl.CompareNoCase("0") != 0 &&
				fpxx.fyxmxx[i].sSl.CompareNoCase("0.0") != 0 &&
				fpxx.fyxmxx[i].sSl.CompareNoCase("0.00") != 0 &&
				fpxx.fyxmxx[i].sSl.CompareNoCase("0.000") != 0)
			{
				if (fpxx.fyxmxx[i].sSl.GetLength() > 0)
				{
					double sTempSl = atof(fpxx.fyxmxx[i].sSl.GetBuffer(0)) * 100;
					fpxx.fyxmxx[i].sSl.Format("%g%s", sTempSl, "%");
				}

				if (fpxx.fyxmxx[i].sSe.Find('.') != -1)  //有小数点
				{
					int leng_Hjje = fpxx.fyxmxx[i].sSe.GetLength();
					if (fpxx.fyxmxx[i].sSe.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
					{
						fpxx.fyxmxx[i].sSe += "0";
					}
				}
				else
				{
					if (fpxx.fyxmxx[i].sSe.GetLength() > 0)
						fpxx.fyxmxx[i].sSe += ".00";
					else
						fpxx.fyxmxx[i].sSe += "0.00";
				}
			}
			else
			{
				if (fpxx.fyxmxx[i].sLslbs.CompareNoCase("1") == 0)
				{
					fpxx.fyxmxx[i].sSl.Format("%s", "免税");
				}
				else if (fpxx.fyxmxx[i].sLslbs.CompareNoCase("2") == 0)
				{
					fpxx.fyxmxx[i].sSl.Format("%s", "不征税");
				}
				else
				{
					fpxx.fyxmxx[i].sSl.Format("%s", "0%");
				}
			}

			if (fpxx.fyxmxx[i].sSl.CompareNoCase("1.5%") == 0)
			{
				fpxx.fyxmxx[i].sSl.Format("%s", "***");
			}

			if (fpdy.sFplxdm.CompareNoCase("004") != 0)
			{
				if (fpxx.fyxmxx[i].sSe.CompareNoCase("0") == 0 ||
					fpxx.fyxmxx[i].sSe.CompareNoCase("0.0") == 0 ||
					fpxx.fyxmxx[i].sSe.CompareNoCase("0.00") == 0)
				{
					fpxx.fyxmxx[i].sSe.Format("***");
				}
			}

			if (fpxx.sZsfs.CompareNoCase("2") == 0)
			{
				fpxx.fyxmxx[i].sSl.Format("***");
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
			if (fpxx.fyxmxx[i].sSe.CompareNoCase("-0.00") == 0)
			{
				fpxx.fyxmxx[i].sSe.Delete(0, 1);
			}
		}
		xml.OutOfElem();
	}

	if (xml.FindElem("zhsl"))   fpxx.sZhsl = xml.GetData();
	if (xml.FindElem("hjje"))   fpxx.sHjje = xml.GetData();
	if (xml.FindElem("hjse"))   fpxx.sHjse = xml.GetData();

	fpxx.sZhsl.TrimLeft(_T("0."));
	fpxx.sZhsl = fpxx.sZhsl.Left(2);
	if (fpxx.sZhsl.GetLength() > 0)
	{
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
	}
	else
	{
		//		fpxx.sHjse.Format("***");
	}

	if (fpdy.sFplxdm.CompareNoCase("004") != 0)
	{
		if (fpxx.sHjse.CompareNoCase("0") == 0 ||
			fpxx.sHjse.CompareNoCase("0.0") == 0 ||
			fpxx.sHjse.CompareNoCase("0.00") == 0)
		{
			fpxx.sHjse.Format("***");
		}
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
	if (xml.FindElem("fhr"))    fpxx.sFhr = xml.GetData();
	if (xml.FindElem("kpr"))    fpxx.sKpr = xml.GetData();
	if (xml.FindElem("jmbbh"))  fpxx.sJmbbh = xml.GetData();
	if (xml.FindElem("zyspmc")) fpxx.sZyspmc = xml.GetData();
	if (xml.FindElem("spsm"))   fpxx.sSpsm = xml.GetData();
	if (xml.FindElem("qdbz"))   fpxx.sQdbz = xml.GetData();
	if (xml.FindElem("ssyf"))   fpxx.sSsyf = xml.GetData();
	if (xml.FindElem("kpjh"))   fpxx.sKpjh = xml.GetData();
	if (xml.FindElem("tzdbh"))  fpxx.sTzdbh = xml.GetData();
	if (xml.FindElem("yfpdm"))  fpxx.sYfpdm = xml.GetData();
	if (xml.FindElem("yfphm"))  fpxx.sYfphm = xml.GetData();
	if (xml.FindElem("zfrq"))   fpxx.sZfrq = xml.GetData();
	if (xml.FindElem("zfr"))    fpxx.sZfr = xml.GetData();
	if (xml.FindElem("qmcs"))   fpxx.sQmcs = xml.GetData();
	if (xml.FindElem("qmz"))    fpxx.sQmz = xml.GetData();
	if (xml.FindElem("ykfsje")) fpxx.sYkfsje = xml.GetData();

	fpxx.sSkm1 = fpxx.sSkm.Mid(0, 28);
	fpxx.sSkm2 = fpxx.sSkm.Mid(28, 28);
	fpxx.sSkm3 = fpxx.sSkm.Mid(56, 28);
	fpxx.sSkm4 = fpxx.sSkm.Mid(84, 28);

	fpxx.kprq.syear = fpxx.sKprq.Mid(0, 4);
	fpxx.kprq.smouth = fpxx.sKprq.Mid(4, 2);
	fpxx.kprq.sday = fpxx.sKprq.Mid(6, 2);

	if (fpxx.sZhsl.GetLength() > 0)
	{
		fpxx.sZhsl.TrimLeft(_T("0."));
		fpxx.sZhsl = fpxx.sZhsl.Left(2);
		fpxx.sZhsl = fpxx.sZhsl + "%";
	}
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
		fpxx.sJshjDx = "(负数)" + fpxx.sJshjDx;
		if (fpxx.sBz.GetLength() > 48)
		{
			CString Temp1 = fpxx.sBz.Mid(0, 48);
			CString Temp2 = fpxx.sBz.Mid(48, fpxx.sBz.GetLength() - 48);
			//			CString Temp3 = "                ";
			fpxx.sBz = Temp1 + /* Temp3 + */Temp2;
		}
	}

	CString strFplx = "";
	if (fpdy.sFplxdm.CompareNoCase("004") == 0)
	{
		strFplx = "01";
	}
	if (fpdy.sFplxdm.CompareNoCase("007") == 0)
	{
		strFplx = "04";
	}

	if (fpdy.sDylx.CompareNoCase("0") == 0)
	{
		fpxx.sKprq = fpxx.sKprq.Left(8);
		CString strEwm = "01," + strFplx + "," + fpxx.sFpdm + "," + fpxx.sFphm + "," + fpxx.sHjje + "," + fpxx.sKprq + "," + fpxx.sJym;
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
		strcat_s(sTempPath, MAX_PATH, "\\Ewm.bmp");
		if (fopen_s(&pFile, sTempPath, "wb") == 0 && pFile != NULL)
		{
			fwrite(str64.c_str(), 1, outLen, pFile);
			fclose(pFile);
		}
		else
		{
			//
		}
	}

	return fpxx;
}

CString CZzsfpdy::GenerateFpdyXml(ZZSFP_FPXX fpmx, CString dylx, FPDY fpdy)
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

CString CZzsfpdy::GenerateItemXml(ZZSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;

	fpmx.sJqbhZW = "机器编号：";
	if (fpmx.sFpzt.CompareNoCase("01") == 0)
	{
		fpmx.sXxfs = "销项负数";
	}

	if (fpmx.sTspz.CompareNoCase("02") == 0)
	{
		fpmx.sNcpsg = "收购";
	}
	else if (fpmx.sTspz.CompareNoCase("08") == 0 || fpmx.sTspz.CompareNoCase("18") == 0)
	{
		fpmx.sCpy = "成品油";
	}

	if (fpdy.sKJFS.CompareNoCase("1") == 0)
	{
		fpmx.sNcpsg = "代开";
	}

	if (fpdy.sFplxdm.CompareNoCase("007") == 0)
	{
		if (m_sHx.IsEmpty())
		{
			fpmx.sBzF = "校验码 " + fpmx.sJym.Mid(0, 5) + " " + fpmx.sJym.Mid(5, 5) + " " + fpmx.sJym.Mid(10, 5) + " " + fpmx.sJym.Mid(15, 5);
		}
		else
		{
			int nTemp = 0;
			for (int i = 0; i < 4; i++)
			{
				fpmx.sJym.Insert(i * 5 + nTemp, ' ');
				++nTemp;
			}
			fpmx.sJym.Insert(0, "校验码");
		}
	}

	if (m_sHx.IsEmpty())
	{
		xywhsf(fpmx.JqbhZW, 180, -100, 350, 50, LS_9, FS, AM_VCL);
		xywhsf(fpmx.Skpbh, 180, -50, 350, 50, LS_11, FT, AM_VCL);
		xywhsf(fpmx.Xxfs, 520, -50, 330, 50, LS_12, FH, AM_VCL);
	}
	else
	{
		xywhsf(fpmx.Hxjym, LX + 220, LY + 250 - 25, LW, LH, LS_9, FT, ZL);
		xywhsf(fpmx.Xxfs, LX + 140 + 380, LY + 180, LW, LH, LS_12, FH, ZL);
	}

	xywhsf(fpmx.Cpy, 560, -100, 270, 50, LS_10, FH, AM_VCL);

	xywhsf(fpmx.Ncpsg, 390, -100, 120, 50, LS_12, FH, AM_VCL);

	xywhsf(fpmx.Fpdm, 1750, -180, 250, 50, LS_9, FS, AM_VCR);
	xywhsf(fpmx.Fphm, 1750, -130, 250, 50, LS_11, FT, AM_VCR);

	xywhsf(fpmx.Kprq, 1660, -80, 350, 60, LS_9, FT, AM_VCL);

	xywhsf(fpmx.Ghdwmc, 340, 0, 780, 55, LS_9, FS, AM_VCL);
	xywhsf(fpmx.Ghdwsbh, 340, 55, 780, 55, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Ghdwdzdh, 340, 110, 780, 55, LS_9, FS, AM_VCL);
	xywhsf(fpmx.Ghdwyhzh, 340, 165, 780, 55, LS_9, FS, AM_VCL);


	xywhsf(fpmx.skm1, 1230, 10, 750, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.skm2, 1230, 60, 750, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.skm3, 1230, 110, 750, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.skm4, 1230, 160, 750, 50, LS_12, FT, AM_VCL);

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{
		fpmx.fyxmxx[i].sSpmc = fpmx.fyxmxx[i].sSpmc.Left(DataPrintMaxLen(fpmx.fyxmxx[i].sSpmc, 92));
		xywhsf(fpmx.fyxmxx[i].ssSpmc, 30, 260 + 45 * i, 460 - 15, 45, LS_9, FS, AM_VCL);

		xywhsf(fpmx.fyxmxx[i].ssGgxh, 540, 260 + 45 * i, 230, 45, LS_9, FS, AM_VCL);

		fpmx.fyxmxx[i].sDw = fpmx.fyxmxx[i].sDw.Left(DataPrintMaxLen(fpmx.fyxmxx[i].sDw, 16));
		xywhsf(fpmx.fyxmxx[i].ssDw, 790, 260 + 45 * i, 100, 45, LS_9, FS, AM_VCL);

		if (fpmx.fyxmxx[i].sSpsl.GetLength() > 13)
		{
			fpmx.fyxmxx[i].sSpsl = fpmx.fyxmxx[i].sSpsl.Mid(0, 13);
		}
		xywhsf(fpmx.fyxmxx[i].ssSpsl, 920, 260 + 45 * i, 170, 45, LS_9, FS, AM_VCR);
		if (fpmx.fyxmxx[i].sDj.GetLength() > 13)
		{
			fpmx.fyxmxx[i].sDj = fpmx.fyxmxx[i].sDj.Mid(0, 13);
		}
		xywhsf(fpmx.fyxmxx[i].ssDj, 1120, 260 + 45 * i, 170, 45, LS_9, FS, AM_VCR);
		xywhsf(fpmx.fyxmxx[i].ssJe, 1320, 260 + 45 * i, 280, 45, LS_9, FS, AM_VCR);
		xywhsf(fpmx.fyxmxx[i].ssSl, 1610, 260 + 45 * i, 95, 45, LS_9, FS, AM_ZC);
		xywhsf(fpmx.fyxmxx[i].ssSe, 1730, 260 + 45 * i, 270, 45, LS_9, FT, AM_VCR);
	}
	xywhsf(fpmx.hjje, 1320 + 30, 610, 280 - 30, 50, LS_10, FT, AM_VCR_S);
	xywhsf(fpmx.hjse, 1730 + 30, 610, 270 - 30, 50, LS_10, FT, AM_VCR_S);

	xywhsf(fpmx.OX, 590, 700, 80, 85, LS_9, FT, ZL);
	xywhsf(fpmx.jshjDx, 610, 660, 790, 85, LS_9, FS, AM_VCL_S);   //大写价税合计
	xywhsf(fpmx.jshj, 1630, 660, 710, 85, LS_11, FT, AM_VCL_S);   //小写价税合计

	if (fpdy.sFplxdm.CompareNoCase("007") == 0 && m_sHx.IsEmpty())
	{
		xywhsf(fpmx.bzF, 1240, 755, 740, 40, LS_9, FS, AM_VCL);
		xywhsf(fpmx.bz, 1240, 795, 740, 155, LS_8, FS, AM_ZL_EX);
	}
	else
	{
		xywhsf(fpmx.bz, 1240, 750, 740, 200, LS_8, FS, AM_ZL_EX);
	}

	xywhsf(fpmx.Xhdwmc, 340, 750, 780, 50, LS_9, FS, AM_VCL);
	xywhsf(fpmx.Xhdwsbh, 340, 800, 780, 50, LS_12, FT, AM_VCL);
	xywhsf(fpmx.Xhdwdzdh, 340, 850, 780, 50, LS_9, FS, AM_VCL);
	xywhsf(fpmx.Xhdwyhzh, 340, 900, 780, 50, LS_9, FS, AM_VCL);

	xywhsf(fpmx.skr, 200, 950, 330, 60, LS_10, FS, AM_VCL);
	xywhsf(fpmx.fhr, 720, 950, 290, 60, LS_10, FS, AM_VCL);
	xywhsf(fpmx.kpr, 1190, 950, 250, 60, LS_10, FS, AM_VCL);

	if (m_sHx.IsEmpty())
	{
		addxml(fpmx.sJqbhZW, fpmx.JqbhZW);
		addxml(fpmx.sSkpbh, fpmx.Skpbh);
	}
	else
	{
		addxml(fpmx.sJym, fpmx.Hxjym);
	}

	addxml(fpmx.sCpy, fpmx.Cpy);
	addxml(fpmx.sNcpsg, fpmx.Ncpsg);
	addxml(fpmx.sXxfs, fpmx.Xxfs);

	addxml(fpmx.sFpdm, fpmx.Fpdm);
	addxml(fpmx.sFphm, fpmx.Fphm);
	CString time = fpmx.kprq.syear + "年" + fpmx.kprq.smouth + "月" + fpmx.kprq.sday + "日";
	addxml(time, fpmx.Kprq);

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sGhdwsbh, fpmx.Ghdwsbh);
	addxml(fpmx.sGhdwdzdh, fpmx.Ghdwdzdh);
	addxml(fpmx.sGhdwyhzh, fpmx.Ghdwyhzh);

	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sXhdwsbh, fpmx.Xhdwsbh);
	addxml(fpmx.sXhdwdzdh, fpmx.Xhdwdzdh);
	addxml(fpmx.sXhdwyhzh, fpmx.Xhdwyhzh);

	addxml(fpmx.sSkm1, fpmx.skm1);
	addxml(fpmx.sSkm2, fpmx.skm2);
	addxml(fpmx.sSkm3, fpmx.skm3);
	addxml(fpmx.sSkm4, fpmx.skm4);

	for (int j = 0; j < fpmx.iFyxmCount; j++)
	{
		addxml(fpmx.fyxmxx[j].sSpmc, fpmx.fyxmxx[j].ssSpmc);
		addxml(fpmx.fyxmxx[j].sGgxh, fpmx.fyxmxx[j].ssGgxh);
		addxml(fpmx.fyxmxx[j].sDw, fpmx.fyxmxx[j].ssDw);
		addxml(fpmx.fyxmxx[j].sSpsl, fpmx.fyxmxx[j].ssSpsl);
		addxml(fpmx.fyxmxx[j].sDj, fpmx.fyxmxx[j].ssDj);
		addxml(fpmx.fyxmxx[j].sJe, fpmx.fyxmxx[j].ssJe);
		addxml(fpmx.fyxmxx[j].sSl, fpmx.fyxmxx[j].ssSl);
		addxml(fpmx.fyxmxx[j].sSe, fpmx.fyxmxx[j].ssSe);
	}

	addxml(fpmx.sHjje, fpmx.hjje);
	addxml(fpmx.sHjse, fpmx.hjse);
	addxml(fpmx.sJshj, fpmx.jshj);
	addxml(fpmx.sJshjDx, fpmx.jshjDx);

	if (fpdy.sFplxdm.CompareNoCase("007") == 0)
	{
		addxml(fpmx.sBzF, fpmx.bzF);
	}
	addxml(fpmx.sBz, fpmx.bz);

	addxml(fpmx.sSkr, fpmx.skr);
	addxml(fpmx.sFhr, fpmx.fhr);
	addxml(fpmx.sKpr, fpmx.kpr);

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

	return xml.GetDoc();
}
