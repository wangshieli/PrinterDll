#pragma once

#include "Fpdy.h"

typedef struct _escfp_kprq
{
	CString syear;
	CString smouth;
	CString sday;

}ESCFP_KPRQ;

typedef struct _escfp_fpxx
{
	CString sFpdm;				// 发票代码
	CString sFphm;				// 发票号码
	CString sJqbh;				// 机器编号
	CString sSkm;				// 税控码
	CString sJym;				// 校验码
	CString sFpzt;				// 发票状态
	CString sScbz;				// 上传标志
	CString sKprq;				// 开票日期		
	CString sGfmc;				// 购买名称		
	CString sGfsfzhm;			// 购方身份证号
	CString sGfdz;				// 购方地址
	CString sGfdh;				// 购方电话
	CString sMfmc;				// 购方名称
	CString sMfsfzhm;			// 购方身份证号
	CString sMfdz;				// 购方地址
	CString sMfdh;				// 购方电话
	CString sCpzh;				// 车牌照号
	CString sDjzh;				// 登记证号
	CString sCllx;				// 车辆类型
	CString sCjhm;				// 车架号码/车辆识别号
	CString sCpxh;				// 厂牌型号
	CString sZrdclglsmc;		// 转入地车辆管理所名称
	CString sCjhj;				// 车价合计
	CString sCjhjDx;			// 车价合计大写
	CString sJypmdw;			// 经营拍卖单位
	CString sJypmdwdz;			// 经营拍卖单位地址
	CString sJypmdwnsrsbh;		// 经营拍卖单位纳税人识别号
	CString sJypmdwyhzh;		// 开户银行账号
	CString sJypmdwdh;			// 电话
	CString sEscsc;				// 二手车市场
	CString sEscscnsrsbh;		// 纳税人识别号
	CString sEscscdz;			// 地址
	CString sEscscyhzh;			// 开户银行账号
	CString sEscscdh;			// 电话
	CString sBz;				// 备注
	CString sKpr;				// 开票人

	XM Fpdm;
	XM Fphm;
	XM Jqbh;
	XM Skm;
	XM Kprq;
	XM Gfmc;
	XM Gfsfzhm;
	XM Gfdz;
	XM Gfdh;
	XM Mfmc;
	XM Mfsfzhm;
	XM Mfdz;
	XM Mfdh;
	XM Cpzh;
	XM Djzh;
	XM Cllx;
	XM Cjhm;
	XM Cpxh;
	XM Zrdclglsmc;
	XM Cjhj;
	XM CjhjDx;
	XM Jypmdw;
	XM Jypmdwdz;
	XM Jypmdwnsrsbh;
	XM Jypmdwyhzh;
	XM Jypmdwdh;
	XM Escsc;
	XM Escscnsrsbh;
	XM Escscdz;
	XM Escscyhzh;
	XM Escscdh;
	XM Bz;
	XM Kpr;
	XM OX;

	CString sSkm1;
	CString sSkm2;
	CString sSkm3;
	CString sSkm4;
	CString sSkm5;

	XM Skm1;// 税控码第一行
	XM Skm2;// 税控码第二行
	XM Skm3;// 税控码第三行
	XM Skm4;// 税控码第四行
	XM Skm5;// 税控码第五行

	ESCFP_KPRQ kprq;

	_escfp_fpxx() {}
	~_escfp_fpxx() {}
}ESCFP_FPXX;

class CEscxstyfp :public CFpdyBase
{
public:
	CEscxstyfp();
	virtual ~CEscxstyfp();

public:
	CString Dlfpdy(LPSTR sInputInfo);
	LONG Print(LPCTSTR billXml, CString strFplxdm);

private:
	CString GenerateXMLFpdy(FPDY fpdy);
	ESCFP_FPXX ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy);
	CString GenerateFpdyXml(ESCFP_FPXX fpmx, CString dylx, FPDY fpdy);
	CString GenerateItemXml(ESCFP_FPXX fpmx, FPDY fpdy);
};

