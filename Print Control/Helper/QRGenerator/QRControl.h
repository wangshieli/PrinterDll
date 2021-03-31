#pragma once
class CQRControl
{
public:
	CQRControl() {}
	virtual ~CQRControl() {}

public:
	bool funcc(LPCTSTR src, LPTSTR* des, HDC hdc, int32_t xoffset, int32_t yoffset, int32_t xsize, int32_t ysize, uint32_t dwRop = SRCCOPY);
};

