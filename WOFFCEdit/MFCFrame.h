#pragma once
#include <afxwin.h>
#include <afxext.h>
#include "MFCRenderFrame.h"
#include "ToolMain.h"
class MFCTransformView;

class CMyFrame : public CFrameWnd
{
protected:
	//	DECLARE_DYNAMIC(CMainFrame)

public:

	CMenu			m_menu1;	//menu bar
	CStatusBar		m_wndStatusBar;
	CToolBar		m_toolBar;
	CChildRender* m_DirXView;
	MFCTransformView* m_transformTreeView;
	CSplitterWnd	m_splitterWnd;

	Game* dx_renderer = nullptr;

public:
	CMyFrame();
	void SetCurrentSelectionID(int ID);
	afx_msg void OnUpdatePage(CCmdUI* pCmdUI);

private:	//overrides
	int		m_selectionID;	//

	//note the afx_message keyword is linking this method to message map access.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;
	// So put your resize code here:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP() // required macro for message map functionality  One per class
};
