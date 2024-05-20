#include "MFCMain.h"

#include "MFCTransformView.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(MFCMain, CWinApp)
	ON_COMMAND(ID_FILE_QUIT, &MFCMain::MenuFileQuit)
	ON_COMMAND(ID_FILE_SAVETERRAIN, &MFCMain::MenuFileSaveTerrain)
	ON_COMMAND(ID_EDIT_SELECT, &MFCMain::MenuEditSelect)
	ON_COMMAND(ID_BUTTON40001, &MFCMain::ToolBarButton1)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)

END_MESSAGE_MAP()

BOOL MFCMain::InitInstance()
{
	//instanciate the mfc frame
	m_frame = new CMyFrame();
	m_pMainWnd = m_frame;

	m_frame->Create(NULL,
		_T("World Of Flim-Flam Craft Editor"),
		WS_OVERLAPPEDWINDOW,
		CRect(100, 100, 1024, 768),
		NULL,
		NULL,
		0,
		NULL
	);

	//show and set the window to run and update.
	m_frame->ShowWindow(SW_SHOW);
	m_frame->UpdateWindow();

	//get the rect from the MFC window so we can get its dimensions
	m_toolHandle = m_frame->m_DirXView->GetSafeHwnd();				//handle of directX child window
	m_frame->m_DirXView->GetClientRect(&m_windowRect);

	//Assign window dimensions
	m_width = m_windowRect.Width();
	m_height = m_windowRect.Height();

	//Initialize tools
	m_ToolSystem.onActionInitialise(m_toolHandle, m_width, m_height);
	//Give tools as a reference to other classes/views
	m_frame->m_transformTreeView->m_toolPtr = &m_ToolSystem;
	m_frame->m_transformTreeView->VisualizeSelectionOnTreeCtrl(&m_ToolSystem);

	//Add reference to from view to the actual render so it
	//updated on window restore
	m_frame->dx_renderer = &m_ToolSystem.m_d3dRenderer;

	//Register all observers of ToolMain
	m_ToolSystem.RegisterObserver(m_frame->m_transformTreeView);

	return TRUE;
}

int MFCMain::Run()
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (true)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			POINT p;
			GetCursorPos(&p);
			HWND overWindow = WindowFromPoint(p);
			if (this->m_toolHandle == overWindow)
			{
				m_ToolSystem.UpdateInput(&msg);
			}
		}
		else
		{
			//int ID = m_ToolSystem.getCurrentSelectionID();
			std::vector<int> ids = m_ToolSystem.getCurrentSelectionIDs();
			std::wstring statusString = L"Selected Object: ";
			for (int id : ids)
			{
				if (id != -1)
				{
					statusString.append(std::to_wstring(id));
					statusString.append(L", ");
				}
			}
			m_ToolSystem.Tick(&msg);

			//send current object ID to status bar in The main frame
			m_frame->m_wndStatusBar.SetPaneText(1, statusString.c_str(), 1);
			//m_frame->m_transformTreeView->UpdateTreeVisuals();
		}
	}

	return (int)msg.wParam;
}

void MFCMain::MenuFileQuit()
{
	//will post message to the message thread that will exit the application normally
	PostQuitMessage(0);
}

void MFCMain::MenuFileSaveTerrain()
{
	m_ToolSystem.onActionSaveTerrain();
}

void MFCMain::MenuEditSelect()
{
	//SelectDialogue m_ToolSelectDialogue(NULL, &m_ToolSystem.m_sceneGraph);		//create our dialoguebox //modal constructor
	//m_ToolSelectDialogue.DoModal();	// start it up modal

	//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
//	m_ToolSelectDialogue.Create(IDD_DIALOG1);	//Start up modeless
//	m_ToolSelectDialogue.Create(IDD_DIALOG2);
//	m_ToolSelectDialogue.ShowWindow(SW_SHOW);
//	m_ToolSelectDialogue.SetObjectData(&m_ToolSystem.m_sceneGraph, &m_ToolSystem.m_selectedObject);
}

void MFCMain::ToolBarButton1()
{
	m_ToolSystem.onActionSave();
}

MFCMain::MFCMain()
{
}

MFCMain::~MFCMain()
{
}