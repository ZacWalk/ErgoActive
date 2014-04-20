#pragma once 

class CJumpingGirlPath
{
public:

	Gdiplus::GraphicsPath m_path;
	Gdiplus::PointF m_point;
	Gdiplus::Rect m_rectPath;

	CJumpingGirlPath() : m_point(0.0f, 0.0f)
	{
		CurveTo(0.0f, 0.0f, -4.978f, 8.098f, -2.951f, 17.582f);
		CurveTo(0.0f, 0.0f, 0.066f, 8.436f, 4.087f, 15.674f);
		CurveTo(0.0f, 0.0f, 3.764f, 4.734f, 5.241f, -3.178f);
		LineTo(0.841f, -12.129f);
		CurveTo(0.0f, 0.0f, 1.804f, -4.416f, 1.597f, -10.021f);
		LineTo(0.533f, -2.682f);
		CurveTo(6.544f, -9.826f, 9.918f, -29.553f, 9.918f, -29.553f);
		LineTo(5.647f, -6.342f);
		CurveTo(14.944f, -19.723f, 38.791f, -53.361f, 38.791f, -53.361f);
		CurveTo(15.031f, -9.705f, 18.345f, -25.037f, 18.345f, -25.037f);
		CurveTo(37.721f, -29.91f, 37.496f, -109.985f);
		CurveTo(0.0f, 0.0f, 9.241f, -1.536f, 13.728f, -6.294f);
		CurveTo(0.0f, 0.0f, -14.842f, -32.645f, -14.958f, -73.768f);
		LineTo(4.729f, -7.395f);
		CurveTo(0.0f, 0.0f, 6.484f, -5.29f, -2.809f, -20.554f);
		CurveTo(0.0f, 0.0f, -1.794f, -15.548f, -1.831f, -28.464f);
		CurveTo(0.0f, 0.0f, 3.487f, -5.019f, 3.404f, -34.016f);
		LineTo(0.664f, -1.319f);
		CurveTo(0.0f, 0.0f, 10.187f, -22.875f, 1.461f, -43.148f);
		LineTo(-1.668f, -0.435f);
		CurveTo(-1.193f, 0.275f, -1.825f, 2.817f, -1.825f, 2.817f);
		CurveTo(-1.857f, 0.141f, -2.164f, 1.148f, -2.164f, 1.148f);
		CurveTo(-1.288f, 1.365f, -2.857f, 20.656f, -2.857f, 20.656f);
		LineTo(-2.571f, 4.578f);
		CurveTo(-1.271f, -5.584f, -5.688f, -8.068f, -5.688f, -8.068f);
		LineTo(-2.25f, -0.082f);
		CurveTo(-0.996f, 1.058f, 0.257f, 2.195f, 0.257f, 2.195f);
		CurveTo(5.182f, 5.434f, 7.379f, 16.851f, 7.379f, 16.851f);
		LineTo(0.762f, 4.04f);
		CurveTo(0.0f, 0.0f, -6.188f, 21.897f, -6.157f, 32.705f);
		CurveTo(0.0f, 0.0f, -2.222f, 9.496f, -2.192f, 20.831f);
		LineTo(-7.495f, 0.021f);
		LineTo(-2.583f, -2.71f);
		CurveTo(0.0f, 0.0f, -1.239f, -15.996f, -9.744f, -24.916f);
		LineTo(-17.113f, -7.162f);
		CurveTo(-26.507f, -3.529f, -33.675f, 22.401f, -33.675f, 22.401f);
		LineTo(-7.546f, -16.499f);
		CurveTo(0.0f, 0.0f, -3.854f, -7.019f, -12.88f, -16.482f);
		LineTo(-13.732f, -23.862f);
		CurveTo(0.0f, 0.0f, 0.313f, -7.206f, -0.199f, -11.29f);
		CurveTo(0.0f, 0.0f, -0.389f, -4.876f, -0.029f, -10.413f);
		CurveTo(0.0f, 0.0f, -3.683f, 1.526f, -4.226f, 8.579f);
		CurveTo(-4.697f, -10.53f, -9.979f, -21.28f, -9.979f, -21.28f);
		CurveTo(-3.976f, -6.491f, -3.962f, -1.878f);
		LineTo(-0.558f, 1.781f);
		LineTo(-2.695f, -2.826f);
		CurveTo(0.0f, 0.0f, -2.88f, -1.705f, 2.541f, 14.755f);
		LineTo(1.017f, 5.664f);
		CurveTo(-2.257f, -2.365f, -4.264f, -4.601f, -4.264f, -4.601f);
		CurveTo(-2.715f, -2.848f, -1.788f, 0.576f, -1.788f, 0.576f);
		CurveTo(2.841f, 2.979f, 5.778f, 10.792f, 5.778f, 10.792f);
		CurveTo(3.693f, 9.128f, 12.391f, 19.647f, 12.391f, 19.647f);
		CurveTo(12.75f, 30.191f, 17.776f, 39.315f, 17.776f, 39.315f);
		LineTo(5.023f, 8.421f);
		CurveTo(5.379f, 15.802f, 14.092f, 32.296f, 14.092f, 32.296f);
		LineTo(0.042f, 15.113f);
		CurveTo(8.678f, 3.843f, 11.685f, 5.942f, 11.685f, 5.942f);
		CurveTo(6.04f, 14.042f, 4.742f, 26.699f, 4.742f, 26.699f);
		CurveTo(1.716f, 17.92f, 1.475f, 49.906f, 1.475f, 49.906f);
		CurveTo(3.175f, 3.329f, 13.385f, 5.585f, 13.385f, 5.585f);
		CurveTo(-26.329f, 11.684f, -29.211f, 70.734f);
		CurveTo(0.0f, 0.0f, 2.558f, 13.283f, -7.469f, 33.691f);
		LineTo(-12.371f, 8.893f);
		LineTo(-36.916f, 29.98f);
		CurveTo(-10.967f, 11.629f, -2.922f, 27.775f, -2.922f, 27.775f);
		CurveTo(-4.486f, 4.756f, -3.95f, 17.234f, -3.95f, 17.234f);
		CurveTo(0.047f, 16.869f, -4.61f, 20.045f, -4.61f, 20.045f);
		CurveTo(-3.647f, 6.689f, -2.959f, 14.771f, -2.959f, 14.771f);
		CurveTo(1.381f, 16.516f, 11.733f, 23.516f, 11.733f, 23.516f);
		CurveTo(4.022f, 7.891f, 7.662f, -1.781f, 7.662f, -1.781f);
		LineTo(1.297f, -13.006f);
		CurveTo(3.315f, -6.336f, 0.966f, -11.953f, 0.966f, -11.953f);
		CurveTo(4.293f, -14.422f, 7.26f, -26.031f, 7.26f, -26.031f);
		LineTo(7.647f, -7.402f);

		m_path.GetBounds(&m_rectPath, NULL, NULL);		
	}

	void MoveTo(Gdiplus::REAL x, Gdiplus::REAL y)
	{
		m_point.X = x;
		m_point.Y = x;
	}

	void LineTo(Gdiplus::REAL x, Gdiplus::REAL y)
	{
		x += m_point.X; 
		y += m_point.Y;
		m_path.AddLine(m_point.X, m_point.Y, x, y);
		m_point.X = x;
		m_point.Y = y;
	}

	void CurveTo(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x2, Gdiplus::REAL y2, Gdiplus::REAL x3, Gdiplus::REAL y3)
	{
		x1 += m_point.X; 
		y1 += m_point.Y;
		x2 += m_point.X; 
		y2 += m_point.Y;
		x3 += m_point.X; 
		y3 += m_point.Y;

		m_path.AddBezier(m_point.X, m_point.Y, x1, y1, x2, y2, x3, y3);
		m_point.X = x3;
		m_point.Y = y3;
	}

	void CurveTo(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x2, Gdiplus::REAL y2)
	{
		CurveTo(x1, y1, x2, y2, x2, y2);
	}
};