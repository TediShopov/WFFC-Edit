// MFCTransformView.cpp : implementation file
//

#include "resource.h"
#include "MFCTransformView.h"

#include <map>
#include <__msvc_filebuf.hpp>

// MFCTransformView
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "DisplayObject.h"
namespace fs = std::experimental::filesystem;
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
	//----------------------------------------------path

	static TCHAR BASED_CODE textureFilter[] = _T(
		"DDS (*.dds)|*.dds||"
		"");
	modelProperties->AddSubItem(
		new CMFCPropertyGridFileProperty(
			_T("Texture"),
			TRUE,
			sceneObjectCopy.tex_diffuse_path.c_str(),
			_T("dds"),
			0,
			textureFilter,
			_T("Specifies the dialog icon"),
			reinterpret_cast<DWORD_PTR>(&sceneObjectCopy.tex_diffuse_path)));
			
			
	//TODO(lowPriority) Add color 
	//Color 
	//----------------------------------------------
			

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

bool MFCTransformView::convert_to_relative_path(const std::string& absolute_path, 
	std::string& relative_path)
{
	try {
		fs::path abs_path(absolute_path);
		fs::path base_path = fs::current_path();

		// Convert both paths to absolute paths
		abs_path = fs::absolute(abs_path);
		base_path = fs::absolute(base_path);

		// Check if the base_path is a prefix of abs_path
		auto abs_iter = abs_path.begin();
		auto base_iter = base_path.begin();

		// Find common prefix
		while (abs_iter != abs_path.end() && base_iter != base_path.end() && (*abs_iter) == (*base_iter)) {
			++abs_iter;
			++base_iter;
		}

		if (base_iter != base_path.end()) {
			relative_path = "database/data/placeholder.dds";
			return false;
		}

		fs::path rel_path;
		while (base_iter != base_path.end()) {
			rel_path /= "..";
			++base_iter;
		}
		while (abs_iter != abs_path.end()) {
			rel_path /= *abs_iter;
			++abs_iter;
		}

		relative_path = rel_path.string();
		return true;
	}
	catch (const fs::filesystem_error& e) {
		relative_path = "database/data/placeholder.dds";
		return false;
	}
}


MFCTransformView::~MFCTransformView()
{
}

void MFCTransformView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
	DDX_Control(pDX, IDC_CHECK1, m_wireframeCheck);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID2, m_propertyGrid);
}

void MFCTransformView::UpdateWireFrameCheck(const ToolMain& data)
{
	auto selected = this->m_toolPtr->GetSelectedDisplayObjects();
	if(selected.size()!=1)
		m_wireframeCheck.SetCheck(false);
	else
		m_wireframeCheck.SetCheck(selected[0]->m_wireframe);
}

void MFCTransformView::Update(const Subject<ToolMain>* subject, const ToolMain& data)
{
	VisualizeSelectionOnTreeCtrl(data);
	auto selected = this->m_toolPtr->GetSelectedObjects();
	UpdatePropertyGridSelection(&selected);
	UpdateWireFrameCheck(data);
}

BEGIN_MESSAGE_MAP(MFCTransformView, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &MFCTransformView::OnClickTree)
	ON_BN_CLICKED(IDC_BUTTON1, &MFCTransformView::OnBnClickedButton1)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &MFCTransformView::OnTransformPropertyChanged)
	ON_BN_CLICKED(IDC_BUTTON3, &MFCTransformView::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &MFCTransformView::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &MFCTransformView::OnBnClickedCheck1)
END_MESSAGE_MAP()

// MFCTransformView diagnostics

void MFCTransformView::UpdatePropertyGridSelection(std::vector<SceneObject*>* selection)
{
	if (selection->size() == 1)
	{
		m_propertyGrid.ShowWindow(SW_SHOW);
		UpdatePropertyGrid((*selection)[0]);
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
	if (TVHT_ONITEMSTATEICON & ht.flags)
	{
		CString id = m_treeCtrl.GetItemText(ht.hItem);

		//Reverse as this event is called before actual update happens
		bool nextState = !m_treeCtrl.GetCheck(ht.hItem);
		int actualIndex = _wtoi(id) - 1;
		if (nextState == true)
		{
			m_toolPtr->AddToSelection(actualIndex);
		}
		else
		{
			m_toolPtr->RemoveFromSelection(actualIndex);
		}
	}
	auto selection = m_toolPtr->GetSelectedObjects();
	UpdatePropertyGridSelection(&selection);
	UpdateWireFrameCheck(*this->m_toolPtr);

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

//SceneObject* MFCTransformView::FindSceneObject(int selectedItemId)
//{
//	SceneObject* obj = nullptr;
//	std::vector<SceneObject>* objects = &m_toolPtr->m_sceneGraph;
//	for (SceneObject& scene_object : *objects)
//	{
//		if (scene_object.ID == selectedItemId + 1)
//		{
//			obj = &scene_object;
//			break;
//		}
//	}
//	return obj;
//}

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
	auto sceneObjects = this->m_toolPtr->GetSelectedObjects();
	if (sceneObjects.size() == 1)
	{
		m_propertyGrid.MarkModifiedProperties(1, 1);
		m_propertyGrid.ShowWindow(SW_SHOW);
		//foundObj = FindSceneObject((m_toolPtr->m_selectedObject)[0]);
		foundObj = sceneObjects[0];
		std::string s = propChanged->GetRuntimeClass()->m_lpszClassName;
		if (s.compare("CMFCPropertyGridFileProperty") == 0)
		{
			std::string* d = reinterpret_cast<std::string*>(propChanged->GetData());
			auto test = propChanged->GetValue();
			std::wstring ws(test.bstrVal, SysStringLen(test.bstrVal));
			std::string s(ws.begin(),ws.end());

			//TODO make to relative path
// Get the current working directory
			std::string absolute_path = s;
			std::string relative_path;
			convert_to_relative_path(absolute_path,  relative_path);
			

			propChanged->SetValue( CString(relative_path.c_str()));

			(*d) =relative_path;
			(*foundObj) = sceneObjectCopy;
			m_toolPtr->SyncDisplayAndSceneObjects(foundObj->ID - 1);
		}
		else
		{
			float* d = reinterpret_cast<float*>(propChanged->GetData());
			auto test = propChanged->GetValue();
			(*d) = test.fltVal;
			(*foundObj) = sceneObjectCopy;
			m_toolPtr->SyncDisplayAndSceneObjects(foundObj->ID - 1);
		}
	}
	return TRUE;
}

#ifdef _DEBUG
void MFCTransformView::VisualizeSelectionOnTreeCtrl(const ToolMain& tool)
{
	std::map<int, HTREEITEM>* idToTreeItems = &objectsTreeItems;
	std::vector<SceneObject> sceneCopy = tool.m_sceneGraph;

	//Update the transform tree if an object from scene was deleted
	if(sceneCopy.size() != idToTreeItems->size())
	{
		std::vector<int> uncontainedIndexes;

		for (std::map<int, HTREEITEM>::iterator it = idToTreeItems->begin(); it != idToTreeItems->end(); ++it)
		{


			auto iter = std::find_if(
				sceneCopy.begin(),
				sceneCopy.end(),
				[it](const SceneObject& s) {return s.ID == it->first; });

			if (iter == sceneCopy.end())
				uncontainedIndexes.push_back(it->first);
		}


		for (int uncontained_index : uncontainedIndexes)
		{
			m_treeCtrl.DeleteItem(idToTreeItems->at(uncontained_index));
			
			idToTreeItems->erase(uncontained_index);
			
		}
		
		
	}



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

	for (int selectionId : tool.getCurrentSelectionIDs())
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
	m_toolPtr->ClearSelection();
	UpdatePropertyGridSelection(&(m_toolPtr->GetSelectedObjects()));
	// TODO: Add your control notification handler code here
	UncheckAllTreeItems(m_treeCtrl);
	//	CMFCPropertyGridProperty* prop = m_propertyGrid.FindItemByData(reinterpret_cast<DWORD_PTR>(&testData));
	//	prop->SetValue(testData);
	m_propertyGrid.MarkModifiedProperties(1, 1);
}


void MFCTransformView::OnBnClickedButton3()
{
	this->m_toolPtr->DeleteSelected();
}


void MFCTransformView::OnBnClickedButton2()
{
	this->m_toolPtr->CreateObject();
}


void MFCTransformView::OnBnClickedCheck1()
{
	auto sel = m_toolPtr->GetSelectedDisplayObjects();
	if(sel.size()==1)
	{
		bool wireframeNewState = m_wireframeCheck.GetCheck();
		sel[0]->m_wireframe = wireframeNewState;

	}
}
