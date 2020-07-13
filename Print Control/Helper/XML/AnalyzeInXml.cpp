/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* 文件名称: TCPKSAnalyzeInXml.cpp
* 文件摘要: 解析输入XML
*
* 当前版本:  
*
* 历史版本: 1.0，税控研发组，2013-12-17
*			a) 创建并实现
*/

#include "StdAfx.h"
#include "AnalyzeInXml.h"

/*
* 函数名称: GetFixItemFromInXml
* 函数功能: 从接口传入XML，获取输入XML固定项信息
* 参    数: 
*			strInXmlData	- 输入数据
*			sInFixItemInfo	- 服务器基本信息
* 返 回 值: 
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
		//得到操作码
		strReturn = inXmlOperate.GetAttrib("id");
	}
	return strReturn;
}
