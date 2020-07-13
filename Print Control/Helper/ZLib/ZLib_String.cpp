/*
* �ļ�����: ZLib_String.cpp
* �ļ�ժҪ: �ַ�������
* ��ǰ�汾: 1.0��2014-02-22
* ������Ϣ��zjg
*/

#include "../../pch.h"
#include "ZLib.h"

/*
 * ��������: ZLib_MidChar
 * ��������: ��ȡָ�������ַ�����ȥ��ĩβ�Ŀո�
 * ��    ��: 
 *		 	 pcInput    - Դ������
 *		 	 dwPos      - ��ʼλ��
 *           dwLen      - ��ȡ����
 * �� �� ֵ: ��ȡ����ַ���
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


//ȥ���ַ����е�ǰ���ո�ͺ󲹿ո�
CString ZLib_Trim(IN CString source)
{
	CString strResult=source;
	strResult.TrimRight();
	strResult.TrimLeft();
	return strResult;
}