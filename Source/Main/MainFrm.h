// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define IDC_DELAY_SLIDER 0x1001
#define IDC_DELAY		0x1002

#define REG_KEY_STRING L"Software\\ErgoActiv"




class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
	public CIdleHandler,
	public Frames::Host
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	enum { MaxDelay = 120, MinDelay = 20 };


	CHooks m_hooks;
	bool m_bSetting;
	int m_nDelay;

	Layout m_layout;
	CEdit m_edit;
	CTrackBarCtrl m_slider;
	CUsageData m_usage;
	CTaskBarIcon m_ti;
	bool m_bHover;
	int m_nLastBalloon;
	CBrush m_brushBackGround;
	

	CMainFrame() : 
		m_bSetting(false), 
		m_nDelay(50), 
		m_nLastBalloon(0)
	{
		m_layout.Parse(m_hWnd, "ergoactive.xml");		

		CRegKey reg;
		DWORD err = reg.Open(HKEY_CURRENT_USER, REG_KEY_STRING, KEY_READ);

		if(err == ERROR_SUCCESS)
		{
			DWORD dw;
			if (ERROR_SUCCESS == reg.QueryDWORDValue(L"Delay", dw))
			{
				m_nDelay = dw;
			}
		}
	}

	~CMainFrame()
	{

		CRegKey reg;
		DWORD err = reg.Open(HKEY_CURRENT_USER, REG_KEY_STRING, KEY_WRITE);

		if(err == ERROR_SUCCESS)
		{
			reg.SetDWORDValue(L"Delay", m_nDelay);
		}
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		if (pMsg->message == WM_KEYDOWN)
		{
			int nChar = static_cast<int>(pMsg->wParam);
			int nFlags = static_cast<int>(pMsg->lParam);

			


			switch(nChar)
			{
			case VK_LEFT:
				m_slider.SetPos(max(m_nDelay - 1, MinDelay));
				SetText();
				return TRUE;

			case VK_RIGHT:
				m_slider.SetPos(min(m_nDelay + 1, MaxDelay));
				SetText();
				return TRUE;

			case VK_END:
				m_layout.End(this);
				return TRUE;

			case VK_HOME:
				m_layout.Home(this);
				return TRUE;
			
			case VK_UP:
			case VK_PRIOR:
				m_layout.PreviousPage(this);
				return TRUE;

			case VK_SPACE:
			case VK_DOWN:
			case VK_NEXT:
				m_layout.NextPage(this);
				return TRUE;

			case VK_F2:
				TestPopup();
				return TRUE;
			}
		}

		return FALSE;
	}

	
	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)

		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)	
		COMMAND_HANDLER(IDC_DELAY, EN_CHANGE, OnChange)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(IDC_ST_RESTORE, OnRestore)		
		MESSAGE_HANDLER(g_nHookMsg, OnHook)

		CHAIN_MSG_MAP_MEMBER(m_ti)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		ModifyStyle(WS_THICKFRAME|WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);
		SetWindowPos(NULL, 0, 0, m_layout.m_size.cx, m_layout.m_size.cy, 0);
		CenterWindow();

		// Install the task bar
		m_ti.Install(m_hWnd, 1, IDR_MAINFRAME);
		m_hooks.Create(m_hWnd, g_nHookMsg);

		// remove old menu
		SetMenu(NULL);

		m_edit.Create(m_hWnd, CRect(0,0,30,20), 0, WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT, 0, IDC_DELAY);
		m_slider.Create(m_hWnd, 0, 0, WS_CHILD|WS_VISIBLE, 0, IDC_DELAY_SLIDER);

		m_edit.SetFont(AtlGetDefaultGuiFont());

		m_slider.SetBuddy(m_edit);
		m_slider.SetRange(MinDelay, MaxDelay);
		m_slider.SetPos(m_nDelay);

		SetText();

		SetTimer(1, (1000 * 60) / CUsageData::TimerGap, 0);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		// Show default page
		m_layout.SetCurrentPage(this, L"ergoactiv");
		Gdiplus::ARGB argb = m_layout.GetFillColor();
		m_brushBackGround.CreateSolidBrush(RGB(GetBValue(argb), GetGValue(argb), GetRValue(argb)));

				

		return 0;
	}

	LRESULT OnHook(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		g_use.m_nKeyBoard += 1;
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if( wParam==SIZE_MINIMIZED) ShowWindow(SW_HIDE);
		bHandled = FALSE;
		Invalidate();
		return 0;
	}

	LRESULT OnRestore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_RESTORE);
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);
		Gdiplus::Graphics graphics(dc);

		CRect rect;
		GetClientRect(rect);

		Gdiplus::Bitmap bitmap(rect.Width(), rect.Height());

		{
			const float cxNav = 180.0f;
			Gdiplus::Graphics g(&bitmap);

			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
			g.Clear(m_layout.GetFillColor());

			DrawNavigation(g, Gdiplus::RectF(rect.left, rect.top, cxNav, rect.Height()));
			DrawPage(g, Gdiplus::RectF(rect.left + cxNav, rect.top, rect.Width() - cxNav, rect.Height()));
		}

		graphics.DrawImage(&bitmap, rect.left, rect.top, rect.right, rect.bottom);

		return 0;
	}

	void DrawPage(Gdiplus::Graphics &graphics, Gdiplus::RectF &rect)
	{
		rect.Inflate(-5.0f, -5.0f);

		if (m_layout.m_pCurrentPage != NULL)
		{
			m_layout.m_pCurrentPage->OnRenderBefore(this, graphics, rect);
			m_layout.m_pCurrentPage->OnRenderAfter(graphics);
		}	
	}

	void DrawNavigation(Gdiplus::Graphics &graphics, Gdiplus::RectF &rect)
	{	
		graphics.FillRectangle(&Gdiplus::SolidBrush(m_layout.m_argbBackground), rect);

		// Draw all the frames
		m_layout.m_navigation.OnRenderBefore(this, graphics, rect);		

		//m_layout.m_pCurrentPage->
		//graphics.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::ARGB(0x40000000)), rect);


		// Draw all the frames
		m_layout.m_navigation.OnRenderAfter(graphics);


	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}	

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

	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return (LRESULT)(HBRUSH)m_brushBackGround;
	}

	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		SetText();		
		return 0;
	}

	void SetText()
	{
		m_bSetting = true;

		int cx = m_slider.GetPos();

		TCHAR sz[100];
		_itot(cx, sz, 10);		

		m_edit.SetWindowText(sz);
		m_nDelay = m_slider.GetPos();

		m_bSetting = false;
	}

	LRESULT OnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		if (m_bSetting)
			return 0;

		TCHAR sz[100 + 1];
		m_edit.GetWindowText(sz, 100);
		int nPos = _ttoi(sz);
		m_slider.SetPos(nPos);		

		return 0;
	}

	void Step(CUse &use)
	{
		m_usage.Step(use);
		m_nLastBalloon += 1;	
		use.Reset();
	}

	

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Step(g_use);

		if (CanShowBalloon())
		{
			int nTimeLastBreak = GetLastBreakMins();		

			if (nTimeLastBreak > m_nDelay)
			{
				m_nLastBalloon = 0;

				CString str;
				str.Format(IDS_BREAK, nTimeLastBreak);

				m_ti.ShowBalloon("ErgoActiv",    // Title
					str, // Text
					str, // Tooltip
					15);
			}
		}

		Invalidate();
		return 0;
	}

	int GetLastBreakMins() const
	{
		int nLastBreak = m_usage.GetLastBreak();
		return nLastBreak / CUsageData::TimerGap;	
	}

	bool CanShowBalloon() const
	{
		const int nTimeFromLastBalloon = m_nLastBalloon / CUsageData::TimerGap;	

		// Only show error if last baloon mor than 3 mins ago
		return (nTimeFromLastBalloon > 3) && m_usage.IsActive();
	}

	// Events that are more often frame to host
	void OnInvalidate(const Gdiplus::RectF &rectF)
	{
		RECT rc = { 
			static_cast<int>(rectF.X - 1.0f), 
				static_cast<int>(rectF.Y - 1.0f), 
				static_cast<int>(rectF.X + rectF.Width + 2.0f), 
				static_cast<int>(rectF.Y + rectF.Height + 2.0f) };

			::InvalidateRect(m_hWnd, &rc, FALSE);
	}

	void OnSetCursor(HCURSOR hCur)
	{
		::SetCursor(hCur);
	}

	void OnCommand(Frames::Frame *pSender, LPCWSTR szName)
	{
		ATLTRACE("Command %s\n", szName);
		m_layout.SetCurrentPage(this, szName);
		Invalidate();
	}

	void OnShowPage(Frames::Group *pGroup)
	{
		bool bShowDefault = pGroup->IsControlParent();

		m_edit.ShowWindow(bShowDefault ? SW_SHOW : SW_HIDE);
		m_slider.ShowWindow(bShowDefault ? SW_SHOW : SW_HIDE);

		m_layout.m_navigation.SetFocus(false);
		pGroup->m_pLinkFrame->SetFocus(true);

		Invalidate();
	}

	void OnDrawGraph(Gdiplus::Graphics &g, Gdiplus::RectF &rect)
	{
		USES_CONVERSION;

		rect.Y += 10;
		rect.Height -= 10;

		// Draw copyright

		const float fText = 14.0f;
		Gdiplus::RectF rectText(rect.X, rect.Y + rect.Height - fText + 2.0f, rect.Width, fText);

		Gdiplus::Font         font(L"Arial", 7);
		Gdiplus::SolidBrush   solidBrush(Gdiplus::Color::Black);
		Gdiplus::StringFormat stringFormat;		

		CString str;
		str.Format(L"ErgoActiv 1.0 (Compiled %s) Copyright 2006", __DATE__);
		g.DrawString(str, -1, &font, rectText, &stringFormat, &solidBrush);

		// Move graph
		int nMargin = 50;
		m_slider.MoveWindow(rect.X + nMargin, rect.Y, rect.Width - (nMargin * 2), 20);

		rect.Y += 25;
		rect.Height -= 25 + fText;

		if (rect.Height > 0.0f)
		{
			g.DrawRectangle(&Gdiplus::Pen(m_layout.m_argbText, 1), rect);
			rect.Inflate(-5,-5);
			m_usage.Draw(g, rect, m_layout.m_argbText);
		}	
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CExitDlg dlg;

		switch(dlg.DoModal())
		{
		case IDOK:
			ShowWindow(SW_MINIMIZE);
			break;
		case ID_EXIT:
			DestroyWindow();
			break;
		default:
			// Do noting
			break;
		}

		return 0;
	}

	

	void TestPopup()
	{
		int nTimeLastBreak = GetLastBreakMins();		

		CString str;
		str.Format(IDS_BREAK, nTimeLastBreak);

		m_ti.ShowBalloon("ErgoActiv",    // Title
			str, // Text
			str, // Tooltip
			15);
	}
};

