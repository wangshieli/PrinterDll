#pragma once

#include "Fpdy.h"

typedef	struct _jsfp_fyqdxx {
	CString sSpmc;		// 商品名称
	CString sSpsl;		// 商品数量
	CString sDj;		// 单价
	CString sJe;		// 金额
	CString sSl;		// 税率
	CString sSe;		// 税额
	CString sHsdj;		// 含税单价
	CString sHsje;		// 含税金额

	XM ssSpmc;			// 商品名称
	XM ssSpsl;			// 商品数量
	XM ssDj;			// 单价
	XM ssJe;			// 金额
	XM ssSl;			// 税率
	XM ssSe;			// 税额
	XM ssHsdj;			// 含税单价
	XM ssHsje;			// 含税金额

	XM sequenceNum;		// 序列号

}JSFP_FYQDXX;

typedef struct _jsfp_fyxmxx {
	CString sSpmc;		// 商品名称
	CString sSpsl;		// 商品数量
	CString sDj;		// 单价
	CString sJe;		// 金额
	CString sSl;		// 税率
	CString sSe;		// 税额
	CString sHsdj;		// 含税单价
	CString sHsje;		// 含税金额

	XM ssSpmc;			// 商品名称
	XM ssSpsl;			// 商品数量
	XM ssDj;			// 单价
	XM ssJe;			// 金额
	XM ssSl;			// 税率
	XM ssSe;			// 税额
	XM ssHsdj;			// 含税单价
	XM ssHsje;			// 含税金额
}JSFP_FYXMXX;

typedef struct _jsfp_kprq
{
	CString syear;
	CString sYear;
	CString smouth;
	CString sMouth;
	CString sday;
	CString sDay;

	XM year;
	XM Year;
	XM mouth;
	XM Mouth;
	XM day;
	XM Day;
}JSFP_KPRQ;

typedef struct _jsfp_fyxmje
{
	CString sFYXM;
	CString sJE;
	XM fyxm;
	XM je;
}JSFP_FYXMJE;

typedef struct _jsfp_fpxx { //发票明细信息
	CString sFppy;
	CString sBlxx;
	CString sNcpxssgqy;
	CString sFpdm;
	CString sFphm;
	CString sFpzt;
	CString sScbz;
	CString sKprq;
	int sWidthRow8;
	int sNarrowRow8;
	int sWidthRow7;
	int sNarrowRow7;
	bool bMxFontSize;
	//CString sKpsj;
	//CString sTspzbs;
	CString sSkpbh;
	//		CString sSkm;
	CString sJym;
	CString sXhdwsbh;
	CString sXhdwmc;
	CString sGhdwsbh;
	CString sGhdwmc;
	CString sJqbhZW;
	CString sXxfs;
	CString sFplxdm;


	XM Ncpxssgqy;
	XM Fpdm;
	XM Fphm;
	XM Kprq;
	//XM Tspzbs;
	XM Skpbh;
	//		XM Skm;
	XM Jym;
	XM Xhdwsbh;
	XM Xhdwmc;
	XM Ghdwsbh;
	XM Ghdwmc;
	XM JqbhZW;
	XM Xxfs;

	int iFyxmCount;
	JSFP_FYXMXX fyxmxx[50];

	int iFyqdCount;
	//		LTFPQDXX fpqdxx;

	//		int iFyqtCount;
	//		struct Fyqtxx{
	//			CString sSl;
	//			CString sJe;
	//			CString sSe;
	// 		} fyqtxx[1000];

	CString sJEtemp;
	CString	sSEtemp;

	CString sHjjeTemp;
	CString sHjseTemp;

	//		CString sZhsl;
	CString sHjje;
	CString sHjse;
	CString sJshj;

	CString sBz;
	CString sSkr;
	//		CString sFhr;
	CString sKpr;
	CString sJmbbh;
	CString sZyspmc;
	CString sSpsm;
	CString sQdbz;
	CString sSsyf;
	CString sKpjh;
	CString sTzdbh;
	CString sYfpdm;
	CString sYfphm;
	CString sZfrq;
	CString sZfr;
	CString sQmcs;
	CString sQmz;
	CString sYkfsje;

	XM yfpdm;
	XM yfphm;

	JSFP_KPRQ kprq;

	CString sBzF;   //负数票备注中添加原始发票代码号码
	CString sJshjDx;//价税合计大写
//		CString sSkm1;
//		CString sSkm2;
//		CString sSkm3;
// 		CString sSkm4;
	CString sXmmc;

	XM xmmc;
	XM xjje;
	XM xjse;

	XM zhsl;
	XM hjje;
	XM hjse;
	XM jshj;
	XM jshjDx;//零壹贰叁肆伍陆柒捌玖拾佰仟万亿角分
	XM OX;//正数发票大写金额前 圈叉 符号位置

	XM bzF;//负数票备注中添加原始发票代码号码
	XM bz;

	XM skr;
	XM fhr;
	XM kpr;

	//		XM skm1;//税控码第一行
	//		XM skm2;//税控码第二行
	//		XM skm3;//税控码第三行
	// 		XM skm4;//税控码第四行

	JSFP_FYXMJE fyxmje[3];

	_jsfp_fpxx() {
		iFyxmCount = 0;

		kprq.sYear = _T("-");
		kprq.sMouth = _T("-");

		fyxmje[0].sFYXM = _T("费用项目");
		fyxmje[1].sFYXM = _T("费用项目");
		fyxmje[2].sFYXM = _T("费用项目");
		fyxmje[0].sJE = _T("金额");
		fyxmje[1].sJE = _T("金额");
		fyxmje[2].sJE = _T("金额");
	}
	~_jsfp_fpxx() {}
}JSFP_FPXX;

class CJsfpdy :public CFpdyBase
{
public:
	CJsfpdy();
	virtual ~CJsfpdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);
	LONG Print(LPCTSTR billXml);

private:
	JSFP_FPXX ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy);
	CString GenerateFpdyXml(JSFP_FPXX fpmx, CString dylx, FPDY fpdy);
	CString GenerateItemXmlA(JSFP_FPXX fpmx, FPDY fpdy);	// 宽短
	CString GenerateItemXmlB(JSFP_FPXX fpmx, FPDY fpdy);	// 宽长
	CString GenerateItemXmlC(JSFP_FPXX fpmx, FPDY fpdy);	// 窄短
	CString GenerateItemXmlD(JSFP_FPXX fpmx, FPDY fpdy);	// 窄长
	CString GenerateItemXmlE(JSFP_FPXX fpmx, FPDY fpdy);	// 湖北
	CString GenerateItemXmlF(JSFP_FPXX fpmx, FPDY fpdy);	// 总局新票种宽长
	CString GenerateItemXmlG(JSFP_FPXX fpmx, FPDY fpdy);	// 总局新票种窄长
	CString GenerateItemXmlH(JSFP_FPXX fpmx, FPDY fpdy);	// 成都

private:
	CString m_strFppy;
};

