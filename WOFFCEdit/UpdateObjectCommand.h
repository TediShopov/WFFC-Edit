#pragma once
#include "SceneCommand.h"
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
	DisplayObject* ptrToUnfinished;
	DisplayObject oldData;
	DisplayObject updatedData;
};

