/*
* 文件名称: ZLib_String.cpp
* 文件摘要: 字符串处理
* 当前版本: 1.0，2014-02-22
* 作者信息：zjg
*/

#include "../../pch.h"
#include "ZLib.h"

/*
 * 函数名称: ZLib_MidChar
 * 函数功能: 截取指定长度字符串并去除末尾的空格
 * 参    数: 
 *		 	 pcInput    - 源缓冲区
 *		 	 dwPos      - 起始位置
 *           dwLen      - 截取长度
 * 返 回 值: 截取后的字符串
 */
CString ZLib_MidChar(IN LPSTR pcInput, IN DWORD dwPos, IN DWORD dwLen)
{
	CString strResult="";
	CString strTempBuf=pcInput;

	if(strTempBuf.GetLength() < dwPos + dwLen) return strResult;
	if(dwLen>0)
	{
		strResult=strTempBuf.Mid(dwPos,dwLen);
	}
	strResult.TrimRight();

	return strResult;
}


//去除字符串中的前补空格和后补空格
CString ZLib_Trim(IN CString source)
{
	CString strResult=source;
	strResult.TrimRight();
	strResult.TrimLeft();
	return strResult;
}