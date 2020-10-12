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
	m_pDlg = new CPrintDialog(FALSE, PD_ALLPAGES);// ��һ������ TRUE:��ʾ��ӡ�Ի���FALSE:��ʾ��ӡ���öԻ��� https://www.cnblogs.com/ericyi/p/4435562.html
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
	//if (!dlg.GetDefaults()) �������棬ʹ��Ĭ�ϴ�ӡ��ֱ�Ӵ�ӡ
	if (NULL == m_pDlg)
		return -10;

	if (0 == m_iPldy)
	{
		CString defPrinter = ""; 
		getSysDefPrinter(defPrinter);
		setSysDefprinter(m_sPrinterName);
		if (m_pDlg->DoModal() == IDCANCEL)
		{
			return -1;// �û�ȡ���˴�ӡ����
		}
		setSysDefprinter(defPrinter);
	}
	else if (1 == m_iPldy)
	{
		HGLOBAL hDevMode = NULL;
		HGLOBAL hDevNames = NULL;
		if (m_sPrinterName.IsEmpty() || !GetPrinterDevice(m_sPrinterName.GetBuffer(0), &hDevNames, &hDevMode)) //ʹ��ָ���Ĵ�ӡ��
		{
			return -7;// �Ҳ�����ӡ��
		}
		m_pDlg->m_pd.hDevNames = hDevNames;
		m_pDlg->m_pd.hDevMode = hDevMode;  // Ϊdlg���ò������ﵽ�������ӡ
	}
	else
	{
		if (!m_pDlg->GetDefaults())
		{
			return -11;// �Ҳ�����ӡ��
		}
	}

	DEVMODE *lpDevMode = m_pDlg->GetDevMode();
	lpDevMode->dmFields |= DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	lpDevMode->dmPaperSize = DMPAPER_USER;
	lpDevMode->dmPaperWidth = pwidth;       // ָ����Ʊʵ����
	lpDevMode->dmPaperLength = plength;
	m_hPrinterDC =m_pDlg->CreatePrinterDC();

	if (NULL == m_hPrinterDC)
	{
		return -2;// ��ӡ�豸δ����
	}

	::SetMapMode(m_hPrinterDC, MM_LOMETRIC);

	return 0;
}

void CFpdyBase::GetQRcodePath()
{
	HANDLE hDllhandle = GetModuleHandle(CString("PRINTCONTROL.dll").GetBuffer(0));
	if (hDllhandle != NULL)
	{
		GetModuleFileName((HMODULE)hDllhandle, m_cQRcodePath, MAX_PATH);// ��ȡ��ǰģ����·��
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

// nType = 1������  2���ֳ�  3ת��Ʊ
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
		|| ((z == AM_ZC || z == AM_ZC_S || z == AM_ZC_CHEKC || z == AM_ZL) && ((rect.top = rect.top - (h - recv_h) / 2) && 0))) //������У����������
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
		|| ((z == AM_ZC || z == AM_ZC_S || z == AM_ZC_CHEKC || z == AM_ZL) && ((rect.top = rect.top - (h - recv_h) / 2) && 0)))) //������У����������
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

	int i = 0; //�ַ����ƶ��±�
	int k = 0; //��¼Ҫ��ȡ�ĳ���

	// �ֶ����һλ���Ǻ��֣���ֱ�ӽ�ȡ
	if ((unsigned char)(lpszData[nLineMaxLen - 1]) < 0xA0)
	{
		k = nLineMaxLen;
	}

	// �ֶ����һλ�Ǻ��֣��������ڶ�λ���Ǻ�����ֱ�ӽ�ȡ�������ڶ�λ
	else if ((unsigned char)(lpszData[nLineMaxLen - 1]) >= 0xA0
		&& (unsigned char)(lpszData[nLineMaxLen - 2]) < 0xA0)
	{
		k = nLineMaxLen - 1;
	}
	// ��ͷ��ʼһ�����ַ����жϣ�
	// �����һ���ַ�������Ҫ��ȡ�ĺ��ֵ�һ����,
	// ���ǲ����ڽ�ȡ�ķ�Χ
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
			fpdy.sErrorInfo = "�û�ȡ���˴�ӡ����";
			break;
		case -2:
			fpdy.sErrorInfo = "��ӡ�豸δ����";
			break;
		case -3:
			fpdy.sErrorInfo = "������ӡ����ʧ��";
			break;
		case -4:
			fpdy.sErrorInfo = "���ʹ�ӡ����ʧ��";
			break;
		case -5:
			fpdy.sErrorInfo = "��ӡ���ݽ���ʧ��";
			break;
		case -6:
			fpdy.sErrorInfo = "��ӡδ֪����";
			break;
		case -7:
			fpdy.sErrorInfo = "�Ҳ�����ӡ��";
			break;
		case -8:
			fpdy.sErrorInfo = "�����ļ�����";
			break;
		case -9:
			fpdy.sErrorInfo = "�Ҳ������õĴ�ӡ������";
			break;
		case -10:
			fpdy.sErrorInfo = "������ӡ����ʧ��";
			break;
		case -11:
			fpdy.sErrorInfo = "�Ҳ���Ĭ�ϴ�ӡ��";
			break;
		default:
			rtn = 0; // StartPage�ɹ����ص���ҳ��
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
	xml.AddAttrib("comment", "��Ʊ��ӡ");
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
