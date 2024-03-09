//#include "CSplitFrame.h"
//
//#include <afxpriv.h>
//
//BOOL CSplitFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
//{
//	CCtrlView* pview;
//
//	// Assign custom view.
//	pContext->m_pNewViewClass = RUNTIME_CLASS(CCtrlView);
//
//	// Create the view.
//	pview = (CCtrlView*)CreateView(pContext, AFX_IDW_PANE_FIRST);
//	if (pview == NULL)
//		return FALSE;
//
//	// Notify the view.
//	pview->SendMessage(WM_INITIALUPDATE);
//	SetActiveView(pview, FALSE);
//
//	return TRUE;
//}
//
//int CSplitFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
//		return -1;
//	if (!m_TransformTree.Create(IDD_DIALOG2, this))
//	{
//		TRACE0("Failed to create toolbar\n");
//		return -1;      // fail to create
//	}
//	//	if (!m_tree_ctrl.Create(, this))
//	//	{
//	//		TRACE0("Failed to create toolbar\n");
//	//		return -1;      // fail to create
//	//	}
////	EnableDocking(CBRS_ALIGN_ANY);
//	return 0;
//}