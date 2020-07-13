/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* �ļ�����: TCPKSAnalyzeInXml.cpp
* �ļ�ժҪ: ��������XML
*
* ��ǰ�汾:  
*
* ��ʷ�汾: 1.0��˰���з��飬2013-12-17
*			a) ������ʵ��
*/

#include "StdAfx.h"
#include "AnalyzeInXml.h"

/*
* ��������: GetFixItemFromInXml
* ��������: �ӽӿڴ���XML����ȡ����XML�̶�����Ϣ
* ��    ��: 
*			strInXmlData	- ��������
*			sInFixItemInfo	- ������������Ϣ
* �� �� ֵ: 
*/
CString GetIDFromInXml(IN const LPSTR strInXmlData)
{
	CString strReturn="";
	CMarkup inXmlOperate;
	inXmlOperate.SetDoc(strInXmlData);
	CString strTtempErr = inXmlOperate.GetError();
	CString strTtempResult = inXmlOperate.GetResult();
	inXmlOperate.ResetMainPos();
	if (inXmlOperate.FindElem("business"))
	{
		//�õ�������
		strReturn = inXmlOperate.GetAttrib("id");
	}
	return strReturn;
}
