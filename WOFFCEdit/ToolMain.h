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
class ToolStateBase;

class ToolMain : public Subject<ToolMain>
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
	void CreateObject();
	DisplayObject* InsertObject(DisplayObject* prototype);

	void ChangeState(ToolStateBase* newState);
	void Notify(const ToolMain& data) override;

	bool IsTransformActionInputted() const;
	ToolStateBase* GetNewTransformUpdateState() ;
	int getCurrentSelectionID() const;
	std::vector<int> getCurrentSelectionIDs() const; //returns the IDS of the selected objects.


public: //variables
	Game m_d3dRenderer; //Instance of D3D rendering system for our tool
	ChunkObject m_chunk; //our landscape chunk
	ToolStateBase* ToolState;
	std::vector<SceneObject> m_sceneGraph; //our scenegraph storing all the objects in the current chunk
	ObjectTransformEditMode EditMode;

	bool HasSelectedObject();
	void ClearSelection();
	void AddToSelection(int id);
	void RemoveFromSelection(int id);
	std::vector<DisplayObject*> GetSelectedDisplayObjects();
	std::vector<SceneObject*> GetSelectedObjects();

	void AddCommandToStack(SceneCommand* command);
	void UndoCommand();
	void RedoCommand();





	//Synchronize Diplsay and scene objects

	void SyncDisplayAndSceneObjects(int i);

private: //methods

		void TransferCommand(std::vector<SceneCommand*>* bufferOne,
			std::vector<SceneCommand*>* bufferTwo);
		void EraseFirstIfExceeeding(std::vector<SceneCommand*>* buffer);


	void InitHandlesDefaults();
	void ResetInputKeyBuffer();
	bool KeyReleased(char key) const;

	SceneObject* GetById(int ID) ;

private: //variables
	int m_commandBufferMaxSize;
	std::vector<SceneCommand*> m_commandBuffer;
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
	ControlHandle* m_activeHandle;
	sqlite3* m_databaseConnection; //sqldatabase handle
};

