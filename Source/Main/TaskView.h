// TestLayoutView.h : interface of the CTestLayoutView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <IWFrame.h>



class EmailFrame : public FrameSet<EmailFrame>
{
public:
	typedef FrameText FrameType1;
	typedef FrameText FrameType2;
	typedef FrameText FrameType3;
	typedef FrameText FrameType4;
	typedef FrameText FrameType5;

	FrameType1 m_frame1;
	FrameType2 m_frame2;
	FrameType3 m_frame3;
	FrameType4 m_frame4;
	FrameType5 m_frame5;

	EmailFrame()
	{
		m_frame1.SetText(L"Select the Image ");
		m_frame2.SetText(L"Click down on the image and drag the mouse to select the cropping area");
		m_frame3.SetText(L"On the Filter menu click Crop no JPEG Loss");
		m_frame4.SetText(L"Simplified user interface ");
		m_frame5.SetText(L"Play list window where lists of files can be created, arbitrarily ordered and then renamed, printed, galleried or slideshowed");
	}
};

class TaskFrame : public FrameSet<TaskFrame>
{
public:

	typedef FrameGroup FrameCommands;
	typedef FrameGroup FramePlaces;

	typedef FrameCommands FrameType1;
	typedef FrameCommands FrameType2;
	typedef FramePlaces FrameType3;

	FrameType1 m_frame1;
	FrameType2 m_frame2;
	FrameType3 m_frame3;

	FramePlaces &GetOtherPlaces()
	{
		return m_frame3;
	}

	EmailFrame m_email;

	TaskFrame()
	{
		m_frame1.SetText(L"Image Tasks");
		m_frame2.SetText(L"File and Folder Tasks");
		m_frame3.SetText(L"Other Places");

		m_frame1.AddFrame(new FrameCommand(33, L"Filter Images"));

		m_frame2.AddFrame(new FrameCommand(33, L"Delete selected items"));
		m_frame2.AddFrame(new FrameCommand(33, L"Email selected items", &m_email));
	}

	~TaskFrame()
	{
	}	
};


class CTaskView :
	public CWindowImpl< CTaskView >,
	public CScrollImpl< CTaskView >,
	public FrameHost
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}	

	float m_nWidth;
	float m_nHeight;

	TaskFrame m_frames;
	

	Frame *m_pFrame;

	CTaskView()
	{
		m_nWidth = 1;
		m_nHeight = 1;
		m_bHover = false;
		m_pFrame = 0;
		m_pFramePendingNav = 0;
	}

	~CTaskView()
	{
	}

	void OnPostEvent()
	{
		if (m_pFramePendingNav)
		{
			DoNavigate(m_pFramePendingNav);
			m_pFramePendingNav = 0;
		}
	}

	typedef std::set<Frame*> FRAMESET;
	FRAMESET m_setSubFrames;

	

	void Attach(Frame *pFrame)
	{
		m_setSubFrames.insert(pFrame);
	}

	void Detach(Frame *pFrame)
	{
		m_setSubFrames.erase(pFrame);
	}

	Frame *m_pFramePendingNav;

	void OnNavigate(Frame *pFrame)
	{
		m_pFramePendingNav = pFrame;
	}

	void DoNavigate(Frame *pFrame)
	{
		if (m_pFrame)
		{ 
			m_pFrame->Detach(this);
			m_pFrame = 0;
		}

		m_pFrame = pFrame;

		if (m_pFrame) 
		{
			m_pFrame->Attach(this);
			Refresh();
		}
	}

	void OnInvalidate(const Gdiplus::RectF &rectF)
	{
		RECT rc = { 
			static_cast<int>(rectF.X), 
			static_cast<int>(rectF.Y), 
			static_cast<int>(rectF.X + rectF.Width), 
			static_cast<int>(rectF.Y + rectF.Height) };

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

		::PostMessage(hWndParent, WM_COMMAND, nCommand, 0);
	}

	void SetLocations(IW::ShellItemList &locations)
	{
		m_frames.GetOtherPlaces().Detach(this);
		m_frames.GetOtherPlaces().RemoveAll();

		for(int i = locations.GetSize() - 1; i >= 0 ; i--)
		{
			m_frames.GetOtherPlaces().AddFrame(new FrameShellItem(locations[i], ID_GOTO_FIRST + i));
		}

		m_frames.GetOtherPlaces().Attach(this);

		if (m_hWnd)
		{
			Refresh();
			Invalidate();
		}
	}

	BEGIN_MSG_MAP(CTaskView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		CHAIN_MSG_MAP(CScrollImpl<CTaskView>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoNavigate(&m_frames);
		bHandled = false;		
		return 0;
	}

	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_sizeClient.cx = GET_X_LPARAM(lParam);
		m_sizeClient.cy = GET_Y_LPARAM(lParam);

		if (m_pFrame) 
		{
			Refresh();
		}

		bHandled = TRUE;			
		return 1;
	}

	void Refresh()
	{
		CClientDC dc(m_hWnd);
		Gdiplus::Graphics graphics(dc);

		m_pFrame->SetPosition(graphics, Gdiplus::RectF(0.0f, 0.0f, static_cast<Gdiplus::REAL>(m_sizeClient.cx), 10000.0f));

		m_nWidth = static_cast<float>(m_sizeClient.cx);
		m_nHeight = static_cast<float>(m_pFrame->m_rect.Height);

		m_sizeAll.cx = static_cast<int>(m_nWidth);
		m_sizeAll.cy = static_cast<int>(m_nHeight);

		int cxMax = m_sizeAll.cx - m_sizeClient.cx;
		int cyMax = m_sizeAll.cy - m_sizeClient.cy;

		if(m_ptOffset.x > cxMax)
		{
			m_ptOffset.x = (cxMax >= 0) ? cxMax : 0;
		}
		if(m_ptOffset.y > cyMax)
		{
			m_ptOffset.y = (cyMax >= 0) ? cyMax : 0;
		}

		// reset current range to prevent scroll bar problems
		SCROLLINFO si = { sizeof(si), 0 };
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;

		si.nMax = max(m_sizeAll.cx - 1, 0);
		si.nPage = m_sizeClient.cx;
		si.nPos = m_ptOffset.x;
		SetScrollInfo(SB_HORZ, &si, true);

		si.nMax = max(m_sizeAll.cy - 1, 0);
		si.nPage = m_sizeClient.cy;
		si.nPos = m_ptOffset.y;
		SetScrollInfo(SB_VERT, &si, true);

		Invalidate();
	}

	//LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	void DoPaint(CDCHandle dc)
	{
		IW::CRender render;

		if (render.Create(dc))
		{
			render.Fill(GetSysColor(COLOR_WINDOW));

			for(FRAMESET::iterator it = m_setSubFrames.begin(); it != m_setSubFrames.end(); ++it)
			{
				(*it)->OnRender(render.GetGraphics());
			}

			render.Flip();
		}

		OnPostEvent();
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

		for(FRAMESET::iterator it = m_setSubFrames.begin(); it != m_setSubFrames.end(); ++it)
		{
			(*it)->OnMouseLeave(this);
		}

		OnPostEvent();
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		Gdiplus::PointF pt( static_cast<Gdiplus::REAL>(GET_X_LPARAM(lParam)), static_cast<Gdiplus::REAL>(GET_Y_LPARAM(lParam)) );

		for(FRAMESET::iterator it = m_setSubFrames.begin(); it != m_setSubFrames.end(); ++it)
		{
			(*it)->OnMouseMove(this, pt);
		}

		if (!m_bHover)
		{
			m_bHover = true;
			StartTrackMouseLeave();
		}

		OnPostEvent();
		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		Gdiplus::PointF pt( static_cast<Gdiplus::REAL>(GET_X_LPARAM(lParam)), static_cast<Gdiplus::REAL>(GET_Y_LPARAM(lParam)) );
		
		for(FRAMESET::iterator it = m_setSubFrames.begin(); it != m_setSubFrames.end(); ++it)
		{
			(*it)->OnMouseClick(this, pt);
		}

		OnPostEvent();
		return 0;
	}


};

