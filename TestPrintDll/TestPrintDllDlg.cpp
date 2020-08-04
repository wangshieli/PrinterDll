
// TestPrintDllDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TestPrintDll.h"
#include "TestPrintDllDlg.h"
#include "afxdialogex.h"
#include "Markup.h"

#include "../Print Control/Print Control.h"

#pragma comment(lib, "../Debug/PRINTCONTROL.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestPrintDllDlg 对话框



CTestPrintDllDlg::CTestPrintDllDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTPRINTDLL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestPrintDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestPrintDllDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_004QD, &CTestPrintDllDlg::OnBnClickedButton004qd)
	ON_BN_CLICKED(IDC_BUTTON_004, &CTestPrintDllDlg::OnClickedButton004)
	ON_BN_CLICKED(IDC_BUTTON_025FP, &CTestPrintDllDlg::OnBnClickedButton025fp)
	ON_BN_CLICKED(IDC_BUTTON_005FP, &CTestPrintDllDlg::OnBnClickedButton005fp)
	ON_BN_CLICKED(IDC_BUTTON_006FP, &CTestPrintDllDlg::OnBnClickedButton006fp)
	ON_BN_CLICKED(IDC_BUTTON1, &CTestPrintDllDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestPrintDllDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestPrintDllDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CTestPrintDllDlg 消息处理程序

BOOL CTestPrintDllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestPrintDllDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestPrintDllDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestPrintDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestPrintDllDlg::OnBnClickedButton004qd()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印004QD!\r\n"));
	CMarkup xml004qd;
	if (!xml004qd.Load("004QD_2P.xml"))
	{
		TRACE(_T("打开004QD.xml失败!\r\n"));
		return;
	}

	CString qdXmlData = xml004qd.GetDoc();
	OutputDebugString(qdXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(qdXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnClickedButton004()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印004FP!\r\n"));
	CMarkup xml004fp;
	if (!xml004fp.Load("004FP.xml"))
	{
		TRACE(_T("打开004FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml004fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnBnClickedButton025fp()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印025FP!\r\n"));
	CMarkup xml025fp;
	if (!xml025fp.Load("025FP.xml"))
	{
		TRACE(_T("打开025FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml025fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnBnClickedButton005fp()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印005FP!\r\n"));
	CMarkup xml005fp;
	if (!xml005fp.Load("005FP.xml"))
	{
		TRACE(_T("打开005FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml005fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnBnClickedButton006fp()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印006FP!\r\n"));
	CMarkup xml006fp;
	if (!xml006fp.Load("006FP.xml"))
	{
		TRACE(_T("打开006FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml006fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印006FP!\r\n"));
	CMarkup xml006fp;
	if (!xml006fp.Load("khbm.xml"))
	{
		TRACE(_T("打开006FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml006fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印006FP!\r\n"));
	CMarkup xml006fp;
	if (!xml006fp.Load("spbm.xml"))
	{
		TRACE(_T("打开006FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml006fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}


void CTestPrintDllDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE(_T("开始打印006FP!\r\n"));
	CMarkup xml006fp;
	if (!xml006fp.Load("HZXXB.xml"))
	{
		TRACE(_T("打开006FP.xml失败!\r\n"));
		return;
	}

	CString fpXmlData = xml006fp.GetDoc();
	OutputDebugString(fpXmlData);

	char zc[1024] = { 0 };
	PostAndRecvEx(fpXmlData.GetBuffer(0), zc);
}
