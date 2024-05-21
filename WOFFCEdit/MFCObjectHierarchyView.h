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
public:
	ToolMain* m_toolPtr;
	MFCTransformView(); // protected constructor used by dynamic creation
	virtual ~MFCTransformView();
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

	//Update method invoked on when tool notifies change
	void Update(const m_Subject<ToolMainChanges>* subject, const ToolMainChanges& data) override;

	//Selective update methods to invoke only part of the visuals
	void UpdateTreeSelections(std::vector<DisplayObject*> selected);
	void UpdatePropertyGrid(DisplayObject* obj);
	void UpdatePropertyGridSelection(std::vector<DisplayObject*>* selection);
	void UpdateWireFrameCheck(const ToolMain* data);
	void RebuildTreeHierarchy(ToolMain* tool);


	//Methods to alter temporary objects from propery changed event
	void AlterTemporaryObjectsParentAndParentId(CMFCPropertyGridProperty* propChanged);
	void AlterTemporaryObjectsFilePaths(CMFCPropertyGridProperty* propChanged);
	void AlterTemporaryObjectsFloatProperty(CMFCPropertyGridProperty* propChanged);

	DECLARE_MESSAGE_MAP()

public:
	//Event from interacting with the tree
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTransformPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);

	//Button and checkbox events
	afx_msg void DeselectAllButton();
	afx_msg void ToggleWireframeMode();
	afx_msg void CreateObjectFromDialog();
	afx_msg void DeleteSelected();


private:
	//The stored temporary copy of an object.
	//This is the object that all properties data pointers point to
	//Data is copied from and to actual objects.
	DisplayObject m_displayObjectCopy; 

	CMFCPropertyGridCtrl m_propertyGrid; // The inspector property grid
	CButton m_wireframeCheck; //Checkbox for toggling wireframe mode

	// The transform hierarchy  
	CTreeCtrl m_treeCtrl;
	//Map from scene object to items in transform hierarchy
	std::map<const DisplayObject*, HTREEITEM> m_treeItems;

	//Convert an absolute path to relative path
	bool ConvertToRelativePath(const std::string& absolute_path,  std::string& relative_path);


	CMFCPropertyGridProperty* CreateFloatProp(const CString name, float* data);
	//Update properties to match temporary object
	void UpdateFloatProp(float* data) const;
	void UpdateFileProp(std::string* data) const;
	void UpdateIDProp(CMFCPropertyGridProperty* prop) const;
public:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
};
