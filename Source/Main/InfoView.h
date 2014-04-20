// ErgoActiveView.h : interface of the CInfoView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// TestLayoutView.h : interface of the CTestLayoutView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once



class CInfoView : 
	public CWindowImpl< CInfoView >,
	public Frames::Host
{
public:
	DECLARE_WND_CLASS(NULL)

	Gdiplus::SolidBrush m_brush;	
	Layout m_layout;


	CInfoView() : m_brush(Gdiplus::Color(NavForeGround))
	{
		m_layout.Parse("ergoactive.xml");
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	// Events that are more often frame to host
	void OnInvalidate(const Gdiplus::RectF &rectF)
	{
		RECT rc = { 
			static_cast<int>(rectF.X), 
				static_cast<int>(rectF.Y), 
				static_cast<int>(rectF.X + rectF.Width + 1.0f), 
				static_cast<int>(rectF.Y + rectF.Height + 1.0f) };

			::InvalidateRect(m_hWnd, &rc, FALSE);
	}

	void OnSetCursor(HCURSOR hCur)
	{
		::SetCursor(hCur);
	}

	void OnCommand(int nCommand)
	{
		HWND hWndParent = m_hWnd;
		HWND hWndTmp;
		while((hWndTmp = ::GetParent(hWndParent)) != NULL)
			hWndParent = hWndTmp;

		//::PostMessage(hWndParent, WM_COMMAND, nCommand, 0);
	}


	BEGIN_MSG_MAP(CInfoView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Resize();
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int cx = GET_X_LPARAM(lParam);
		int cy = GET_Y_LPARAM(lParam);

		Resize();

		return 1;
	}

	void Resize()
	{
		CClientDC dc(m_hWnd);
		Gdiplus::Graphics graphics(dc);		

		Invalidate();
	}



	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);
		Gdiplus::Graphics graphics(dc);

		CRect rect;
		GetClientRect(rect);

		Gdiplus::Bitmap bitmap(rect.Width(), rect.Height());
		OnPaint(Gdiplus::Graphics(&bitmap), Gdiplus::RectF(rect.left, rect.top, rect.Width(), rect.Height()));
		graphics.DrawImage(&bitmap, rect.left, rect.top, rect.right, rect.bottom);

		return 0;
	}

	void OnPaint(Gdiplus::Graphics &graphics, const Gdiplus::RectF &rect)
	{	
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

		graphics.Clear(Gdiplus::Color(NavBackGround));

		// Draw all the frames
		m_layout.m_navigation.OnRenderBefore(graphics, rect);


		// Draw all the frames
		m_layout.m_navigation.OnRenderAfter(graphics, rect);


	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	bool m_bHover;

	BOOL StartTrackMouseLeave()
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		return _TrackMouseEvent(&tme);
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_bHover = false;
		m_layout.m_navigation.OnMouseLeave(this);
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		Gdiplus::PointF pt( static_cast<Gdiplus::REAL>(GET_X_LPARAM(lParam)), static_cast<Gdiplus::REAL>(GET_Y_LPARAM(lParam)) );
		m_layout.m_navigation.OnMouseMove(this, pt);

		if (!m_bHover)
		{
			m_bHover = true;
		}

		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		Gdiplus::PointF pt( static_cast<Gdiplus::REAL>(GET_X_LPARAM(lParam)), static_cast<Gdiplus::REAL>(GET_Y_LPARAM(lParam)) );
		m_layout.m_navigation.OnMouseClick(this, pt);		
		return 0;
	}
};




