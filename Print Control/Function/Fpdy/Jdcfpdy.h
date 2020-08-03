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
	CString sFpdm;				// ��Ʊ����
	CString sFphm;				// ��Ʊ����
	CString sFpzt;				// ��Ʊ״̬
	CString sScbz;				// �ϴ���־
	CString sKprq;				// ��Ʊ����
	CString sJqbh;				// �������
	CString sSkm;				// ˰����
	CString sGhfmc;				// ��������
	CString sSfzhm;				// ���֤����/��֯��������
	CString sGhfsbh;			// ����ʶ���
	CString sScqymc;            // ������ҵ����
	CString sCllx;				// ��������
	CString sCpxh;				// �����ͺ�
	CString sCd;				// ����
	CString sHgzh;				// �ϸ�֤��
	CString sJkzmsh;			// ����֤�����
	CString sSjdh;				// �̼쵥��
	CString sFdjhm;				// ����������
	CString sCjhm;				// ���ܺ���/����ʶ���
	CString sJshj;				// ��˰�ϼ�
	CString sJshjDx;			// ��˰�ϼƴ�д
	CString sXhdwmc;			// ������λ����
	CString sXhdwsbh;			// ������λʶ���
	CString sDh;				// �绰
	CString sZh;				// �ʺ�
	CString sDz;				// ��ַ
	CString sKhyh;				// ��������
	CString sZzssl;				// ��ֵ˰˰�ʻ�������
	CString sZzsse;				// ��ֵ˰˰��
	CString sSwjgdm;			// ˰����ش���
	CString sSwjgmc;			// ˰���������
	CString sBhsj;				// ����˰��
	CString sWspzhm;			// ��˰ƾ֤����
	CString sDw;				// ��λ
	CString sXcrs;				// �޳�����
	CString sLslbs;				// ��˰�ʱ�ʶ
	CString sBz;				// ��ע
	CString sKpr;				// ��Ʊ��
	CString sYfpdm;				// ԭ��Ʊ����
	CString sYfphm;				// ԭ��Ʊ����
	CString sZfrq;				// ��������
	CString sZfr;				// ������
	CString sQmcs;				// ǩ������
	CString sQmz;				// ǩ��ֵ
	CString sYkfsje;			// �ѿ��������
	CString sYqjg;				// ��ǩ���

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

	XM Skm1;// ˰�����һ��
	XM Skm2;// ˰����ڶ���
	XM Skm3;// ˰���������
	XM Skm4;// ˰���������
	XM Skm5;// ˰���������

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

