#pragma once
#include "InputCommands.h"
class ToolMain;

class ToolStateBase
{
public:
	ToolMain* MainTool;
	ToolStateBase() { MainTool = nullptr; }

	void virtual Init(ToolMain* tool, const InputCommands&) = 0;

	void virtual Update(const InputCommands& commands) = 0;
};
