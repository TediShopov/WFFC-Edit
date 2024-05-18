#pragma once

#include <afxext.h>
#include "pch.h"
#include "Game.h"
#include "sqlite3.h"
#include "SceneObject.h"
#include "InputCommands.h"
#include "Observer.h"
using namespace DirectX;

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
	void CreateObject();

	void ChangeState(ToolStateBase* newState);
	void Notify(const ToolMain& data) override;

	bool ShouldStartSelectDragging() const;
	int getCurrentSelectionID() const;
	std::vector<int> getCurrentSelectionIDs() const; //returns the IDS of the selected objects.


public: //variables
	Game m_d3dRenderer; //Instance of D3D rendering system for our tool
	ChunkObject m_chunk; //our landscape chunk
	ToolStateBase* ToolState;
	std::vector<int> m_selectedObject; //ID of current Selection
	std::vector<SceneObject> m_sceneGraph; //our scenegraph storing all the objects in the current chunk

private: //methods
	void InitHandlesDefaults();
	void ResetInputKeyBuffer();

private: //variables

	char m_keyArray[256];
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
