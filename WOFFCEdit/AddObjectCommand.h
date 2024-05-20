#pragma once
#include "SceneCommand.h"
//#include "DisplayObject.h"

class DisplayObject;
class AddObjectCommand : public SceneCommand
{
public:
	AddObjectCommand(DisplayObject* subject);
	void Execute(ToolMain* tool) override;
	void ExecuteSilent(ToolMain* tool) override;
	void Revert(ToolMain* tool) override;
private:
	DisplayObject* Subject;

};

