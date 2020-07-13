#include "../../pch.h"
#include "Zzsfpdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/XML/AnalyzeInXml.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"
#include "../../Helper/QRGenerator/QRGenerator.h"
#include "../../Helper/QRGenerator/Base64.h"

#define LINEFEED_P (22+4) //换行数，标识 竖向
#define LINEFEED_L (16) //换行数，标识 横向

CZzsfpdy::CZzsfpdy():m_nOrientation(DMORIENT_PORTRAIT),
	m_nPageSize(LINEFEED_P)
{
}

CZzsfpdy::~CZzsfpdy()
{
}

CString CZzsfpdy::Dlfpdy(LPTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	ZZSFP_FPXX fpmx;
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
	if (xml.FindElem("fplxdm")) fpdy.sFplxdm = xml.GetData();
	if (xml.FindElem("dylx"))   fpdy.sDylx = xml.GetData();
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
		rtn = Print(printXml, fpdy.sFplxdm, fpmx.sHjje, fpmx.sHjse);
	}
	else
	{
		rtn = PrintQD(printXml, fpdy.sFplxdm);
	}

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CZzsfpdy::Print(LPCTSTR billXml, CString strFplxdm, CString hjje, CString hjse)
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
			::DeleteDC(m_hPrinterDC);
			nrt = -3;// 启动打印任务失败
			break;
		}

		nrt = ::StartPage(m_hPrinterDC);
		if (nrt <= 0)
		{
			::EndDoc(m_hPrinterDC);
			::DeleteDC(m_hPrinterDC);
			nrt = -4;// 发送打印内容失败
			break;
		}

		int n_x = 100;
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
				::StretchBlt(m_hPrinterDC, nXoff + 80 + 180 - 160, -(nYoff + 30 + (180 - 160)), nScaledWidth, -nScaledHeight, dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			}

			::SelectObject(dcMem, hOldBmp);
			::DeleteDC(dcMem);
			::DeleteObject(hBitmap);

			MoveToEx(m_hPrinterDC, 1682 + nXoff - 95 + n_x, -945 - nYoff + 15, NULL);
			LineTo(m_hPrinterDC, 1692 + nXoff - 95 + n_x, -958 - nYoff + 15);
			MoveToEx(m_hPrinterDC, 1702 + nXoff - 95 + n_x, -945 - nYoff + 15, NULL);
			LineTo(m_hPrinterDC, 1692 + nXoff - 95 + n_x, -958 - nYoff + 15);
			MoveToEx(m_hPrinterDC, 1692 + nXoff - 95 + n_x, -958 - nYoff + 15, NULL);
			LineTo(m_hPrinterDC, 1692 + nXoff - 95 + n_x, -976 - nYoff + 15);
			MoveToEx(m_hPrinterDC, 1682 + nXoff - 95 + n_x, -958 - nYoff + 15, NULL);
			LineTo(m_hPrinterDC, 1702 + nXoff - 95 + n_x, -958 - nYoff + 15);
			MoveToEx(m_hPrinterDC, 1682 + nXoff - 95 + n_x, -967 - nYoff + 15, NULL);
			LineTo(m_hPrinterDC, 1702 + nXoff - 95 + n_x, -967 - nYoff + 15);

			MoveToEx(m_hPrinterDC, 1672 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -870 - nYoff + 10, NULL);
			LineTo(m_hPrinterDC, 1682 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -883 - nYoff + 10);
			MoveToEx(m_hPrinterDC, 1692 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -870 - nYoff + 10, NULL);
			LineTo(m_hPrinterDC, 1682 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -883 - nYoff + 10);
			MoveToEx(m_hPrinterDC, 1682 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -883 - nYoff + 10, NULL);
			LineTo(m_hPrinterDC, 1682 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -901 - nYoff + 10);
			MoveToEx(m_hPrinterDC, 1672 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -883 - nYoff + 10, NULL);
			LineTo(m_hPrinterDC, 1692 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -883 - nYoff + 10);
			MoveToEx(m_hPrinterDC, 1672 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -890 - nYoff + 10, NULL);
			LineTo(m_hPrinterDC, 1692 - (int)(_nHjjeLen * nHjjeFontSize) + nXoff - 90 + n_x - 10, -890 - nYoff + 10);
			if (hjse.CompareNoCase(" *  *  * ") != 0)
			{
				MoveToEx(m_hPrinterDC, 2042 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -870 - nYoff + 10, NULL);
				LineTo(m_hPrinterDC, 2052 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -883 - nYoff + 10);
				MoveToEx(m_hPrinterDC, 2062 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -870 - nYoff + 10, NULL);
				LineTo(m_hPrinterDC, 2052 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -883 - nYoff + 10);
				MoveToEx(m_hPrinterDC, 2052 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -883 - nYoff + 10, NULL);
				LineTo(m_hPrinterDC, 2052 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -901 - nYoff + 10);
				MoveToEx(m_hPrinterDC, 2042 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -883 - nYoff + 10, NULL);
				LineTo(m_hPrinterDC, 2062 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -883 - nYoff + 10);
				MoveToEx(m_hPrinterDC, 2042 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -890 - nYoff + 10, NULL);
				LineTo(m_hPrinterDC, 2062 - (int)(_nHjseLen * nHjseFontSize) + nXoff - 60 + n_x - 10, -890 - nYoff + 10);
			}
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
		::DeleteDC(m_hPrinterDC);
	} while (false);

	return nrt;
}

LONG CZzsfpdy::PrintQD(LPCSTR billxml, CString strFplxdm)
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

		nrt = InitPrinter(FPWidth, FPLength * 2);
		if (0 != nrt)
			break;

		DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };
		nrt = ::StartDoc(m_hPrinterDC, &di);
		if (nrt <= 0)
		{
			::DeleteDC(m_hPrinterDC);
			nrt = -3;// 启动打印任务失败
			break;
		}

		while (xml.FindElem("NewPage"))
		{
			nrt = ::StartPage(m_hPrinterDC);
			if (nrt <= 0)
			{
				::EndDoc(m_hPrinterDC);
				::DeleteDC(m_hPrinterDC);
				nrt = -4;// 发送打印内容失败
				break;
			}

			int npn = atoi(xml.GetAttrib("pn"));
			xml.IntoElem();

			CString strTemp;
			CFont *pOldFont;
			CFont fontHeader;
			fontHeader.CreatePointFont(150, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			CRect rect1;
			rect1.left = 0 + nXoff;
			rect1.top = 0 - nYoff;
			rect1.right = 2010 + nXoff;
			rect1.bottom = -1830 - 900 - nYoff;
			if (m_nOrientation == DMORIENT_LANDSCAPE)
			{
				rect1.left = 0 + nXoff;
				rect1.top = 0 - nYoff;
				rect1.right = 2010 + nXoff;
				rect1.bottom = -2010 - nYoff;
			}
			
			TextOut(m_hPrinterDC, rect1.left + 670, rect1.top - 20, CString("销售货物或者提供应税劳务、服务清单"), 34);
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			fontHeader.CreatePointFont(120, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			TextOut(m_hPrinterDC, rect1.left + 40, rect1.top - 100, CString("购物单位名称："), 14);
			TextOut(m_hPrinterDC, rect1.left + 40, rect1.top - 165, CString("销货单位名称："), 14);
			if (!m_sTempFplxdm.CompareNoCase("007"))
			{
				TextOut(m_hPrinterDC, rect1.left + 40, rect1.top - 230, CString("所属增值税普通发票代码："), 24);
			}
			else if (!m_sTempFplxdm.CompareNoCase("004"))
			{
				TextOut(m_hPrinterDC, rect1.left + 40, rect1.top - 230, CString("所属增值税专用发票代码："), 24);
			}
			else
			{
				//
			}
			TextOut(m_hPrinterDC, rect1.left + 1005, rect1.top - 230, CString("号码："), 6);
			TextOut(m_hPrinterDC, rect1.left + 1005 + 600, rect1.top - 230, CString("共"), 2);
			TextOut(m_hPrinterDC, rect1.left + 1005 + 680, rect1.top - 230, CString("页"), 2);
			TextOut(m_hPrinterDC, rect1.left + 1005 + 740, rect1.top - 230, CString("第"), 2);
			TextOut(m_hPrinterDC, rect1.left + 1005 + 830, rect1.top - 230, CString("页"), 2);
			char buffer[5] = { 0 };
			itoa(npn, buffer, 10);
			TextOut(m_hPrinterDC, rect1.left + 1005 + 780, rect1.top - 230, buffer, strlen(buffer));
			itoa(m_nAllPageNum, buffer, 10);
			TextOut(m_hPrinterDC, rect1.left + 1005 + 640, rect1.top - 230, buffer, strlen(buffer));

			TextOut(m_hPrinterDC, rect1.left + 80, rect1.bottom + 108, CString("销货单位(章)："), 14);
			TextOut(m_hPrinterDC, rect1.left + 1206, rect1.bottom + 108, CString("开票日期："), 10);

			Rectangle(m_hPrinterDC, rect1.left + 38, rect1.top - 300, rect1.right - 38, rect1.bottom + 150);
			MoveToEx(m_hPrinterDC, rect1.left + 38, rect1.top - 370, NULL);
			LineTo(m_hPrinterDC, rect1.right - 38, rect1.top - 370);

			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject(); // 框体打印完成

			fontHeader.CreatePointFont(95, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));
			TextOut(m_hPrinterDC, rect1.left + 55, rect1.top - 315, CString("序号"), 4);

			MoveToEx(m_hPrinterDC, rect1.left + 38, rect1.bottom + 230, NULL);
			LineTo(m_hPrinterDC, rect1.right - 38, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 50, rect1.bottom + 208, CString("备注"), 4);
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			fontHeader.CreatePointFont(80, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));
			TextOut(m_hPrinterDC, rect1.left + 80, rect1.bottom + 50, CString("注：本清单一式两联：\
							第一联，销售方留存；第二联，销售方送交购买方"), 72);
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			TextOut(m_hPrinterDC, rect1.left + 50, rect1.bottom + 290, CString("合计"), 4);
			TextOut(m_hPrinterDC, rect1.left + 50, rect1.bottom + 350, CString("折扣"), 4);
			TextOut(m_hPrinterDC, rect1.left + 50, rect1.bottom + 410, CString("小计"), 4);

			fontHeader.CreatePointFont(95, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));
			MoveToEx(m_hPrinterDC, rect1.left + 140, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 140, rect1.bottom + 150);
			TextOut(m_hPrinterDC, rect1.left + 210, rect1.top - 315, CString("货物（劳务）名称"), 16);

			MoveToEx(m_hPrinterDC, rect1.left + 620, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 620, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 670, rect1.top - 315, CString("规格型号"), 8);

			MoveToEx(m_hPrinterDC, rect1.left + 770 + 105, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 770 + 105, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 790 + 125, rect1.top - 315, CString("单位"), 4);

			MoveToEx(m_hPrinterDC, rect1.left + 830 + 200, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 830 + 200, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 870 + 210, rect1.top - 315, CString("数量"), 4);

			MoveToEx(m_hPrinterDC, rect1.left + 910 + 280, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 910 + 280, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 940 + 310, rect1.top - 315, CString("单价"), 4);

			MoveToEx(m_hPrinterDC, rect1.left + 1030 + 360, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 1030 + 360, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 1050 + 430, rect1.top - 315, CString("金额"), 4);

			MoveToEx(m_hPrinterDC, rect1.left + 1140 + 500, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 1140 + 500, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 1160 + 500, rect1.top - 315, CString("税率"), 4);

			MoveToEx(m_hPrinterDC, rect1.left + 1180 + 560, rect1.top - 300, NULL);
			LineTo(m_hPrinterDC, rect1.left + 1180 + 560, rect1.bottom + 230);
			TextOut(m_hPrinterDC, rect1.left + 1800 + (rect1.right - 38 - rect1.left - 1800) / 2 - 60, rect1.top - 315, CString("税额"), 4);
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			while (xml.FindElem("Item"))
			{
				CFont ftPrint;
				RECT printRect;

				int x = atoi(xml.GetAttrib("x"));
				int y = atoi(xml.GetAttrib("y"));
				int w = atoi(xml.GetAttrib("w"));
				int h = atoi(xml.GetAttrib("h"));
				int nFontSize = atoi(xml.GetAttrib("s"));
				CString strFontName = xml.GetAttrib("f");
				int z = atoi(xml.GetAttrib("z"));
				CString strText = xml.GetData();

				ftPrint.CreatePointFont(nFontSize, strFontName, CDC::FromHandle(m_hPrinterDC));
				::SelectObject(m_hPrinterDC, ftPrint);

				printRect.left = x + nXoff + 130;
				printRect.top = (-y - 365 - nYoff);
				printRect.right = x + nXoff + 160 + w;
				printRect.bottom = (-y - 365 - h - nYoff);

				if (z == 0)
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
				}
				else
				{
					::DrawText(m_hPrinterDC, strText, strText.GetLength(), &printRect, DT_RIGHT | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
				}

				ftPrint.DeleteObject();
			}
			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
		::DeleteDC(m_hPrinterDC);
	} while (false);

	return nrt;
}

ZZSFP_FPXX CZzsfpdy::ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy)
{
	CMarkup xml;
	ZZSFP_FPXX fpxx;
	ZZSFP_FYQDXX qdxx;
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

	if (xml.FindElem("qdxm"))
	{
		fpxx.iFyqdCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxx.iFyqdCount; i++)
		{
			xml.FindElem("group");
			xml.IntoElem();
			if (xml.FindElem("fphxz")) qdxx.sFphxz = xml.GetData();
			if (xml.FindElem("spmc"))   qdxx.sSpmc = xml.GetData();
			if (xml.FindElem("spsm"))   qdxx.sSpsm = xml.GetData();
			if (xml.FindElem("ggxh"))   qdxx.sGgxh = xml.GetData();
			if (xml.FindElem("dw"))     qdxx.sDw = xml.GetData();
			if (xml.FindElem("spsl"))   qdxx.sSpsl = xml.GetData();
			if (xml.FindElem("dj"))     qdxx.sDj = xml.GetData();
			if (xml.FindElem("je"))     qdxx.sJe = xml.GetData();
			if (xml.FindElem("sl"))     qdxx.sSl = xml.GetData();
			if (i == 0)
			{
				sl = qdxx.sSl;
			}
			if (qdxx.sSl.CompareNoCase(sl) != 0)
			{
				booltrue = true;  //清单多税率
				fpxx.fyxmxx[0].sSl = "";
			}
			if (xml.FindElem("se"))     qdxx.sSe = xml.GetData();
			if (xml.FindElem("hsbz"))   qdxx.sHsbz = xml.GetData();
			if (xml.FindElem("lslbs"))  qdxx.sLslbs = xml.GetData();
			xml.OutOfElem();
			CString sTempSl = qdxx.sSl;
			sTempSl.TrimLeft(_T("0."));
			sTempSl = sTempSl.Left(2);
			if (sTempSl.GetLength() > 0)
			{
				double dTempSl = atof(qdxx.sSl.GetBuffer(0)) * 100;
				qdxx.sSl.Format("%g%s", dTempSl, "%");
				if (qdxx.sSe.Find('.') != -1)  //有小数点
				{
					int leng_Hjje = qdxx.sSe.GetLength();
					if (qdxx.sSe.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
					{
						qdxx.sSe += "0";
					}
				}
				else
				{
					if (qdxx.sSe.GetLength() > 0)
						qdxx.sSe += ".00";
					else
						qdxx.sSe += "0.00";
				}
			}
			else
			{
				if (qdxx.sLslbs.CompareNoCase("1") == 0)
				{
					qdxx.sSl.Format("%s", "免税");
				}
				else if (qdxx.sLslbs.CompareNoCase("2") == 0)
				{
					qdxx.sSl.Format("%s", "不征税");
				}
				else if (qdxx.sLslbs.CompareNoCase("3") == 0)
				{
					qdxx.sSl.Format("%s", "0%");
				}
				else
				{
					qdxx.sSl.Format("%s", "0%");
				}
			}
			if (fpdy.sFplxdm.CompareNoCase("004") != 0)
			{
				if (qdxx.sSe.CompareNoCase("0") == 0 ||
					qdxx.sSe.CompareNoCase("0.0") == 0 ||
					qdxx.sSe.CompareNoCase("0.00") == 0)
				{
					qdxx.sSe.Format("***");
				}
			}
			if (qdxx.sJe.Find('.') != -1)  //有小数点
			{
				int leng_Hjje = qdxx.sJe.GetLength();
				if (qdxx.sJe.Mid(leng_Hjje - 3, 1).CompareNoCase(".") != 0) //小数点后只有一位
				{
					qdxx.sJe += "0";
				}
			}
			else
			{
				if (qdxx.sJe.GetLength() > 0)
					qdxx.sJe += ".00";
				else
					qdxx.sJe += "0.00";
			}

			if (qdxx.sSe.CompareNoCase("-0.00") == 0)
			{
				qdxx.sSe.Delete(0, 1);
			}

			fpxx.fpqdxx.push_back(qdxx);
		}
		xml.OutOfElem();
	}

	if (xml.FindElem("qtxm"))
	{
		fpxx.iFyqtCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < fpxx.iFyqtCount; i++)
		{
			xml.FindElem("group");
			xml.IntoElem();
			if (xml.FindElem("sl")) fpxx.fyqtxx[i].sSl = xml.GetData();
			if (xml.FindElem("je")) fpxx.fyqtxx[i].sJe = xml.GetData();
			if (xml.FindElem("se")) fpxx.fyqtxx[i].sSe = xml.GetData();
			xml.OutOfElem();
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
	ZLib_ChineseFee(strDxje, 100, strsc);//小写金额转换为大写金额
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
		else
		{
			xml += GenerateItemMXXml(fpmx);
		}
	}
	xml += "</Print>";

	return xml;
}

CString CZzsfpdy::GenerateItemXml(ZZSFP_FPXX fpmx, FPDY fpdy)
{
	CMarkup xml;

	//计算出购货方名称的长度	
	char* str = fpmx.sGhdwmc.GetBuffer(0);
	int LengOfGhfmc = strlen(str);

	//计算出购货方地址电话长度
	char* str2 = fpmx.sGhdwdzdh.GetBuffer(0);
	int LengOfGhfdzdh = strlen(str2);

	//计算出购货方银行账号
	char* str3 = fpmx.sGhdwyhzh.GetBuffer(0);
	int LengOfGhfyhzh = strlen(str3);

	//计算出销货方名称的长度
	char* strXH = fpmx.sXhdwmc.GetBuffer(0);
	int LengOfXhfmc = strlen(strXH);

	//计算出销货单位地址电话长度
	char* str4 = fpmx.sXhdwdzdh.GetBuffer(0);
	int LengOfXhdwdzdh = strlen(str4);

	//计算出销货单位银行账号
	char* str5 = fpmx.sXhdwyhzh.GetBuffer(0);
	int LengOfXhdwyhzh = strlen(str5);

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

	//fpmx.sTspzbs = "农场品销售";"农场品收购";
	//				            X                                      Y              W	          H	          S		        F      Z
	if (m_sHx.IsEmpty())
	{
		xywhsf(fpmx.JqbhZW, LX + 220 - 10, LY + 200 - 10, LW, LH, LS_9, FS, ZL);
		xywhsf(fpmx.Skpbh, LX + 220 - 10, LY + 250 - 10, LW, LH, LS_11, FT, ZL);
		xywhsf(fpmx.Xxfs, LX + 140 + 380, LY + 240, LW, LH, LS_12, FH, ZL);
	}
	else
	{
		xywhsf(fpmx.Hxjym, LX + 220, LY + 250 - 25, LW, LH, LS_9, FT, ZL);
		xywhsf(fpmx.Xxfs, LX + 140 + 380, LY + 180, LW, LH, LS_12, FH, ZL);
	}
	//	xywhsf(fpmx.Xxfs          ,LX+ 140+380                            ,LY+ 240       ,LW         ,LH         ,LS_12         ,FH    ,ZL);

	xywhsf(fpmx.Cpy, LX + 140 + 390, LY + 180, LW, LH, LS_10, FH, ZL);

	xywhsf(fpmx.Ncpsg, LX + 380, LY + 180, LW, LH, LS_12, FH, ZL);
	//xywhsf(fpmx.Ncpxssgqy     ,LX+ 50								 ,LY+ 170       ,LW         ,LH         ,LS_16         ,FH    ,ZL);

	if (fpdy.sFplxdm.Compare("007") == 0 && fpmx.sFpdm.GetLength() == 12)
	{
		xywhsf(fpmx.Fpdm, LX + 1855 - 10/* 1910 */, LY + 130 - 10, LW, LH, LS_9, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Fpdm, LX + 1890 - 10/* 1910 */, LY + 130 - 10, LW, LH, LS_9, FS, ZL);
	}
	xywhsf(fpmx.Fphm, LX + 1860 - 10/* 1880 */, LY + 170 - 10, LW, LH, LS_11, FT, ZL);

	xywhsf(fpmx.kprq.year, LX + 1730, LY + 220, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.kprq.Year, LX + 1808, LY + 220, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.kprq.mouth, LX + 1848, LY + 220, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.kprq.Mouth, LX + 1883, LY + 220, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.kprq.day, LX + 1918 + 5, LY + 220, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.kprq.Day, LX + 1958, LY + 220, LW, LH, LS_9, FT, ZL);
	xywhsf(fpmx.OX, LX + 650, LY + 980, LW, LH, LS_9, FT, ZL);

	if (LengOfGhfmc <= 50)
	{
		xywhsf(fpmx.Ghdwmc, LX + 380, LY + 310, LW + 800, LH + 60, LS_9, FS, ZL);
	}
	else if (LengOfGhfmc > 50 && LengOfGhfmc <= 56)
	{
		xywhsf(fpmx.Ghdwmc, LX + 380, LY + 310, LW + 800, LH + 60, LS_8, FS, ZL);
	}
	else if (LengOfGhfmc > 56 && LengOfGhfmc <= 64)
	{
		xywhsf(fpmx.Ghdwmc, LX + 380, LY + 310, LW + 800, LH + 60, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Ghdwmc, LX + 380, LY + 310, LW + 800, LH + 60, LS_6, FS, ZL);
	}

	xywhsf(fpmx.Ghdwsbh, LX + 360 + 50, LY + 310 + 55 - 15, LW, LH, LS_12, FT, ZL);

	if (LengOfGhfdzdh <= 50)
	{
		xywhsf(fpmx.Ghdwdzdh, LX + 380/*360*/, LY + 310 + 110 - 5, LW + 800, LH + 60, LS_9, FS, ZL);
	}
	else if (LengOfGhfdzdh > 50 && LengOfGhfdzdh <= 56)
	{
		xywhsf(fpmx.Ghdwdzdh, LX + 380, LY + 310 + 110 - 5, LW + 800, LH + 60, LS_8, FS, ZL);
	}
	else if (LengOfGhfdzdh > 56 && LengOfGhfdzdh <= 64)
	{
		xywhsf(fpmx.Ghdwdzdh, LX + 380, LY + 310 + 110 - 5, LW + 800, LH + 60, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Ghdwdzdh, LX + 380, LY + 310 + 110 - 5, LW + 800, LH + 60, LS_6, FS, ZL);
	}

	if (LengOfGhfyhzh <= 50)
	{
		xywhsf(fpmx.Ghdwyhzh, LX + 380, LY + 310 + 160 - 5, LW + 800, LH + 60, LS_9, FS, ZL);
	}
	else if (LengOfGhfyhzh > 50 && LengOfGhfyhzh <= 56)
	{
		xywhsf(fpmx.Ghdwyhzh, LX + 380, LY + 310 + 160 - 5, LW + 800, LH + 60, LS_8, FS, ZL);
	}
	else if (LengOfGhfyhzh > 56 && LengOfGhfyhzh <= 64)
	{
		xywhsf(fpmx.Ghdwyhzh, LX + 380, LY + 310 + 160 - 5, LW + 800, LH + 60, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Ghdwyhzh, LX + 380, LY + 310 + 160 - 5, LW + 800, LH + 60, LS_6, FS, ZL);
	}

	xywhsf(fpmx.skm1, LX + 1310 - 35, LY + 310, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.skm2, LX + 1310 - 35, LY + 310 + 45, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.skm3, LX + 1310 - 35, LY + 310 + 90, LW, LH, LS_12, FT, ZL);
	xywhsf(fpmx.skm4, LX + 1310 - 35, LY + 310 + 135, LW, LH, LS_12, FT, ZL);

	for (int i = 0; i < fpmx.iFyxmCount; i++)
	{
		//计算商品名称长度
		char* strSP = fpmx.fyxmxx[i].sSpmc.GetBuffer(0);
		int LengOfSpmc = strlen(strSP);

		if (LengOfSpmc >= 40)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 70, LY + 560 + i * 43, LW + 500, LH + 60, LS_6, FS, ZL);
		}
		else if (LengOfSpmc >= 34 && LengOfSpmc < 40)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 70, LY + 560 + i * 43, LW + 500, LH + 60, LS_7, FS, ZL);
		}
		else if (LengOfSpmc >= 30 && LengOfSpmc < 34)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 70, LY + 560 + i * 43, LW + 500, LH + 60, LS_8, FS, ZL);
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSpmc, LX + 70, LY + 560 + i * 43, LW + 500, LH + 60, LS_9, FS, ZL);
		}
		//计算商品规格型号长度
		char* str2 = fpmx.fyxmxx[i].sGgxh.GetBuffer(0);
		int LengOfGgxh = strlen(str2);
		if (LengOfGgxh >= 18)
		{
			xywhsf(fpmx.fyxmxx[i].ssGgxh, LX + 595, LY + 560 + i * 43, LW + 250, LH + 60, LS_6, FS, ZL);
		}
		else if (LengOfGgxh >= 16 && LengOfGgxh < 18)
		{
			xywhsf(fpmx.fyxmxx[i].ssGgxh, LX + 595, LY + 560 + i * 43, LW + 250, LH + 60, LS_7, FS, ZL);
		}
		else if (LengOfGgxh >= 14 && LengOfGgxh < 16)
		{
			xywhsf(fpmx.fyxmxx[i].ssGgxh, LX + 595, LY + 560 + i * 43, LW + 250, LH + 60, LS_8, FS, ZL);
		}
		else {

			xywhsf(fpmx.fyxmxx[i].ssGgxh, LX + 595, LY + 560 + i * 43, LW + 250, LH + 60, LS_9, FS, ZL);
		}
		//计算单位长度
		char* str3 = fpmx.fyxmxx[i].sDw.GetBuffer(0);
		int LengOfDw = strlen(str3);
		if (LengOfDw >= 8)
		{
			xywhsf(fpmx.fyxmxx[i].ssDw, LX + 845, LY + 560 + i * 43, LW + 120, LH + 60, LS_6, FS, ZL);
		}
		else if (LengOfDw >= 6 && LengOfDw < 8)
		{
			xywhsf(fpmx.fyxmxx[i].ssDw, LX + 845, LY + 560 + i * 43, LW + 120, LH + 60, LS_7, FS, ZL);
		}
		else {
			xywhsf(fpmx.fyxmxx[i].ssDw, LX + 845, LY + 560 + i * 43, LW + 120, LH + 60, LS_9, FS, ZL);
		}
		//计算数量长度
// 		char* str4 = fpmx.fyxmxx[i].sSpsl.GetBuffer(0);
		int LengOfSpsl = 0;
		if (fpmx.fyxmxx[i].sSpsl.GetLength() > 13)
		{
			fpmx.fyxmxx[i].sSpsl = fpmx.fyxmxx[i].sSpsl.Mid(0, 13);
		}
		char* str4 = fpmx.fyxmxx[i].sSpsl.GetBuffer(0);
		LengOfSpsl = strlen(str4);

		if (LengOfSpsl >= 11 && LengOfSpsl <= 13)
		{
			xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 940 + 5, LY + 560 + i * 43, LW + 200, LH + 43, LS_8, FS, ZR);
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSpsl, LX + 940 + 5, LY + 560 + i * 43, LW + 200, LH + 43, LS_9, FS, ZR);
		}
		//计算单价长度
// 		char* str5 = fpmx.fyxmxx[i].sDj.GetBuffer(0);
		int LengOfDj = 0;
		if (fpmx.fyxmxx[i].sDj.GetLength() > 13)
		{
			fpmx.fyxmxx[i].sDj = fpmx.fyxmxx[i].sDj.Mid(0, 13);
		}
		char* str5 = fpmx.fyxmxx[i].sDj.GetBuffer(0);
		LengOfDj = strlen(str5);

		if (LengOfDj >= 11 && LengOfDj <= 13)
		{
			xywhsf(fpmx.fyxmxx[i].ssDj, LX + 1150, LY + 560 + i * 43, LW + 200, LH + 43, LS_8, FS, ZR);
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssDj, LX + 1150, LY + 560 + i * 43, LW + 200, LH + 43, LS_9, FS, ZR);
		}

		xywhsf(fpmx.fyxmxx[i].ssJe, LX + 1340, LY + 560 + i * 43, LW + 300, LH + 43, LS_9, FS, ZR);
		if (fpmx.fyxmxx[i].sSl.GetLength() > 4)
		{
			xywhsf(fpmx.fyxmxx[i].ssSl, LX + 1660, LY + 560 + i * 43, LW + 100, LH + 43, LS_7, FT, ZC);
		}
		else
		{
			xywhsf(fpmx.fyxmxx[i].ssSl, LX + 1660, LY + 560 + i * 43, LW + 100, LH + 43, LS_9, FS, ZC);
		}
		xywhsf(fpmx.fyxmxx[i].ssSe, LX + 1740, LY + 560 + i * 43, LW + 300, LH + 43, LS_9, FT, ZR);
	}
	if (fpmx.sHjje.GetLength() > 8)
	{
		xywhsf(fpmx.hjje, LX + 1340, LY + 900, LW + 300, LH + 60, LS_9, FT, ZR);
	}
	else
	{
		xywhsf(fpmx.hjje, LX + 1340, LY + 900, LW + 300, LH + 60, LS_10, FT, ZR);
	}

	if (fpmx.sHjse.GetLength() > 8)
	{
		xywhsf(fpmx.hjse, LX + 1740, LY + 900, LW + 300, LH + 60, LS_9, FT, ZR);
	}
	else
	{
		xywhsf(fpmx.hjse, LX + 1740, LY + 900, LW + 300, LH + 60, LS_10, FT, ZR);
	}

	if (fpmx.sJshjDx.GetLength() > 50)
	{
		xywhsf(fpmx.jshjDx, LX + 650 + 30, LY + 975, LW, LH, LS_8, FS, ZL);   //大写价税合计
	}
	else
	{
		xywhsf(fpmx.jshjDx, LX + 650 + 30, LY + 975, LW, LH, LS_9, FS, ZL);   //大写价税合计
	}
	xywhsf(fpmx.jshj, LX + 1660, LY + 970, LW, LH, LS_11, FT, ZL);   //小写价税合计
	/*if(fpmx.sFpzt.CompareNoCase("1") == 0 )
	{
		xywhsf(fpmx.bzF                  ,LX+ 1260                   ,LY+ 1045      ,LW +800    ,LH +35     ,LS_9          ,FS    ,ZL );
		xywhsf(fpmx.bz                   ,LX+ 1260                   ,LY+ 1045+35   ,LW +800    ,LH +200    ,LS_9          ,FS    ,ZL );
	}
	else
	{*/
	//xywhsf(fpmx.bz						     ,LX+ 1260               ,LY+ 1045      ,LW +800    ,LH +200     ,LS_9         ,FS    ,ZL );
	//}

	if (fpdy.sFplxdm.CompareNoCase("007") == 0 && m_sHx.IsEmpty())
	{
		xywhsf(fpmx.bzF, LX + 1260, LY + 1045, LW + 800, LH + 200, LS_9, FS, ZL);
		xywhsf(fpmx.bz, LX + 1260, LY + 1045 + 38, LW + 800, LH + 200, LS_9, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.bz, LX + 1260, LY + 1045, LW + 800, LH + 200, LS_9, FS, ZL);
	}

	if (LengOfXhfmc <= 50)
	{
		xywhsf(fpmx.Xhdwmc, LX + 380, LY + 1045 + 5, LW + 800, LH + 60, LS_9, FS, ZL);
	}
	else if (LengOfXhfmc > 50 && LengOfXhfmc <= 56)
	{
		xywhsf(fpmx.Xhdwmc, LX + 380, LY + 1045 + 5, LW + 800, LH + 60, LS_8, FS, ZL);
	}
	else if (LengOfXhfmc > 56 && LengOfXhfmc <= 64)
	{
		xywhsf(fpmx.Xhdwmc, LX + 380, LY + 1045 + 5, LW + 800, LH + 60, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Xhdwmc, LX + 380, LY + 1045 + 5, LW + 800, LH + 60, LS_6, FS, ZL);
	}

	xywhsf(fpmx.Xhdwsbh, LX + 380 + 50, LY + 1045 + 40 + 5 + 5, LW, LH, LS_12, FT, ZL);

	if (LengOfXhdwdzdh <= 50)
	{
		xywhsf(fpmx.Xhdwdzdh, LX + 380, LY + 1045 + 95 + 5, LW + 800, LH + 60, LS_9, FS, ZL);
	}
	else if (LengOfXhdwdzdh > 50 && LengOfXhdwdzdh <= 56)
	{
		xywhsf(fpmx.Xhdwdzdh, LX + 380, LY + 1045 + 95 + 5, LW + 800, LH + 60, LS_8, FS, ZL);
	}
	else if (LengOfXhdwdzdh > 56 && LengOfXhdwdzdh <= 64)
	{
		xywhsf(fpmx.Xhdwdzdh, LX + 380, LY + 1045 + 95 + 5, LW + 800, LH + 60, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Xhdwdzdh, LX + 380, LY + 1045 + 95 + 5, LW + 800, LH + 60, LS_6, FS, ZL);
	}

	if (LengOfXhdwyhzh <= 50)
	{
		xywhsf(fpmx.Xhdwyhzh, LX + 380, LY + 1045 + 140 + 5 - 4, LW + 800, LH + 60, LS_9, FS, ZL);
	}
	else if (LengOfXhdwyhzh > 50 && LengOfXhdwyhzh <= 56)
	{
		xywhsf(fpmx.Xhdwyhzh, LX + 380, LY + 1045 + 140 + 5 - 4, LW + 800, LH + 60, LS_8, FS, ZL);
	}
	else if (LengOfXhdwyhzh > 56 && LengOfXhdwyhzh <= 64)
	{
		xywhsf(fpmx.Xhdwyhzh, LX + 380, LY + 1045 + 140 + 5 - 4, LW + 800, LH + 60, LS_7, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.Xhdwyhzh, LX + 380, LY + 1045 + 140 + 5 - 4, LW + 800, LH + 60, LS_6, FS, ZL);
	}

	xywhsf(fpmx.skr, LX + 270, LY + 1250 + 5, LW, LH, LS_10, FS, ZL);
	xywhsf(fpmx.fhr, LX + 790, LY + 1250 + 5, LW, LH, LS_10, FS, ZL);
	xywhsf(fpmx.kpr, LX + 1260, LY + 1250 + 5, LW, LH, LS_10, FS, ZL);

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
	//addxml(fpmx.sNcpxssgqy, fpmx.Ncpxssgqy);

	addxml(fpmx.sFpdm, fpmx.Fpdm);
	addxml(fpmx.sFphm, fpmx.Fphm);
	addxml(fpmx.kprq.sDay, fpmx.kprq.Day);
	addxml(fpmx.kprq.sday, fpmx.kprq.day);
	addxml(fpmx.kprq.sMouth, fpmx.kprq.Mouth);
	addxml(fpmx.kprq.smouth, fpmx.kprq.mouth);
	addxml(fpmx.kprq.sYear, fpmx.kprq.Year);
	addxml(fpmx.kprq.syear, fpmx.kprq.year);

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

	/*
	if( fpmx.sFpzt.CompareNoCase("0") == 0)//发票状态标志为0 正数发票有圈叉符号
	{*/
	xml.AddElem("OX", "1");
	xml.AddAttrib("x", fpmx.OX.x);
	xml.AddAttrib("y", fpmx.OX.y);
	xml.AddAttrib("w", fpmx.OX.w);
	xml.AddAttrib("h", fpmx.OX.h);
	xml.AddAttrib("s", fpmx.OX.s);
	xml.AddAttrib("f", fpmx.OX.f);
	xml.IntoElem();
	xml.OutOfElem();
	//}

	return xml.GetDoc();
}

CString CZzsfpdy::GenerateItemMXXml(ZZSFP_FPXX fpmx)
{
	CMarkup xml;
	int i = 0;
	int k = 0;
	double nJE = 0.00;
	double nSE = 0.00;
	double nHjje = 0.00;
	double nHjse = 0.00;
	double dZKJE = 0.00;
	double dZKSE = 0.00;
	fpmx.sKprq = fpmx.sKprq.Mid(0, 8);

	int nTmp = 900;
	int nTmpKprq = 35;

	if (m_nOrientation == DMORIENT_LANDSCAPE)
	{
		m_nPageSize = LINEFEED_L;
		nTmp = 195;
		nTmpKprq = 20;
	}

	int nLY = 70;

	LTFPQDXX_ZZSFP::iterator pos;
	for (pos = fpmx.fpqdxx.begin(); pos != fpmx.fpqdxx.end(); pos++)
	{
		//计算商品名称长度
		char* strSP = pos->sSpmc.GetBuffer(0);
		int LengOfSpmc = strlen(strSP);
		char* tempGgxh = pos->sGgxh.GetBuffer(0);
		int LenOfGgxh = lstrlen(tempGgxh);

		if (LengOfSpmc >= 40)
		{
			xywhsf(pos->ssSpmc, LX + 60, LY + nLY + i * nLY, LW + 440, LH + 50, LS_6, FS, ZL);
		}
		else if (LengOfSpmc >= 34 && LengOfSpmc < 40)
		{
			xywhsf(pos->ssSpmc, LX + 60, LY + nLY + i * nLY, LW + 440, LH + 50, LS_7, FS, ZL);
		}
		else if (LengOfSpmc >= 30 && LengOfSpmc < 34)
		{
			xywhsf(pos->ssSpmc, LX + 60, LY + nLY + i * nLY, LW + 440, LH + 50, LS_8, FS, ZL);
		}
		else
		{
			xywhsf(pos->ssSpmc, LX + 60, LY + nLY + i * nLY, LW + 440, LH + 50, LS_9, FS, ZL);
		}

		if (LenOfGgxh >= 18)
		{
			xywhsf(pos->ssGgxh, LX + 595 - 60, LY + nLY + i * nLY, LW + 210, LH + 50, LS_7, FS, ZL);
		}
		else if (LenOfGgxh >= 14 && LenOfGgxh < 18)
		{
			xywhsf(pos->ssGgxh, LX + 595 - 60, LY + nLY + i * nLY, LW + 210, LH + 50, LS_8, FS, ZL);
		}
		else
		{
			xywhsf(pos->ssGgxh, LX + 595 - 60, LY + nLY + i * nLY, LW + 210, LH + 50, LS_9, FS, ZL);
		}

		xywhsf(pos->ssDw, LX + 845, LY + nLY + i * nLY, LW + 120, LH + 50, LS_9, FS, ZL);

		int LenOfSpsl = 0;
		if (pos->sSpsl.GetLength() > 13)
		{
			pos->sSpsl = pos->sSpsl.Mid(0, 13);
		}
		char * strSpsl = pos->sSpsl.GetBuffer(0);
		LenOfSpsl = strlen(strSpsl);

		if (LenOfSpsl >= 11 && LenOfSpsl <= 13)
		{
			xywhsf(pos->ssSpsl, LX + 810 + 40, LY + nLY + i * nLY, LW + 200, LH + 50, LS_6, FS, ZR);
		}
		else
		{
			xywhsf(pos->ssSpsl, LX + 810 + 40, LY + nLY + i * nLY, LW + 200, LH + 50, LS_9, FS, ZR);
		}

		int LengOfDj = 0;
		if (pos->sDj.GetLength() > 13)
		{
			pos->sDj = pos->sDj.Mid(0, 13);
		}
		char* str5 = pos->sDj.GetBuffer(0);
		LengOfDj = strlen(str5);

		if (LengOfDj >= 11 || LengOfDj <= 13)
		{
			xywhsf(pos->ssDj, LX + 1080 - 15, LY + nLY + i * nLY, LW + 200, LH + 50, LS_8, FS, ZR);
		}
		else
		{
			xywhsf(pos->ssDj, LX + 1080 - 15, LY + nLY + i * nLY, LW + 200, LH + 50, LS_9, FS, ZR);
		}
		xywhsf(pos->ssJe, LX + 1230 - 30, LY + nLY + i * nLY, LW + 300, LH + 50, LS_9, FS, ZR);
		if (pos->sSl.GetLength() > 4)
		{
			xywhsf(pos->ssSl, LX + 1510, LY + nLY + i * nLY, LW + 100, LH + 50, LS_8, FS, ZR);
		}
		else
		{
			xywhsf(pos->ssSl, LX + 1510, LY + nLY + i * nLY, LW + 100, LH + 50, LS_9, FS, ZR);
		}
		xywhsf(pos->ssSe, LX + 1710 - 170, LY + nLY + i * nLY, LW + 300, LH + 50, LS_9, FS, ZR);

		xywhsf(pos->sequenceNum, LX - 10, LY + nLY + i * nLY, LW + 500, LH + 50, LS_9, FS, ZL);

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
	for (pos = fpmx.fpqdxx.begin(); pos != fpmx.fpqdxx.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum++);
			xml.IntoElem();
			bNewPage = FALSE;
		}
		addxml(pos->sSpmc, pos->ssSpmc);
		addxml(pos->sGgxh, pos->ssGgxh);
		addxml(pos->sDw, pos->ssDw);
		addxml(pos->sSpsl, pos->ssSpsl);
		addxml(pos->sDj, pos->ssDj);
		addxml(pos->sJe, pos->ssJe);
		addxml(pos->sSl, pos->ssSl);
		addxml(pos->sSe, pos->ssSe);
		addxml(j + 1, pos->sequenceNum);

		if (pos->sFphxz.CompareNoCase("1") == 0)
		{
			dZKJE += atof(pos->sJe.GetBuffer(0));
			dZKSE += atof(pos->sSe.GetBuffer(0));
		}
		else
		{
			nJE += atof(pos->sJe.GetBuffer(0));
			nSE += atof(pos->sSe.GetBuffer(0));
		}

		num++;
		if (num % m_nPageSize == 0)
		{
			xywhsf(fpmx.xjje, LX + 1520 - 500 - 10, LY + 1098 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
			xywhsf(fpmx.xjse, LX + 1910 - 555 - 10, LY + 1098 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
			xywhsf(fpmx.zkje, LX + 1520 - 500 - 10, LY + 1153 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
			xywhsf(fpmx.zkse, LX + 1910 - 555 - 10, LY + 1153 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
			xywhsf(fpmx.hjje, LX + 1520 - 500 - 10, LY + 1208 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
			xywhsf(fpmx.hjse, LX + 1910 - 555 - 10, LY + 1208 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
			xywhsf(fpmx.Ghdwmc, LX + 230, LY - 228, LW + 1000, LH + 50, LS_11, FS, ZL);
			xywhsf(fpmx.Xhdwmc, LX + 230, LY - 163, LW + 1000, LH + 50, LS_11, FS, ZL);
			xywhsf(fpmx.Fpdm, LX + 440 - 10, LY - 98, LW + 500, LH + 50, LS_11, FS, ZL);
			xywhsf(fpmx.Fphm, LX + 1130 - 80, LY - 98, LW + 500, LH + 50, LS_11, FS, ZL);
			xywhsf(fpmx.Kprq, LX + 1430 - 120, LY + 1362 + nTmp + nTmpKprq, LW + 500, LH + 50, LS_11, FS, ZL);
			if (fpmx.sBz.GetLength() < 80)
			{
				xywhsf(fpmx.bz, LX + 60, LY + 1285 + nTmp, LW + 1700, LH + 60, LS_9, FS, ZL);
			}
			else
			{
				xywhsf(fpmx.bz, LX + 60, LY + 1285 + nTmp, LW + 1700, LH + 60, LS_8, FS, ZL);
			}

			if (k == 0)
			{
				fpmx.sKprq.Insert(4, _T("年"));
				fpmx.sKprq.Insert(8, _T("月"));
				fpmx.sKprq.Insert(12, _T("日"));
			}

			fpmx.sJEtemp.Format("%.2f", (nJE + dZKJE));
			fpmx.sSEtemp.Format("%.2f", (nSE + dZKSE));

			fpmx.sZKJETemp.Format("%.2f", dZKJE);
			fpmx.sZKSETemp.Format("%.2f", dZKSE);

			nHjje += (nJE + dZKJE);
			nHjse += (nSE + dZKSE);
			fpmx.sHjjeTemp.Format("%.2f", nHjje);
			fpmx.sHjseTemp.Format("%.2f", nHjse);

			addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
			addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
			addxml(fpmx.sFpdm, fpmx.Fpdm);
			addxml(fpmx.sFphm, fpmx.Fphm);
			addxml(fpmx.sJEtemp, fpmx.xjje);
			addxml(fpmx.sSEtemp, fpmx.xjse);
			addxml(fpmx.sZKJETemp, fpmx.zkje);
			addxml(fpmx.sZKSETemp, fpmx.zkse);
			addxml(fpmx.sHjjeTemp, fpmx.hjje);
			addxml(fpmx.sHjseTemp, fpmx.hjse);
			addxml(fpmx.sBz, fpmx.bz);
			addxml(fpmx.sKprq, fpmx.Kprq);
			k++;
			nJE = 0;
			nSE = 0;
			dZKJE = 0;
			dZKSE = 0;
			xml.OutOfElem();
			bNewPage = TRUE;
		}
		j++;
	}

	xywhsf(fpmx.xjje, LX + 1520 - 500 - 10, LY + 1098 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
	xywhsf(fpmx.xjse, LX + 1910 - 555 - 10, LY + 1098 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
	xywhsf(fpmx.zkje, LX + 1520 - 500 - 10, LY + 1153 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
	xywhsf(fpmx.zkse, LX + 1910 - 555 - 10, LY + 1153 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
	xywhsf(fpmx.hjje, LX + 1520 - 500 - 10, LY + 1208 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
	xywhsf(fpmx.hjse, LX + 1910 - 555 - 10, LY + 1208 + nTmp, LW + 500, LH + 50, LS_9, FS, ZR);
	xywhsf(fpmx.Ghdwmc, LX + 230, LY - 228, LW + 1000, LH + 50, LS_11, FS, ZL);
	xywhsf(fpmx.Xhdwmc, LX + 230, LY - 163, LW + 1000, LH + 50, LS_11, FS, ZL);
	xywhsf(fpmx.Fpdm, LX + 440 - 10, LY - 98, LW + 500, LH + 50, LS_11, FS, ZL);
	xywhsf(fpmx.Fphm, LX + 1130 - 80, LY - 98, LW + 500, LH + 50, LS_11, FS, ZL);
	xywhsf(fpmx.Kprq, LX + 1430 - 120, LY + 1362 + nTmp + nTmpKprq, LW + 500, LH + 50, LS_11, FS, ZL);// -10
	if (fpmx.sBz.GetLength() < 80)
	{
		xywhsf(fpmx.bz, LX + 60, LY + 1285 + nTmp, LW + 1700, LH + 60, LS_9, FS, ZL);
	}
	else
	{
		xywhsf(fpmx.bz, LX + 60, LY + 1285 + nTmp, LW + 1700, LH + 60, LS_8, FS, ZL);
	}

	addxml(fpmx.sGhdwmc, fpmx.Ghdwmc);
	addxml(fpmx.sXhdwmc, fpmx.Xhdwmc);
	addxml(fpmx.sFpdm, fpmx.Fpdm);
	addxml(fpmx.sFphm, fpmx.Fphm);

	fpmx.sJEtemp.Format("%.2f", (nJE + dZKJE));
	fpmx.sSEtemp.Format("%.2f", (nSE + dZKSE));

	fpmx.sZKJETemp.Format("%.2f", dZKJE);
	fpmx.sZKSETemp.Format("%.2f", dZKSE);

	nHjje += (nJE + dZKJE);
	nHjse += (nSE + dZKSE);
	fpmx.sHjjeTemp.Format("%.2f", nHjje);
	fpmx.sHjseTemp.Format("%.2f", nHjse);

	addxml(fpmx.sJEtemp, fpmx.xjje);
	addxml(fpmx.sSEtemp, fpmx.xjse);
	addxml(fpmx.sZKJETemp, fpmx.zkje);
	addxml(fpmx.sZKSETemp, fpmx.zkse);
	addxml(fpmx.sHjjeTemp, fpmx.hjje);
	addxml(fpmx.sHjseTemp, fpmx.hjse);

	addxml(fpmx.sBz, fpmx.bz);

	if (num < m_nPageSize && k == 0)
	{
		fpmx.sKprq.Insert(4, _T("年"));
		fpmx.sKprq.Insert(8, _T("月"));
		fpmx.sKprq.Insert(12, _T("日"));
	}
	addxml(fpmx.sKprq, fpmx.Kprq);
	xml.OutOfElem();
	bNewPage = TRUE;

	return xml.GetDoc();
}
