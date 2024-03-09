//// TransformHirarchyTree.cpp : implementation file
////
//
//#include "stdafx.h"
//#include "TransformHirarchyTree.h"
//
//#include <map>
//
//// TransformHirarchyTree dialog
//
//IMPLEMENT_DYNAMIC(TransformHirarchyTree, CDialogEx)
//BEGIN_MESSAGE_MAP(TransformHirarchyTree, CDialogEx)
//	ON_COMMAND(IDOK, &TransformHirarchyTree::End)
//	ON_NOTIFY(NM_CLICK, IDC_TREE1, &TransformHirarchyTree::OnClickTree)
//END_MESSAGE_MAP()
//
//TransformHirarchyTree::TransformHirarchyTree(CWnd* pParent, SceneGraph graph)
//	: CDialogEx(IDD_DIALOG2, pParent)
//{
//	m_sceneGraph = graph;
//	m_transformHierarchy.ModifyStyle(0, TVS_CHECKBOXES | TVS_EX_MULTISELECT);
//}
//TransformHirarchyTree::TransformHirarchyTree(CWnd* pParent)
//	: CDialogEx(IDD_DIALOG2, pParent)
//{
//	//	m_transformHierarchy.ModifyStyle(0, TVS_CHECKBOXES);SendMessageA
//}
//bool isContainedInTree(SceneObject  sceneObj);
//void TransformHirarchyTree::SetObjectData(SceneGraph graph, std::vector<int>* selections)
//{
//	m_sceneGraph = graph;
//	m_currentSelections = selections;
//	std::map<int, HTREEITEM> idToTreeItems;
//	std::vector<SceneObject> sceneCopy = *graph;
//
//	while (sceneCopy.size() != 0)
//	{
//		for (const SceneObject& element : sceneCopy)
//		{
//			if (element.parent_id == 0)
//			{
//				HTREEITEM treeitem = m_transformHierarchy.InsertItem(std::to_wstring(element.ID).c_str());
//				idToTreeItems.insert({ element.ID, treeitem });
//			}
//			else
//			{
//				if (idToTreeItems.find(element.parent_id) != idToTreeItems.end())
//				{
//					HTREEITEM parentItem = idToTreeItems.at(element.parent_id);
//					HTREEITEM treeitem = m_transformHierarchy.InsertItem(
//						std::to_wstring(element.ID).c_str()
//						, parentItem);
//					idToTreeItems.insert({ element.ID, treeitem });
//				}
//			}
//		}
//
//		auto removed = std::remove_if(sceneCopy.begin(), sceneCopy.end(),
//			[idToTreeItems](SceneObject v) { return idToTreeItems.find(v.ID) != idToTreeItems.end(); });
//		sceneCopy.erase(removed, sceneCopy.end());
//	}
//}
//
//void TransformHirarchyTree::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
//
//	// Get the selected item
//	HTREEITEM hItem = m_transformHierarchy.GetSelectedItem();
//	TVHITTESTINFO ht = { 0 };
//
//	DWORD dwpos = GetMessagePos();
//	ht.pt.x = GET_X_LPARAM(dwpos);
//	ht.pt.y = GET_Y_LPARAM(dwpos);
//	LPNMHDR lpnmh = (LPNMHDR)pNMHDR;
//	CWnd* pWnd = CWnd::FromHandle(pNMHDR->hwndFrom);
//	//MapWindowPoints(pWnd, &ht.pt, 1);
//	m_transformHierarchy.ScreenToClient(&ht.pt);
//	m_transformHierarchy.HitTest(&ht);
//
//	if (TVHT_ONITEMSTATEICON & ht.flags)
//	{
//		CString id = m_transformHierarchy.GetItemText(ht.hItem);
//		m_currentSelections->push_back(_wtoi(id));
//	}
//
//	// Handle multi-selection logic here
//
//	*pResult = 0;
//}
//
//BOOL TransformHirarchyTree::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//	m_transformHierarchy.ModifyStyle(0,
//		TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_CHECKBOXES);
//	return TRUE;
//}
//
//TransformHirarchyTree::~TransformHirarchyTree()
//{
//}
//
//void TransformHirarchyTree::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_TREE1, m_transformHierarchy);
//}
//void TransformHirarchyTree::End()
//{
//	DestroyWindow();
//}
//
//// TransformHirarchyTree message handlers