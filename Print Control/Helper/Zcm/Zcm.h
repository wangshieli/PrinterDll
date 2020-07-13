#ifndef _ZcmCheck_H_
#define _ZcmCheck_H_

enum _ZcmIndex{
	Zcm_Nsrsbh=0,
	Zcm_Skph,   //1
	Zcm_Qsrq,   //��ʼʱ��
	Zcm_Jzrq,   //��ֹʱ��
	Zcm_TaxApp,  //����˰Ӧ��
	Zcm_Jgdm,   //��������  5
	Zcm_Rjdm,   //�������
	Zcm_Rjbb,  //����汾��
	Zcm_Fplxdm,
	Zcm_Sbcs,   //�̵�λ
	Zcm_Fwdw,   //����λ   10
	Zcm_Uid,   //UID
	Zcm_Scrq,  //��������
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