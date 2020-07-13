#ifndef _ZcmCheck_H_
#define _ZcmCheck_H_

enum _ZcmIndex{
	Zcm_Nsrsbh=0,
	Zcm_Skph,   //1
	Zcm_Qsrq,   //起始时间
	Zcm_Jzrq,   //截止时间
	Zcm_TaxApp,  //国地税应用
	Zcm_Jgdm,   //地区代码  5
	Zcm_Rjdm,   //软件代码
	Zcm_Rjbb,  //软件版本号
	Zcm_Fplxdm,
	Zcm_Sbcs,   //盘单位
	Zcm_Fwdw,   //服务单位   10
	Zcm_Uid,   //UID
	Zcm_Scrq,  //生成日期
	Zcm_Resv1,
	Zcm_Resv2,
	Zcm_Resv3,   //15
	MaxZcmNum
};

typedef _ZcmIndex TZcmIndex;

#ifdef ZCM_EXPORTS
#define _DllPreFix __declspec(dllexport)
#else
#define _DllPreFix __declspec(dllimport)
#endif


#define _cplusplus
#ifdef _cplusplus
extern "C" 
{
#endif

_DllPreFix DWORD64 importZcm(IN LPSTR pYzcm, OUT DWORD64 &errCode, OUT std::string& errMsg);
_DllPreFix BOOL CheckZcm(IN TZcmIndex zcmInd, IN LPCSTR pCmpInfo, OUT DWORD64 &errCode, OUT std::string& errMsg);
_DllPreFix BOOL GetZcmQssj(OUT std::string& qssj);
_DllPreFix BOOL GetZcmZzsj(OUT std::string& zzsj);


#ifdef _cplusplus
}
#endif


#endif