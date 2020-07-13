/*
* Copyright (c) 2013, Beijing Watertek Information Technology Co.,Ltd.
* All rights reserved.
*
* 文件名称: TCPKSAnalyzeInXml.h
* 文件摘要: 解析输入XML
*
* 当前版本:  
*
* 历史版本: 1.0，税控研发组，2013-12-17
*			a) 创建并实现
*/

#ifndef _ANALYZEINXML_H_
#define _ANALYZEINXML_H_

/*
* 函数名称: GetLoginInfoFromInXml
* 函数功能: 从接口传入XML，获取登录信息
* 参    数: 
*			strInXmlData	- 输入数据
*			sLoginInfo		- 登录信息
* 返 回 值: 
*			no
*/
CString GetIDFromInXml(IN const LPSTR strInXmlData);

#endif
