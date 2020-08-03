#pragma once

#include "Fpdy.h"

typedef struct _jdcfp_kprq
{
	CString syear;
	CString smouth;
	CString sday;
}JDCFP_KPRQ;

typedef struct _jdcfp_fpxx
{
	CString sFpdm;				// 发票代码
	CString sFphm;				// 发票号码
	CString sFpzt;				// 发票状态
	CString sScbz;				// 上传标志
	CString sKprq;				// 开票日期
	CString sJqbh;				// 机器编号
	CString sSkm;				// 税控码
	CString sGhfmc;				// 购买方名称
	CString sSfzhm;				// 身份证号码/组织机构代码
	CString sGhfsbh;			// 购买方识别号
	CString sScqymc;            // 生产企业名称
	CString sCllx;				// 车辆类型
	CString sCpxh;				// 厂牌型号
	CString sCd;				// 产地
	CString sHgzh;				// 合格证号
	CString sJkzmsh;			// 进口证明书号
	CString sSjdh;				// 商检单号
	CString sFdjhm;				// 发动机号码
	CString sCjhm;				// 车架号码/车辆识别号
	CString sJshj;				// 价税合计
	CString sJshjDx;			// 价税合计大写
	CString sXhdwmc;			// 销货单位名称
	CString sXhdwsbh;			// 销货单位识别号
	CString sDh;				// 电话
	CString sZh;				// 帐号
	CString sDz;				// 地址
	CString sKhyh;				// 开户银行
	CString sZzssl;				// 增值税税率或征收率
	CString sZzsse;				// 增值税税额
	CString sSwjgdm;			// 税务机关代码
	CString sSwjgmc;			// 税务机关名称
	CString sBhsj;				// 不含税价
	CString sWspzhm;			// 完税凭证号码
	CString sDw;				// 吨位
	CString sXcrs;				// 限乘人数
	CString sLslbs;				// 零税率标识
	CString sBz;				// 备注
	CString sKpr;				// 开票人
	CString sYfpdm;				// 原发票代码
	CString sYfphm;				// 原发票号码
	CString sZfrq;				// 作废日期
	CString sZfr;				// 作废人
	CString sQmcs;				// 签名参数
	CString sQmz;				// 签名值
	CString sYkfsje;			// 已开负数金额
	CString sYqjg;				// 验签结果

	XM Fpdm;
	XM Fphm;
	XM Jqbh;
	XM Ghfmc;
	XM Kprq;
	XM Sfzhm;
	XM Ghfsbh;
	XM Cllx;
	XM Cpxh;
	XM Cd;
	XM Hgzh;
	XM Jkzmsh;
	XM Sjdh;
	XM Fdjhm;
	XM Cjhm;
	XM Jshj;
	XM JshjDx;
	XM OX;
	XM Xhdwmc;
	XM Xhdwsbh;
	XM Dh;
	XM Zh;
	XM Dz;
	XM Khyh;
	XM Zzssl;
	XM Zzsse;
	XM Swjgdm;
	XM Swjgmc;
	XM Bhsj;
	XM Wspzhm;
	XM Dw;
	XM Xcrs;
	XM Kpr;

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

	JDCFP_KPRQ kprq;

	_jdcfp_fpxx() {}
	~_jdcfp_fpxx() {}
}JDCFP_FPXX;

class CJdcfpdy :public CFpdyBase
{
public:
	CJdcfpdy();
	virtual ~CJdcfpdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);
	LONG Print(LPCTSTR billXml, CString strFplxdm, CString zzzse);

private:
	JDCFP_FPXX ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy);
	CString GenerateFpdyXml(JDCFP_FPXX fpmx, CString dylx, FPDY fpdy);
	CString GenerateItemXml(JDCFP_FPXX fpmx, FPDY fpdy);
};

