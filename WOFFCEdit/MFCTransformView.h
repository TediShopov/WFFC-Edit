#pragma once
#include <afxcmn.h>
#include <afxext.h>
#include <afxpropertygridctrl.h>
#include <map>
#include <vector>

#include "ToolMain.h"
#include "Observer.h"
#include "DisplayObject.h"

//#include "TransformHirarchyTree.h"
class ToolMain;

class DisplayObject;
// MFCTransformView form view
typedef std::vector<DisplayObject> SceneGraph;

class MFCTransformView : public CFormView, public Observer<ToolMainChanges>
{
	DECLARE_DYNCREATE(MFCTransformView)

protected:
	MFCTransformView(); // protected constructor used by dynamic creation
	virtual ~MFCTransformView();

public:
	std::map<int, HTREEITEM> objectsTreeItems;
	ToolMain* m_toolPtr;
	void VisualizeSelectionOnTreeCtrl(ToolMain* tool);
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
	void UpdateWireFrameCheck(const ToolMain* data);
	void UpdateTreeSelections(std::vector<DisplayObject*> selected);
	//void Update(const Subject<ToolMain>* subject, const ToolMain& data) override;
	void Update(const Subject<ToolMainChanges>* subject, const ToolMainChanges& data) override;

	DECLARE_MESSAGE_MAP()

public:
	void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	void UncheckTreeItemAndChildren(CTreeCtrl& treeCtrl, HTREEITEM item);

	// Function to traverse the entire tree and uncheck all items
	void UncheckAllTreeItems(CTreeCtrl& treeCtrl);
	//DisplayObject* FindSceneObject(int selectedItemId);
	void UpdatePropertyGridSelection(std::vector<DisplayObject*>* selection);
	void UpdatePropertyGrid(DisplayObject* obj);
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
	std::map<const DisplayObject*, HTREEITEM> m_treeItems;
	bool ConvertToRelativePath(const std::string& absolute_path,  std::string& relative_path);

	//Poistion Controls
	DisplayObject displayObjectCopy;
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
};
