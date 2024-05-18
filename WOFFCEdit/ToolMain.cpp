#include "ToolMain.h"
//#include "pch.h"
#include <vector>
#include <sstream>

#include "ObjectSelectionState.h"
#include "PostionControlHandle.h"
#include "CreateObjectDialog.h"

//#include "CreateObjectDialog.h"
const std::string DefaultArrowModel = "database/data/placeholder.cmo";
const std::string DefaultArrowTexture = "database/data/placeholder.dds";
//
//ToolMain Class
ToolMain::ToolMain()
{
	m_currentChunk = 0; //default value

	m_sceneGraph.clear(); //clear the vector for the scenegraph
	m_databaseConnection = NULL;

	//zero input commands
	m_toolInputCommands.forward = false;
	m_toolInputCommands.back = false;
	m_toolInputCommands.left = false;
	m_toolInputCommands.right = false;
	m_toolInputCommands.mouse_x = 0;
	m_toolInputCommands.mouse_y = 0;
	m_toolInputCommands.mouse_LB_Down = false;
	m_toolInputCommands.CTRL_Down = false;
	ToolState = new ObjectSelectionState();
	ToolState->Init(this, m_toolInputCommands);
}

ToolMain::~ToolMain()
{
	sqlite3_close(m_databaseConnection); //close the database connection
}

int ToolMain::getCurrentSelectionID() const
{
	return m_selectedObject[0];
}

std::vector<int> ToolMain::getCurrentSelectionIDs() const
{
	return m_selectedObject;
}

void ToolMain::onActionInitialise(HWND handle, int width, int height)
{
	//window size, handle etc for directX
	m_width = width;
	m_height = height;

	m_d3dRenderer.Initialize(handle, m_width, m_height);
	m_toolHandle = handle;

	//database connection establish
	int rc;
	rc = sqlite3_open_v2("database/test.db", &m_databaseConnection, SQLITE_OPEN_READWRITE, NULL);

	if (rc)

	{
		TRACE("Can't open database");
		//if the database cant open. Perhaps a more catastrophic error would be better here
	}
	else
	{
		TRACE("Opened database successfully");
	}

	onActionLoad();

	//After build list has been initialized from scene
	InitHandlesDefaults();
}

void ToolMain::onActionLoad()
{
	//load current chunk and objects into lists
	if (!m_sceneGraph.empty()) //is the vector empty
	{
		m_sceneGraph.clear(); //if not, empty it
	}

	//SQL
	int rc;
	char* sqlCommand;
	char* ErrMSG = 0;
	sqlite3_stmt* pResults; //results of the query
	sqlite3_stmt* pResultsChunk;

	//OBJECTS IN THE WORLD
	//prepare SQL Text
	sqlCommand = "SELECT * from Objects"; //sql command which will return all records from the objects table.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);

	//loop for each row in results until there are no more rows.  ie for every row in the results. We create and object
	while (sqlite3_step(pResults) == SQLITE_ROW)
	{
		SceneObject newSceneObject;
		newSceneObject.ID = sqlite3_column_int(pResults, 0);
		newSceneObject.chunk_ID = sqlite3_column_int(pResults, 1);
		newSceneObject.model_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 2));
		newSceneObject.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 3));
		newSceneObject.posX = sqlite3_column_double(pResults, 4);
		newSceneObject.posY = sqlite3_column_double(pResults, 5);
		newSceneObject.posZ = sqlite3_column_double(pResults, 6);
		newSceneObject.rotX = sqlite3_column_double(pResults, 7);
		newSceneObject.rotY = sqlite3_column_double(pResults, 8);
		newSceneObject.rotZ = sqlite3_column_double(pResults, 9);
		newSceneObject.scaX = sqlite3_column_double(pResults, 10);
		newSceneObject.scaY = sqlite3_column_double(pResults, 11);
		newSceneObject.scaZ = sqlite3_column_double(pResults, 12);
		newSceneObject.render = sqlite3_column_int(pResults, 13);
		newSceneObject.collision = sqlite3_column_int(pResults, 14);
		newSceneObject.collision_mesh = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 15));
		newSceneObject.collectable = sqlite3_column_int(pResults, 16);
		newSceneObject.destructable = sqlite3_column_int(pResults, 17);
		newSceneObject.health_amount = sqlite3_column_int(pResults, 18);
		newSceneObject.editor_render = sqlite3_column_int(pResults, 19);
		newSceneObject.editor_texture_vis = sqlite3_column_int(pResults, 20);
		newSceneObject.editor_normals_vis = sqlite3_column_int(pResults, 21);
		newSceneObject.editor_collision_vis = sqlite3_column_int(pResults, 22);
		newSceneObject.editor_pivot_vis = sqlite3_column_int(pResults, 23);
		newSceneObject.pivotX = sqlite3_column_double(pResults, 24);
		newSceneObject.pivotY = sqlite3_column_double(pResults, 25);
		newSceneObject.pivotZ = sqlite3_column_double(pResults, 26);
		newSceneObject.snapToGround = sqlite3_column_int(pResults, 27);
		newSceneObject.AINode = sqlite3_column_int(pResults, 28);
		newSceneObject.audio_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 29));
		newSceneObject.volume = sqlite3_column_double(pResults, 30);
		newSceneObject.pitch = sqlite3_column_double(pResults, 31);
		newSceneObject.pan = sqlite3_column_int(pResults, 32);
		newSceneObject.one_shot = sqlite3_column_int(pResults, 33);
		newSceneObject.play_on_init = sqlite3_column_int(pResults, 34);
		newSceneObject.play_in_editor = sqlite3_column_int(pResults, 35);
		newSceneObject.min_dist = sqlite3_column_double(pResults, 36);
		newSceneObject.max_dist = sqlite3_column_double(pResults, 37);
		newSceneObject.camera = sqlite3_column_int(pResults, 38);
		newSceneObject.path_node = sqlite3_column_int(pResults, 39);
		newSceneObject.path_node_start = sqlite3_column_int(pResults, 40);
		newSceneObject.path_node_end = sqlite3_column_int(pResults, 41);
		newSceneObject.parent_id = sqlite3_column_int(pResults, 42);
		newSceneObject.editor_wireframe = sqlite3_column_int(pResults, 43);
		newSceneObject.name = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 44));

		newSceneObject.light_type = sqlite3_column_int(pResults, 45);
		newSceneObject.light_diffuse_r = sqlite3_column_double(pResults, 46);
		newSceneObject.light_diffuse_g = sqlite3_column_double(pResults, 47);
		newSceneObject.light_diffuse_b = sqlite3_column_double(pResults, 48);
		newSceneObject.light_specular_r = sqlite3_column_double(pResults, 49);
		newSceneObject.light_specular_g = sqlite3_column_double(pResults, 50);
		newSceneObject.light_specular_b = sqlite3_column_double(pResults, 51);
		newSceneObject.light_spot_cutoff = sqlite3_column_double(pResults, 52);
		newSceneObject.light_constant = sqlite3_column_double(pResults, 53);
		newSceneObject.light_linear = sqlite3_column_double(pResults, 54);
		newSceneObject.light_quadratic = sqlite3_column_double(pResults, 55);

		//send completed object to scenegraph
		m_sceneGraph.push_back(newSceneObject);
	}

	//THE WORLD CHUNK
	//prepare SQL Text
	sqlCommand = "SELECT * from Chunks";
	//sql command which will return all records from  chunks table. There is only one tho.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResultsChunk, 0);

	sqlite3_step(pResultsChunk);
	m_chunk.ID = sqlite3_column_int(pResultsChunk, 0);
	m_chunk.name = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 1));
	m_chunk.chunk_x_size_metres = sqlite3_column_int(pResultsChunk, 2);
	m_chunk.chunk_y_size_metres = sqlite3_column_int(pResultsChunk, 3);
	m_chunk.chunk_base_resolution = sqlite3_column_int(pResultsChunk, 4);
	m_chunk.heightmap_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 5));
	m_chunk.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 6));
	m_chunk.tex_splat_alpha_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 7));
	m_chunk.tex_splat_1_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 8));
	m_chunk.tex_splat_2_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 9));
	m_chunk.tex_splat_3_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 10));
	m_chunk.tex_splat_4_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 11));
	m_chunk.render_wireframe = sqlite3_column_int(pResultsChunk, 12);
	m_chunk.render_normals = sqlite3_column_int(pResultsChunk, 13);
	m_chunk.tex_diffuse_tiling = sqlite3_column_int(pResultsChunk, 14);
	m_chunk.tex_splat_1_tiling = sqlite3_column_int(pResultsChunk, 15);
	m_chunk.tex_splat_2_tiling = sqlite3_column_int(pResultsChunk, 16);
	m_chunk.tex_splat_3_tiling = sqlite3_column_int(pResultsChunk, 17);
	m_chunk.tex_splat_4_tiling = sqlite3_column_int(pResultsChunk, 18);

	//Process REsults into renderable
	//m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
	m_d3dRenderer.BuildDisplayHierarchy(&m_sceneGraph);
	//build the renderable chunk
	m_d3dRenderer.BuildDisplayChunk(&m_chunk);
}

void ToolMain::onActionSave()
{
	//SQL
	int rc;
	char* sqlCommand;
	char* ErrMSG = 0;
	sqlite3_stmt* pResults; //results of the query

	//OBJECTS IN THE WORLD Delete them all
	//prepare SQL Text
	sqlCommand = "DELETE FROM Objects"; //will delete the whole object table.   Slightly risky but hey.
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	sqlite3_step(pResults);

	//Populate with our new objects
	std::wstring sqlCommand2;
	int numObjects = m_sceneGraph.size(); //Loop thru the scengraph.

	for (int i = 0; i < numObjects; i++)
	{
		std::stringstream command;
		command << "INSERT INTO Objects "
			<< "VALUES(" << m_sceneGraph.at(i).ID << ","
			<< m_sceneGraph.at(i).chunk_ID << ","
			<< "'" << m_sceneGraph.at(i).model_path << "'" << ","
			<< "'" << m_sceneGraph.at(i).tex_diffuse_path << "'" << ","
			<< m_sceneGraph.at(i).posX << ","
			<< m_sceneGraph.at(i).posY << ","
			<< m_sceneGraph.at(i).posZ << ","
			<< m_sceneGraph.at(i).rotX << ","
			<< m_sceneGraph.at(i).rotY << ","
			<< m_sceneGraph.at(i).rotZ << ","
			<< m_sceneGraph.at(i).scaX << ","
			<< m_sceneGraph.at(i).scaY << ","
			<< m_sceneGraph.at(i).scaZ << ","
			<< m_sceneGraph.at(i).render << ","
			<< m_sceneGraph.at(i).collision << ","
			<< "'" << m_sceneGraph.at(i).collision_mesh << "'" << ","
			<< m_sceneGraph.at(i).collectable << ","
			<< m_sceneGraph.at(i).destructable << ","
			<< m_sceneGraph.at(i).health_amount << ","
			<< m_sceneGraph.at(i).editor_render << ","
			<< m_sceneGraph.at(i).editor_texture_vis << ","
			<< m_sceneGraph.at(i).editor_normals_vis << ","
			<< m_sceneGraph.at(i).editor_collision_vis << ","
			<< m_sceneGraph.at(i).editor_pivot_vis << ","
			<< m_sceneGraph.at(i).pivotX << ","
			<< m_sceneGraph.at(i).pivotY << ","
			<< m_sceneGraph.at(i).pivotZ << ","
			<< m_sceneGraph.at(i).snapToGround << ","
			<< m_sceneGraph.at(i).AINode << ","
			<< "'" << m_sceneGraph.at(i).audio_path << "'" << ","
			<< m_sceneGraph.at(i).volume << ","
			<< m_sceneGraph.at(i).pitch << ","
			<< m_sceneGraph.at(i).pan << ","
			<< m_sceneGraph.at(i).one_shot << ","
			<< m_sceneGraph.at(i).play_on_init << ","
			<< m_sceneGraph.at(i).play_in_editor << ","
			<< m_sceneGraph.at(i).min_dist << ","
			<< m_sceneGraph.at(i).max_dist << ","
			<< m_sceneGraph.at(i).camera << ","
			<< m_sceneGraph.at(i).path_node << ","
			<< m_sceneGraph.at(i).path_node_start << ","
			<< m_sceneGraph.at(i).path_node_end << ","
			<< m_sceneGraph.at(i).parent_id << ","
			<< m_sceneGraph.at(i).editor_wireframe << ","
			<< "'" << m_sceneGraph.at(i).name << "'" << ","

			<< m_sceneGraph.at(i).light_type << ","
			<< m_sceneGraph.at(i).light_diffuse_r << ","
			<< m_sceneGraph.at(i).light_diffuse_g << ","
			<< m_sceneGraph.at(i).light_diffuse_b << ","
			<< m_sceneGraph.at(i).light_specular_r << ","
			<< m_sceneGraph.at(i).light_specular_g << ","
			<< m_sceneGraph.at(i).light_specular_b << ","
			<< m_sceneGraph.at(i).light_spot_cutoff << ","
			<< m_sceneGraph.at(i).light_constant << ","
			<< m_sceneGraph.at(i).light_linear << ","
			<< m_sceneGraph.at(i).light_quadratic

			<< ")";
		std::string sqlCommand2 = command.str();
		rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand2.c_str(), -1, &pResults, 0);
		sqlite3_step(pResults);
	}
	MessageBox(NULL, L"Objects Saved", L"Notification", MB_OK);
}

void ToolMain::onActionSaveTerrain()
{
	m_d3dRenderer.SaveDisplayChunk(&m_chunk);
}

void ToolMain::DeleteSelected()
{
	if(this->m_selectedObject.size() >= 1)
	{
		for (int i = 0; i < m_selectedObject.size(); ++i)
		{
			//this->m_sceneGraph.erase(this->m_sceneGraph.begin() + m_selectedObject[i]);
			this->m_d3dRenderer.RemoveDisplayObject(this->m_selectedObject[i]);
		}
		this->m_selectedObject.clear();
		Notify(*this);
		m_toolInputCommands.deleteSelected = false;
	}
}

void ToolMain::CreateObject()
{
	ResetInputKeyBuffer();
	CreateObjectDialog newD(nullptr);
	newD.SetObjectData(this);
	newD.DoModal();
	m_toolInputCommands.insertObject = false;
	//	MessageBox(NULL, L"Insert New Object", L"Notification",MB_OK);
}

void ToolMain::Tick(MSG* msg)
{
	//Put the selected object as handles transform

	if (m_toolInputCommands.deleteSelected)
	{
		DeleteSelected();
	}

	if (m_toolInputCommands.insertObject)
	{
		CreateObject();
	}


	m_toolInputCommands.handleHit = false;
	//If hanle is picked by mouse

	if (m_toolInputCommands.mouse_LB_Down)
	{
		if (this->m_activeHandle == nullptr)
		{
			ControlHandle* selecedHandle = m_d3dRenderer.ControlHandleHitTest();
			this->m_activeHandle = selecedHandle;
			if (selecedHandle != nullptr)
				this->ChangeState(selecedHandle->OnMouseClick());
		}
	}
	else
	{
		this->m_activeHandle = nullptr;
	}
	ToolState->Update(m_toolInputCommands);
	m_d3dRenderer.Tick(&m_toolInputCommands);

	//Toggle mouse arc ball control 
	if (this->m_selectedObject.size() == 1)
	{
		this->m_d3dRenderer.camera.isArcBallMode = m_toolInputCommands.CTRL_Down;
		this->m_d3dRenderer.camera.arcBallTarget =
			this->m_d3dRenderer.GetDisplayObject(m_selectedObject[0])->GetWorldPosition();
		//Get the world posiiuton of the object
	}
}

void ToolMain::UpdateInput(MSG* msg)
{
	//TODO ASSIGN MOUSE LB INPUT
	switch (msg->message)
	{
	//Global inputs,  mouse position and keys etc
	case WM_KEYDOWN:
		m_keyArray[msg->wParam] = true;
		break;

	case WM_KEYUP:
		m_keyArray[msg->wParam] = false;
		break;

	case WM_MOUSEMOVE:
		m_toolInputCommands.mouse_x = GET_X_LPARAM(msg->lParam);
		m_toolInputCommands.mouse_y = GET_Y_LPARAM(msg->lParam);
		break;

	case WM_LBUTTONDOWN: //mouse button down,  you will probably need to check when its up too
		m_toolInputCommands.mouse_LB_Down = true;
		break;
	case WM_LBUTTONUP: //mouse button down,  you will probably need to check when its up too
		m_toolInputCommands.mouse_LB_Down = false;
		break;
	case WM_RBUTTONDOWN:
		m_toolInputCommands.mouse_RB_Down = true;
		break;
	case WM_RBUTTONUP:
		m_toolInputCommands.mouse_RB_Down = false;
		break;
	}
	m_toolInputCommands.scroll_wheel_delta =
		GET_WHEEL_DELTA_WPARAM(msg->wParam) / 120;


	//here we update all the actual app functionality that we want.  This information will either be used int toolmain, or sent down to the renderer (Camera movement etc
	//WASD movement
	m_toolInputCommands.forward = m_keyArray['W'];
	m_toolInputCommands.CTRL_Down = m_keyArray[VK_CONTROL];

	m_toolInputCommands.back = m_keyArray['S'];
	m_toolInputCommands.left = m_keyArray['A'];

	m_toolInputCommands.right = m_keyArray['D'];
	//rotation
	m_toolInputCommands.rotRight = m_keyArray['E'];
	m_toolInputCommands.rotLeft = m_keyArray['Q'];

	//Plane and axis movement keys
	m_toolInputCommands.plane_x = m_keyArray['Z'];
	m_toolInputCommands.plane_y = m_keyArray['X'];
	m_toolInputCommands.plane_z = m_keyArray['C'];

	//Insert Key Pressed
	m_toolInputCommands.insertObject = m_keyArray[45];
	//Delete1 Key Pressed
	m_toolInputCommands.deleteSelected = m_keyArray[46];
}

void ToolMain::Notify(const ToolMain& data)
{
	for (Observer<ToolMain>* observer : observers_)
	{
		observer->Update(this, data);
	}
}

void ToolMain::ChangeState(ToolStateBase* newState)
{
	if (newState == nullptr) return;
	if (ToolState != nullptr)
	{
		//Do on state end;
	}
	ToolState = newState;
	ToolState->Init(this, m_toolInputCommands);
}

bool ToolMain::ShouldStartSelectDragging() const
{
	return (m_toolInputCommands.plane_x ||
		m_toolInputCommands.plane_y
		|| m_toolInputCommands.plane_z) && m_selectedObject.size() == 1;
}

bool ToolMain::HasSelectedObject()
{
	return this->m_selectedObject.size() >= 1;
}

void ToolMain::ClearSelection()
{ m_selectedObject.clear(); }

std::vector<DisplayObject*> ToolMain::GetSelectedDisplayObjects()
{
	std::vector<DisplayObject*> selectedDisplayObjects;
	for (int i = 0; i < m_selectedObject.size(); ++i)
	{
		selectedDisplayObjects.push_back(
			this->m_d3dRenderer.GetDisplayObject(i)
		);
	}
	return selectedDisplayObjects;
	
}

std::vector<SceneObject*> ToolMain::GetSelectedObjects()
{
	std::vector<SceneObject*> selectedObjects;
	for (int i = 0; i < m_selectedObject.size(); ++i)
	{
		selectedObjects.push_back(&m_sceneGraph[m_selectedObject[i]]);
	}
	return selectedObjects;
}

void ToolMain::AddToSelection(int index)
{

	if (std::find(
		m_selectedObject.begin(),
		m_selectedObject.end(),
		index) == m_selectedObject.end()) {
		this->m_selectedObject.push_back(index);
	}
}

void ToolMain::RemoveFromSelection(int index)
{
	auto iter = std::find(
		m_selectedObject.begin()
				, m_selectedObject.end(),
				index);
	m_selectedObject.erase(iter);
}

void ToolMain::InitHandlesDefaults()
{
	//Defines a scene object that is to converted to display object
	SceneObject tempPositionHandle;

	///CREATE PLANE DRAG HANDLES
	m_d3dRenderer.AddVisualHandle(new PostionControlHandle(
		Z_AXIS,
		Y_AXIS,
		this,
		DefaultArrowModel,
		Colors::Blue
	));
	m_d3dRenderer.AddVisualHandle(new PostionControlHandle(
		X_AXIS,
		Y_AXIS,
		this,
		DefaultArrowModel,
		Colors::Red
	));
	m_d3dRenderer.AddVisualHandle(new PostionControlHandle(
		X_AXIS,
		Z_AXIS,
		this,
		DefaultArrowModel,
		Colors::Green
	));
	//	//CREATE HANDLES TO MOVE OBJECT BY LOCAL AXIS
	//	//Add x position handle
	m_d3dRenderer.AddVisualHandle(new PostionControlHandle(
		X_AXIS,
		this,
		DefaultArrowModel,
		Colors::Red
	));
	//Add z position handle
	m_d3dRenderer.AddVisualHandle(new PostionControlHandle(
		Y_AXIS,
		this,
		DefaultArrowModel,
		Colors::Green
	));
	//CREATE HANDLES TO MOVE OBJECT BY LOCAL AXIS
	//Add y position handle
	m_d3dRenderer.AddVisualHandle(new PostionControlHandle(
		Z_AXIS,
		this,
		DefaultArrowModel,
		Colors::Blue
	));
}

void ToolMain::ResetInputKeyBuffer()
{
	for (int i = 0; i < 256; ++i)
	{
		m_keyArray[i] = false;
	}
}
