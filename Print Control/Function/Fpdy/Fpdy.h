#pragma once

#define XMLHEAD "<?xml version=\"1.0\" encoding=\"gbk\"?>\r\n"

//增值税发票
#define FPWidth  (2200+200)		//横向长（单位0.1mm）
#define FPLength  (1390+7)	//纵向长（单位0.1mm）
#define XMGS  8				//项目条数
#define LX  -40			//左边距（单位0.1mm）
#define LY  -40			//上边距（单位0.1mm）
#define LW   0				//宽
#define LH   0				//高

//货运增值税
#define FPWidthHY 3000//2400  //货运增值税发票
#define FPLengthHY 1780
#define XMGSHY  18 //项目个/条数
#define LXHY  0//10				//左边距  单位0.1mm（0.1毫米）
#define LYHY  -100//30			//上边距

#define LS_5    50			// 字号5
#define LS_6    60			//字号6
#define LS_7    70			//字号7
#define LS_8    80			//字号8
#define LS_9    90			//字号9
#define LS_10   100			//字号10
#define LS_11   110			//字号11
#define LS_12   120			//字号12
#define LS_16   160
#define LS_18	180


#define FS  "宋体"			//宋体
#define FT "Courier New"	//T Courier New
#define FH "黑体"           //黑体

#define ZL 0				//左对齐
#define ZR 1				//右对齐
#define ZC 2				//水平居中


//设置打印各项/字段属性
#define xywhsf(A,X,Y,W,H,S,F,Z)\
		((A).x)=(X) ;((A).y)= (Y) ;((A).w)= (W); \
		((A).h)=(H) ;((A).s)= (S) ;((A).f)= (F); \
		((A).z)=(Z)  

//生成XML内部段
#define addxml(a,b)  \
	xml.AddElem("Item",a);xml.AddAttrib("x",b.x);  \
	xml.AddAttrib("y",b.y);xml.AddAttrib("w",b.w); \
	xml.AddAttrib("h",b.h);xml.AddAttrib("s",b.s); \
	xml.AddAttrib("f",b.f);xml.AddAttrib("z",b.z); \

struct XM //打印属性（坐标、宽高、字体、字号）
{
	int x; //坐标
	int y;
	int w; //宽高
	int h;
	int s;//字号
	CString f;//字体
	int z;//左对齐或者右对齐
};

struct FPDY
{
	int iYylxdm;
	CString sKpzdbs;
	CString sNsrsbh;
	CString sSkpbh;
	CString sSkpkl;
	CString sFplxdm;
	CString sFpdm;
	CString sFphm;
	CString sFpmx;
	CString sDylx;
	CString sDyfs;
	CString sDytest;
	CString sKJFS;
	CString	sErrorCode;
	CString	sErrorInfo;

	FPDY() {
		iYylxdm = 0;
		sErrorCode = _T("0");
		sErrorInfo = _T("成功");
	}
	~FPDY() {}
};

class t_Exception
{
private:
	int m_nEcptCode;
	char* m_sErrMsg;

public:
	t_Exception(int nEcptCode, char* sErrMsg)
	{
		m_nEcptCode = nEcptCode;
		m_sErrMsg = sErrMsg;
	}

	int getErrCode() { return m_nEcptCode; }
	char* getErrMsg() { return m_sErrMsg; }
};

class CFpdyBase
{
public:
	CFpdyBase();
	virtual ~CFpdyBase();

public:
	int InitPrinter(short pwidth, short plength);
	CString GenerateXMLFpdy(FPDY fpdy, int rtn = 0);

protected:
	BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

private:
	void GetQRcodePath();

protected:
	char m_cQRcodePath[MAX_PATH];
	int m_iPldy;
	HDC m_hPrinterDC;
	CPrintDialog *m_pDlg;
 	CString m_sPrinterName;
	CString m_sTempFplxdm;
	CString m_sFpzt;
	CString m_sHx;
	CString m_sPrintTaskName;
	CString m_sPrintTaskNameFlag;
	BOOL m_bStatus;
};