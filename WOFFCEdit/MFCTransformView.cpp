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
#include "UpdateObjectCommand.h"
namespace fs = std::experimental::filesystem;
IMPLEMENT_DYNCREATE(MFCTransformView, CFormView)

MFCTransformView::MFCTransformView()
	: CFormView(IDD_FORMVIEW)
{
	auto transform = new CMFCPropertyGridProperty("Transform");
	auto globalPosGroup = new CMFCPropertyGridProperty("Position");
	globalPosGroup->AddSubItem(CreateFloatProp("X", &displayObjectCopy.m_position.x));
	globalPosGroup->AddSubItem(CreateFloatProp("Y", &displayObjectCopy.m_position.y));
	globalPosGroup->AddSubItem(CreateFloatProp("Z", &displayObjectCopy.m_position.z));

	//Var scale group
	auto localScaleGroup = new CMFCPropertyGridProperty("Scale");
	localScaleGroup->AddSubItem(CreateFloatProp("X", &displayObjectCopy.m_scale.x));
	localScaleGroup->AddSubItem(CreateFloatProp("Y", &displayObjectCopy.m_scale.y));
	localScaleGroup->AddSubItem(CreateFloatProp("Z", &displayObjectCopy.m_scale.z));

	auto localOrientationGroup = new CMFCPropertyGridProperty("Rotation");
	localOrientationGroup->AddSubItem(CreateFloatProp("X", &displayObjectCopy.m_orientation.x));
	localOrientationGroup->AddSubItem(CreateFloatProp("Y", &displayObjectCopy.m_orientation.y));
	localOrientationGroup->AddSubItem(CreateFloatProp("Z", &displayObjectCopy.m_orientation.z));
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
			displayObjectCopy.model_path.c_str(),
			_T("ico"),
			0,
			modelFilter,
			_T("Specifies the dialog icon"),
			reinterpret_cast<DWORD_PTR>(&displayObjectCopy.model_path)));

	//Texture Mesh Dialog
	//----------------------------------------------path

	static TCHAR BASED_CODE textureFilter[] = _T(
		"DDS (*.dds)|*.dds||"
		"");
	modelProperties->AddSubItem(
		new CMFCPropertyGridFileProperty(
			_T("Texture"),
			TRUE,
			displayObjectCopy.texture_path.c_str(),
			_T("dds"),
			0,
			textureFilter,
			_T("Specifies the dialog icon"),
			reinterpret_cast<DWORD_PTR>(&displayObjectCopy.texture_path)));
			
			
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

bool MFCTransformView::ConvertToRelativePath(const std::string& absolute_path, 
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
	auto selected = this->m_toolPtr->GetSelectedDisplayObjects();
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

void MFCTransformView::UpdatePropertyGridSelection(std::vector<DisplayObject*>* selection)
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
		int actualIndex = _wtoi(id);
		if (nextState == true)
		{
			m_toolPtr->AddToSelection(actualIndex);
		}
		else
		{
			m_toolPtr->RemoveFromSelection(actualIndex);
		}
	}
	auto selection = m_toolPtr->GetSelectedDisplayObjects();
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

//DisplayObject* MFCTransformView::FindSceneObject(int selectedItemId)
//{
//	DisplayObject* obj = nullptr;
//	std::vector<DisplayObject>* objects = &m_toolPtr->m_sceneGraph;
//	for (DisplayObject& scene_object : *objects)
//	{
//		if (scene_object.m_ID == selectedItemId + 1)
//		{
//			obj = &scene_object;
//			break;
//		}
//	}
//	return obj;
//}

void MFCTransformView::UpdatePropertyGrid(DisplayObject* obj)
{
	if (obj != nullptr)
		displayObjectCopy = *obj;
	else
		displayObjectCopy =  DisplayObject();

	//Update transform
	UpdateFloatProp(&displayObjectCopy.m_position.x);
	UpdateFloatProp(&displayObjectCopy.m_position.y);
	UpdateFloatProp(&displayObjectCopy.m_position.z);
	UpdateFloatProp(&displayObjectCopy.m_scale.x);
	UpdateFloatProp(&displayObjectCopy.m_scale.y);
	UpdateFloatProp(&displayObjectCopy.m_scale.z);
	UpdateFloatProp(&displayObjectCopy.m_orientation.x);
	UpdateFloatProp(&displayObjectCopy.m_orientation.y);
	UpdateFloatProp(&displayObjectCopy.m_orientation.z);

	UpdateFileProp(&displayObjectCopy.model_path);
	UpdateFileProp(&displayObjectCopy.texture_path);

	m_propertyGrid.MarkModifiedProperties(1, 1);
}

LRESULT MFCTransformView::OnTransformPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty* propChanged = (CMFCPropertyGridProperty*)lparam;

	DisplayObject* foundObj = nullptr;
	//IF property changed was string
	//IF property changed was float
	auto sceneObjects = this->m_toolPtr->GetSelectedDisplayObjects();
	if (sceneObjects.size() == 1)
	{
		foundObj = sceneObjects[0];
		//Start update command of found objects
		UpdateObjectCommand* updateCommand = new UpdateObjectCommand(sceneObjects[0]);



		m_propertyGrid.MarkModifiedProperties(1, 1);
		m_propertyGrid.ShowWindow(SW_SHOW);
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
			ConvertToRelativePath(absolute_path,  relative_path);
			

			propChanged->SetValue( CString(relative_path.c_str()));

			(*d) =relative_path;
			(*foundObj) = displayObjectCopy;
			m_toolPtr->SyncDisplayAndSceneObjects(foundObj->m_ID );
		}
		else
		{
			float* d = reinterpret_cast<float*>(propChanged->GetData());
			auto test = propChanged->GetValue();
			(*d) = test.fltVal;
			(*foundObj) = displayObjectCopy;
			m_toolPtr->SyncDisplayAndSceneObjects(foundObj->m_ID);
		}

		//Finalize update command and append to tools command stack
		updateCommand->FinishUpdate();
		updateCommand->Execute(m_toolPtr);

	}
	return TRUE;
}

#ifdef _DEBUG

void MFCTransformView::VisualizeSelectionOnTreeCtrl(const ToolMain& tool)
{
	std::vector<DisplayObject*> sceneCopy = tool.m_d3dRenderer.m_displayList;
	m_treeCtrl.DeleteAllItems();


	int maxTreeDepth = 0;
	for (DisplayObject* object : sceneCopy)
	{
		int d = object->GetDepth();
		if (maxTreeDepth < d)
			maxTreeDepth = d;
	}
	std::map<const DisplayObject*, HTREEITEM> treeItems;

	for (size_t i = 0; i < maxTreeDepth; i++)
	{
		for (int j = 0; j < sceneCopy.size(); ++j)
		{

			if(sceneCopy[j]->GetDepth() == i)
			{
				auto element = sceneCopy[j];
				if (element->parentObject == nullptr)
				{
					HTREEITEM treeitem = m_treeCtrl.InsertItem(std::to_wstring(element->m_ID).c_str());
					treeItems.insert({ element, treeitem });
				}
				else
				{
					if (treeItems.find(element->parentObject) != treeItems.end())
					{
						HTREEITEM parentItem = treeItems.at(element->parentObject);
						HTREEITEM treeitem = m_treeCtrl.InsertItem(
							std::to_wstring(element->m_ID).c_str()
							, parentItem);
						treeItems.insert({ element, treeitem });
					}
				}
			}
		}
	}



	//Reset all checkboxes to false
	UncheckAllTreeItems(m_treeCtrl);

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
	UpdatePropertyGridSelection(&(m_toolPtr->GetSelectedDisplayObjects()));
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
