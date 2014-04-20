// TestLayoutView.h : interface of the CTestLayoutView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <AtlScrl.h>
#include <AtlStr.h>
#include <Gdiplus.h>

#include <set>


class Frame;

class ATL_NO_VTABLE FrameHost
{
public:
	//Event Sink
	virtual void Attach(Frame *pFrame) = 0;
	virtual void Detach(Frame *pFrame) = 0;
	

	// Events that are more often frame to host
	virtual void OnNavigate(Frame *pFrame) = 0;
	virtual void OnSetCursor(HCURSOR hCur) = 0;
	virtual void OnInvalidate(const Gdiplus::RectF &rectF) = 0;
	virtual void OnCommand(int nCommand) = 0;
};

class Frame
{
public:

	Gdiplus::RectF m_rect;

	Frame() : m_rect(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	virtual ~Frame()
	{
	}

	virtual void Attach(FrameHost *pHost) 
	{
		pHost->Attach(this);
	};

	virtual void Detach(FrameHost *pHost) 
	{
		pHost->Detach(this);
	};

	virtual void SetPosition(Gdiplus::Graphics &g, const Gdiplus::RectF &rectIn) {};

	// Events that are normally host to frames
	virtual void OnMouseLeave(FrameHost *pHost) {};
	virtual void OnMouseMove(FrameHost *pHost, const Gdiplus::PointF& pt) {};
	virtual void OnMouseClick(FrameHost *pHost, const Gdiplus::PointF& pt) {};
	virtual void OnRender(Gdiplus::Graphics &g) {};
};


class FrameStyle
{
public:

	FrameStyle()
	{
		CWindowDC dc(NULL);

		LOGFONT lf;
		::GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		m_pFontLink = new Gdiplus::Font(dc, &lf);

		lf.lfUnderline = TRUE;
		m_pFontLinkHover = new Gdiplus::Font(dc, &lf);

		lf.lfUnderline = FALSE;
		lf.lfHeight += -2;
		m_pFontHeader = new Gdiplus::Font(dc, &lf);
		m_hCursorLink = ::CreateCursor(ATL::_AtlBaseModule.GetModuleInstance(), _AtlHyperLink_CursorData.xHotSpot, _AtlHyperLink_CursorData.yHotSpot, _AtlHyperLink_CursorData.cxWidth, _AtlHyperLink_CursorData.cyHeight, _AtlHyperLink_CursorData.arrANDPlane, _AtlHyperLink_CursorData.arrXORPlane);
		ATLASSERT(m_hCursorLink != NULL);
	}

	virtual ~FrameStyle()
	{
		// It was created, not loaded, so we have to destroy it
		if(m_hCursorLink != NULL)
			::DestroyCursor(m_hCursorLink);
	}

	Gdiplus::Font *m_pFontHeader;
	Gdiplus::Font *m_pFontLink;
	Gdiplus::Font *m_pFontLinkHover;
	HCURSOR m_hCursorLink;
};

__declspec(selectany) FrameStyle *pStyle = 0;

template<class T>
class FrameTextT : public Frame
{
protected:

	CStringW m_str;
	Gdiplus::SizeF m_sizeTextMax;

public:
	FrameTextT(LPCWSTR sz = L"") : m_str(sz)
	{
		if (pStyle == 0)
		{
			pStyle = new FrameStyle;
		}		

		m_sizeTextMax.Width = 0.0f;
		m_sizeTextMax.Height = 0.0f;
	}

	void SetText(LPCWSTR sz)
	{
		m_str = sz;
	}

	enum { nPadding = 4 };

	void SetPosition(Gdiplus::Graphics &g, const Gdiplus::RectF &rectIn)
	{
		T *pThis = static_cast<T*>(this);

		if (m_sizeTextMax.Width == 0.0f)
		{
			Gdiplus::RectF rectOut, rectMax(rectIn.X, rectIn.Y, 10000.0f, 10000.0f);
			g.MeasureString(m_str, m_str.GetLength(), pThis->GetFont(), rectIn, &(pThis->m_stringformat), &rectOut);

			m_rect.Width = m_sizeTextMax.Width = rectOut.Width + nPadding * 2;
			m_rect.Height = m_sizeTextMax.Height = rectOut.Height;
		}

		if (m_sizeTextMax.Width < rectIn.Width)
		{
			m_rect.X = rectIn.X;
			m_rect.Y = rectIn.Y;
			m_rect.Width = m_sizeTextMax.Width;
			m_rect.Height = m_sizeTextMax.Height;
		}
		else if (m_rect.Width != rectIn.Width)
		{
			g.MeasureString(m_str, m_str.GetLength(), pThis->GetFont(), rectIn, &(pThis->m_stringformat), &(pThis->m_rect));
		}
	}

	void OnRender(Gdiplus::Graphics &g)
	{
		T *pThis = static_cast<T*>(this);
		g.DrawString(m_str, m_str.GetLength(), pThis->GetFont(), m_rect, &(pThis->m_stringformat), &(pThis->m_brush));
	}
};

class FrameText : public FrameTextT<FrameText>
{
public:
	Gdiplus::StringFormat m_stringformat;
	Gdiplus::SolidBrush m_brush;	

	FrameText() : m_brush(Gdiplus::Color(255,0,0,0))
	{
	}

	Gdiplus::Font *GetFont()
	{
		return pStyle->m_pFontLink;
	}

	
};

class FrameLink : public FrameTextT<FrameLink>
{
protected:

	bool m_bHover;

public:

	Gdiplus::StringFormat m_stringformat;
	Gdiplus::SolidBrush m_brush;	


	FrameLink(LPCWSTR sz = L"") : FrameTextT<FrameLink>(sz), m_brush(Gdiplus::Color(255,0,0,0))
	{
		m_bHover = false;
	}

	Gdiplus::Font *GetFont()
	{
		return m_bHover ? pStyle->m_pFontLinkHover : pStyle->m_pFontLink;
	}

	void SetHover(FrameHost *pHost, bool bHover)
	{
		if (m_bHover != bHover)
		{
			pHost->OnInvalidate(m_rect);
			m_bHover = bHover;
		}
	}

	void OnMouseLeave(FrameHost *pHost)
	{
		SetHover(pHost, false);
	}

	void OnMouseMove(FrameHost *pHost, const Gdiplus::PointF& pt)
	{
		SetHover(pHost, m_rect.Contains(pt) != 0);
		if (m_bHover) pHost->OnSetCursor(pStyle->m_hCursorLink);
	}	

};

template<class TSubFrame>
class FrameBullet : public Frame
{
protected:
	TSubFrame m_subframe;
	HIMAGELIST m_hImageList;
	int m_nImage;

public:

	void SetBullet(int nImage, LPCWSTR sz)
	{
		m_nImage = nImage;
		m_subframe.SetText(sz);
	}

	FrameBullet() : m_nImage(0), m_hImageList(0)
	{
	}

	~FrameBullet()
	{
	}

	enum { cxImage = 24, cyImage = 24, padding = 2 };

	void SetPosition(Gdiplus::Graphics &g, const Gdiplus::RectF &rectIn)
	{
		Gdiplus::RectF r = m_rect = rectIn;
		r.X += cxImage + padding;
		r.Width -= cxImage + padding;		
		m_subframe.SetPosition(g, r);
		m_rect.Height = max(m_subframe.m_rect.Height, cyImage);

		if (m_subframe.m_rect.Height < cyImage)
		{
			m_subframe.m_rect.Y += (cyImage - m_subframe.m_rect.Height) / 2.0f;
		}
	}

	void OnRender(Gdiplus::Graphics &g)
	{
		HDC hdc = g.GetHDC();

		ImageList_Draw(
			m_hImageList, 
			m_nImage, 
			hdc, 
			static_cast<int>(m_rect.X), 
			static_cast<int>(m_rect.Y), 
			ILD_TRANSPARENT);

		g.ReleaseHDC(hdc);
	}

	void Attach(FrameHost *pHost) 
	{
		pHost->Attach(this);
		m_subframe.Attach(pHost);
	};

	void Detach(FrameHost *pHost) 
	{
		pHost->Detach(this);
		m_subframe.Detach(pHost);
	};
};



class FrameTitle : public FrameTextT<FrameTitle>
{
protected:

	bool m_bHover;

public:

	Gdiplus::StringFormat m_stringformat;
	Gdiplus::SolidBrush m_brush;	


	FrameTitle(LPCWSTR sz = L"") : FrameTextT<FrameTitle>(sz), m_brush(Gdiplus::Color(255, 51, 102, 104)), m_stringformat(Gdiplus::StringFormatFlagsNoWrap)
	{		
	}

	~FrameTitle()
	{
	}

	Gdiplus::Font *GetFont()
	{
		return pStyle->m_pFontHeader;
	}
};



class FrameArray : public Frame
{
public:	

	CSimpleValArray<Frame*> m_frames;

	FrameArray()
	{
	}

	~FrameArray()
	{
		RemoveAll();		
	}

	void RemoveAll()
	{
		for(int i = 0; i < m_frames.GetSize(); i++)
		{
			delete m_frames[i];
		}

		m_frames.RemoveAll();
	}

	int GetSize() const
	{
		return m_frames.GetSize();
	}

	void AddFrame(Frame* pFrame)
	{
		m_frames.Add(pFrame);
	}

	Frame* operator[] (int nIndex)
	{
		return m_frames[nIndex];
	}

	Frame* GetFrame(int nIndex)
	{
		return m_frames[nIndex];
	}

	enum { ePad = 2 };

	void SetPosition(Gdiplus::Graphics &g, const Gdiplus::RectF &rectIn)
	{
		Gdiplus::RectF r = m_rect = rectIn;

		for(int i = 0; i < m_frames.GetSize(); i++)
		{
			Frame *pFrame = m_frames[i];
			pFrame->SetPosition(g, r);
			r.Y += pFrame->m_rect.Height + ePad;
		} 

		m_rect.Height = r.Y - m_rect.Y;
	}

	void Attach(FrameHost *pHost) 
	{
		for(int i = 0; i < m_frames.GetSize(); i++)
		{
			m_frames[i]->Attach(pHost);
		}
	};

	void Detach(FrameHost *pHost) 
	{
		for(int i = 0; i < m_frames.GetSize(); i++)
		{
			m_frames[i]->Detach(pHost);
		}
	};
};

template<class T>
class FrameSet : public Frame
{
public:

	template<class T>
	void SetPositionHelper(Gdiplus::Graphics &g, T &frame, Gdiplus::RectF &r)
	{
		frame.SetPosition(g, r);
		r.Y += frame.m_rect.Height;
		r.Height -= frame.m_rect.Height;
		m_rect.Height += frame.m_rect.Height;
	}

	void SetPosition(Gdiplus::Graphics &g, const Gdiplus::RectF &rectIn)
	{
		T *pThis = static_cast<T*>(this);

		Gdiplus::RectF r = m_rect = rectIn;	
		m_rect.Height = 0;

		__if_exists(T::FrameType1::SetPosition) { SetPositionHelper(g, pThis->m_frame1, r); };
		__if_exists(T::FrameType2::SetPosition) { SetPositionHelper(g, pThis->m_frame2, r); };
		__if_exists(T::FrameType3::SetPosition) { SetPositionHelper(g, pThis->m_frame3, r); };
		__if_exists(T::FrameType4::SetPosition) { SetPositionHelper(g, pThis->m_frame4, r); };
		__if_exists(T::FrameType5::SetPosition) { SetPositionHelper(g, pThis->m_frame5, r); };
	}

	void Attach(FrameHost *pHost) 
	{
		T *pThis = static_cast<T*>(this);	

		__if_exists(T::FrameType1::Attach) { pThis->m_frame1.Attach(pHost); };
		__if_exists(T::FrameType2::Attach) { pThis->m_frame2.Attach(pHost); };
		__if_exists(T::FrameType3::Attach) { pThis->m_frame3.Attach(pHost); };
		__if_exists(T::FrameType4::Attach) { pThis->m_frame4.Attach(pHost); };
		__if_exists(T::FrameType5::Attach) { pThis->m_frame5.Attach(pHost); };
	};

	void Detach(FrameHost *pHost) 
	{
		T *pThis = static_cast<T*>(this);

		__if_exists(T::FrameType1::Detach) { pThis->m_frame1.Detach(pHost); };
		__if_exists(T::FrameType2::Detach) { pThis->m_frame2.Detach(pHost); };
		__if_exists(T::FrameType3::Detach) { pThis->m_frame3.Detach(pHost); };
		__if_exists(T::FrameType4::Detach) { pThis->m_frame4.Detach(pHost); };
		__if_exists(T::FrameType5::Detach) { pThis->m_frame5.Detach(pHost); };
	};
};


class FrameGroup : public FrameSet<FrameGroup>
{
protected:
    
public:

	typedef FrameTitle FrameType1;
	typedef FrameArray FrameType2;

	FrameType1 m_frame1;
	FrameType2 m_frame2;

	void SetText(LPCWSTR sz)
	{
		m_frame1.SetText(sz);
	}

	void RemoveAll()
	{
		m_frame2.RemoveAll();
	}

	void AddFrame(Frame *pFrame)
	{
		m_frame2.AddFrame(pFrame);
	}

	Frame* operator[] (int nIndex)
	{
		return m_frame2[nIndex];
	}

	enum { ePad = 4 };

	void SetPosition(Gdiplus::Graphics &g, const Gdiplus::RectF &rectIn)
	{
		Gdiplus::RectF r = rectIn;		
		r.Inflate(-ePad, -ePad);
		m_frame1.SetPosition(g, r);

		r.Y += m_frame1.m_rect.Height + ePad;
		r.Height -= m_frame1.m_rect.Height + ePad;

		m_frame2.SetPosition(g, r);

		m_rect = rectIn;
		m_rect.Height = m_frame1.m_rect.Height + m_frame2.m_rect.Height + (ePad * 3);
	}

	void Attach(FrameHost *pHost) 
	{
		FrameSet<FrameGroup>::Attach(pHost);
		pHost->Attach(this);
	};

	void Detach(FrameHost *pHost) 
	{
		FrameSet<FrameGroup>::Detach(pHost);
		pHost->Detach(this);
	};

	void OnRender(Gdiplus::Graphics &g)
	{
		Gdiplus::RectF r = m_rect;
		r.Inflate(-1, -1);

		Gdiplus::LinearGradientBrush linGrBrush(
			Gdiplus::PointF(r.X, r.Y),
			Gdiplus::PointF(r.GetRight(), r.GetBottom()),
			Gdiplus::Color(255, 204, 221, 255),
			Gdiplus::Color(255, 240, 240, 240));

		//Gdiplus::Pen pen(Gdiplus::Color(255, 51, 102, 104));
		Gdiplus::Pen pen(Gdiplus::Color(255, 204, 221, 255));

		g.FillRectangle(&linGrBrush, r);
		g.DrawRectangle(&pen, r);
	}
};

class FrameCommand : public FrameBullet<FrameLink>
{
public:

	Frame *m_pFrameNav;

	FrameCommand(int nImage, LPCWSTR sz, Frame *pFrameNav = 0) : m_pFrameNav(pFrameNav)
	{
		m_nImage = nImage;
		m_subframe.SetText(sz);
	}

	void OnMouseClick(FrameHost *pHost, const Gdiplus::PointF& pt)
	{
		if (m_rect.Contains(pt))
		{
			if (m_pFrameNav)
			{
				pHost->OnNavigate(m_pFrameNav);
			}
		}
	}
};