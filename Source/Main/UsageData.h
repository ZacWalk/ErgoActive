#pragma once

#include "..\Hooks\Hooks.h"

class CUsageData
{
private:

	int m_nLastBreak;
	int m_nNowBreak;

public:
	enum { MaxMinutes = 240 };
	enum { TimerGap = 6 };
	enum { ThreeMinutes = 3 * TimerGap };
	enum { MaxUses = ThreeMinutes + (MaxMinutes * TimerGap) };

	CUse m_uses[MaxUses];

	CUsageData(void)
	{
		m_nLastBreak = 0;
		m_nNowBreak = -1;
	}

	~CUsageData(void)
	{
	}

	void Step(const CUse &use)
	{
		// Move it all along
		for(int i = (MaxUses - 2); i >= 0 ; i--)
		{
			m_uses[i + 1] = m_uses[i];
		}

		m_nLastBreak += 1;

		if (use.GetUsage() == 0)
		{
			m_nNowBreak += 1;

			if (m_nNowBreak > ThreeMinutes)
			{
				m_nLastBreak = 0;
			}
		}
		else
		{
			m_nNowBreak = -1;		
		}

		m_uses[0] = use;	
	}

	void Draw(Gdiplus::Graphics &graphics, const Gdiplus::RectF &rect, Gdiplus::Color clr) const
	{
		int nWidth = min(rect.Width, MaxUses);
		int nMaxHeight = 0;

		// What is the max val?
		for(int i = 0; i < nWidth; i++)
		{
			int nHeight = m_uses[i].GetUsage();
			if (nMaxHeight < nHeight) nMaxHeight = nHeight;
		}

		bool bFirst = true;

		// Create a Pen object.
		Gdiplus::Pen pen(clr, 1);
		Gdiplus::Point point1, point2;

		// Draw graph
		for(int i = 0; i < nWidth; i++)
		{
			int nHeight = m_uses[i].GetUsage();

			nHeight = ::MulDiv(nHeight, rect.Height, nMaxHeight);
			point2.X = rect.Width + rect.X - i;
			point2.Y = rect.Height + rect.Y - nHeight;

			if (bFirst) 
			{
				point1 = point2;
				bFirst = false;
			}

			graphics.DrawLine(&pen, point1, point2);
			point1 = point2;
		}

		
		CStringW str;
		str.Format(L"Your computer usage\nLast 3 min break was %d mins ago\nGraphing last %d mins", 
			GetLastBreak() / TimerGap, nWidth / TimerGap);

		Gdiplus::Font         font(L"Arial", 7);
		Gdiplus::SolidBrush   solidBrush(clr);
		Gdiplus::StringFormat stringFormat;

		graphics.DrawString(str, -1, &font, rect, &stringFormat, &solidBrush);
	}

	int GetLastBreak() const
	{
		return m_nLastBreak;
	}

	bool IsActive() const
	{
		return m_uses[0].GetUsage() > 0;
	}

	static bool Test()
	{
		CUsageData ud;

		// By default should be 0
		if (ud.GetLastBreak() != 0)
		{
			return false;
		}

		for(int i = 0; i < TimerGap * 30; i++)
		{
			ud.Step(CUse(1,1));
		}

		if (ud.GetLastBreak() != TimerGap * 30)
		{
			return false;
		}

		for(int i = 0; i < TimerGap; i++)
		{
			ud.Step(CUse(0,0));
		}

		if (ud.GetLastBreak() != TimerGap * 31)
		{
			return false;
		}

		for(int i = 0; i < TimerGap * 3; i++)
		{
			ud.Step(CUse(0,0));
		}

		if (ud.GetLastBreak() != 0)
		{
			return false;
		}

		for(int i = 0; i < TimerGap * 30; i++)
		{
			ud.Step(CUse(1,1));
		}

		if (ud.GetLastBreak() != TimerGap * 30)
		{
			return false;
		}

		return true;
	}

};
