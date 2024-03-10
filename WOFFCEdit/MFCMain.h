#pragma once

#include <afxwin.h>
#include <afx.h>
#include "pch.h"
#include "ToolMain.h"
#include "MFCFrame.h"

class MFCMain : public CWinApp
{
public:
	MFCMain();
	~MFCMain();
	BOOL InitInstance();
	int  Run();

private:

	CMyFrame* m_frame;	//handle to the frame where all our UI is
	HWND m_toolHandle;	//Handle to the MFC window
	ToolMain m_ToolSystem;	//Instance of Tool System that we interface to.
	CRect WindowRECT;	//Window area rectangle.
	//TransformHirarchyTree m_ToolSelectDialogue;			//for modeless dialogue, declare it here

	int m_width;
	int m_height;

	//Interface funtions for menu and toolbar etc requires
	afx_msg void MenuFileQuit();
	afx_msg void MenuFileSaveTerrain();
	afx_msg void MenuEditSelect();
	afx_msg	void ToolBarButton1();

	DECLARE_MESSAGE_MAP()	// required macro for message map functionality  One per class
};
