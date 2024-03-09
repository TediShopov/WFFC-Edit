#pragma once
#include <afxcmn.h>
#include <afxext.h>
#include <vector>

class SceneObject;
// MFCTransformView form view
typedef std::vector<SceneObject>* SceneGraph;

class MFCTransformView : public CFormView
{
	DECLARE_DYNCREATE(MFCTransformView)

protected:
	MFCTransformView();           // protected constructor used by dynamic creation
	virtual ~MFCTransformView();

public:
	SceneGraph m_sceneGraph;
	CTreeCtrl m_transformHierarchy;
	std::vector<int>* m_currentSelections;
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
