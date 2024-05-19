#pragma once


class ToolMain;
class SceneCommand
{
public:
	virtual void Execute(ToolMain* tool)=0;
	virtual void Revert(ToolMain* tool)=0;
};


