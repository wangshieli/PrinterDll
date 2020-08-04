#include "../../pch.h"
#include "Hzxxbdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"

#define LINEFEED_P (22+4) //����������ʶ ����
#define LINEFEED_L (16) //����������ʶ ����

#define MC_W	408
#define SL_W	306
#define DJ_W	204
#define JE_W	204
#define SLV_W	204
#define SE_W	204

CHzxxbdy::CHzxxbdy() :m_nPageSize(LINEFEED_P)
{
	m_sGfmc = "";
	m_sGfsh = "";
	m_sXfmc = "";
	m_sXfsh = "";
	m_sFpdm = "";
	m_sFphm = "";
	m_sHjje = "";
	m_sHjse = "";
	m_sSqf = "";// 0����  1����
	m_sSqyy = ""; // 0�ѵֿ�  1δ�ֿ�
	m_sSqdbh = "";
}

CHzxxbdy::~CHzxxbdy()
{
}

CString CHzxxbdy::Dlfpdy(LPCTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	HZXXB_BBXX bmxx;
	CString printXml("");

	int rtn = 0;
	CString sCode = "", sMsg = "";

	xml.SetDoc(sInputInfo);
	if (xml.FindElem("business"))
	{
		//�õ�������
		CString id = xml.GetAttrib("id");
		if (0 == id.Mid(0, 1).Compare("3"))
			m_bStatus = TRUE;
		else
			m_bStatus = FALSE;
	}
	xml.IntoElem();
	xml.FindElem("body");
	if (xml.GetAttrib("yylxdm").CompareNoCase("1") != 0 && xml.GetAttrib("yylxdm").CompareNoCase("2") != 0)
	{
		fpdy.sErrorCode.Format("%d", -3);
		fpdy.sErrorInfo = "����XML��ʽ��yylxdm����ȷ";
		return GenerateXMLFpdy(fpdy);
	}
	fpdy.iYylxdm = atoi(xml.GetAttrib("yylxdm"));
	xml.IntoElem();
	if (xml.FindElem("returncode"))		sCode = xml.GetData();
	if (xml.FindElem("returnmsg"))		sMsg = xml.GetData();
	if (sCode.Compare("0") != 0)
	{
		fpdy.sErrorCode = sCode;
		fpdy.sErrorInfo = sMsg;
		return GenerateXMLFpdy(fpdy);
	}

	bmxx = ParseFpmxFromXML(sInputInfo, fpdy);

	printXml = GenerateFpdyXml(bmxx, fpdy.sDylx, fpdy);

	rtn = PrintQD(printXml, fpdy.sFplxdm);

	return GenerateXMLFpdy(fpdy, rtn);
}

LONG CHzxxbdy::PrintQD(LPCSTR billxml, CString strFplxdm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	int nXoff = 0;
	int nYoff = 0;

	// ���Ӷ�ȡ�����ļ�����
	CString _sTop = "";
	CString _sLeft = "";
	CString _sQRSize = "";
	CString _sItem = strFplxdm + "_QD";
	ZLib_GetIniYbjValue(_sItem, _sTop, _sLeft, _sQRSize);
	nXoff = atoi(_sLeft) * 10;
	nYoff = atoi(_sTop) * 10;

	do
	{
		CMarkup xml;
		if (!xml.SetDoc(billxml) || !xml.FindElem("Print"))
		{
			nrt = -5;// ��ӡ���ݽ���ʧ��
			break;
		}
		xml.IntoElem();

		nrt = InitPrinter(A4_W, A4_H);
		if (0 != nrt)
			break;

		DOCINFO di = { sizeof(DOCINFO), m_sPrintTaskName.GetBuffer(0), NULL };
		nrt = ::StartDoc(m_hPrinterDC, &di);
		if (nrt <= 0)
		{
			nrt = -3;// ������ӡ����ʧ��
			break;
		}

		while (xml.FindElem("NewPage"))
		{
			nrt = ::StartPage(m_hPrinterDC);
			if (nrt <= 0)
			{
				::EndDoc(m_hPrinterDC);
				nrt = -4;// ���ʹ�ӡ����ʧ��
				break;
			}

			int npn = atoi(xml.GetAttrib("pn"));
			xml.IntoElem();

			CString strTemp;
			CFont *pOldFont;
			CFont fontHeader;

			CRect PaintRect;
			PaintRect.left = 0 + nXoff;
			PaintRect.top = 0 - nYoff;
			PaintRect.right = 2010 + nXoff;
			PaintRect.bottom = -1830 - 900 - nYoff;

			RECT rect;
			rect.left = PaintRect.left;
			rect.top = PaintRect.top;
			rect.right = PaintRect.right;
			rect.bottom = PaintRect.top - 100;

			PaintTile(150, "FixedSys", rect, "���ߺ�����ֵ˰ר�÷�Ʊ��Ϣ��");

			fontHeader.CreatePointFont(120, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			TextOut(m_hPrinterDC, PaintRect.left + 40, PaintRect.top - 100, CString("����ڣ�"), strlen("����ڣ�") + 1);

			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 600, PaintRect.top - 100, CString("��"), 2);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 680, PaintRect.top - 100, CString("ҳ"), 2);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 740, PaintRect.top - 100, CString("��"), 2);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 830, PaintRect.top - 100, CString("ҳ"), 2);
			char buffer[5] = { 0 };
			itoa(npn, buffer, 10);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 780, PaintRect.top - 100, buffer, strlen(buffer));
			itoa(m_nAllPageNum, buffer, 10);
			TextOut(m_hPrinterDC, PaintRect.left + 1005 + 640, PaintRect.top - 100, buffer, strlen(buffer));

			//Rectangle �����ο�
			Rectangle(m_hPrinterDC, PaintRect.left + 38, PaintRect.top - 165, PaintRect.right - 38, PaintRect.bottom);// �ײ�����150
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject(); // �����ӡ���

			fontHeader.CreatePointFont(95, "FixedSys", CDC::FromHandle(m_hPrinterDC));// ������
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			int nShiting = 38;

#define XFT_H	(70+70+35)	// ͷ��Ŀ��
#define XFT_1_W	(102 * 2)	// ���������� ��
#define XFT_2_W	(102 * 3)	// ������ ���ƿ�
#define XFMC_H (105)		// ���������� ��
#define XFSH_H (XFT_H - XFMC_H)	// ������˰�� ��
#define XFMC_D_W (967 - XFT_1_W - XFT_2_W)	// ������ �������ݿ�
			int fontSize = 95;
			LPCSTR fontType = FH;
			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + XFT_1_W; // 1
			rect.bottom = PaintRect.top - 165 - XFT_H;
			PaintTile(fontSize, fontType, rect, "���۷�");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			nShiting += XFT_1_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + XFT_2_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H;
			PaintTile(fontSize, fontType, rect, "����");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFMC_H;
			rect.right = PaintRect.left + nShiting + XFT_2_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H - XFSH_H;
			PaintTile(fontSize, fontType, rect, "˰��");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			nShiting += XFT_2_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + XFMC_D_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H;
			PaintTile(fontSize, fontType, rect, m_sXfmc);
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFMC_H;
			rect.right = PaintRect.left + nShiting + XFMC_D_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H - XFSH_H;
			PaintTile(fontSize, fontType, rect, m_sXfsh);
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			nShiting += XFMC_D_W;



			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + XFT_1_W; // 1
			rect.bottom = PaintRect.top - 165 - XFT_H;
			PaintTile(fontSize, fontType, rect, "������");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			nShiting += XFT_1_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + XFT_2_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H;
			PaintTile(fontSize, fontType, rect, "����");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFMC_H;
			rect.right = PaintRect.left + nShiting + XFT_2_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H - XFSH_H;
			PaintTile(fontSize, fontType, rect, "˰��");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			nShiting += XFT_2_W;

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + nShiting + XFMC_D_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H;
			PaintTile(fontSize, fontType, rect, m_sGfmc);
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);

			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFMC_H;
			rect.right = PaintRect.left + nShiting + XFMC_D_W; // 1
			rect.bottom = PaintRect.top - 165 - XFMC_H - XFSH_H;
			PaintTile(fontSize, fontType, rect, m_sGfsh);
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			nShiting += XFMC_D_W;

			nShiting = 38;
#define KJNR_H	(1225)
			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFT_H;
			rect.right = PaintRect.left + nShiting + XFT_1_W; // 1
			rect.bottom = PaintRect.top - 165 - XFT_H - KJNR_H;
			PaintTile(fontSize, fontType, rect, "����\r\n����\r\nר��\r\n��Ʊ\r\n����");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, PaintRect.right - nShiting, rect.bottom);// ֱ�ӻ������ұ�
			nShiting += XFT_1_W;
			int nSmPointLeft = rect.right; // ˵��ģ�鿪ʼ�����
			int nSmPointTop = rect.bottom; // ˵��ģ�鿪ʼ���ұ�

			nShiting = 38;
#define SM_H (800)
			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFT_H - KJNR_H;
			rect.right = PaintRect.left + nShiting + XFT_1_W; // 1
			rect.bottom = PaintRect.top - 165 - XFT_H - KJNR_H - SM_H;
			PaintTile(fontSize, fontType, rect, "˵��");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, PaintRect.right - nShiting, rect.bottom);// ֱ�ӻ������ұ�
			nShiting += XFT_1_W;

			rect.left = nSmPointLeft + 50;
			rect.top = nSmPointTop - 135;
			rect.right = rect.left + 1000;
			rect.bottom = rect.top - 100;
			PaintTile(125, fontType, rect, "һ������", ZL);

			rect.left = nSmPointLeft + 50;
			rect.top = rect.bottom;
			rect.right = rect.left + 1000;
			rect.bottom = rect.top - 50;
			PaintTile(80, "FixedSys", rect, "��Ӧ����ר�÷�Ʊ�ֿ���ֵ˰����˰�����", ZL);

			rect.left = nSmPointLeft + 50 + 70;
			rect.top = rect.bottom;
			rect.right = rect.left + 1000;
			rect.bottom = rect.top - 70;
			PaintTile(90, fontType, rect,"1���ѵֿ�", ZL);

			rect.left = nSmPointLeft + 50 + 70;
			rect.top = rect.bottom;
			rect.right = rect.left + 1000;
			rect.bottom = rect.top - 50;
			PaintTile(90, fontType, rect, "2��δ�ֿ�", ZL);

			rect.left = nSmPointLeft + 50;
			rect.top = rect.bottom;
			rect.right = rect.left + 1500;
			rect.bottom = rect.top - 50;
			PaintTile(80, "FixedSys", rect, "��Ӧ����ר�÷�Ʊ�Ĵ��룺________________ ���룺________________", ZL);

			rect.left = nSmPointLeft + 50;
			rect.top = rect.bottom;
			rect.right = rect.left + 1000;
			rect.bottom = rect.top - 100;
			PaintTile(125, fontType, rect, "�������۷�", ZL);

			rect.left = nSmPointLeft + 50;
			rect.top = rect.bottom;
			rect.right = rect.left + 1500;
			rect.bottom = rect.top - 50;
			PaintTile(80, "FixedSys", rect, "��Ӧ����ר�÷�Ʊ�Ĵ��룺________________ ���룺________________", ZL);

			nShiting = 38;
#define SQBH_H (335)
			rect.left = PaintRect.left + nShiting;
			rect.top = PaintRect.top - 165 - XFT_H - KJNR_H - SM_H;
			rect.right = PaintRect.left + nShiting + XFT_1_W; // 1
			rect.bottom = PaintRect.bottom;
			PaintTile(fontSize, fontType, rect, "����ר\r\n�÷�Ʊ\r\n��Ϣ��\r\n���");
			MoveToEx(m_hPrinterDC, rect.right, rect.top, NULL);// �ұ���
			LineTo(m_hPrinterDC, rect.right, rect.bottom);
			MoveToEx(m_hPrinterDC, rect.left, rect.bottom, NULL); // �ײ���
			LineTo(m_hPrinterDC, PaintRect.right - nShiting, rect.bottom);// ֱ�ӻ������ұ�

			//int fontSize = 95;
			//LPCSTR fontType = FH;
			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + XU_W; // 1
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "���");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + XU_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + XU_W, PaintRect.bottom);
			//nShiting += XU_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + MC_W; // 4
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "����");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + MC_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + MC_W, PaintRect.bottom);
			//nShiting += MC_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + JM_W; // 1.5
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "����");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + JM_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + JM_W, PaintRect.bottom);
			//nShiting += JM_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + SH_W;
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "˰��");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + SH_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + SH_W, PaintRect.bottom);
			//nShiting += SH_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + ZJ_W;
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "����֤(��֯����)����");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + ZJ_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + ZJ_W, PaintRect.bottom);
			//nShiting += ZJ_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + DZ_W;
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "��ַ�绰");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + DZ_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + DZ_W, PaintRect.bottom);
			//nShiting += DZ_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + YJ_W;
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "�����м��˻�");
			//MoveToEx(m_hPrinterDC, PaintRect.left + nShiting + YJ_W, PaintRect.top - 165, NULL);
			//LineTo(m_hPrinterDC, PaintRect.left + nShiting + YJ_W, PaintRect.bottom);
			//nShiting += YJ_W;

			//rect.left = PaintRect.left + nShiting;
			//rect.top = PaintRect.top - 165;
			//rect.right = PaintRect.left + nShiting + YJ_W;
			//rect.bottom = PaintRect.top - 165 - 70;
			//PaintTile(fontSize, fontType, rect, "�ʼ���ַ");

			//::SelectObject(m_hPrinterDC, pOldFont);
			//fontHeader.DeleteObject();

			//while (xml.FindElem("Item"))
			//{
			//	RECT itemRect;

			//	int x = atoi(xml.GetAttrib("x"));
			//	int y = atoi(xml.GetAttrib("y"));
			//	int w = atoi(xml.GetAttrib("w"));
			//	int h = atoi(xml.GetAttrib("h"));
			//	int nFontSize = atoi(xml.GetAttrib("s"));
			//	CString strFontName = xml.GetAttrib("f");
			//	int z = atoi(xml.GetAttrib("z"));
			//	CString strText = xml.GetData();

			//	itemRect.left = x + nXoff + 38;
			//	itemRect.top = (-y - 165 - nYoff);
			//	itemRect.right = x + nXoff + 38 + w;
			//	itemRect.bottom = (-y - 165 - h - nYoff);

			//	PaintTile(nFontSize, strFontName, itemRect, strText);
			//	MoveToEx(m_hPrinterDC, itemRect.left, itemRect.bottom, NULL);
			//	LineTo(m_hPrinterDC, itemRect.right, itemRect.bottom);
			//}

			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

HZXXB_BBXX CHzxxbdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	HZXXB_BBXX bbxx;
	bbxx.clear();
	HZXXB_BBXM bbxm;
	int booltrue = false;
	CString sl = "";

	xml.SetDoc(inXml);
	xml.FindElem("business");
	xml.IntoElem();
	xml.FindElem("body");
	xml.IntoElem();
	xml.FindElem("returndata");
	xml.IntoElem();
	xml.FindElem("bbxx");
	xml.IntoElem();
	xml.FindElem("group");
	xml.IntoElem();

	if (xml.FindElem("gfmc")) m_sGfmc = xml.GetData();
	if (xml.FindElem("gfsh")) m_sGfsh = xml.GetData();
	if (xml.FindElem("xfmc")) m_sXfmc = xml.GetData();
	if (xml.FindElem("xfsh")) m_sXfsh = xml.GetData();
	if (xml.FindElem("fpdm")) m_sFpdm = xml.GetData();
	if (xml.FindElem("fphm")) m_sFphm = xml.GetData();
	if (xml.FindElem("hjje")) m_sHjje = xml.GetData();
	if (xml.FindElem("hjse")) m_sHjse = xml.GetData();
	if (xml.FindElem("sqf")) m_sSqf = xml.GetData();
	if (xml.FindElem("sqyy")) m_sSqyy = xml.GetData();
	if (xml.FindElem("sqdbh")) m_sSqdbh = xml.GetData();

	if (xml.FindElem("bbxm"))
	{
		bbxx.st_nBbxmCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < bbxx.st_nBbxmCount; i++)
		{
			xml.FindElem("group");
			bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("mc")) bbxm.st_sMc = xml.GetData();
			if (xml.FindElem("sl"))   bbxm.st_sSl = xml.GetData();
			if (xml.FindElem("dj"))   bbxm.st_sDj = xml.GetData();
			if (xml.FindElem("je"))   bbxm.st_sJe = xml.GetData();
			if (xml.FindElem("slv"))     bbxm.st_sSlv = xml.GetData();
			if (xml.FindElem("se"))   bbxm.st_sSe = xml.GetData();

			bbxx.st_lHzxxbBbxm.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CHzxxbdy::GenerateFpdyXml(HZXXB_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CHzxxbdy::GenerateItemMXXml(HZXXB_BBXX bbxx)
{
	CMarkup xml;
	int i = 0;
	int k = 0;

	int nTmp = 900;
	int nTmpKprq = 35;

	//if (m_nOrientation == DMORIENT_LANDSCAPE)
	//{
	//	m_nPageSize = LINEFEED_L;
	//	nTmp = 195;
	//	nTmpKprq = 20;
	//}

	int nLY = 70;

	LTHZXXB_BBXM::iterator pos;
	for (pos = bbxx.st_lHzxxbBbxm.begin(); pos != bbxx.st_lHzxxbBbxm.end(); pos++)
	{
		int nShiting = 0;

		xywhsf(pos->xmMc, nShiting, nLY + i * nLY, MC_W, 70, LS_9, FS, ZL);
		nShiting += MC_W;

		xywhsf(pos->xmSl, nShiting, nLY + i * nLY, SL_W, 70, LS_9, FS, ZC);
		nShiting += SL_W;

		xywhsf(pos->xmJe, nShiting, nLY + i * nLY, JE_W, 70, LS_9, FS, ZC);
		nShiting += JE_W;

		xywhsf(pos->xmSlv, nShiting, nLY + i * nLY, SLV_W, 70, LS_9, FS, ZC);
		nShiting += SLV_W;

		xywhsf(pos->xmSe, nShiting, nLY + i * nLY, SE_W, 70, LS_9, FS, ZC);
		nShiting += SE_W;

		i++;
		if (i%m_nPageSize == 0)
		{
			i = 0;
		}
		m_nLineNum++;
	}

	m_nAllPageNum = m_nLineNum / m_nPageSize;
	if (0 != m_nLineNum % m_nPageSize)
	{
		m_nAllPageNum++;
	}

	int j = 0;
	int num = 0;
	BOOL bNewPage = TRUE;
	int nNewPageNum = 1;
	for (pos = bbxx.st_lHzxxbBbxm.begin(); pos != bbxx.st_lHzxxbBbxm.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum++);
			xml.IntoElem();
			bNewPage = FALSE;
		}
		addxml(pos->st_sMc, pos->xmMc);
		addxml(pos->st_sSl, pos->xmSl);
		addxml(pos->st_sDj, pos->xmDj);
		addxml(pos->st_sJe, pos->xmJe);
		addxml(pos->st_sSlv, pos->xmSlv);
		addxml(pos->st_sSe, pos->xmSe);

		num++;
		if (num % m_nPageSize == 0)
		{
			// ��һҳ������ҳ��
			xml.OutOfElem();
			bNewPage = TRUE;
		}
		j++;
	}

	// ���һҳ���� ҳ��
	xml.OutOfElem();
	bNewPage = TRUE;

	return xml.GetDoc();
}