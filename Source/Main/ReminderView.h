#pragma once

#define IDC_DELAY_SLIDER 0x1001
#define IDC_DELAY		0x1002



class CReminderView : 
	public CWindowImpl<CReminderView>
{
public:
	DECLARE_WND_CLASS(NULL)



	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CReminderView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)	
		COMMAND_HANDLER(IDC_DELAY, EN_CHANGE, OnChange)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)

	END_MSG_MAP()

	CBrush m_brushBackGround;

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

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_edit.Create(m_hWnd, CRect(0,0,30,20), 0, WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT, 0, IDC_DELAY);
		m_slider.Create(m_hWnd, 0, 0, WS_CHILD|WS_VISIBLE, 0, IDC_DELAY_SLIDER);

		m_edit.SetFont(AtlGetDefaultGuiFont());

		m_slider.SetBuddy(m_edit);
		m_slider.SetRange(1, 120);
		m_slider.SetPos(m_nDelay);

		m_brushBackGround.CreateSolidBrush(RGB(GetBValue(NavBackGround), GetGValue(NavBackGround), GetRValue(NavBackGround)));
		SetText();

		SetTimer(1, (1000 * 60) / CUsageData::TimerGap, 0);
		
		return 0;
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

		// Vertical text
		WCHAR sz[] = L"ergo active";

		Gdiplus::FontFamily   fontFamily(L"Arial");
		Gdiplus::Font         font(&fontFamily, 72, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
		Gdiplus::PointF       pointF(rect.Width, rect.Height / 2.0f);	

		Gdiplus::Color clr(NavForeGround);
		Gdiplus::SolidBrush   solidBrush(clr);

		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		stringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsDirectionVertical);

		graphics.DrawString(sz, -1, &font, pointF, &stringFormat, &solidBrush);

		DrawText(graphics, rect);

		

	}

	void DrawText(Gdiplus::Graphics &graphics, const Gdiplus::RectF &rect)
	{
		Gdiplus::RectF rectText(rect.X + 10.0f, rect.Y + 20.0f, rect.Width - 120.0f, rect.Height);

		// Info
		WCHAR szText[] = L"Ergoactive will help computer users stay healthy while using a computer.\n\n"
			L"The navigation pane on the left can be used to view information and tips on how to stay fit while using a computer.\n\n"
			L"Ergoactive also monitors you keyboard activity and will unobtrusively remind you to take a break. Taking regular breaks has been proven to reduce the risk of Carpal Tunnel Syndrome and RSI.\n\n\n"
			L"After how many minutes of computer usage would you like to take a break:";

		Gdiplus::Font         font(L"Arial", 10, Gdiplus::FontStyleBold);
		Gdiplus::SolidBrush   solidBrush(Gdiplus::Color::White);
		Gdiplus::StringFormat stringFormat;

		Gdiplus::RectF	rectTextOut;

		graphics.MeasureString(szText, -1, &font, rectText, &stringFormat, &rectTextOut);
		graphics.DrawString(szText, -1, &font, rectText, &stringFormat, &solidBrush);


		int y = rectTextOut.Height + rectTextOut.Y + 20;		
		m_slider.MoveWindow(60, y, rectTextOut.Width - 100, 20);
		//m_edit.MoveWindow(20, y, 30, 20);

		y += 40;

		Gdiplus::RectF rectGraph(rect.X + 20.0f, y, rect.Width - 160.0f, rect.Height - (y + 20.0f));

		graphics.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(NavForeGround)), rectGraph);
		m_usage.Draw(graphics, rectGraph);

	}

	void Step(CUse &use)
	{
		m_usage.Step(use);
		m_nLastBalloon += 1;	
		use.Reset();
	}

	int m_nLastBalloon;

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
				str.Format("You have not had a 3 minute break for %d or more minutes!", nTimeLastBreak);

				m_ti.ShowBalloon("ErgoActive",    // Title
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

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 1;
	}
};