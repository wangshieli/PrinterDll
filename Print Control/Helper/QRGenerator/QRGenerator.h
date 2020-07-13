


/*
* 函数名称: QRGeneratorBmp
* 函数功能: 生成二维码图片
* 参    数: 
*            strIn  - 输入的数据
*            strOut - 输出的图片地址
* 返 回 值: 0成功
*/
int WINAPI QRGeneratorBmp(LPSTR IN strIn,  LPSTR IN strOut);

/*
* 函数名称: QRGeneratorBit
* 函数功能: 生成二维码数据流
* 参    数: 
			 strIn - 要加密的数据
*            fout - 输出的流数据
			 nSize - 输出数据大小
* 返 回 值: 0成功
*/
int WINAPI QRGeneratorBit(LPSTR IN strIn, 
													unsigned char* OUT fout, 
													/*BITMAPFILEHEADER* OUT kFileHeader, 
													BITMAPINFOHEADER* OUT kInfoHeader, */
													int OUT *nSize);

/*
* 函数名称: getSize
* 函数功能: 获取接收流数据所需内存空间大小
* 参    数: 
*            strIn  - 输入数据
*            strOut - 输出内存大小
* 返 回 值: 0成功
*/
int WINAPI getSize(LPSTR IN strIn,  unsigned int* strOut);

