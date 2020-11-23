#pragma once

#include "Fpdy.h"
#include <list>

using namespace std;

typedef struct _zzsfp_fyxmxx
{
	CString sFphxz;
	CString sSpmc;
	CString sSpsm;
	CString sGgxh;
	CString sDw;
	CString sSpsl;
	CString sDj;
	CString sJe;
	CString sSl;
	CString sSe;
	CString sHsbz;
	CString sLslbs;

	XM ssFphxz;
	XM ssSpmc;
	XM ssSpsm;
	XM ssGgxh;
	XM ssDw;
	XM ssSpsl;
	XM ssDj;
	XM ssJe;
	XM ssSl;
	XM ssSe;
	XM ssHsbz;
}ZZSFP_FYXMXX;

typedef struct _zzsfp_kprq
{
	CString syear;
	CString sYear;
	CString smouth;
	CString sMouth;
	CString sday;
	CString sDay;
}ZZSFP_KPRQ;

typedef struct _zzsfp_fpxx
{
	CString sTspz;
	CString sNcpsg;
	CString sCpy;
	CString sBlxx;
	CString sNcpxssgqy;
	CString sFpdm;
	CString sFphm;
	CString sFpzt;
	CString sScbz;
	CString sKprq;
	//CString sKpsj;
	//CString sTspzbs;
	CString sSkpbh;
	CString sSkm;
	CString sJym;
	CString sXhdwsbh;
	CString sXhdwmc;
	CString sXhdwdzdh;
	CString sXhdwyhzh;
	CString sGhdwsbh;
	CString sGhdwmc;
	CString sGhdwdzdh;
	CString sGhdwyhzh;
	CString sZsfs;
	CString sJqbhZW;
	CString sXxfs;
	CString sFplxdm;


	XM Ncpxssgqy;
	XM Fpdm;
	XM Fphm;
	XM Kprq;
	//XM Tspzbs;
	XM Skpbh;
	XM Skm;
	XM Hxjym;
	XM Xhdwsbh;
	XM Xhdwmc;
	XM Xhdwdzdh;
	XM Xhdwyhzh;
	XM Ghdwsbh;
	XM Ghdwmc;
	XM Ghdwdzdh;
	XM Ghdwyhzh;
	XM JqbhZW;
	XM Xxfs;
	XM Ncpsg;
	XM Cpy;

	int iFyxmCount;
	ZZSFP_FYXMXX fyxmxx[10];

	CString sZKJETemp;
	CString sZKSETemp;

	CString sJEtemp;
	CString	sSEtemp;

	CString sHjjeTemp;
	CString sHjseTemp;

	CString sZhsl;
	CString sHjje;
	CString sHjse;
	CString sJshj;
	CString sBz;
	CString sSkr;
	CString sFhr;
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

	ZZSFP_KPRQ kprq;

	CString sBzF;   //����Ʊ��ע�����ԭʼ��Ʊ�������
	CString sJshjDx;//��˰�ϼƴ�д
	CString sSkm1;
	CString sSkm2;
	CString sSkm3;
	CString sSkm4;

	XM zkje;
	XM zkse;

	XM xjje;
	XM xjse;

	XM zhsl;
	XM hjje;
	XM hjse;
	XM jshj;
	XM jshjDx;//��Ҽ��������½��ƾ�ʰ��Ǫ���ڽǷ�
	XM OX;//������Ʊ��д���ǰ Ȧ�� ����λ��

	XM bzF;//����Ʊ��ע�����ԭʼ��Ʊ�������
	XM bz;

	XM skr;
	XM fhr;
	XM kpr;

	XM skm1;//˰�����һ��
	XM skm2;//˰����ڶ���
	XM skm3;//˰���������
	XM skm4;//˰���������

	_zzsfp_fpxx()
	{
		iFyxmCount = 0;

		kprq.sYear = _T("��");
		kprq.sMouth = _T("��");
		kprq.sDay = _T("��");
	}
	~_zzsfp_fpxx() {}
}ZZSFP_FPXX;

class CZzsfpdy :public CFpdyBase
{
public:
	CZzsfpdy();
	virtual ~CZzsfpdy();

public:
	CString Dlfpdy(LPCTSTR sInputInfo);

private:
	LONG Print(LPCTSTR billXml, CString hjje, CString hjse);

private:
	ZZSFP_FPXX ParseFpmxFromXML(LPCTSTR inXml, FPDY fpdy);
	CString GenerateFpdyXml(ZZSFP_FPXX fpmx, CString dylx, FPDY fpdy);
	CString GenerateItemXml(ZZSFP_FPXX fpmx, FPDY fpdy);

private:
	CString m_sHx;
};

