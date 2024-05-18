#pragma once
#include <afxcmn.h>
#include <afxext.h>
#include <afxpropertygridctrl.h>
#include <map>
#include <vector>

#include "ToolMain.h"
#include "Observer.h"

//#include "TransformHirarchyTree.h"
class ToolMain;

class SceneObject;
// MFCTransformView form view
typedef std::vector<SceneObject> SceneGraph;

class MFCTransformView : public CFormView, public Observer<ToolMain>
{
	DECLARE_DYNCREATE(MFCTransformView)

protected:
	MFCTransformView(); // protected constructor used by dynamic creation
	virtual ~MFCTransformView();

public:
	std::map<int, HTREEITEM> objectsTreeItems;
	ToolMain* m_toolPtr;
	void VisualizeSelectionOnTreeCtrl(const ToolMain& tool);
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	void UpdateWireFrameCheck(const ToolMain& data);
	void Update(const Subject<ToolMain>* subject, const ToolMain& data) override;

	DECLARE_MESSAGE_MAP()

public:
	void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	void UncheckTreeItemAndChildren(CTreeCtrl& treeCtrl, HTREEITEM item);

	// Function to traverse the entire tree and uncheck all items
	void UncheckAllTreeItems(CTreeCtrl& treeCtrl);
	SceneObject* FindSceneObject(int selectedItemId);
	void UpdatePropertyGridSelection(const std::vector<int>* selection);
	void UpdatePropertyGrid(SceneObject* obj);
	afx_msg LRESULT OnTransformPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);

	CTreeCtrl m_treeCtrl;
	CMFCPropertyGridCtrl m_propertyGrid;
	CButton m_wireframeCheck;
	afx_msg void OnBnClickedButton1();
	CMFCPropertyGridProperty* CreateFloatProp(
		const CString name, float* data);
	void UpdateFloatProp(float* data);
	void UpdateFileProp(std::string* data);

private:
	bool convert_to_relative_path(const std::string& absolute_path,  std::string& relative_path);

	//Poistion Controls
	SceneObject sceneObjectCopy;
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
};
