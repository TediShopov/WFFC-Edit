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
//typedef std::vector<DisplayObject> SceneGraph;

class MFCTransformView : public CFormView, public Observer<ToolMainChanges>
{
	DECLARE_DYNCREATE(MFCTransformView)

protected:
	MFCTransformView(); // protected constructor used by dynamic creation
	virtual ~MFCTransformView();

public:
	ToolMain* m_toolPtr;




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

	//Update method invoked on when tool notifies change
	void Update(const Subject<ToolMainChanges>* subject, const ToolMainChanges& data) override;

	//Selective update methods to invoke only part of the visuals
	void UpdateTreeSelections(std::vector<DisplayObject*> selected);
	void UpdatePropertyGrid(DisplayObject* obj);
	void UpdatePropertyGridSelection(std::vector<DisplayObject*>* selection);
	void UpdateWireFrameCheck(const ToolMain* data);
	void RebuildTreeHierarchy(ToolMain* tool);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTransformPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);
	afx_msg void DeselectAllButton();
	afx_msg void ToggleWireframeMode();
	afx_msg void CreateObjectFromDialog();
	afx_msg void DeleteSelected();
	void UpdateFloatProp(float* data);
	void UpdateFileProp(std::string* data);

private:
	CTreeCtrl m_treeCtrl;
	CMFCPropertyGridCtrl m_propertyGrid;
	CButton m_wireframeCheck;
	DisplayObject m_displayObjectCopy;

	//Map from scene object to items in transform hierarchy
	std::map<const DisplayObject*, HTREEITEM> m_treeItems;
	bool ConvertToRelativePath(const std::string& absolute_path,  std::string& relative_path);

	CMFCPropertyGridProperty* CreateFloatProp(const CString name, float* data);
public:
};
