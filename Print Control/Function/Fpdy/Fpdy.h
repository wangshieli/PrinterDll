#pragma once
#include "../../Helper/XML/Markup.h"

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
#define ZVC 3				//垂直居中
#define ZVL 4				//垂直居中左对齐

#define AM_ZC	0	// 水平居中
#define AM_VC	1	// 垂直居中
#define AM_ZL	2	// 左对齐切水平居中，主要用于机动车吨位
#define AM_VCL	3	// 垂直居中左对齐
#define AM_VCR	4	// 垂直居中右对齐

#define AM_VCL_S	5	// 垂直居中左对齐
#define AM_VCR_S	6	// 垂直居中右对齐
#define AM_ZC_S		7	// 水平居中
#define AM_ZL_EX	8	// 多行，不调整字体大小

#define AM_ZC_CHEKC 9 // 绘画区域过大时，普通水平垂直居中不能自动调整左对齐，主要用在发票查询打印客户名称商品名称

#define AM_ZL_L		10
#define AM_ZR_S		11	// 单行靠右，不居中

#define LINE_STATE_0	0x00000000	// 默认自动画矩形
#define LINE_STATE_L	0x00000100  // 画左边竖线
#define LINE_STATE_T	0x00000200	// 画上面横线
#define LINE_STATE_R	0x00000300	// 画右边竖向
#define LINE_STATE_B	0x00000400	// 画下面横线
#define LINE_STATE_LB	0x00000500	// 画左竖线和下横线
#define LINE_STATE_LR	0x00000600	// 画左竖向和右竖线
#define LINE_STATE_LTB	0x00000700	// 画左竖向和上横线和右竖线
#define LINE_STATE_LBR	0x00000800	// 画左竖向和下横线和右竖线

#define COIN_Y			0x01000000	// 钱币符号 Y
#define COIN_O			0x02000000	// 钱币符号 O

#define JSFP_RN			0x03000000	// 卷式发票增加换行符

#define YKFP_LINE_H_MIN	75
#define YKFP_LINE_H_MAX	150

#define A4_W	2100
#define A4_H	2970


//设置打印各项/字段属性
#define xywhsf(A,X,Y,W,H,S,F,Z)\
{\
		((A).x)=(X) ;((A).y)= (Y) ;((A).w)= (W); \
		((A).h)=(H) ;((A).s)= (S) ;((A).f)= (F); \
		((A).z)=(Z) ;((A).fc)= 1;} 

#define xywhsf_fc(A,X,Y,W,H,S,F,Z,FC)\
{\
		((A).x)=(X) ;((A).y)= (Y) ;((A).w)= (W); \
		((A).h)=(H) ;((A).s)= (S) ;((A).f)= (F); \
		((A).z)=(Z) ;((A).fc)= (FC);} 

//生成XML内部段
#define addxml(a,b)  \
{						\
	xml.AddElem("Item",a);xml.AddAttrib("x",b.x);  \
	xml.AddAttrib("y",b.y);xml.AddAttrib("w",b.w); \
	xml.AddAttrib("h",b.h);xml.AddAttrib("s",b.s); \
	xml.AddAttrib("f",b.f);xml.AddAttrib("z",b.z); \
	xml.AddAttrib("fc",b.fc);}					   

struct XM //打印属性（坐标、宽高、字体、字号）
{
	int x; //坐标
	int y;
	int w; //宽高
	int h;
	int s;//字号
	CString f;//字体
	int z;//左对齐或者右对齐
	int fc;//字号变大幅度
};

typedef struct _dy
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

	_dy() {
		iYylxdm = 0;
		sErrorCode = _T("0");
		sErrorInfo = _T("成功");
	}
	~_dy() {}
}FPDY, BBDY;

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
	void InitXYoff();
	bool CheckDyjOnline();

	// InitPrinter1(YKFPCXDY), 获取指定或默认打印机HDC,用于计算预览页码数据项高度使用
	int InitPrinter1(short pwidth, short plength);

	// InitPrinter， 常规打印机初始化
	int InitPrinter(short pwidth, short plength);
	CString GenerateXMLFpdy(FPDY fpdy, int rtn = 0);

protected:
	char* PCLib_ChineseFee(char* dest, size_t destsize, char* src);

protected:
	BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
	void setBuiltInOffset(IN int nType, OUT int &_x, OUT int &_y);

	// 根据设置的字符个数插入换行'\n'
	int DealData1(CString& m_szText, int s, LONG width);

	int DealData(CDC *pDC, CString& m_szText, int s, LONG width);
	int Deal(CFont* fontOld, CFont* fontNew, LPCSTR data, RECT rect, int f, LPCSTR FontType, CDC* pDC, UINT flags, RECT& _trect);

	// PaintTile4 （钱币符号），用于钱币符号打印
	void PaintTile4(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	// PaintTile3 （YKFPCXDY），计算指定字号数据项换行打印高度
	LONG PaintTile3(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	// PaintTile2， 计算数据项适应打印区域之后的右边距、行高、字号
	LONG PaintTile2(int iType, int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	// PaintTile， 正常的打印
	void PaintTile(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	void PaintTileXml(int x, int y, int w, int h, int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, CMarkup &printxml, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	void PaintLine(RECT rect, int ls);

	void Line_L(RECT rect);
	void Line_T(RECT rect);
	void Line_R(RECT rect);
	void Line_B(RECT rect);

	int DataPrintMaxLen(const char *lpszData, int nLineMaxLen);

private:
	void GetQRcodePath();

	void getSysDefPrinter(CString& printer);
	void setSysDefprinter(CString& printer);

protected:
	char m_cQRcodePath[MAX_PATH]; // 二维码
	int m_iPldy;	// 打印方式0:普通打印 (弹框打印)  1:连续打印 (不弹框打印) 2:使用默认打印机不弹框打印
	HDC m_hPrinterDC;
	CPrintDialog *m_pDlg;
 	CString m_sPrinterName;
	CString m_sTempFplxdm;
	CString m_sFpzt;
	CString m_sPrintTaskName;
	CString m_sPrintTaskNameFlag;
	BOOL m_bStatus;
	CString m_sEwm;

	int nXoff;
	int nYoff;
	int nQRCodeSize;
	int nZzlx;

	int m_nLineNum;
	int m_nAllPageNum;

	//	int m_nOrientation;
	int m_nPageSize;

	int m_nFromPage;
	int m_nToPage;
	int m_nCopies;
};