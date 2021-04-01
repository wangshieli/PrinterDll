#include "../../pch.h"
#include "QRControl.h"
#include "../../cximage/ximage.h"
#include "../../qrcode/qrencode.h"
#include "CRC.h"
#include "Base64.h"

#ifdef _DEBUG
#pragma comment(lib, "qrcode/qrencode_d.lib")
#else
#pragma comment(lib, "qrcode/qrencode.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "cximage/cximage_d.lib")
#pragma comment(lib, "cximage/jasper_d.lib")
#pragma comment(lib, "cximage/libpsd_d.lib")
#pragma comment(lib, "cximage/libdcr_d.lib")
#pragma comment(lib, "cximage/mng_d.lib")
#pragma comment(lib, "cximage/png_d.lib")
#pragma comment(lib, "cximage/jpeg_d.lib")
#pragma comment(lib, "cximage/tiff_d.lib")
#pragma comment(lib, "cximage/zlib_d.lib")
#else
#pragma comment(lib, "cximage/cximage.lib")
#pragma comment(lib, "cximage/jasper.lib")
#pragma comment(lib, "cximage/libpsd.lib")
#pragma comment(lib, "cximage/libdcr.lib")
#pragma comment(lib, "cximage/mng.lib")
#pragma comment(lib, "cximage/png.lib")
#pragma comment(lib, "cximage/jpeg.lib")
#pragma comment(lib, "cximage/tiff.lib")
#pragma comment(lib, "cximage/zlib.lib")
#endif

#define OUT_FILE_PIXEL_PRESCALER	2											// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension ..bmp文件中每个QRCode像素的像素数，在每个维度上)

#define PIXEL_COLOR_R				0											// Color of bmp pixels
#define PIXEL_COLOR_G				0
#define PIXEL_COLOR_B				0

bool CQRControl::funcc(LPCTSTR src, LPTSTR * des, HDC hdc, int32_t xoffset, int32_t yoffset, int32_t xsize, int32_t ysize, uint32_t dwRop)
{
	TCHAR* pData = new TCHAR[_tcslen(src) + 1 + 4 + 1 + 1];
	memset(pData, 0, _tcslen(src) + 1 + 4 + 1 + 1);
	memcpy_s(pData, _tcslen(src), src, _tcslen(src));
	memcpy_s(pData + _tcslen(src), 1, ",", 1);
	unsigned char crcData[5] = { 0 };
	get_ewm_crc_str(_tcslen(src) + 1, (unsigned char*)pData, crcData);
	memcpy_s(pData + _tcslen(src) + 1, 4, crcData, 4);
	memcpy_s(pData + _tcslen(src) + 1 + 4, 1, ",", 1);

	QRcode* pQRC = NULL;
	int nWidth = 0;
	int nWidthAdjusted = 0;
	int nDataBytes = 0;
	int x = 0;
	int y = 0;
	int l = 0;
	int n = 0;
	unsigned char* pSrcData = NULL;
	unsigned char* pRGBData = NULL;
	unsigned char* pSourceData = NULL;
	unsigned char* pDestData = NULL;

	//pQRC = QRcode_encodeString(pData, 5, QR_ECLEVEL_M, QR_MODE_AN, 1);
	pQRC = QRcode_encodeData(_tcslen(pData) + 1, (const unsigned char*)pData, 5, QR_ECLEVEL_M);
	if (pQRC == NULL)
	{
		delete pData;
		pData = NULL;
		return false;
	}
	delete pData;
	pData = NULL;
	
	nWidth = pQRC->width;
	nWidthAdjusted = nWidth * OUT_FILE_PIXEL_PRESCALER * 3;//3
	if (nWidthAdjusted % 4)
		nWidthAdjusted = (nWidthAdjusted / 4 + 1) * 4;
	nDataBytes = nWidthAdjusted * nWidth * OUT_FILE_PIXEL_PRESCALER;

	BITMAPFILEHEADER kFileHeader;
	kFileHeader.bfType = 0x4d42;  // "BM"
	kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) +
		nDataBytes;
	kFileHeader.bfReserved1 = 0;
	kFileHeader.bfReserved2 = 0;
	kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER kInfoHeader;
	kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	kInfoHeader.biWidth = nWidth * OUT_FILE_PIXEL_PRESCALER;
	kInfoHeader.biHeight = -((int)nWidth * OUT_FILE_PIXEL_PRESCALER);
	kInfoHeader.biPlanes = 1;
	kInfoHeader.biBitCount = 24;
	kInfoHeader.biCompression = BI_RGB;
	kInfoHeader.biSizeImage = 0;
	kInfoHeader.biXPelsPerMeter = 0;
	kInfoHeader.biYPelsPerMeter = 0;
	kInfoHeader.biClrUsed = 0;
	kInfoHeader.biClrImportant = 0;

	// Allocate pixels buffer
	int lccc = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nDataBytes;
	if (!(pSrcData = (unsigned char*)malloc(lccc)))
	{
		QRcode_free(pQRC);
		return false;
	}
	memcpy(pSrcData, &kFileHeader, sizeof(BITMAPFILEHEADER));
	memcpy(pSrcData + sizeof(BITMAPFILEHEADER), &kInfoHeader, sizeof(BITMAPINFOHEADER));
	memset(pSrcData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 0xff, nDataBytes);

	// Convert QrCode bits to bmp pixels
	pRGBData = pSrcData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	pSourceData = pQRC->data;
	for (y = 0; y < nWidth; y++)
	{
		pDestData = pRGBData + nWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
		for (x = 0; x < nWidth; x++)
		{
			if (*pSourceData & 1)
			{
				for (l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
				{
					for (n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
					{
						*(pDestData + n * 3 + nWidthAdjusted * l) = PIXEL_COLOR_B;
						*(pDestData + 1 + n * 3 + nWidthAdjusted * l) = PIXEL_COLOR_G;
						*(pDestData + 2 + n * 3 + nWidthAdjusted * l) = PIXEL_COLOR_R;
					}
				}
			}
			pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
			pSourceData++;
		}
	}

	CxImage im(pSrcData, lccc, CXIMAGE_FORMAT_BMP);
	im.Stretch(hdc, xoffset, yoffset, xsize, ysize, dwRop);

	free(pSrcData);
	QRcode_free(pQRC);

	return true;
}
