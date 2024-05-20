// CreateObjectDialog.cpp : implementation file
//

#include "ToolMain.h"
#include "DisplayObject.h"
#include "afxdialogex.h"
#include "CreateObjectDialog.h"

#include "resource.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "AddObjectCommand.h"
#include "experimental/filesystem"

namespace fs = std::experimental::filesystem;


// CreateObjectDialog dialog

IMPLEMENT_DYNAMIC(CreateObjectDialog, CDialogEx)

CreateObjectDialog::CreateObjectDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	ObjectPrototype = new DisplayObject();
	auto modelProperties = new CMFCPropertyGridProperty("Model");
	static TCHAR BASED_CODE modelFilter[] = _T(
		"Visual Studio Starter Kit Files(*.cmo)|*.cmo|"
		"Direct X SDK(*.sdkmesh)|*.sdkmesh|"
		"VBO (*.vbo)|*.vbo||"
		"");
	modelProperties->AddSubItem(
		new CMFCPropertyGridFileProperty(
			_T("Model Mesh"),
			TRUE,
			ObjectPrototype->model_path.c_str(),
			_T("ico"),
			0,
			modelFilter,
			_T("Specifies the dialog icon"),
			reinterpret_cast<DWORD_PTR>(&ObjectPrototype->model_path)));

	//Texture Mesh Dialog
	//----------------------------------------------path
	static TCHAR BASED_CODE textureFilter[] = _T(
		"DDS (*.dds)|*.dds||"
		"");
	modelProperties->AddSubItem(
		new CMFCPropertyGridFileProperty(
			_T("Texture"),
			TRUE,
			ObjectPrototype->texture_path.c_str(),
			_T("dds"),
			0,
			textureFilter,
			_T("Specifies the dialog icon"),
			reinterpret_cast<DWORD_PTR>(&ObjectPrototype->texture_path)));
			
			
	this->m_grid.AddProperty(modelProperties, 1, 1);

}

CreateObjectDialog::~CreateObjectDialog()
{
}

void CreateObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_grid);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_grid);
}

LRESULT CreateObjectDialog::OnTransformPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	
	CMFCPropertyGridProperty* propChanged = (CMFCPropertyGridProperty*)lparam;


	std::string* d = reinterpret_cast<std::string*>(propChanged->GetData());
	auto test = propChanged->GetValue();
	std::wstring ws(test.bstrVal, SysStringLen(test.bstrVal));
	std::string s(ws.begin(), ws.end());

	//TODO make to relative path
// Get the current working directory
	std::string absolute_path = s;
	std::string relative_path;
	ConvertToRelativePath(absolute_path, relative_path);
	propChanged->SetValue(CString(relative_path.c_str()));
	(*d) = relative_path;
	return TRUE;
}

void CreateObjectDialog::SetObjectData(ToolMain* tool)
{
	this->tool_main = tool;
}

bool CreateObjectDialog::ConvertToRelativePath(const std::string& absolute_path, std::string& relative_path)
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


BEGIN_MESSAGE_MAP(CreateObjectDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CreateObjectDialog::OnBnClickedOk)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CreateObjectDialog::OnTransformPropertyChanged)
END_MESSAGE_MAP()


// CreateObjectDialog message handlers


void CreateObjectDialog::OnBnClickedOk()
{

	auto command = new AddObjectCommand( new DisplayObject(*ObjectPrototype));
	command->Execute(tool_main);
	delete ObjectPrototype;
	ObjectPrototype = nullptr;
	
//.	auto maxID= std::max_element(this->tool_main->m_sceneGraph.begin(),
//.		tool_main->m_sceneGraph.end(),
//.		[](const SceneObject& obj, const SceneObject& objOther) {return obj.ID < objOther.ID; });
//.	ObjectPrototype.ID = maxID->ID + 1;
//.	this->tool_main->m_sceneGraph.push_back(ObjectPrototype);
//.	this->tool_main->Notify(*tool_main);
//.	auto displayObj =
//.		this->tool_main->m_d3dRenderer.CreateDisplayObject(&tool_main->m_sceneGraph[tool_main->m_sceneGraph.size() - 1]);
//.	tool_main->m_d3dRenderer.AddDisplayObject(displayObj);
	CDialogEx::OnOK();
}


