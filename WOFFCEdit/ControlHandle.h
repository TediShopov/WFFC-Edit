#pragma once
#include "DisplayObject.h"

class ToolMain;
class ToolStateBase;

class ControlHandle : public DisplayObject
{
public:

	ToolMain* mainTool;
	virtual void Update() = 0;
	virtual ToolStateBase* OnMouseClick() = 0;
private:
};
