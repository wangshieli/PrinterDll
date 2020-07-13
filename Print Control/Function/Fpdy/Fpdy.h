#pragma once

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


//���ô�ӡ����/�ֶ�����
#define xywhsf(A,X,Y,W,H,S,F,Z)\
		((A).x)=(X) ;((A).y)= (Y) ;((A).w)= (W); \
		((A).h)=(H) ;((A).s)= (S) ;((A).f)= (F); \
		((A).z)=(Z)  

//����XML�ڲ���
#define addxml(a,b)  \
	xml.AddElem("Item",a);xml.AddAttrib("x",b.x);  \
	xml.AddAttrib("y",b.y);xml.AddAttrib("w",b.w); \
	xml.AddAttrib("h",b.h);xml.AddAttrib("s",b.s); \
	xml.AddAttrib("f",b.f);xml.AddAttrib("z",b.z); \

struct XM //��ӡ���ԣ����ꡢ��ߡ����塢�ֺţ�
{
	int x; //����
	int y;
	int w; //���
	int h;
	int s;//�ֺ�
	CString f;//����
	int z;//���������Ҷ���
};

struct FPDY
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

	FPDY() {
		iYylxdm = 0;
		sErrorCode = _T("0");
		sErrorInfo = _T("�ɹ�");
	}
	~FPDY() {}
};

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
	int InitPrinter(short pwidth, short plength);
	CString GenerateXMLFpdy(FPDY fpdy, int rtn = 0);

protected:
	BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

private:
	void GetQRcodePath();

protected:
	char m_cQRcodePath[MAX_PATH];
	int m_iPldy;
	HDC m_hPrinterDC;
	CPrintDialog *m_pDlg;
 	CString m_sPrinterName;
	CString m_sTempFplxdm;
	CString m_sFpzt;
	CString m_sHx;
	CString m_sPrintTaskName;
	CString m_sPrintTaskNameFlag;
	BOOL m_bStatus;
};