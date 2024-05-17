#pragma once
#include "afxdialogex.h"
#include <afxpropertygridctrl.h>
#include <map>

#include "ToolMain.h"

// CreateObjectDialog dialog

class ToolMain;
class SceneObject;

class CreateObjectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CreateObjectDialog)

public:
	CreateObjectDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CreateObjectDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

	void SetObjectData(ToolMain* toolMain);
protected:

bool convert_to_relative_path(const std::string& absolute_path, 
	std::string& relative_path);
	SceneObject ObjectPrototype;
	ToolMain* tool_main;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg LRESULT OnTransformPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	CMFCPropertyGridCtrl m_grid;
};
