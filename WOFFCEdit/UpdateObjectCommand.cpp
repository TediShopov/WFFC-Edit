#include "UpdateObjectCommand.h"



UpdateObjectCommand::UpdateObjectCommand(DisplayObject* old)
{
	oldData = *old;
	ptrToUnfinished = old;
}

void UpdateObjectCommand::FinishUpdate()
{
	updatedData = *ptrToUnfinished;
}

void UpdateObjectCommand::Execute(ToolMain* tool)
{
	ExecuteSilent(tool);
	tool->AddCommandToStack(this);
}

void UpdateObjectCommand::ExecuteSilent(ToolMain* tool)
{
	DisplayObject* actual =
	tool->m_d3dRenderer.GetDisplayObject(updatedData.m_ID);
	(*actual) = updatedData;
	tool->SynchroniseSceneToDisplayObject(updatedData.m_ID);
	bool hierarchyUpdated = oldData.parentObject != updatedData.parentObject;
	tool->Notify(false,true,hierarchyUpdated);
}

void UpdateObjectCommand::Revert(ToolMain* tool)
{
	//Find by id
	DisplayObject* actual =
	tool->m_d3dRenderer.GetDisplayObject(updatedData.m_ID);
	(*actual) = oldData;
	tool->SynchroniseSceneToDisplayObject(updatedData.m_ID);
	bool hierarchyUpdated = oldData.parentObject != updatedData.parentObject;
	tool->Notify(false,true,hierarchyUpdated);
}
