#pragma once
#include "SceneCommand.h"
//This include is neccesary
#include "ToolMain.h"
#include "DisplayObject.h"

class UpdateObjectCommand : public SceneCommand
{
public:
	UpdateObjectCommand(DisplayObject* old);
	void FinishUpdate();
	void Execute(ToolMain* tool) override;
	void ExecuteSilent(ToolMain* tool) override;
	void Revert(ToolMain* tool) override;
private:
	//Pointer useful to 
	DisplayObject* m_ptrToUnfinished;
	DisplayObject m_oldData;
	DisplayObject m_updatedData;
};

