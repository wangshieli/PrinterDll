#pragma once
#include "../../Helper/XML/Markup.h"

#define XMLHEAD "<?xml version=\"1.0\" encoding=\"gbk\"?>\r\n"

//��ֵ˰��Ʊ
#define FPWidth  (2200+200)		//���򳤣���λ0.1mm��
#define FPLength  (1390+7)	//���򳤣���λ0.1mm��
#define XMGS  8				//��Ŀ����
#define LX  -40			//��߾ࣨ��λ0.1mm��
#define LY  -40			//�ϱ߾ࣨ��λ0.1mm��
#define LW   0				//��
#define LH   0				//��

//������ֵ˰
#define FPWidthHY 3000//2400  //������ֵ˰��Ʊ
#define FPLengthHY 1780
#define XMGSHY  18 //��Ŀ��/����
#define LXHY  0//10				//��߾�  ��λ0.1mm��0.1���ף�
#define LYHY  -100//30			//�ϱ߾�

#define LS_5    50			// �ֺ�5
#define LS_6    60			//�ֺ�6
#define LS_7    70			//�ֺ�7
#define LS_8    80			//�ֺ�8
#define LS_9    90			//�ֺ�9
#define LS_10   100			//�ֺ�10
#define LS_11   110			//�ֺ�11
#define LS_12   120			//�ֺ�12
#define LS_16   160
#define LS_18	180


#define FS  "����"			//����
#define FT "Courier New"	//T Courier New
#define FH "����"           //����

#define ZL 0				//�����
#define ZR 1				//�Ҷ���
#define ZC 2				//ˮƽ����
#define ZVC 3				//��ֱ����
#define ZVL 4				//��ֱ���������

#define AM_ZC	0	// ˮƽ����
#define AM_VC	1	// ��ֱ����
#define AM_ZL	2	// �������ˮƽ���У���Ҫ���ڻ�������λ
#define AM_VCL	3	// ��ֱ���������
#define AM_VCR	4	// ��ֱ�����Ҷ���

#define AM_VCL_S	5	// ��ֱ���������
#define AM_VCR_S	6	// ��ֱ�����Ҷ���
#define AM_ZC_S		7	// ˮƽ����
#define AM_ZL_EX	8	// ���У������������С

#define AM_ZC_CHEKC 9 // �滭�������ʱ����ͨˮƽ��ֱ���в����Զ���������룬��Ҫ���ڷ�Ʊ��ѯ��ӡ�ͻ�������Ʒ����

#define AM_ZL_L		10
#define AM_ZR_S		11	// ���п��ң�������

#define LINE_STATE_0	0x00000000	// Ĭ���Զ�������
#define LINE_STATE_L	0x00000100  // ���������
#define LINE_STATE_T	0x00000200	// ���������
#define LINE_STATE_R	0x00000300	// ���ұ�����
#define LINE_STATE_B	0x00000400	// ���������
#define LINE_STATE_LB	0x00000500	// �������ߺ��º���
#define LINE_STATE_LR	0x00000600	// ���������������
#define LINE_STATE_LTB	0x00000700	// ����������Ϻ��ߺ�������
#define LINE_STATE_LBR	0x00000800	// ����������º��ߺ�������

#define COIN_Y			0x01000000	// Ǯ�ҷ��� Y
#define COIN_O			0x02000000	// Ǯ�ҷ��� O

#define JSFP_RN			0x03000000	// ��ʽ��Ʊ���ӻ��з�

#define YKFP_LINE_H_MIN	75
#define YKFP_LINE_H_MAX	150

#define A4_W	2100
#define A4_H	2970


//���ô�ӡ����/�ֶ�����
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

//����XML�ڲ���
#define addxml(a,b)  \
{						\
	xml.AddElem("Item",a);xml.AddAttrib("x",b.x);  \
	xml.AddAttrib("y",b.y);xml.AddAttrib("w",b.w); \
	xml.AddAttrib("h",b.h);xml.AddAttrib("s",b.s); \
	xml.AddAttrib("f",b.f);xml.AddAttrib("z",b.z); \
	xml.AddAttrib("fc",b.fc);}					   

struct XM //��ӡ���ԣ����ꡢ��ߡ����塢�ֺţ�
{
	int x; //����
	int y;
	int w; //���
	int h;
	int s;//�ֺ�
	CString f;//����
	int z;//���������Ҷ���
	int fc;//�ֺű�����
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
		sErrorInfo = _T("�ɹ�");
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

	// InitPrinter1(YKFPCXDY), ��ȡָ����Ĭ�ϴ�ӡ��HDC,���ڼ���Ԥ��ҳ��������߶�ʹ��
	int InitPrinter1(short pwidth, short plength);

	// InitPrinter�� �����ӡ����ʼ��
	int InitPrinter(short pwidth, short plength);
	CString GenerateXMLFpdy(FPDY fpdy, int rtn = 0);

protected:
	char* PCLib_ChineseFee(char* dest, size_t destsize, char* src);

protected:
	BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
	void setBuiltInOffset(IN int nType, OUT int &_x, OUT int &_y);

	// �������õ��ַ��������뻻��'\n'
	int DealData1(CString& m_szText, int s, LONG width);

	int DealData(CDC *pDC, CString& m_szText, int s, LONG width);
	int Deal(CFont* fontOld, CFont* fontNew, LPCSTR data, RECT rect, int f, LPCSTR FontType, CDC* pDC, UINT flags, RECT& _trect);

	// PaintTile4 ��Ǯ�ҷ��ţ�������Ǯ�ҷ��Ŵ�ӡ
	void PaintTile4(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	// PaintTile3 ��YKFPCXDY��������ָ���ֺ�������д�ӡ�߶�
	LONG PaintTile3(int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	// PaintTile2�� ������������Ӧ��ӡ����֮����ұ߾ࡢ�иߡ��ֺ�
	LONG PaintTile2(int iType, int FontSize, int FontSizeEC, LPCSTR FontType, RECT rect, LPCSTR data, int z = AM_ZC, RECT _rect = { 0, 0, 0, 0 });

	// PaintTile�� �����Ĵ�ӡ
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
	char m_cQRcodePath[MAX_PATH]; // ��ά��
	int m_iPldy;	// ��ӡ��ʽ0:��ͨ��ӡ (�����ӡ)  1:������ӡ (�������ӡ) 2:ʹ��Ĭ�ϴ�ӡ���������ӡ
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