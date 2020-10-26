#pragma once

#include "Fpdy.h"

typedef	struct _jsfp_fyqdxx {
	CString sSpmc;		// ��Ʒ����
	CString sSpsl;		// ��Ʒ����
	CString sDj;		// ����
	CString sJe;		// ���
	CString sSl;		// ˰��
	CString sSe;		// ˰��
	CString sHsdj;		// ��˰����
	CString sHsje;		// ��˰���

	XM ssSpmc;			// ��Ʒ����
	XM ssSpsl;			// ��Ʒ����
	XM ssDj;			// ����
	XM ssJe;			// ���
	XM ssSl;			// ˰��
	XM ssSe;			// ˰��
	XM ssHsdj;			// ��˰����
	XM ssHsje;			// ��˰���

	XM sequenceNum;		// ���к�

}JSFP_FYQDXX;

typedef struct _jsfp_fyxmxx {
	CString sSpmc;		// ��Ʒ����
	CString sSpsl;		// ��Ʒ����
	CString sDj;		// ����
	CString sJe;		// ���
	CString sSl;		// ˰��
	CString sSe;		// ˰��
	CString sHsdj;		// ��˰����
	CString sHsje;		// ��˰���

	XM ssSpmc;			// ��Ʒ����
	XM ssSpsl;			// ��Ʒ����
	XM ssDj;			// ����
	XM ssJe;			// ���
	XM ssSl;			// ˰��
	XM ssSe;			// ˰��
	XM ssHsdj;			// ��˰����
	XM ssHsje;			// ��˰���
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

typedef struct _jsfp_fpxx { //��Ʊ��ϸ��Ϣ
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

	CString sBzF;   //����Ʊ��ע�����ԭʼ��Ʊ�������
	CString sJshjDx;//��˰�ϼƴ�д
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
	XM jshjDx;//��Ҽ��������½��ƾ�ʰ��Ǫ���ڽǷ�
	XM OX;//������Ʊ��д���ǰ Ȧ�� ����λ��

	XM bzF;//����Ʊ��ע�����ԭʼ��Ʊ�������
	XM bz;

	XM skr;
	XM fhr;
	XM kpr;

	//		XM skm1;//˰�����һ��
	//		XM skm2;//˰����ڶ���
	//		XM skm3;//˰���������
	// 		XM skm4;//˰���������

	JSFP_FYXMJE fyxmje[3];

	_jsfp_fpxx() {
		iFyxmCount = 0;

		kprq.sYear = _T("-");
		kprq.sMouth = _T("-");

		fyxmje[0].sFYXM = _T("������Ŀ");
		fyxmje[1].sFYXM = _T("������Ŀ");
		fyxmje[2].sFYXM = _T("������Ŀ");
		fyxmje[0].sJE = _T("���");
		fyxmje[1].sJE = _T("���");
		fyxmje[2].sJE = _T("���");
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
	CString GenerateItemXmlA(JSFP_FPXX fpmx, FPDY fpdy);	// ���
	CString GenerateItemXmlB(JSFP_FPXX fpmx, FPDY fpdy);	// ��
	CString GenerateItemXmlC(JSFP_FPXX fpmx, FPDY fpdy);	// խ��
	CString GenerateItemXmlD(JSFP_FPXX fpmx, FPDY fpdy);	// խ��
	CString GenerateItemXmlE(JSFP_FPXX fpmx, FPDY fpdy);	// ����
	CString GenerateItemXmlF(JSFP_FPXX fpmx, FPDY fpdy);	// �ܾ���Ʊ�ֿ�
	CString GenerateItemXmlG(JSFP_FPXX fpmx, FPDY fpdy);	// �ܾ���Ʊ��խ��
	CString GenerateItemXmlH(JSFP_FPXX fpmx, FPDY fpdy);	// �ɶ�

private:
	CString m_strFppy;
};

