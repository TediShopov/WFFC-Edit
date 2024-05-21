#pragma once

#include <afxext.h>
#include <stack>

#include "pch.h"
#include "Game.h"
#include "sqlite3.h"
#include "SceneObject.h"
#include "InputCommands.h"
#include "Observer.h"
//#include "DisplayObject.h"
class SceneCommand;
using namespace DirectX;
enum ObjectTransformEditMode
{
	MODE_POSITION,
	MODE_SCALE,
	MODE_ROTATION
};
class ToolMain;
struct ToolMainChanges
{
	bool SelectionUpdates;
	bool ObjectUpdates;
	bool HierarchyUpdates;

	ToolMain* Tool;

};



class ToolStateBase;

class ToolMain : public m_Subject<ToolMainChanges>
{
public: //methods
	ToolMain();
	~ToolMain() override;

	void Tick(MSG* msg);
	void UpdateInput(MSG* msg);

	//onAction - These are the interface to MFC
	void onActionLoad(); //load the current chunk
	void onActionInitialise(HWND handle, int width, int height);

	afx_msg void onActionSave(); //save the current chunk
	afx_msg void onActionSaveTerrain(); //save chunk geometry

	void DeleteSelected();
	void DeleteById(int id);
	void GetHiearchyOf(int id, std::vector<int>* hierarchy) const;
	void CreateObject();

	void ChangeState(ToolStateBase* newState);
	ToolStateBase* GetToolStateFromEditType() ;
	DisplayObject* InsertObject(DisplayObject* prototype);

	void Notify(const ToolMainChanges& data) override;
	void Notify(bool selection =false, bool objects = false, bool hierarchy = false) ;

	bool IsTransformActionInputted() const;
	int getCurrentSelectionID() const;
	std::vector<int> getCurrentSelectionIDs() const; //returns the IDS of the selected objects.

	bool HasSelectedObject() const;
	void ClearSelection();
	void AddToSelection(int id);
	void RemoveFromSelection(int id);

	//Return pointer to the display objects in the renderer
	std::vector<DisplayObject*> GetSelectedDisplayObjects();
	//Return pointer to the objects that are to be stored in the database
	std::vector<SceneObject*> GetSelectedObjects(); 

	void AddCommandToStack(SceneCommand* command);
	void UndoCommand();
	void RedoCommand();

	void SynchroniseSceneToDisplayObject(int i);
public: //variables
	Game m_d3dRenderer; //Instance of D3D rendering system for our tool
	ChunkObject m_chunk; //our landscape chunk
	std::vector<SceneObject> m_sceneGraph; //our scenegraph storing all the objects in the current chunk
	ToolStateBase* toolState; //tool state which is updated every frame.
	ObjectTransformEditMode editMode; // mode of the next edit tool state

private: //methods

	void InitHandlesDefaults();// a method to initialize visual handles (widgets)
	void TransferCommand(std::vector<SceneCommand*>* bufferOne,
		std::vector<SceneCommand*>* bufferTwo);
	void EraseFirstIfExceeeding(std::vector<SceneCommand*>* buffer);

	bool KeyReleased(char key) const;

	void ResetInputKeyBuffer();

	SceneObject* GetById(int ID) ;

private: //variables



	//the max size for both buffers. If max size is exceeded
	//elements are removed from the front
	int m_commandBufferMaxSize;
	//a buffer holding the commands that could be undone
	std::vector<SceneCommand*> m_commandBuffer;
	// buffer holding the commands that could be redone
	std::vector<SceneCommand*> m_undoedCommandBuffer;

	std::vector<int> m_selectedObject; //ID of current Selection
	char m_keyArray[256];
	char m_PrevFramekeyArray[256];
	bool m_handlesInitialized = false; //If handle default are already constructed
	int m_width; //dimensions passed to directX
	int m_height;
	int m_currentChunk; //the current chunk of thedatabase that we are operating on.  Dictates loading and saving.

	HWND m_toolHandle; //Handle to the  window
	InputCommands m_toolInputCommands; //input commands that we want to use and possibly pass over to the renderer
	CRect m_windowRect; //Window area rectangle.
	ControlHandle* m_activeHandle; //the control handle(widget) that is dragged if any
	sqlite3* m_databaseConnection; //sqldatabase handle
};

