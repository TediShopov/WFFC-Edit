// MFCTransformView.cpp : implementation file
//

#include "resource.h"
#include "MFCTransformView.h"

#include <map>

#include "ToolMain.h"

// MFCTransformView

IMPLEMENT_DYNCREATE(MFCTransformView, CFormView)

MFCTransformView::MFCTransformView()
	: CFormView(IDD_FORMVIEW)
{
	auto transform = new CMFCPropertyGridProperty("Transform");
	auto globalPosGroup = new CMFCPropertyGridProperty("Position");
	globalPosGroup->AddSubItem(new CMFCPropertyGridProperty("X", COleVariant(0.0)));
	globalPosGroup->AddSubItem(new CMFCPropertyGridProperty("Y", COleVariant(0.0)));
	globalPosGroup->AddSubItem(new CMFCPropertyGridProperty("Z", COleVariant(0.0)));
	auto localScaleGroup = new CMFCPropertyGridProperty("Scale");
	localScaleGroup->AddSubItem(new CMFCPropertyGridProperty("X", COleVariant(0.0)));
	localScaleGroup->AddSubItem(new CMFCPropertyGridProperty("Y", COleVariant(0.0)));
	localScaleGroup->AddSubItem(new CMFCPropertyGridProperty("Z", COleVariant(0.0)));
	transform->AddSubItem(globalPosGroup);
	transform->AddSubItem(localScaleGroup);

	m_propertyGrid.AddProperty(transform, 1, 1);
}

MFCTransformView::~MFCTransformView()
{
}

void MFCTransformView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID2, m_propertyGrid);
}

void MFCTransformView::Update(const Subject<ToolMain>* subject, const ToolMain& data)
{
	VisualizeSelectionOnTreeCtrl(data);
}

BEGIN_MESSAGE_MAP(MFCTransformView, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &MFCTransformView::OnClickTree)
	ON_BN_CLICKED(IDC_BUTTON1, &MFCTransformView::OnBnClickedButton1)
END_MESSAGE_MAP()

// MFCTransformView diagnostics

void MFCTransformView::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// Get the selected item
//	HTREEITEM hItem = this->Transfo0rmSelections.m_currentSelections.GetSelectedItem();
	TVHITTESTINFO ht = { 0 };

	DWORD dwpos = GetMessagePos();
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);
	LPNMHDR lpnmh = (LPNMHDR)pNMHDR;
	CWnd* pWnd = CWnd::FromHandle(pNMHDR->hwndFrom);
	//MapWindowPoints(pWnd, &ht.pt, 1);
	m_treeCtrl.ScreenToClient(&ht.pt);
	m_treeCtrl.HitTest(&ht);
	if (TVHT_ONITEMSTATEICON & ht.flags)
	{
		auto selection = &m_toolPtr->m_selectedObject;
		CString id = m_treeCtrl.GetItemText(ht.hItem);

		//Reverse as this event is called before actual update happens
		bool nextState = !m_treeCtrl.GetCheck(ht.hItem);
		if (nextState == true)
		{
			selection->push_back(_wtoi(id));
		}
		else
		{
			auto iter = std::find(
				selection->begin()
				, selection->end(),
				_wtoi(id));
			selection->erase(iter);
		}
	}

	// Handle multi-selection logic here
	*pResult = 0;
}

void MFCTransformView::UncheckTreeItemAndChildren(CTreeCtrl& treeCtrl, HTREEITEM item)
{
	// Set the check state to unchecked
	treeCtrl.SetCheck(item, FALSE);

	// Get the child item
	HTREEITEM childItem = treeCtrl.GetChildItem(item);

	// Recursively uncheck children until no more child is found
	while (childItem) {
		UncheckTreeItemAndChildren(treeCtrl, childItem);
		childItem = treeCtrl.GetNextItem(childItem, TVGN_NEXT);
	}
}

void MFCTransformView::UncheckAllTreeItems(CTreeCtrl& treeCtrl)
{
	// Get the root item
	HTREEITEM treeItem = treeCtrl.GetRootItem();

	while (treeItem)
	{
		// Call the recursive function to uncheck the root and its children
		UncheckTreeItemAndChildren(treeCtrl, treeItem);
		treeItem = treeCtrl.GetNextSiblingItem(treeItem);
	}
}

#ifdef _DEBUG
void MFCTransformView::VisualizeSelectionOnTreeCtrl(const ToolMain& tool)
{
	std::map<int, HTREEITEM>* idToTreeItems = &objectsTreeItems;
	std::vector<SceneObject> sceneCopy = tool.m_sceneGraph;

	while (sceneCopy.size() != 0)
	{
		//Remove items which id's are already presented in object to tree map
		auto removed = std::remove_if(sceneCopy.begin(), sceneCopy.end(),
			[idToTreeItems](SceneObject v) { return idToTreeItems->find(v.ID) != idToTreeItems->end(); });
		sceneCopy.erase(removed, sceneCopy.end());
		for (const SceneObject& element : sceneCopy)
		{
			if (element.parent_id == 0)
			{
				HTREEITEM treeitem = m_treeCtrl.InsertItem(std::to_wstring(element.ID).c_str());
				idToTreeItems->insert({ element.ID, treeitem });
			}
			else
			{
				if (idToTreeItems->find(element.parent_id) != idToTreeItems->end())
				{
					HTREEITEM parentItem = idToTreeItems->at(element.parent_id);
					HTREEITEM treeitem = m_treeCtrl.InsertItem(
						std::to_wstring(element.ID).c_str()
						, parentItem);
					idToTreeItems->insert({ element.ID, treeitem });
				}
			}
		}
	}

	//Reset all checkboxes to false
	UncheckAllTreeItems(m_treeCtrl);

	//Update selection state of the tree ctrl
	for (int selectionId : tool.m_selectedObject)
	{
		auto objectTreeMap = idToTreeItems->find(selectionId);
		if (objectTreeMap != idToTreeItems->end())
		{
			HTREEITEM& treeitem = objectTreeMap->second;
			m_treeCtrl.SetCheck(treeitem, true);
		}
	}
}
// Function to uncheck a tree item and its children recursively

void MFCTransformView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void MFCTransformView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// MFCTransformView message handlers

void MFCTransformView::OnBnClickedButton1()
{
	this->m_toolPtr->m_selectedObject.clear();
	// TODO: Add your control notification handler code here
	UncheckAllTreeItems(m_treeCtrl);
}