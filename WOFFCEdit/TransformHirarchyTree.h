////#pragma once
//#include <vector>
//#include "afxdialogex.h"
//#include "resource.h"
//#include "SceneObject.h"
//
//// TransformHirarchyTree dialog
//typedef std::vector<SceneObject>* SceneGraph;
//
//class TransformHirarchyTree : public CDialogEx
//{
//	DECLARE_DYNAMIC(TransformHirarchyTree)
//
//public:
//	TransformHirarchyTree(CWnd* pParent, SceneGraph);   // standard constructor
//	TransformHirarchyTree(CWnd* pParent = NULL);   // standard constructor
//
//	virtual ~TransformHirarchyTree();
//	void SetObjectData(SceneGraph scene, std::vector<int>* selections);	//passing in pointers to the data the class will operate on.
//	void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
//	BOOL OnInitDialog() override;
//
//	// Dialog Data
//#ifdef AFX_DESIGN_TIME
//	enum { IDD = IDD_DIALOG2 };
//#endif
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	void End();
//
//	DECLARE_MESSAGE_MAP()
//public:
//	SceneGraph m_sceneGraph;
//	CTreeCtrl m_transformHierarchy;
//	std::vector<int>* m_currentSelections;
//};
