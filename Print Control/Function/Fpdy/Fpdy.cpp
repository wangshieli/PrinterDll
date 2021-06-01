#include "../../pch.h"
#include "Fpdy.h"
#include <winspool.h>

#include "../../Helper/XML/Markup.h"
#include "../../Helper/ZLib/ZLib.h"

CFpdyBase::CFpdyBase():m_iPldy(0),
	m_hPrinterDC(NULL),
	m_sPrinterName(""),
	m_sTempFplxdm(""),
	m_sFpzt("0"),
	m_sPrintTaskName(""),
	m_sPrintTaskNameFlag(""),
	m_bStatus(FALSE),
	m_sEwm(""),
	nXoff(0),
	nYoff(0),
	nQRCodeSize(0),
	m_nLineNum(0),
	m_nAllPageNum(0),
	m_nPageSize(0),
	m_nFromPage(1),
	m_nToPage(1),
	m_nCopies(1)
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

void CFpdyBase::InitXYoff()
{
	CString _sTop = "";
	CString _sLeft = "";
	CString _sQRSize = "";
	CString _sZzlx = "";
	ZLib_GetIniYbjValue(m_sTempFplxdm, _sTop, _sLeft, _sQRSize, _sZzlx);
	nXoff = atoi(_sLeft);
	nYoff = atoi(_sTop);
	nQRCodeSize = atoi(_sQRSize);
	nZzlx = atoi(_sZzlx);

	//int _nXoff = 0;
	//int _nYoff = 0;
	//setBuiltInOffset(2, _nXoff, _nYoff);

	//nXoff += _nXoff;
	//nYoff += _nYoff;
}

bool CFpdyBase::CheckDyjOnline()
{
	if (m_sPrinterName.IsEmpty())
		return false;

	HGLOBAL hDevMode = NULL;
	HGLOBAL hDevNames = NULL;

	if (!GetPrinterDevice(m_sPrinterName.GetBuffer(0), &hDevNames, &hDevMode)) //使用指定的打印机
		return false;

	m_pDlg->m_pd.hDevNames = hDevNames;
	m_pDlg->m_pd.hDevMode = hDevMode;  // 为dlg设置参数，达到不弹框打印

	if (0 == m_iPldy) // 不是批量打印直接弹框
		return false;

	// 是批量打印，检查打印机在线状态
	DWORD _dwSize = 0;
	HANDLE _hPrinter = NULL;
	PRINTER_INFO_2* _pInfo = NULL;

	if (!OpenPrinter(m_sPrinterName.GetBuffer(0), &_hPrinter, NULL))
		return false;

	if (!GetPrinter(_hPrinter, 2, NULL, 0, &_dwSize))
	{
		_pInfo = (PRINTER_INFO_2*)malloc(_dwSize);
		GetPrinter(_hPrinter, 2, (LPBYTE)_pInfo, _dwSize, &_dwSize);
	}

	ClosePrinter(_hPrinter);

	DWORD _dwStatus = _pInfo->Attributes;
	free(_pInfo);

	if (_dwStatus & PRINTER_ATTRIBUTE_WORK_OFFLINE)
	{
		return false;
	}

	return true;
}

int CFpdyBase::InitPrinter1(short pwidth, short plength)
{
	//if (!dlg.GetDefaults()) 不弹界面，使用默认打印机直接打印
	if (NULL == m_pDlg)
		return -10;


	HGLOBAL hDevMode = NULL;
	HGLOBAL hDevNames = NULL;
	if (GetPrinterDevice(m_sPrinterName.GetBuffer(0), &hDevNames, &hDevMode)) //使用指定的打印机
	{
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
	m_hPrinterDC = m_pDlg->CreatePrinterDC();

	if (NULL == m_hPrinterDC)
	{
		return -2;// 打印设备未就绪
	}

	::SetMapMode(m_hPrinterDC, MM_LOMETRIC);

	return 0;
}

int CFpdyBase::InitPrinter(short pwidth, short plength)
{
	//if (!dlg.GetDefaults()) 不弹界面，使用默认打印机直接打印
	if (NULL == m_pDlg)
		return -10;

	m_nToPage = m_nAllPageNum == 0 ? 1 : m_nAllPageNum;
	if (!CheckDyjOnline())
	{
		m_pDlg->m_pd.nMinPage = m_pDlg->m_pd.nFromPage = 1;
		m_pDlg->m_pd.nMaxPage = m_pDlg->m_pd.nToPage = m_nToPage;
		if (m_pDlg->DoModal() == IDCANCEL)
		{
			return -1;// 用户取消了打印操作
		}

		m_nFromPage = m_pDlg->m_pd.nFromPage;
		m_nToPage = m_pDlg->m_pd.nToPage;
		m_nCopies = m_pDlg->m_pd.nCopies;
		m_sPrinterName = m_pDlg->GetDeviceName();

		// 更新配置文件
		ZLib_SetIniDyjName(m_sTempFplxdm, m_sPrinterName);
	}

	DEVMODE *lpDevMode = m_pDlg->GetDevMode();
	lpDevMode->dmFields |= DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	lpDevMode->dmPaperSize = DMPAPER_USER;
	lpDevMode->dmPaperWidth = pwidth;       // 指定发票实物宽高
	if (nZzlx == 1)
		plength = 2794;
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

char* CFpdyBase::GbkToUtf8(const char* src_str, int& rlen)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	delete wstr;
	rlen = len + 1;
	return str;
}

CString CFpdyBase::Utf8ToGbk(const char* src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	CString rStr(szGBK);
	delete[] wszGBK;
	delete[] szGBK;
	return rStr;
}

int CFpdyBase::charCal(unsigned char c)
{
	if ((c >>= 1) == 0x7E) return 6; // 11111100
	if ((c >>= 1) == 0x3E) return 5; // 11111000
	if ((c >>= 1) == 0x1E) return 4; // 11110000
	if ((c >>= 1) == 0x0E) return 3; // 11100000
	if ((c >>= 1) == 0x06) return 2; // 11000000

	return 1; // 01111111
}

int CFpdyBase::Utf8StrLen(const char* str)
{
	int length = 0;
	unsigned char c;
	
	while ((c = *str) != '\0')
	{
		length += charCal(c);
		str++;
	} 

	return length;
}

int CFpdyBase::Utf8StrSize(const char* str)
{
	int size = 0;
	int count = Utf8StrLen(str);
	for (int i = 0; i < count; i++)
	{
		int nL = charCal(str[i]);
		size += (nL == 1 ? 1 : 2);
	}
	return size;
}

int CFpdyBase::Utf8StringInsert(char** data, int pos, int datasize, char c)
{
	char* p = *data;

	char* pDes = new char[datasize + 1 + 1];// datasize不包含元数据结束符
	memset(pDes, 0, datasize + 1 + 1);
	memcpy(pDes, p, pos);
	memset(pDes + pos, c, 1);
	memcpy(pDes + pos + 1, p + pos, datasize - pos);

	delete[] p;
	*data = pDes;
	
	return 0;
}

int CFpdyBase::Utf8StringWraps(CDC* pDC, char** pdata, int s, size_t width, bool bChange, bool type)
{
	char* data = *pdata;
	int i = s;
	int nTemp = 0; // 逻辑字符个数
	int count = Utf8StrLen(data);

	char* pSrc = data + s;

	for (; i < count;)
	{
		char c = data[i];
		if (c == '\n')
		{
			i += 1;
			return Utf8StringWraps(pDC, pdata, i, width, bChange, type) + 1;
		}

		int nL = charCal(c);
		i += nL;

		if (!type)
		{
			int len = i - s;
			char* buf = new char[len + 1];
			memset(buf, 0, len + 1);
			memcpy_s(buf, len + 1, pSrc, len);
			CString str = Utf8ToGbk(buf);
			delete[] buf;

			CSize size = pDC->GetTextExtent(str);
			nTemp = size.cx;			
		}
		else
		{
			nTemp += (nL == 1 ? 1 : 2);
		}

		if (nTemp / width) {
			if (nTemp % width)
			{
				i -= nL;
			}
			if (bChange && i < count)
			{
				Utf8StringInsert(&data, i, count);
				i += 1;
				*pdata = data;
			}

			return Utf8StringWraps(pDC, pdata, i, width, bChange, type) + 1;
		}
	}

	return s < count ? 1 : 0;
}

int CFpdyBase::Utf8StringSub(char* data, int maxlen)
{
	int i = 0;
	int nTemp = 0;
	int count = Utf8StrLen(data);

	for (; i < count;)
	{
		char c = *(data + i);

		int nL = charCal(c);
		i += nL;
		nTemp += (nL == 1 ? 1 : 2);
		if (nTemp / maxlen) {
			if (nTemp % maxlen)
			{
				i -= nL;
			}

			data[i] = '\0';

			return i;
		}
	}

	return i;
}

int CFpdyBase::utf8Deal(CFont* fontOld, CFont* fontNew, LPCSTR data, RECT rect, int f, LPCSTR FontType, CDC* pDC, UINT flags, RECT& _trect)
{
	fontNew->CreatePointFont(f, FontType, pDC);
	fontOld = (CFont*)(::SelectObject(m_hPrinterDC, *fontNew));

	int len = 0;
	char* pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pDC, &pUtf8, 0, rect.right - rect.left);
	CString data1 = Utf8ToGbk(pUtf8);

	RECT trect = rect;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags);
	LONG r = trect.right;
	if (((h >= recv_h
		|| (r > recv_r)) && ((f -= 1) || 1)))
	{
		::SelectObject(m_hPrinterDC, fontOld);
		fontNew->DeleteObject();

		return utf8Deal(fontOld, fontNew, data, rect, f, FontType, pDC, flags, _trect);
	}

	_trect = trect;

	return h;
}

// nType = 1机动车  2二手车  3转普票
void CFpdyBase::setBuiltInOffset(IN int nType, OUT int & _x, OUT int & _y)
{
	return;
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

int CFpdyBase::DealData1(CString & m_szText, int s, LONG width)
{
	BOOL chineseFlag = FALSE;
	int i = s;
	CString str = m_szText.Mid(s);
	int nCount = m_szText.GetLength();
	for (; i < nCount; ++i) {
		int size;
		chineseFlag = FALSE;
		char c = m_szText.GetAt(i);

		if (c == '\n')
		{
			s = i + 1;
			return DealData1(m_szText, s, width) + 1;
		}

		//if ((unsigned char)c >= 0xA0 && (unsigned char)m_szText.GetAt(i + 1) >= 0xA0)
		if ((unsigned char)c & 0x80 && (unsigned char)m_szText.GetAt(i + 1) & 0x80)
		{
			chineseFlag = TRUE;
			i += 1;
		}

		size = str.Left(i + 1 - s).GetLength();
		if (size / width) {
			if (chineseFlag == TRUE) {
				if (size % width != 0)
				{
					i -= 1;
					m_szText.Insert(i, '\n');
				}
				else
				{
					if (i + 1 < nCount)
					{
						i += 1;
						if (m_szText.GetAt(i) != '\n')
						{
							m_szText.Insert(i, '\n');
						}
					}
				}
			}
			else {
				if (i + 1 < nCount)
				{
					i += 1;
					if (m_szText.GetAt(i) != '\n')
					{
						m_szText.Insert(i, '\n');
					}
				}
			}

			s = i + 1;
			return DealData1(m_szText, s, width) + 1;
		}
	}
	return (nCount == i && s < nCount) ? 1 : 0;
}

int CFpdyBase::DealData(CDC * pDC, CString& m_szText, int s, LONG width)
{
	BOOL chineseFlag = FALSE;
	int i = s;
	CString str = m_szText.Mid(s);
	int nCount = m_szText.GetLength();
	for (; i < nCount; ++i) {
		CSize size;
		chineseFlag = FALSE;
		char c = m_szText.GetAt(i);

		if ( c == '\n')
		{
			s = i + 1;
			return DealData(pDC, m_szText, s, width);
		}

		//if ((unsigned char)c >= 0xA0 && (unsigned char)m_szText.GetAt(i + 1) >= 0xA0)
		if ((unsigned char)c & 0x80 && (unsigned char)m_szText.GetAt(i + 1) & 0x80)
		{
			chineseFlag = TRUE;
			i += 1;
		}

		CString dd = str.Left(i + 1 - s);
		size = pDC->GetTextExtent(str.Left(i + 1 - s));
		if ((size.cx) / width) {
			if (chineseFlag == TRUE) {
				if (size.cx % width != 0)
				{
					i -= 1;
					m_szText.Insert(i, '\n');
				}
				else
				{
					if (i + 1 < nCount)
					{
						i += 1;
						if (m_szText.GetAt(i) != '\n')
						{
							m_szText.Insert(i, '\n');
						}							
					}			
				}
			}
			else {
				if (size.cx % width != 0)
				{
					m_szText.Insert(i, '\n');
				}
				else
				{
					if (i + 1 < nCount)
					{
						i += 1;
						if (m_szText.GetAt(i) != '\n')
						{
							m_szText.Insert(i, '\n');
						}
					}
				}
			}

			s = i + 1;
			return DealData(pDC, m_szText, s, width);
		}
	}
	return 0;
}

int CFpdyBase::Deal(CFont* fontOld, CFont* fontNew, LPCSTR data, RECT rect, int f, LPCSTR FontType, CDC* pDC, UINT flags, RECT& _trect)
{
	fontNew->CreatePointFont(f, FontType, pDC);
	fontOld = (CFont *)(::SelectObject(m_hPrinterDC, *fontNew));

	CString data1 = data;

	DealData(pDC, data1, 0, rect.right - rect.left);

	RECT trect = rect;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags);
	LONG r = trect.right;
	if (((h >= recv_h
		|| (r > recv_r)) && ((f -= 1) || 1)))
	{
		::SelectObject(m_hPrinterDC, fontOld);
		fontNew->DeleteObject();

		return Deal(fontOld, fontNew, data, rect, f, FontType, pDC, flags, _trect);
	}

	_trect = trect;

	return h;
}

void CFpdyBase::PaintTile4(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z1, RECT _rect)
{

	int z = z1 & 0x000000ff;
	int ls = z1 & 0xff00ff00;

	rect = {
		rect.left + _rect.left,
		rect.top - _rect.top,
		rect.right - _rect.right,
		rect.bottom + _rect.bottom
	};

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
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZL || z == AM_ZL_EX || z == AM_ZL_L)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
	}
	else if (z == AM_ZR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
	}

	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;

	CDC* pCDC = CDC::FromHandle(m_hPrinterDC);

	SIZE coin_xy_Y, coin_xy_O;// 调整钱币符号中心坐标
	coin_xy_Y.cx = 15;
	coin_xy_Y.cy = 3;

	coin_xy_O.cx = 20;
	coin_xy_O.cy = 0;

	pCDC->LPtoDP(&coin_xy_Y);
	pCDC->LPtoDP(&coin_xy_O);

	SIZE coin_top_Y; // 调整钱币符号Y上半部分俩点坐标
	coin_top_Y.cx = 10;
	coin_top_Y.cy = 13;
	pCDC->LPtoDP(&coin_top_Y);

	SIZE coin_mid_Y; // 调整钱币符号Y中间部分坐标
	coin_mid_Y.cx = 10;
	coin_mid_Y.cy = 6;
	pCDC->LPtoDP(&coin_mid_Y);

	SIZE coin_bottom_Y; // 调整钱币符号Y下半部分坐标
	coin_bottom_Y.cx = 0;
	coin_bottom_Y.cy = 15;
	pCDC->LPtoDP(&coin_bottom_Y);

	SIZE coin_ep_ltrb_O; // 调整钱币符号XO左上右下坐标
	coin_ep_ltrb_O.cx = 15;
	coin_ep_ltrb_O.cy = 15;
	pCDC->LPtoDP(&coin_ep_ltrb_O);

	SIZE coin_ep_x1_O; // 调整钱币符号XO起始位置坐标
	coin_ep_x1_O.cx = 12;
	coin_ep_x1_O.cy = 11;
	pCDC->LPtoDP(&coin_ep_x1_O);

	SIZE coin_ep_x2_O; // 调整钱币符号XO斜线终止位置坐标
	coin_ep_x2_O.cx = 11;
	coin_ep_x2_O.cy = 11;
	pCDC->LPtoDP(&coin_ep_x2_O);

	pCDC->LPtoDP(&rect);

	pCDC->SetMapMode(MM_TEXT);

	fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

#ifdef UTF8_TEST
	int len = 0;
	char* pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data1 = Utf8ToGbk(pUtf8);
#else
	CString data1 = data;
	CString data2 = data;
	DealData(pCDC, data1, 0, rect.right - rect.left);
#endif // UTF8_TEST

	RECT trect = rect;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags1);
	LONG r = trect.right;
	if (((h >= recv_h
		|| (r > recv_r
			&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S || z == AM_ZR_S
				|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1))) //如果多行，居中左对齐
	{
		::SelectObject(m_hPrinterDC, pOldFont);
		fontHeader.DeleteObject();
#ifdef UTF8_TEST
		h = utf8Deal(pOldFont, &fontHeader, data, rect, fontSize, FontType, pCDC, flags1, trect);
#else
		h = Deal(pOldFont, &fontHeader, data2, rect, fontSize, FontType, pCDC, flags1, trect);
#endif // UTF8_TEST	
	}

	int x = 0;
	int y = 0;

	if (COIN_Y == (ls & 0xff000000))
	{
		::SelectObject(m_hPrinterDC, pOldFont);
		fontHeader.DeleteObject();

		if (z == AM_VCL_S)
			x = rect.left - coin_xy_Y.cx;
		else if (z == AM_VCR_S)
			x = rect.right - (trect.right - rect.left) - coin_xy_Y.cx;

		y = (rect.top + rect.bottom) / 2 - coin_xy_Y.cy;

		MoveToEx(m_hPrinterDC, x, y, NULL);
		LineTo(m_hPrinterDC, x - coin_top_Y.cx, y - coin_top_Y.cy);

		MoveToEx(m_hPrinterDC, x, y, NULL);
		LineTo(m_hPrinterDC, x + coin_top_Y.cx, y - coin_top_Y.cy);

		MoveToEx(m_hPrinterDC, x - coin_mid_Y.cx, y, NULL);
		LineTo(m_hPrinterDC, x + coin_mid_Y.cx, y);

		MoveToEx(m_hPrinterDC, x - coin_mid_Y.cx, y + coin_mid_Y.cy, NULL);
		LineTo(m_hPrinterDC, x + coin_mid_Y.cx, y + coin_mid_Y.cy);

		MoveToEx(m_hPrinterDC, x, y, NULL);
		LineTo(m_hPrinterDC, x, y + coin_bottom_Y.cy);
	}
	else if (COIN_O == (ls & 0xff000000))
	{
		if (z == AM_VCL_S)
			x = rect.left - coin_xy_O.cx;
		else if (z == AM_VCR_S)
			x = rect.right - (trect.right - rect.left) - coin_xy_O.cx;

		y = (rect.top + rect.bottom) / 2;

		::Ellipse(m_hPrinterDC, x - coin_ep_ltrb_O.cx, y - coin_ep_ltrb_O.cy, x + coin_ep_ltrb_O.cx, y + coin_ep_ltrb_O.cy);
		::MoveToEx(m_hPrinterDC, x - coin_ep_x1_O.cx, y + coin_ep_x1_O.cy, NULL);
		::LineTo(m_hPrinterDC, x + coin_ep_x2_O.cx, y - coin_ep_x2_O.cy);
		::MoveToEx(m_hPrinterDC, x - coin_ep_x1_O.cx, y - coin_ep_x1_O.cy, NULL);
		::LineTo(m_hPrinterDC, x + coin_ep_x2_O.cx, y + coin_ep_x2_O.cy);

		::SelectObject(m_hPrinterDC, pOldFont);
		fontHeader.DeleteObject();
	}

	pCDC->SetMapMode(MM_LOMETRIC);
}

LONG CFpdyBase::PaintTile3(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z, RECT _rect)
{
	rect = {
		rect.left + _rect.left,
		rect.top - _rect.top,
		rect.right - _rect.right,
		rect.bottom + _rect.bottom
	};

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
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZL || z == AM_ZL_EX || z == AM_ZL_L)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
	}
	else if (z == AM_ZR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
	}

	if (AM_ZC_S == z)
		return YKFP_LINE_H_MIN;

	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;

	CDC* pCDC = CDC::FromHandle(m_hPrinterDC);
	pCDC->LPtoDP(&rect);
	pCDC->SetMapMode(MM_TEXT);

	fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

#ifdef UTF8_TEST
	int len = 0;
	char* pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data1 = Utf8ToGbk(pUtf8);
#else
	CString data1 = data;
	DealData(pCDC, data1, 0, rect.right - rect.left);
#endif // UTF8_TEST

	RECT trect = rect;

	flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags1);

	pCDC->SetMapMode(MM_LOMETRIC);

	LONG lData = 0;

	SIZE s;
	s.cx = 0;
	s.cy = h;
	pCDC->DPtoLP(&s);
	lData = s.cy;


	::SelectObject(m_hPrinterDC, pOldFont);
	fontHeader.DeleteObject();

	return lData > YKFP_LINE_H_MAX ? YKFP_LINE_H_MAX : lData;
}

LONG CFpdyBase::PaintTile2(int iType, int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z, RECT _rect)
{
	rect = {
		rect.left + _rect.left,
		rect.top - _rect.top,
		rect.right - _rect.right,
		rect.bottom + _rect.bottom
	};

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
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZL || z == AM_ZL_EX || z == AM_ZL_L)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
	}
	else if (z == AM_ZR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
	}

	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;

	CDC* pCDC = CDC::FromHandle(m_hPrinterDC);
	pCDC->LPtoDP(&rect);
	pCDC->SetMapMode(MM_TEXT);

	fontHeader.CreatePointFont(fontSize, FontType, CDC::FromHandle(m_hPrinterDC));
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

#ifdef UTF8_TEST
	int len = 0;
	char* pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data1 = Utf8ToGbk(pUtf8);
#else
	CString data1 = data;
	CString data2 = data;
	DealData(pCDC, data1, 0, rect.right - rect.left);
#endif // UTF8_TEST

	RECT trect = rect;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags1);
	LONG r = trect.right;
	if (((h >= recv_h
		|| (r > recv_r
			&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S || z == AM_ZR_S
				|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1))) //如果多行，居中左对齐
	{
		::SelectObject(m_hPrinterDC, pOldFont);
		fontHeader.DeleteObject();
#ifdef UTF8_TEST
		h = utf8Deal(pOldFont, &fontHeader, data, rect, fontSize, FontType, pCDC, flags1, trect);
#else
		h = Deal(pOldFont, &fontHeader, data2, rect, fontSize, FontType, pCDC, flags1, trect);
#endif // UTF8_TEST	
	}

	pCDC->SetMapMode(MM_LOMETRIC);

	LONG lData = 0;
	if (iType == 1)
	{
		pCDC->DPtoLP(&trect);
		lData = trect.right;
	}
	else if (iType == 2)
	{
		SIZE s;
		s.cx = 0;
		s.cy = h;
		pCDC->DPtoLP(&s);
		lData = s.cy;
	}
	else if (iType == 3)
	{
		lData = fontSize;
	}

	::SelectObject(m_hPrinterDC, pOldFont);
	fontHeader.DeleteObject();

	return lData;
}

void CFpdyBase::PaintTile(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z, RECT _rect)
{
	rect = {
		rect.left + _rect.left,
		rect.top - _rect.top,
		rect.right - _rect.right,
		rect.bottom + _rect.bottom
	};

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
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZL || z == AM_ZL_EX || z == AM_ZL_L)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
	}
	else if (z == AM_ZR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
	}

	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;

	CDC* pCDC = CDC::FromHandle(m_hPrinterDC);
	pCDC->LPtoDP(&rect);
	pCDC->SetMapMode(MM_TEXT);
	
	fontHeader.CreatePointFont(fontSize, FontType, pCDC);
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));	

#ifdef UTF8_TEST
	int len = 0;
	char* pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data1 = Utf8ToGbk(pUtf8);
#else
	CString data1 = data;
	CString data2 = data;
	DealData(pCDC, data1, 0, rect.right - rect.left);
#endif // UTF8_TEST

	RECT trect = rect;
	LONG nSp = 0;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags1);
	LONG r = trect.right;
	if ((z != AM_ZL_EX) && (((h >= recv_h
		|| (r > recv_r
			&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S || z == AM_ZR_S
				|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1))
		|| ((z == AM_ZC || z == AM_ZC_S || z == AM_ZC_CHEKC || z == AM_ZL) && ((nSp = (trect.top - rect.top + rect.bottom - trect.bottom) / 2) && 0)))) //如果多行，居中左对齐
	{		
		::SelectObject(m_hPrinterDC, pOldFont);
		fontHeader.DeleteObject();
#ifdef UTF8_TEST
		h = utf8Deal(pOldFont, &fontHeader, data, rect, fontSize, FontType, pCDC, flags1, trect);
#else
		h = Deal(pOldFont, &fontHeader, data2, rect, fontSize, FontType, pCDC, flags1, trect);
#endif // UTF8_TEST		

		if (z != AM_ZL_L && z != AM_ZR_S && z != AM_ZL_EX)
			nSp = (trect.top - rect.top + rect.bottom - trect.bottom) / 2;
	}

	if (nSp != 0)
	{
		rect.top = rect.top + nSp;
		rect.bottom = rect.bottom - nSp;
	}

#ifdef UTF8_TEST
	pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data2 = Utf8ToGbk(pUtf8);
#else
	DealData(pCDC, data2, 0, rect.right - rect.left);
#endif // UTF8_TEST
	

	if (rect.right >= trect.right)
		::DrawText(m_hPrinterDC, data2, -1, &rect, flags2);
	else
		::DrawText(m_hPrinterDC, data2, -1, &rect, flags3);

	::SelectObject(m_hPrinterDC, pOldFont);
	fontHeader.DeleteObject();

	pCDC->SetMapMode(MM_LOMETRIC);
}

void CFpdyBase::PaintTileXml(int _x, int _y, int _w, int _h, int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, CMarkup &xml, int z1, RECT _rect)
{
	int z = z1 & 0x000000ff;

	rect = {
		rect.left + _rect.left,
		rect.top - _rect.top,
		rect.right - _rect.right,
		rect.bottom + _rect.bottom
	};

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
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	}
	else if (z == AM_ZL || z == AM_ZL_EX || z == AM_ZL_L)
	{
		flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX;
		flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX;
		flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	}
	else if (z == AM_VCL || z == AM_VCL_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX;
	}
	else if (z == AM_VCR || z == AM_VCR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX;
	}
	else if (z == AM_ZR_S)
	{
		flags1 = DT_SINGLELINE | DT_CALCRECT | DT_RIGHT | DT_NOPREFIX;
		flags2 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
		flags3 = DT_SINGLELINE | DT_RIGHT | DT_NOPREFIX;
	}

	CFont *pOldFont;
	CFont fontHeader;
	int fontSize = FontSize;

	CDC* pCDC = CDC::FromHandle(m_hPrinterDC);
	pCDC->LPtoDP(&rect);
	pCDC->SetMapMode(MM_TEXT);

	fontHeader.CreatePointFont(fontSize, FontType, pCDC);
	pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

#ifdef UTF8_TEST
	int len = 0;
	char* pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data1 = Utf8ToGbk(pUtf8);
#else
	CString data1 = data;
	CString data2 = data;
	DealData(pCDC, data1, 0, rect.right - rect.left);
#endif // UTF8_TEST

	RECT trect = rect;
	LONG nSp = 0;

	int recv_h = rect.bottom - rect.top;
	int recv_r = rect.right;
	int h = ::DrawText(m_hPrinterDC, data1, -1, &trect, flags1);
	LONG r = trect.right;
	if ((z != AM_ZL_EX) && (((h >= recv_h
		|| (r > recv_r
			&& (z == AM_VCR_S || z == AM_VCL_S || z == AM_ZC_S || z == AM_ZR_S
				|| (((flags1 = DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags2 = DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX) || 1)
					&& ((flags3 = DT_EDITCONTROL | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX) || 1))))) && ((fontSize -= FontSizeEC) || 1))
		|| ((z == AM_ZC || z == AM_ZC_S || z == AM_ZC_CHEKC || z == AM_ZL) && ((nSp = (trect.top - rect.top + rect.bottom - trect.bottom) / 2) && 0)))) //如果多行，居中左对齐
	{
		::SelectObject(m_hPrinterDC, pOldFont);
		fontHeader.DeleteObject();
#ifdef UTF8_TEST
		h = utf8Deal(pOldFont, &fontHeader, data, rect, fontSize, FontType, pCDC, flags1, trect);
#else
		h = Deal(pOldFont, &fontHeader, data2, rect, fontSize, FontType, pCDC, flags1, trect);
#endif // UTF8_TEST	

		if (z != AM_ZL_L && z != AM_ZR_S && z != AM_ZL_EX)
			nSp = (trect.top - rect.top + rect.bottom - trect.bottom) / 2;
	}

	if (nSp != 0)
	{
		rect.top = rect.top + nSp;
		rect.bottom = rect.bottom - nSp;
	}

#ifdef UTF8_TEST
	pUtf8 = GbkToUtf8(data, len);
	Utf8StringWraps(pCDC, &pUtf8, 0, rect.right - rect.left);
	CString data2 = Utf8ToGbk(pUtf8);
#else
	DealData(pCDC, data2, 0, rect.right - rect.left);
#endif // UTF8_TEST

	XM _xm;
	_xm.x = _x;
	_xm.y = _y;
	_xm.w = _w;
	_xm.h = _h;
	_xm.z = z1;
	_xm.f = FontType;
	_xm.s = FontSize;
	addxml(data2, _xm);

	::SelectObject(m_hPrinterDC, pOldFont);
	fontHeader.DeleteObject();

	pCDC->SetMapMode(MM_LOMETRIC);
}

void CFpdyBase::PaintLine(RECT rect, int ls)
{
	if (ls == LINE_STATE_0)
	{
		Line_L(rect);
		Line_T(rect);
		Line_R(rect);
		Line_B(rect);
	}
	else if (ls == LINE_STATE_L)
	{
		Line_L(rect);
	}
	else if (ls == LINE_STATE_T)
	{
		Line_T(rect);
	}
	else if (ls == LINE_STATE_R)
	{
		Line_R(rect);
	}
	else if (ls == LINE_STATE_B)
	{
		Line_B(rect);
	}
	else if (ls == LINE_STATE_LB)
	{
		Line_L(rect);
		Line_B(rect);
	}
	else if (ls == LINE_STATE_LR)
	{
		Line_L(rect);
		Line_R(rect);
	}
	else if (ls == LINE_STATE_LTB)
	{
		Line_L(rect);
		Line_T(rect);
		Line_B(rect);
	}
	else if (ls == LINE_STATE_LBR)
	{
		Line_L(rect);
		Line_B(rect);
		Line_R(rect);
	}
}

void CFpdyBase::Line_L(RECT rect)
{
	MoveToEx(m_hPrinterDC, rect.left, rect.top, NULL);
	LineTo(m_hPrinterDC, rect.left, rect.bottom);
}

void CFpdyBase::Line_T(RECT rect)
{
	MoveToEx(m_hPrinterDC, rect.left, rect.top, NULL);
	LineTo(m_hPrinterDC, rect.right, rect.top);
}

void CFpdyBase::Line_R(RECT rect)
{
	MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);
	LineTo(m_hPrinterDC, rect.right, rect.bottom);
}

void CFpdyBase::Line_B(RECT rect)
{
	MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL);
	LineTo(m_hPrinterDC, rect.right, rect.bottom);
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
	//if ((unsigned char)(lpszData[nLineMaxLen - 1]) < 0xA0)
	if (!((unsigned char)(lpszData[nLineMaxLen - 1]) & 0x80))
	{
		k = nLineMaxLen;
	}

	// 字段最后一位是汉字，而倒数第二位不是汉字则直接截取到倒数第二位
	/*else if ((unsigned char)(lpszData[nLineMaxLen - 1]) >= 0xA0
		&& (unsigned char)(lpszData[nLineMaxLen - 2]) < 0xA0)*/
	else if ((unsigned char)(lpszData[nLineMaxLen - 1]) & 0x80
		&& !((unsigned char)(lpszData[nLineMaxLen - 2]) & 0x80))
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
			/*if ((unsigned char)(lpszData[i]) >= 0xA0

				&& (unsigned char)(lpszData[i + 1]) >= 0xA0)*/
			if ((unsigned char)(lpszData[i]) & 0x80

				&& (unsigned char)(lpszData[i + 1]) & 0x80)
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
		case -100:
			fpdy.sErrorCode = "不支持此种票样";
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

char* CFpdyBase::PCLib_ChineseFee(char * dest, size_t destsize, char * src)
{
	ZLib_ChineseFee(dest, destsize, src);

	return dest;
}
