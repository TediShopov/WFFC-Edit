#pragma once


class ToolMain;

//A base class that alter the scene in some way and can be reverted
class SceneCommand
{
public:
	virtual void Execute(ToolMain* tool)=0;
	virtual void ExecuteSilent(ToolMain* tool)=0;
	virtual void Revert(ToolMain* tool)=0;
};


