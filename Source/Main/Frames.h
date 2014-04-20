#pragma once

#include <vector>
#include <map>

inline COLORREF Darken(const COLORREF  c)
{
	return 0xFF000000 | RGB(GetRValue(c) >> 1, GetGValue(c) >> 1, GetBValue(c) >> 1);
}

namespace Frames
{
	class Frame;
	class Group;

	class Style
	{
	public:

		Gdiplus::SolidBrush brush;
		Gdiplus::Font font;		
		float fPadding;

		Style(LPCWSTR szFontName, int nFontSize, bool bFontBold, Gdiplus::ARGB argb) :		
			brush(Gdiplus::Color(argb)),
			font(szFontName, nFontSize, bFontBold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular),
			fPadding(2.0f)
		{
		}
	};
	

	class Host
	{
	public:

		// Events that are more often frame to host
		virtual void OnSetCursor(HCURSOR hCur) = 0;
		virtual void OnInvalidate(const Gdiplus::RectF &rectF) = 0;
		virtual void OnCommand(Frame *pSender, LPCWSTR szName) = 0;
		virtual void OnShowPage(Frames::Group *pGroup) = 0;
		virtual void OnDrawGraph(Gdiplus::Graphics &g, Gdiplus::RectF &rect) = 0;
	};

	class Frame
	{
	public:
		Frame() {};
		virtual ~Frame() {};

		// Events that are normally host to frames
		virtual void OnMouseLeave(Host *pHost) {};
		virtual void OnMouseMove(Host *pHost, const Gdiplus::PointF& pt) {};
		virtual void OnMouseClick(Host *pHost, const Gdiplus::PointF& pt) {};

		virtual void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect) {};
		virtual void OnRenderAfter(Gdiplus::Graphics &g) {};
		virtual void SetFocus(bool bFocus) {};
	};

	class Text : public Frame
	{
	public:

		CStringW str;

		Gdiplus::RectF m_rect;
		Gdiplus::StringFormat stringformat;
		Style &style;

		Text(LPCWSTR sz, Style &s) :		
			str(sz),
			style(s)
		{
		}

		~Text()
		{
		}

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			g.MeasureString(str, str.GetLength(), &style.font, rect, &stringformat, &m_rect);

			m_rect.Height += style.fPadding  * 2.0f;

			m_rect.X = rect.X;
			m_rect.Width = rect.Width;

			rect.Y += m_rect.Height;
			rect.Height -= m_rect.Height;
		}

		void OnRenderAfter(Gdiplus::Graphics &g)
		{
			Gdiplus::RectF rectText(m_rect);
			rectText.Inflate(-style.fPadding, -style.fPadding);

			g.DrawString(str, str.GetLength(), &style.font, rectText, &stringformat, &style.brush);
		}
	};

	class Bullet : public Frame
	{
	public:

		CStringW str;

		Gdiplus::RectF m_rect;
		Gdiplus::RectF m_rectText;

		Gdiplus::StringFormat stringformat;		
		Style &style;
		Gdiplus::Bitmap *pBitmap;

		Bullet(LPCWSTR sz, Style &s, Gdiplus::Bitmap *pb) :		
			str(sz),
			style(s),
			pBitmap(pb)
		{
		}

		~Bullet()
		{
		}		

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			const float fMargin = 20.0f;

			Gdiplus::RectF rectText(rect.X + fMargin, rect.Y, rect.Width - fMargin, rect.Height);
			g.MeasureString(str, str.GetLength(), &style.font, rectText, &stringformat, &m_rectText);
			
			m_rect.X = rect.X;
			m_rect.Y = rect.Y;
			m_rect.Width = rect.Width;
			m_rect.Height = max(m_rectText.Height + 4, pBitmap->GetHeight());

			rect.Y += m_rect.Height + style.fPadding;
			rect.Height -= m_rect.Height + style.fPadding;
		}

		void OnRenderAfter(Gdiplus::Graphics &g)
		{
			g.DrawImage(pBitmap, m_rect.X + style.fPadding + 6, m_rect.Y + 2.0f);
			g.DrawString(str, str.GetLength(), &style.font, m_rectText, &stringformat, &style.brush);
		}
	};

	class Image : public Frame
	{
	public:

		Gdiplus::RectF m_rect;	
		Gdiplus::Bitmap m_bitmap;

		Image(LPCWSTR szImageFileName) : m_bitmap(szImageFileName)
		{
		}

		~Image()
		{
		}

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			int cx = m_bitmap.GetWidth();
			int cy = m_bitmap.GetHeight();

			m_rect.X = rect.X + rect.Width - cx;
			m_rect.Y = rect.Y;
			m_rect.Width = rect.Width;
			m_rect.Height = rect.Height;

			rect.Width -= cx;
		}

		void OnRenderAfter(Gdiplus::Graphics &g)
		{
			int cx = m_bitmap.GetWidth();
			int cy = m_bitmap.GetHeight();

			g.DrawImage(&m_bitmap, m_rect.X, m_rect.Y + ((m_rect.Height - cy) / 2.0f));
		}
	};

	

	class Graph : public Frame
	{
	public:

		Style &style;

		Graph(Style &s) : style(s)
		{
		}

		~Graph()
		{
		}

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			pHost->OnDrawGraph(g, rect);			
		}
	};

	class Hover : public Frame
	{
	public:

		bool m_bHover;
		bool m_bFocus;
		Gdiplus::RectF m_rect;
		CStringW str;

		Hover(LPCWSTR sz) : str(sz), m_bHover(false), m_bFocus(false)
		{
		}

		~Hover()
		{
		}

		static HCURSOR GetCursorLink()
		{
			static HCURSOR hCursorLink = ::CreateCursor(ATL::_AtlBaseModule.GetModuleInstance(), _AtlHyperLink_CursorData.xHotSpot, _AtlHyperLink_CursorData.yHotSpot, _AtlHyperLink_CursorData.cxWidth, _AtlHyperLink_CursorData.cyHeight, _AtlHyperLink_CursorData.arrANDPlane, _AtlHyperLink_CursorData.arrXORPlane);
			return hCursorLink;
		}

		void SetFocus(bool bFocus)
		{
			m_bFocus = bFocus;
		}

		void SetHover(Host *pHost, bool bHover)
		{
			if (m_bHover != bHover)
			{
				pHost->OnInvalidate(m_rect);
				m_bHover = bHover;
			}
		}

		void OnMouseLeave(Host *pHost)
		{
			SetHover(pHost, false);
		}

		void OnMouseMove(Host *pHost, const Gdiplus::PointF& pt)
		{
			SetHover(pHost, m_rect.Contains(pt) != 0);
			if (m_bHover) pHost->OnSetCursor(GetCursorLink());
		}	

		void OnMouseClick(Host *pHost, const Gdiplus::PointF& pt)
		{
			if (m_rect.Contains(pt) != 0)
			{
				pHost->OnCommand(this, str);				
			}
		}	
	};

	class Link : public Hover
	{
	public:
		
		Gdiplus::StringFormat stringformat;
		Style &style;
		Gdiplus::SolidBrush brushHotText;
		Gdiplus::SolidBrush brushBackground;
		Gdiplus::SolidBrush brushFocus;

		Link(LPCWSTR sz, Gdiplus::ARGB argb, Style &s) :		
			Hover(sz),
			style(s),
			brushHotText(Gdiplus::Color(Darken(argb))),
			brushBackground(Gdiplus::Color(argb)),
			brushFocus(Gdiplus::Color(argb))
		{
			stringformat.SetAlignment(Gdiplus::StringAlignmentCenter);
		}

		~Link()
		{
		}

		

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			g.MeasureString(str, str.GetLength(), &style.font, rect, &stringformat, &m_rect);

			m_rect.Height += style.fPadding  * 2.0f;

			m_rect.X = rect.X;
			m_rect.Width = rect.Width;

			if (m_bHover)
			{
				g.FillRectangle(&brushBackground, m_rect);
			}
			else if (m_bFocus)
			{
				g.FillRectangle(&brushFocus, m_rect);
			}

			rect.Y += m_rect.Height;
			rect.Height -= m_rect.Height;
		}

		void OnRenderAfter(Gdiplus::Graphics &g)
		{
			Gdiplus::RectF rectText(m_rect);
			rectText.Inflate(-style.fPadding, -style.fPadding);
			g.DrawString(str, str.GetLength(), &style.font, rectText, &stringformat, m_bHover ? &brushHotText : &style.brush);
		}

		
	};

	class LinkImage : public Hover
	{
	public:

		Gdiplus::Bitmap m_bitmap;
		Gdiplus::SolidBrush brushBackground;

		LinkImage(LPCWSTR sz, LPCWSTR szImage, Gdiplus::ARGB argb) :		
			Hover(sz),
			m_bitmap(szImage),
			brushBackground(Gdiplus::Color(argb))
		{
		}

		~LinkImage()
		{
		}

		

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			m_rect.X = rect.X;
			m_rect.Y = rect.Y;
			m_rect.Height = (2.0f  * 2.0f) + m_bitmap.GetHeight();			
			m_rect.Width = rect.Width;

			if (m_bHover)
			{
				g.FillRectangle(&brushBackground, m_rect);
			}

			rect.Y += m_rect.Height;
			rect.Height -= m_rect.Height;
		}

		void OnRenderAfter(Gdiplus::Graphics &g)
		{
			float x = m_rect.X + ((m_rect.Width - m_bitmap.GetWidth()) / 2.0f);
			float y = m_rect.Y + 2.0f;

			g.DrawImage(&m_bitmap, x, y);
		}
	};

	class Group
	{
	public:

		typedef std::vector<Frame*> FRAMESET;
		FRAMESET m_frames;
		Gdiplus::ARGB m_argb;
		bool m_bControlParent;
		Frames::Frame *m_pLinkFrame;

		Group(Gdiplus::ARGB argb, Frames::Frame *pLinkFrame) :	m_argb(argb), m_pLinkFrame(pLinkFrame), m_bControlParent(false)
		{
		}

		void SetControlParent(bool bControlParent = true)
		{
			m_bControlParent = bControlParent;
		}

		bool IsControlParent() const 
		{ 
			return m_bControlParent; 
		};

		void Add(Frame *pFrame)
		{
			m_frames.push_back(pFrame);
		}

		~Group()
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				delete (*it);
			}
		};

		void OnMouseLeave(Host *pHost)
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				(*it)->OnMouseLeave(pHost);
			}
		};

		void OnMouseMove(Host *pHost, const Gdiplus::PointF& pt)
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				(*it)->OnMouseMove(pHost, pt);
			}
		};

		void OnMouseClick(Host *pHost, const Gdiplus::PointF& pt)
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				(*it)->OnMouseClick(pHost, pt);
			}
		};

		void OnRenderBefore(Host *pHost, Gdiplus::Graphics &g, Gdiplus::RectF &rect)
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				(*it)->OnRenderBefore(pHost, g, rect);
			}
		};

		void OnRenderAfter(Gdiplus::Graphics &g)
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				(*it)->OnRenderAfter(g);
			}
		};

		void SetFocus(bool bFocus)
		{
			for(FRAMESET::iterator it = m_frames.begin(); it != m_frames.end(); ++it)
			{
				(*it)->SetFocus(bFocus);
			}
		}
	};

	
}