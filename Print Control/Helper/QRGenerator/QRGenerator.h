


/*
* ��������: QRGeneratorBmp
* ��������: ���ɶ�ά��ͼƬ
* ��    ��: 
*            strIn  - ���������
*            strOut - �����ͼƬ��ַ
* �� �� ֵ: 0�ɹ�
*/
int WINAPI QRGeneratorBmp(LPSTR IN strIn,  LPSTR IN strOut);

/*
* ��������: QRGeneratorBit
* ��������: ���ɶ�ά��������
* ��    ��: 
			 strIn - Ҫ���ܵ�����
*            fout - �����������
			 nSize - ������ݴ�С
* �� �� ֵ: 0�ɹ�
*/
int WINAPI QRGeneratorBit(LPSTR IN strIn, 
													unsigned char* OUT fout, 
													/*BITMAPFILEHEADER* OUT kFileHeader, 
													BITMAPINFOHEADER* OUT kInfoHeader, */
													int OUT *nSize);

/*
* ��������: getSize
* ��������: ��ȡ���������������ڴ�ռ��С
* ��    ��: 
*            strIn  - ��������
*            strOut - ����ڴ��С
* �� �� ֵ: 0�ɹ�
*/
int WINAPI getSize(LPSTR IN strIn,  unsigned int* strOut);

