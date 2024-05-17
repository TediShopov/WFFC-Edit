#pragma once

#include <afxext.h>
#include "pch.h"
#include "Game.h"
#include "sqlite3.h"
#include "SceneObject.h"
#include "InputCommands.h"
#include <vector>

#include "Observer.h"
using namespace DirectX;

class ToolStateBase;
class ToolMain : public Subject<ToolMain>
{
public: //methods
	ToolMain();
	~ToolMain();

	//onAction - These are the interface to MFC
	int		getCurrentSelectionID();										//returns the selection number of currently selected object so that It can be displayed.
	std::vector<int>  getCurrentSelectionIDs();									//returns the IDS of the selected objects.
	void	onActionInitialise(HWND handle, int width, int height);			//Passes through handle and hieght and width and initialises DirectX renderer and SQL LITE
	void	onActionFocusCamera();
	void	onActionLoad();													//load the current chunk
	afx_msg	void	onActionSave();											//save the current chunk
	afx_msg void	onActionSaveTerrain();									//save chunk geometry

	void	Tick(MSG* msg);
	void	UpdateInput(MSG* msg);
	Game	m_d3dRenderer;		//Instance of D3D rendering system for our tool
	ToolStateBase* ToolState;
	void Notify(const ToolMain& data) override;
	void ChangeState(ToolStateBase* newState);

public:	//variables
	std::vector<SceneObject>    m_sceneGraph;	//our scenegraph storing all the objects in the current chunk
	ChunkObject					m_chunk;		//our landscape chunk
	std::vector<int> m_selectedObject;						//ID of current Selection
	bool ShouldStartSelectDragging() const;

private:	//methods
	ControlHandle* m_activeHandle;
	void InitHandlesDefaults();
	void ResetInputKeyBuffer();

	//std::vector<int> PositionHandles;
	void	onContentAdded();

private:	//variables

	bool	m_handlesInitialized = false; //If handle default are already constructed
	HWND	m_toolHandle;		//Handle to the  window
	InputCommands m_toolInputCommands;		//input commands that we want to use and possibly pass over to the renderer
	CRect	WindowRECT;		//Window area rectangle.
	char	m_keyArray[256];
	sqlite3* m_databaseConnection;	//sqldatabase handle

	int m_width;		//dimensions passed to directX
	int m_height;
	int m_currentChunk;			//the current chunk of thedatabase that we are operating on.  Dictates loading and saving.
};
