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
	CString sFpdm;				// ��Ʊ����
	CString sFphm;				// ��Ʊ����
	CString sJqbh;				// �������
	CString sSkm;				// ˰����
	CString sJym;				// У����
	CString sFpzt;				// ��Ʊ״̬
	CString sScbz;				// �ϴ���־
	CString sKprq;				// ��Ʊ����		
	CString sGfmc;				// ��������		
	CString sGfsfzhm;			// �������֤��
	CString sGfdz;				// ������ַ
	CString sGfdh;				// �����绰
	CString sMfmc;				// ��������
	CString sMfsfzhm;			// �������֤��
	CString sMfdz;				// ������ַ
	CString sMfdh;				// �����绰
	CString sCpzh;				// �����պ�
	CString sDjzh;				// �Ǽ�֤��
	CString sCllx;				// ��������
	CString sCjhm;				// ���ܺ���/����ʶ���
	CString sCpxh;				// �����ͺ�
	CString sZrdclglsmc;		// ת��س�������������
	CString sCjhj;				// ���ۺϼ�
	CString sCjhjDx;			// ���ۺϼƴ�д
	CString sJypmdw;			// ��Ӫ������λ
	CString sJypmdwdz;			// ��Ӫ������λ��ַ
	CString sJypmdwnsrsbh;		// ��Ӫ������λ��˰��ʶ���
	CString sJypmdwyhzh;		// ���������˺�
	CString sJypmdwdh;			// �绰
	CString sEscsc;				// ���ֳ��г�
	CString sEscscnsrsbh;		// ��˰��ʶ���
	CString sEscscdz;			// ��ַ
	CString sEscscyhzh;			// ���������˺�
	CString sEscscdh;			// �绰
	CString sBz;				// ��ע
	CString sKpr;				// ��Ʊ��

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

	XM Skm1;// ˰�����һ��
	XM Skm2;// ˰����ڶ���
	XM Skm3;// ˰���������
	XM Skm4;// ˰���������
	XM Skm5;// ˰���������

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

