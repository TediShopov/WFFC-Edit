// MFCTransformView.cpp : implementation file
//

#include "resource.h"
#include "MFCTransformView.h"

#include <map>

// MFCTransformView

IMPLEMENT_DYNCREATE(MFCTransformView, CFormView)

MFCTransformView::MFCTransformView()
	: CFormView(IDD_FORMVIEW)
{
	auto transform = new CMFCPropertyGridProperty("Transform");
	auto globalPosGroup = new CMFCPropertyGridProperty("Position");
	globalPosGroup->AddSubItem(CreateFloatProp("X", &sceneObjectCopy.posX));
	globalPosGroup->AddSubItem(CreateFloatProp("Y", &sceneObjectCopy.posY));
	globalPosGroup->AddSubItem(CreateFloatProp("Z", &sceneObjectCopy.posZ));

	//Var scale group
	auto localScaleGroup = new CMFCPropertyGridProperty("Scale");
	localScaleGroup->AddSubItem(CreateFloatProp("X", &sceneObjectCopy.scaX));
	localScaleGroup->AddSubItem(CreateFloatProp("Y", &sceneObjectCopy.scaY));
	localScaleGroup->AddSubItem(CreateFloatProp("Z", &sceneObjectCopy.scaZ));

	auto localOrientationGroup = new CMFCPropertyGridProperty("Rotation");
	localOrientationGroup->AddSubItem(CreateFloatProp("X", &sceneObjectCopy.rotX));
	localOrientationGroup->AddSubItem(CreateFloatProp("Y", &sceneObjectCopy.rotY));
	localOrientationGroup->AddSubItem(CreateFloatProp("Z", &sceneObjectCopy.rotZ));
	transform->AddSubItem(globalPosGroup);
	transform->AddSubItem(localScaleGroup);
	transform->AddSubItem(localOrientationGroup);

	//==============================================
	//Adding texture string search through windows

	//NULL, reinterpret_cast<DWORD_PTR>(data));
	auto modelProperties = new CMFCPropertyGridProperty("Model");
	//Model Mesh File Dialog
	//----------------------------------------------
	//TODO Add a single option for all supported files
	static TCHAR BASED_CODE modelFilter[] = _T(
		"Visual Studio Starter Kit Files(*.cmo)|*.cmo|"
		"Direct X SDK(*.sdkmesh)|*.sdkmesh|"
		"VBO (*.vbo)|*.vbo||"
		"");
	modelProperties->AddSubItem(
		new CMFCPropertyGridFileProperty(
			_T("Model Mesh"),
			TRUE,
			sceneObjectCopy.model_path.c_str(),
			_T("ico"),
			0,
			modelFilter,
			_T("Specifies the dialog icon"),
			reinterpret_cast<DWORD_PTR>(&sceneObjectCopy.model_path)));

	//Texture Mesh Dialog
	//----------------------------------------------

	static TCHAR BASED_CODE textureFilter[] = _T(
		"DDS (*.dds)|*.dds||"
		"");
	modelProperties->AddSubItem(
		new CMFCPropertyGridFileProperty(
			_T("Texture"),
			TRUE,
			_T(""),
			_T("dds"),
			0,
			textureFilter,
			_T("Specifies the dialog icon")));

	//TODO Add Control For Object Color
	m_propertyGrid.AddProperty(modelProperties, 1, 1);

	m_propertyGrid.AddProperty(transform, 1, 1);
}

CMFCPropertyGridProperty* MFCTransformView::CreateFloatProp(
	const CString name, float* data)
{
	return new CMFCPropertyGridProperty(name, COleVariant(*data),
	                                    NULL, reinterpret_cast<DWORD_PTR>(data));
}

void MFCTransformView::UpdateFloatProp(float* data)
{
	CMFCPropertyGridProperty* prop = m_propertyGrid.FindItemByData(
		reinterpret_cast<DWORD_PTR>(data), TRUE);
	prop->SetValue(*data);
}

void MFCTransformView::UpdateFileProp(std::string* data)
{
	CStringW w(data->c_str());
	CMFCPropertyGridProperty* prop = m_propertyGrid.FindItemByData(
		reinterpret_cast<DWORD_PTR>(data), TRUE);
	if (prop != nullptr)
		prop->SetValue(w);
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
	UpdatePropertyGridSelection(&data.m_selectedObject);
}

BEGIN_MESSAGE_MAP(MFCTransformView, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &MFCTransformView::OnClickTree)
	ON_BN_CLICKED(IDC_BUTTON1, &MFCTransformView::OnBnClickedButton1)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &MFCTransformView::OnTransformPropertyChanged)
END_MESSAGE_MAP()

// MFCTransformView diagnostics

void MFCTransformView::UpdatePropertyGridSelection(const std::vector<int>* selection)
{
	SceneObject* foundObj = nullptr;
	if (selection->size() == 1)
	{
		m_propertyGrid.ShowWindow(SW_SHOW);
		foundObj = FindSceneObject((*selection)[0]);
		UpdatePropertyGrid(foundObj);
	}
	else
	{
		m_propertyGrid.ShowWindow(SW_HIDE);
	}
}

void MFCTransformView::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// Get the selected item
	//	HTREEITEM hItem = this->Transfo0rmSelections.m_currentSelections.GetSelectedItem();
	TVHITTESTINFO ht = {0};

	DWORD dwpos = GetMessagePos();
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);
	LPNMHDR lpnmh = (LPNMHDR)pNMHDR;
	CWnd* pWnd = CWnd::FromHandle(pNMHDR->hwndFrom);
	//MapWindowPoints(pWnd, &ht.pt, 1);
	m_treeCtrl.ScreenToClient(&ht.pt);
	m_treeCtrl.HitTest(&ht);
	auto selection = &m_toolPtr->m_selectedObject;
	if (TVHT_ONITEMSTATEICON & ht.flags)
	{
		CString id = m_treeCtrl.GetItemText(ht.hItem);

		//Reverse as this event is called before actual update happens
		bool nextState = !m_treeCtrl.GetCheck(ht.hItem);
		if (nextState == true)
		{
			selection->push_back(_wtoi(id) - 1);
		}
		else
		{
			auto iter = std::find(
				selection->begin()
				, selection->end(),
				_wtoi(id) - 1);
			selection->erase(iter);
		}
	}

	UpdatePropertyGridSelection(selection);

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
	while (childItem)
	{
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

SceneObject* MFCTransformView::FindSceneObject(int selectedItemId)
{
	SceneObject* obj = nullptr;
	std::vector<SceneObject>* objects = &m_toolPtr->m_sceneGraph;
	for (SceneObject& scene_object : *objects)
	{
		if (scene_object.ID == selectedItemId + 1)
		{
			obj = &scene_object;
			break;
		}
	}
	return obj;
}

void MFCTransformView::UpdatePropertyGrid(SceneObject* obj)
{
	if (obj != nullptr)
		sceneObjectCopy = *obj;
	else
		sceneObjectCopy = SceneObject();

	//Update transform
	UpdateFloatProp(&sceneObjectCopy.posX);
	UpdateFloatProp(&sceneObjectCopy.posY);
	UpdateFloatProp(&sceneObjectCopy.posZ);
	UpdateFloatProp(&sceneObjectCopy.scaX);
	UpdateFloatProp(&sceneObjectCopy.scaY);
	UpdateFloatProp(&sceneObjectCopy.scaZ);
	UpdateFloatProp(&sceneObjectCopy.rotX);
	UpdateFloatProp(&sceneObjectCopy.rotY);
	UpdateFloatProp(&sceneObjectCopy.rotZ);

	UpdateFileProp(&sceneObjectCopy.model_path);
	UpdateFileProp(&sceneObjectCopy.tex_diffuse_path);

	m_propertyGrid.MarkModifiedProperties(1, 1);
}

LRESULT MFCTransformView::OnTransformPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty* propChanged = (CMFCPropertyGridProperty*)lparam;

	SceneObject* foundObj = nullptr;
	//IF property changed was string
	//IF property changed was float
	if (m_toolPtr->m_selectedObject.size() == 1)
	{
		m_propertyGrid.MarkModifiedProperties(1, 1);
		m_propertyGrid.ShowWindow(SW_SHOW);
		foundObj = FindSceneObject((m_toolPtr->m_selectedObject)[0]);
		std::string s = propChanged->GetRuntimeClass()->m_lpszClassName;
		if (s.compare("CMFCPropertyGridFileProperty") == 0)
		{
			int a = 3;
		}
		else
		{
			float* d = reinterpret_cast<float*>(propChanged->GetData());
			auto test = propChanged->GetValue();
			(*d) = test.fltVal;
			(*foundObj) = sceneObjectCopy;
			m_toolPtr->m_d3dRenderer.
			           UpdateDisplayElementTransform(
				           foundObj->ID - 1, &m_toolPtr->m_sceneGraph
			           );
		}
	}
	return TRUE;
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
		                              [idToTreeItems](SceneObject v)
		                              {
			                              return idToTreeItems->find(v.ID) != idToTreeItems->end();
		                              });
		sceneCopy.erase(removed, sceneCopy.end());
		for (const SceneObject& element : sceneCopy)
		{
			if (element.parent_id == 0)
			{
				HTREEITEM treeitem = m_treeCtrl.InsertItem(std::to_wstring(element.ID).c_str());
				idToTreeItems->insert({element.ID, treeitem});
			}
			else
			{
				if (idToTreeItems->find(element.parent_id) != idToTreeItems->end())
				{
					HTREEITEM parentItem = idToTreeItems->at(element.parent_id);
					HTREEITEM treeitem = m_treeCtrl.InsertItem(
						std::to_wstring(element.ID).c_str()
						, parentItem);
					idToTreeItems->insert({element.ID, treeitem});
				}
			}
		}
	}

	//Reset all checkboxes to false
	UncheckAllTreeItems(m_treeCtrl);

	//Update selection state of the tree ctrl
	for (int selectionId : tool.m_selectedObject)
	{
		auto objectTreeMap = idToTreeItems->find(selectionId + 1);
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
	UpdatePropertyGridSelection(&m_toolPtr->m_selectedObject);
	// TODO: Add your control notification handler code here
	UncheckAllTreeItems(m_treeCtrl);
	//	CMFCPropertyGridProperty* prop = m_propertyGrid.FindItemByData(reinterpret_cast<DWORD_PTR>(&testData));
	//	prop->SetValue(testData);
	m_propertyGrid.MarkModifiedProperties(1, 1);
}
