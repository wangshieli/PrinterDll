#include "../../pch.h"
#include "Fpdy.h"
#include <winspool.h>

#include "../../Helper/XML/Markup.h"

#pragma comment(lib, "Helper/QRGenerator/QRGenerator.lib")

CFpdyBase::CFpdyBase():m_iPldy(0),
	m_hPrinterDC(NULL),
	m_sPrinterName(""),
	m_sTempFplxdm(""),
	m_sFpzt("0"),
	m_sPrintTaskName(""),
	m_sPrintTaskNameFlag(""),
	m_bStatus(FALSE)
{
	ZeroMemory(m_cQRcodePath, MAX_PATH);
	GetQRcodePath();
	m_pDlg = new CPrintDialog(FALSE, PD_ALLPAGES);// 第一个参数 TRUE:显示打印对话框，FALSE:显示打印设置对话框 https://www.cnblogs.com/ericyi/p/4435562.html
}

CFpdyBase::~CFpdyBase()
{
	if (NULL != m_hPrinterDC)
		::DeleteDC(m_hPrinterDC);

	if (NULL != m_pDlg)
		delete m_pDlg;
}

int CFpdyBase::InitPrinter(short pwidth, short plength)
{
	//if (!dlg.GetDefaults()) 不弹界面，使用默认打印机直接打印
	if (NULL == m_pDlg)
		return -10;

	if (0 == m_iPldy)
	{
		CString defPrinter = ""; 
		getSysDefPrinter(defPrinter);
		setSysDefprinter(m_sPrinterName);
		if (m_pDlg->DoModal() == IDCANCEL)
		{
			return -1;// 用户取消了打印操作
		}
		setSysDefprinter(defPrinter);
	}
	else if (1 == m_iPldy)
	{
		HGLOBAL hDevMode = NULL;
		HGLOBAL hDevNames = NULL;
		if (m_sPrinterName.IsEmpty() || !GetPrinterDevice(m_sPrinterName.GetBuffer(0), &hDevNames, &hDevMode)) //使用指定的打印机
		{
			return -7;// 找不到打印机
		}
		m_pDlg->m_pd.hDevNames = hDevNames;
		m_pDlg->m_pd.hDevMode = hDevMode;  // 为dlg设置参数，达到不弹框打印
	}
	else
	{
		if (!m_pDlg->GetDefaults())
		{
			return -11;// 找不到打印机
		}
	}

	DEVMODE *lpDevMode = m_pDlg->GetDevMode();
	lpDevMode->dmFields |= DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	lpDevMode->dmPaperSize = DMPAPER_USER;
	lpDevMode->dmPaperWidth = pwidth;       // 指定发票实物宽高
	lpDevMode->dmPaperLength = plength;
	m_hPrinterDC =m_pDlg->CreatePrinterDC();

	if (NULL == m_hPrinterDC)
	{
		return -2;// 打印设备未就绪
	}

	::SetMapMode(m_hPrinterDC, MM_LOMETRIC);

	return 0;
}

void CFpdyBase::GetQRcodePath()
{
	HANDLE hDllhandle = GetModuleHandle(CString("PRINTCONTROL.dll").GetBuffer(0));
	if (hDllhandle != NULL)
	{
		GetModuleFileName((HMODULE)hDllhandle, m_cQRcodePath, MAX_PATH);// 获取当前模块句柄路径
		char *p = strrchr(m_cQRcodePath, '\\');
		*(p + 1) = 0;
	}
	strcat_s(m_cQRcodePath, MAX_PATH, "_SKSClog");
}

void CFpdyBase::getSysDefPrinter(CString& printer)
{
	DWORD dwSize = 0;
	if (!::GetDefaultPrinter(NULL, &dwSize) && ERROR_INSUFFICIENT_BUFFER == GetLastError())
	{
		TCHAR* name = new TCHAR[dwSize + 1];
		if (::GetDefaultPrinter(name, &dwSize))
		{
			printer = name;
		}
		delete name;
	}
}

void CFpdyBase::setSysDefprinter(CString& printer)
{
	::SetDefaultPrinter(printer);
}

// nType = 1机动车  2二手车  3转普票
void CFpdyBase::setBuiltInOffset(IN int nType, OUT int & _x, OUT int & _y)
{
	//return;
	CString _sPrinter = m_sPrinterName;
	_sPrinter.MakeUpper();

	if (_sPrinter.Find("EPSON") != -1)
	{
		if (nType == 1)
		{
			_x = -30;
			_y = -20;
		}
		else if (nType == 2)
		{
			_x = -25;
			_y = -45;
		}
		else if (nType == 3)
		{
			_x = -30;
			_y = -50;
		}
	}
	else if (_sPrinter.Find("FUJITSU") != -1)
	{
		if (nType == 1)
		{
			_x = 10;
			_y = 0;
		}
		else if (nType == 2)
		{
			_x = 0;
			_y = 0;
		}
		else if (nType == 3)
		{
			_x = 0;
			_y = -10;
		}
	}
	else if (_sPrinter.Find("JOLIMARK") != -1)
	{
		if (nType == 1)
		{
			_x = 0;
			_y = -40;
		}
		else if (nType == 2)
		{
			_x = 0;
			_y = -40;
		}
		else if (nType == 3)
		{
			_x = 0;
			_y = -55;
		}
	}
}

LONG CFpdyBase::PaintTile2(int FontSize, LPCSTR FontType, RECT rect, LPCSTR data, int z, int FontSizeEC, int _s, int _l, int _r)
{
	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;
	fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

	UINT flags1 = 0;
	UINT flags2 = 0;
	UINT flags3 = 0;
	if (z == AM_ZC)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_CENTER | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_CENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZC_S || z == AM_ZC_CHEKC)
	{
		flags1 = DT_SINGLELINE | DT_EDITCONTROL | DT_CALCRECT | DT_CENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_EDITCONTROL | DT_CENTER | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX;
	}
	else if (z == AM_VC)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER;
		flags2 = DT_SINGLELINE | DT_VCENTER;
		flags3 = DT_SINGLELINE | DT_VCENTER;
	}
	else if (z == AM_ZL)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
	}

	rect.left += _l;
	rect.right -= _r;

	RECT trect = rect;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data, -1, &trect, flags1);
	LONG r = trect.right;
	if (((h <= recv_h + _s
		|| (r > recv_r
			&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S
				|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1))
		|| ((z == AM_ZC || z == AM_ZC_S || z == AM_ZC_CHEKC || z == AM_ZL) && ((rect.top = rect.top - (h - recv_h) / 2) && 0))) //如果多行，居中左对齐
	{
		do
		{
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
			trect = rect;
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));
			h = ::DrawText(m_hPrinterDC, data, -1, &trect, flags1);
			r = trect.right;
		} while (((h <= recv_h + _s
			|| (r > recv_r
				&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S
					|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
						&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
						&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1)));
	}

	::SelectObject(m_hPrinterDC, pOldFont);
	fontHeader.DeleteObject();

	return r;
}

void CFpdyBase::PaintTile(int FontSize, LPCSTR FontType, RECT rect, LPCSTR data, int z, int FontSizeEC, int _s, int _l, int _r)
{
	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;
	fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

	UINT flags1 = 0;
	UINT flags2 = 0;
	UINT flags3 = 0;
	if (z == AM_ZC)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_CENTER | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_CENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZC_S || z == AM_ZC_CHEKC)
	{
		flags1 = DT_SINGLELINE | DT_EDITCONTROL | DT_CALCRECT | DT_CENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_EDITCONTROL | DT_CENTER | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX;
	}
	else if (z == AM_VC)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER;
		flags2 = DT_SINGLELINE | DT_VCENTER;
		flags3 = DT_SINGLELINE | DT_VCENTER;
	}
	else if (z == AM_ZL || z == AM_ZL_EX)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
	}

	rect.left += _l;
	rect.right -= _r;

	RECT trect = rect;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data, -1, &trect, flags1);
	LONG r = trect.right;
	if ((z != AM_ZL_EX) && (((h <= recv_h + _s
		|| (r > recv_r
			&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S
				|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1))
		|| ((z == AM_ZC || z == AM_ZC_S || z == AM_ZC_CHEKC || z == AM_ZL) && ((rect.top = rect.top - (h - recv_h) / 2) && 0)))) //如果多行，居中左对齐
	{
		do
		{
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
			trect = rect;
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));
			h = ::DrawText(m_hPrinterDC, data, -1, &trect, flags1);
			r = trect.right;
		} while (((h <= recv_h + _s
			|| (r > recv_r
				&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S
					|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
						&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
						&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1)));
		rect.top = rect.top - (h - recv_h) / 2;
	}

	if (rect.right >= trect.right)
		::DrawText(m_hPrinterDC, data, -1, &rect, flags2);
	else
		::DrawText(m_hPrinterDC, data, -1, &rect, flags3);

	::SelectObject(m_hPrinterDC, pOldFont);
	fontHeader.DeleteObject();
}

int CFpdyBase::DataPrintMaxLen(const char * lpszData, int nLineMaxLen)
{
	if (NULL == lpszData || 0 >= nLineMaxLen)
		return 0;

	int nLen = strlen(lpszData);
	if (nLen <= nLineMaxLen)
		return nLen;

	int i = 0; //字符串移动下标
	int k = 0; //记录要截取的长度

	// 字段最后一位不是汉字，则直接截取
	if ((unsigned char)(lpszData[nLineMaxLen - 1]) < 0xA0)
	{
		k = nLineMaxLen;
	}

	// 字段最后一位是汉字，而倒数第二位不是汉字则直接截取到倒数第二位
	else if ((unsigned char)(lpszData[nLineMaxLen - 1]) >= 0xA0
		&& (unsigned char)(lpszData[nLineMaxLen - 2]) < 0xA0)
	{
		k = nLineMaxLen - 1;
	}
	// 从头开始一个个字符的判断，
	// 看最后一个字符是属于要截取的汉字的一部分,
	// 还是不属于截取的范围
	else
	{
		while (i < nLineMaxLen)
		{
			if ((unsigned char)(lpszData[i]) >= 0xA0

				&& (unsigned char)(lpszData[i + 1]) >= 0xA0)
			{
				if (k + 2 <= nLineMaxLen)
				{
					k += 2;
					i += 2;
				}
				else
				{
					break;
				}
			}
			else
			{
				if (k + 1 <= nLineMaxLen)
				{
					k++;
					i++;
				}
				else
				{
					break;
				}
			}
		}
	}

	return k;
}

BOOL CFpdyBase::GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL * phDevNames, HGLOBAL * phDevMode)
{
	// if NULL is passed, then assume we are setting app object's  
	// devmode and devnames  
	if (phDevMode == NULL || phDevNames == NULL)
		return FALSE;

	// Open printer  
	HANDLE hPrinter;
	if (OpenPrinter(pszPrinterName, &hPrinter, NULL) == FALSE)
		return FALSE;

	// obtain PRINTER_INFO_2 structure and close printer  
	DWORD dwBytesReturned, dwBytesNeeded;
	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
	PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR,
		dwBytesNeeded);
	if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded,
		&dwBytesReturned) == 0) {
		GlobalFree(p2);
		ClosePrinter(hPrinter);
		return FALSE;
	}
	ClosePrinter(hPrinter);

	// Allocate a global handle for DEVMODE  
	HGLOBAL  hDevMode = GlobalAlloc(GHND, sizeof(*p2->pDevMode) +
		p2->pDevMode->dmDriverExtra);
	ASSERT(hDevMode);
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(hDevMode);
	ASSERT(pDevMode);

	// copy DEVMODE data from PRINTER_INFO_2::pDevMode  
	memcpy(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode) +
		p2->pDevMode->dmDriverExtra);
	GlobalUnlock(hDevMode);

	// Compute size of DEVNAMES structure from PRINTER_INFO_2's data  
	DWORD drvNameLen = lstrlen(p2->pDriverName) + 1;  // driver name  
	DWORD ptrNameLen = lstrlen(p2->pPrinterName) + 1; // printer name  
	DWORD porNameLen = lstrlen(p2->pPortName) + 1;    // port name  

	// Allocate a global handle big enough to hold DEVNAMES.  
	HGLOBAL hDevNames = GlobalAlloc(GHND,
		sizeof(DEVNAMES) +
		(drvNameLen + ptrNameLen + porNameLen) * sizeof(TCHAR));
	ASSERT(hDevNames);
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);
	ASSERT(pDevNames);

	// Copy the DEVNAMES information from PRINTER_INFO_2  
	// tcOffset = TCHAR Offset into structure  
	int tcOffset = sizeof(DEVNAMES) / sizeof(TCHAR);
	ASSERT(sizeof(DEVNAMES) == tcOffset * sizeof(TCHAR));

	pDevNames->wDriverOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName,
		drvNameLen * sizeof(TCHAR));
	tcOffset += drvNameLen;

	pDevNames->wDeviceOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName,
		ptrNameLen * sizeof(TCHAR));
	tcOffset += ptrNameLen;

	pDevNames->wOutputOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName,
		porNameLen * sizeof(TCHAR));
	pDevNames->wDefault = 0;

	GlobalUnlock(hDevNames);
	GlobalFree(p2);   // free PRINTER_INFO_2  

	// set the new hDevMode and hDevNames  
	*phDevMode = hDevMode;
	*phDevNames = hDevNames;
	return TRUE;
}

CString CFpdyBase::GenerateXMLFpdy(FPDY fpdy, int rtn)
{
	CMarkup xml;

	if (0 != rtn)
	{
		switch (rtn)
		{
		case -1:
			fpdy.sErrorInfo = "用户取消了打印操作";
			break;
		case -2:
			fpdy.sErrorInfo = "打印设备未就绪";
			break;
		case -3:
			fpdy.sErrorInfo = "启动打印任务失败";
			break;
		case -4:
			fpdy.sErrorInfo = "发送打印内容失败";
			break;
		case -5:
			fpdy.sErrorInfo = "打印内容解析失败";
			break;
		case -6:
			fpdy.sErrorInfo = "打印未知错误";
			break;
		case -7:
			fpdy.sErrorInfo = "找不到打印机";
			break;
		case -8:
			fpdy.sErrorInfo = "配置文件错误";
			break;
		case -9:
			fpdy.sErrorInfo = "找不到配置的打印机名称";
			break;
		case -10:
			fpdy.sErrorInfo = "创建打印对象失败";
			break;
		case -11:
			fpdy.sErrorInfo = "找不到默认打印机";
			break;
		default:
			rtn = 0; // StartPage成功返回的是页码
			break;
		}
	}
	fpdy.sErrorCode.Format("%d", rtn);

	xml.SetDoc(XMLHEAD);
	xml.AddElem("business");
	if (m_bStatus)
		xml.AddAttrib("id", "30004");
	else
		xml.AddAttrib("id", "20004");
	xml.AddAttrib("comment", "发票打印");
	xml.IntoElem();
	xml.AddElem("body");
	xml.AddAttrib("yylxdm", fpdy.iYylxdm);
	xml.IntoElem();
	xml.AddElem("returncode", fpdy.sErrorCode);
	xml.AddElem("returnmsg", fpdy.sErrorInfo);
	xml.OutOfElem();
	xml.OutOfElem();

	return xml.GetDoc();
}
