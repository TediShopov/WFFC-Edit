#include "MFCFrame.h"

#include "MFCTransformView.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CMyFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_TOOL
};
//FRAME CLASS

//frame initialiser
CMyFrame::CMyFrame()
{
	m_selectionID = 999; //an obviously wrong selection ID,  to verify its working
}

void CMyFrame::SetCurrentSelectionID(int ID)
{
	m_selectionID = ID;
}

void CMyFrame::OnUpdatePage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	CString strPage;
	strPage.Format(_T("%d"), m_selectionID);
	pCmdUI->SetText(strPage);
}

//oncretae, called after init but before window is shown.
int CMyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// create a view to occupy the client areCSplitterWnda of the frame //This is where DirectX is rendered
//	if (!m_DirXView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(200, 200, 400, 400), this, AFX_IDW_PANE_FIRST, NULL))
//	{
//		TRACE0("Failed to create view window\n");
//		return -1;
//	}

	m_menu1.LoadMenuW(IDR_MENU1);
	SetMenu(&m_menu1);

	if (!m_toolBar.CreateEx(this, TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CRect rect;
	GetClientRect(&rect);
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_TOOL, SBPS_NORMAL, rect.Width() - 500);//set width of status bar panel

	return 0;
}

int CMyFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	//	RECT clientRect;
	//	this->GetClientRect(&clientRect); // Calculate frame width and height
	//	int frameWidth = clientRect.right - clientRect.left /* + 2 * borderWidth*/;
	//	int frameHeight = clientRect.bottom - clientRect.top /* +  titleBarHeight + 2 * borderWidth*/;
	//
	//	SIZE splitSize;
	//	splitSize.cx = frameWidth / 2.0f;
	//	splitSize.cy = frameHeight;
	//
	//	m_splitterWnd.CreateStatic(this,
	//		1,
	//		2,
	//		WS_CAPTION | WS_VISIBLE | WS_CLIPCHILDREN | WS_CHILD);
	//	return 1;

  // create a view to occupy the client area of the frame //This is where DirectX is rendered
//	if (!m_DirXView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
//	{
//		TRACE0("Failed to create view window\n");
//		return -1;
//	}
	m_splitterWnd.CreateStatic(this, 1, 2);
	bool s = m_splitterWnd.CreateView(0, 0, RUNTIME_CLASS(MFCTransformView), SIZE{ 175,500 }, pContext);
	bool stwo = m_splitterWnd.CreateView(0, 1, RUNTIME_CLASS(CChildRender), SIZE{ 175,500 }, pContext);
	auto tht = (MFCTransformView*)m_splitterWnd.GetPane(0, 0);
	auto thtTwo = (CChildRender*)m_splitterWnd.GetPane(0, 1);

	m_transformTreeView = tht;
	m_DirXView = thtTwo;

	//	m_splitterWnd.CreateView(0, 0, m_DirXView->GetRuntimeClass(), SIZE{ 10,10 }, pContext);
	//	m_splitterWnd.CreateView(0, 1, m_transformControl.GetRuntimeClass(), SIZE{ 10,10 }, pContext);
	return 1;
}

void CMyFrame::OnSize(UINT nType, int cx, int cy)
{
	//	bool d = this->SetWindowPos(
	//		&CWnd::wndNoTopMost,
	//		0,
	//		0,
	//		cx,
	//		cy,
	//		SWP_SHOWWINDOW);
	//m_splitterWnd.MoveWindow(0, 0, cx, cy, 0);
	//	bool suc = this->m_splitterWnd.SetWindowPos(
	//		this,
	//		50,
	//		50,
	//		cx,
	//		cy,
	//		SWP_DRAWFRAME);
	CFrameWnd::OnSize(nType, cx, cy);
	if (m_splitterWnd.GetAccessibleChildCount() > 0)
	{
		this->m_splitterWnd.RecalcLayout();
		if (dx_renderer != nullptr)
		{
			int i;
			int XDim, YDim;
			m_splitterWnd.GetColumnInfo(1, XDim, i);
			m_splitterWnd.GetRowInfo(0, YDim, i);
			dx_renderer->OnWindowSizeChanged(XDim, YDim);

			m_DirXView->GetClientRect(&dx_renderer->m_ScreenDimensions);
			int a = 3;
		}
		//		this->m_splitterWnd.GetPane(0, 1)->
		//			MoveWindow(150, 150, 150, 150);
	}

	//	this->m_splitterWnd.GetPane(0, 1)->SetWindowPos(
	//		&CWnd::wndTopMost,
	//		0,
	//		0,
	//		cx,
	//		cy,
	//		SWP_SHOWWINDOW);
	//	this->m_splitterWnd.GetPane(0, 2)->SetWindowPos(
	//		&CWnd::wndTopMost,
	//		0,
	//		0,
	//		cx,
	//		cy,
	//		SWP_SHOWWINDOW);
}