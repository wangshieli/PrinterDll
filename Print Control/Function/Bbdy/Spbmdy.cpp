#include "../../pch.h"
#include "Spbmdy.h"

#include "../../Helper/XML/Markup.h"
#include "../../Helper/Log/TraceLog.h"
#include "../../Helper/ZLib/ZLib.h"

#define LINEFEED_P (22+4) //����������ʶ ����
#define LINEFEED_L (16) //����������ʶ ����

CSpbmdy::CSpbmdy() :m_nPageSize(LINEFEED_P)
{
}

CSpbmdy::~CSpbmdy()
{
}

CString CSpbmdy::Dlfpdy(LPTSTR sInputInfo)
{
	FPDY fpdy;
	CMarkup xml;
	SPBM_BBXX bmxx;
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

LONG CSpbmdy::PrintQD(LPCSTR billxml, CString strFplxdm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nrt = 0;

	int nXoff = 0;
	int nYoff = 0;

	// ��Ӷ�ȡ�����ļ�����
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

		nrt = InitPrinter(FPWidth, FPLength * 2);
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

			PaintTile(150, "FixedSys", rect, "��Ʒ����");

			fontHeader.CreatePointFont(120, "FixedSys", CDC::FromHandle(m_hPrinterDC));
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			TextOut(m_hPrinterDC, PaintRect.left + 40, PaintRect.top - 100, CString("�Ʊ����ڣ�"), strlen("�Ʊ����ڣ�") + 1);

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
			MoveToEx(m_hPrinterDC, PaintRect.left + 38, PaintRect.top - 235, NULL);
			LineTo(m_hPrinterDC, PaintRect.right - 38, PaintRect.top - 235);	// �� --> ����� = 70
			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject(); // �����ӡ���

			fontHeader.CreatePointFont(95, "FixedSys", CDC::FromHandle(m_hPrinterDC));// ������
			pOldFont = (CFont *)(::SelectObject(m_hPrinterDC, fontHeader));

			int fontSize = 95;
			LPCSTR fontType = FH;
			rect.left = PaintRect.left + 38;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 38 + 102; // 1
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "����");
			MoveToEx(m_hPrinterDC, PaintRect.left + 38 + 102, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 38 + 102, PaintRect.bottom);

			rect.left = PaintRect.left + 140;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 140 + 408; // 4
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "˰�ձ���");
			MoveToEx(m_hPrinterDC, PaintRect.left + 140 + 408, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 140 + 408, PaintRect.bottom);

			rect.left = PaintRect.left + 548;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 548 + 153; // 1.5
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "����");
			MoveToEx(m_hPrinterDC, PaintRect.left + 548 + 153, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 548 + 153, PaintRect.bottom);

			rect.left = PaintRect.left + 701;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 701 + 255;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "˰��");
			MoveToEx(m_hPrinterDC, PaintRect.left + 701 + 255, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 701 + 255, PaintRect.bottom);

			rect.left = PaintRect.left + 956;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 956 + 255;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "����ͺ�");
			MoveToEx(m_hPrinterDC, PaintRect.left + 956 + 255, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 956 + 255, PaintRect.bottom);

			rect.left = PaintRect.left + 1211;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 1211 + 255;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "��λ");
			MoveToEx(m_hPrinterDC, PaintRect.left + 1211 + 255, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 1211 + 255, PaintRect.bottom);

			rect.left = PaintRect.left + 1466;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 1466 + 255;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "����");
			MoveToEx(m_hPrinterDC, PaintRect.left + 1466 + 255, PaintRect.top - 165, NULL);
			LineTo(m_hPrinterDC, PaintRect.left + 1466 + 255, PaintRect.bottom);

			rect.left = PaintRect.left + 1721;
			rect.top = PaintRect.top - 165;
			rect.right = PaintRect.left + 1721 + 251;
			rect.bottom = PaintRect.top - 165 - 70;
			PaintTile(fontSize, fontType, rect, "��˰��־");

			::SelectObject(m_hPrinterDC, pOldFont);
			fontHeader.DeleteObject();

			/*while (xml.FindElem("Item"))
			{
				RECT itemRect;

				int x = atoi(xml.GetAttrib("x"));
				int y = atoi(xml.GetAttrib("y"));
				int w = atoi(xml.GetAttrib("w"));
				int h = atoi(xml.GetAttrib("h"));
				int nFontSize = atoi(xml.GetAttrib("s"));
				CString strFontName = xml.GetAttrib("f");
				int z = atoi(xml.GetAttrib("z"));
				CString strText = xml.GetData();

				itemRect.left = x + nXoff + 38;
				itemRect.top = (-y - 165 - nYoff);
				itemRect.right = x + nXoff + 38 + w;
				itemRect.bottom = (-y - 165 - h - nYoff);

				PaintTile(nFontSize, strFontName, itemRect, strText);
				MoveToEx(m_hPrinterDC, itemRect.left, itemRect.bottom, NULL);
				LineTo(m_hPrinterDC, itemRect.right, itemRect.bottom);
			}*/

			::EndPage(m_hPrinterDC);
			xml.OutOfElem();
		}
		xml.OutOfElem();
		::EndDoc(m_hPrinterDC);
	} while (false);

	return nrt;
}

SPBM_BBXX CSpbmdy::ParseFpmxFromXML(LPCTSTR inXml, BBDY bbdy)
{
	CMarkup xml;
	SPBM_BBXX bbxx;
	bbxx.clear();
	SPBM_BBXM bbxm;
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

	if (xml.FindElem("bbxm"))
	{
		bbxx.st_nBbxmCount = atoi(xml.GetAttrib("count"));
		xml.IntoElem();
		for (int i = 0; i < bbxx.st_nBbxmCount; i++)
		{
			xml.FindElem("group");
			bbxm.st_nXh = atoi(xml.GetAttrib("xh"));
			xml.IntoElem();
			if (xml.FindElem("dj")) bbxm.st_sDj = xml.GetData();
			if (xml.FindElem("ggxh"))   bbxm.st_sGgxh = xml.GetData();
			if (xml.FindElem("hsbz"))   bbxm.st_sHsbz = xml.GetData();
			if (xml.FindElem("jldw"))   bbxm.st_sJldw = xml.GetData();
			if (xml.FindElem("jm"))     bbxm.st_sJm = xml.GetData();
			if (xml.FindElem("mc"))   bbxm.st_sMc = xml.GetData();
			if (xml.FindElem("sl"))     bbxm.st_sSl = xml.GetData();
			if (xml.FindElem("spflbm"))     bbxm.st_sSpflbm = xml.GetData();

			bbxx.st_lSpbmBbxm.push_back(bbxm);
		}
		xml.OutOfElem();
	}

	return bbxx;
}

CString CSpbmdy::GenerateFpdyXml(SPBM_BBXX bbxx, CString dylx, BBDY bbdy)
{
	CString xml;
	xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<Print>";

	xml += GenerateItemMXXml(bbxx);

	xml += "</Print>";

	return xml;
}

CString CSpbmdy::GenerateItemMXXml(SPBM_BBXX bbxx)
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

	LTSPBM_BBXM::iterator pos;
	for (pos = bbxx.st_lSpbmBbxm.begin(); pos != bbxx.st_lSpbmBbxm.end(); pos++)
	{
		//������Ʒ���Ƴ���
		int LengOfMc = pos->st_sMc.GetLength();
		if (LengOfMc >= 40)
		{
			xywhsf(pos->xmMc, 102, nLY + i * nLY, 408, 70, LS_6, FS, ZC);
		}
		else if (LengOfMc >= 34 && LengOfMc < 40)
		{
			xywhsf(pos->xmMc, 102, nLY + i * nLY, 408, 70, LS_7, FS, ZC);
		}
		else if (LengOfMc >= 30 && LengOfMc < 34)
		{
			xywhsf(pos->xmMc, 102, nLY + i * nLY, 408, 70, LS_8, FS, ZC);
		}
		else
		{
			xywhsf(pos->xmMc, 102, nLY + i * nLY, 408, 70, LS_9, FS, ZC);
		}

		int LenOfJm = pos->st_sJm.GetLength();
		if (LenOfJm >= 18)
		{
			xywhsf(pos->xmJm, 510, nLY + i * nLY, 153, 70, LS_7, FS, ZC);
		}
		else if (LenOfJm >= 14 && LenOfJm < 18)
		{
			xywhsf(pos->xmJm, 510, nLY + i * nLY, 153, 70, LS_8, FS, ZC);
		}
		else
		{
			xywhsf(pos->xmJm, 510, nLY + i * nLY, 153, 70, LS_9, FS, ZC);
		}

		//xywhsf(pos->xmNsrsbh, 663, nLY + i * nLY, 255, 70, LS_9, FS, ZC);

	/*	int LenOfKzl = pos->st_sKzl.GetLength();
		if (LenOfKzl >= 11 && LenOfKzl <= 13)
		{
			xywhsf(pos->xmKzl, 918, nLY + i * nLY, 255, 70, LS_6, FS, ZC);
		}
		else
		{
			xywhsf(pos->xmKzl, 918, nLY + i * nLY, 255, 70, LS_9, FS, ZC);
		}*/

		//int LengOfDz = pos->st_sDz.GetLength();
		//if (LengOfDz >= 11 || LengOfDz <= 13)
		//{
		//	xywhsf(pos->xmDz, 1173, nLY + i * nLY, 255, 70, LS_8, FS, ZC);
		//}
		//else
		//{
		//	xywhsf(pos->xmDz, 1173, nLY + i * nLY, 255, 70, LS_9, FS, ZC);
		//}

		//int lenOfYhzh = pos->st_sYhzh.GetLength();
		//xywhsf(pos->xmYhzh, 1428, nLY + i * nLY, 255, 70, LS_9, FS, ZC);

		//int lenOfYjdz = pos->st_sYjdz.GetLength();
		//if (lenOfYjdz > 4)
		//{
		//	xywhsf(pos->xmYjdz, 1683, nLY + i * nLY, 251, 70, LS_8, FS, ZC);
		//}
		//else
		//{
		//	xywhsf(pos->xmYjdz, 1683, nLY + i * nLY, 251, 70, LS_9, FS, ZC);
		//}
		//xywhsf(pos->ssSe, LX + 1710 - 170, LY + nLY + i * nLY, LW + 300, LH + 50, LS_9, FS, ZR);

		xywhsf(pos->xmXh, 0, nLY + i * nLY, 102, 70, LS_9, FS, ZL);

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
	for (pos = bbxx.st_lSpbmBbxm.begin(); pos != bbxx.st_lSpbmBbxm.end(); pos++)
	{
		if (bNewPage)
		{
			xml.AddElem("NewPage");
			xml.AddAttrib("pn", nNewPageNum++);
			xml.IntoElem();
			bNewPage = FALSE;
		}
		addxml(pos->st_sDj, pos->xmDj);
		addxml(pos->st_sGgxh, pos->xmGgxh);
		addxml(pos->st_sHsbz, pos->xmHsbz);
		addxml(pos->st_sJldw, pos->xmJldw);
		addxml(pos->st_sJm, pos->xmJm);
		addxml(pos->st_sMc, pos->xmMc);
		addxml(pos->st_sSl, pos->xmSl);
		addxml(pos->st_sSpflbm, pos->xmSpflbm);

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
