#pragma once

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace IW
{

	class GdiplusStartup
	{
		Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
		ULONG_PTR           m_gdiplusToken;


	public:
		GdiplusStartup()
		{
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
		}

		~GdiplusStartup()
		{
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
		}
	};
}