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
	tool->SyncDisplayAndSceneObjects(updatedData.m_ID);
	tool->Notify(*tool);
	tool->AddCommandToStack(this);
}

void UpdateObjectCommand::Revert(ToolMain* tool)
{
	//Find by id
	DisplayObject* actual =
	tool->m_d3dRenderer.GetDisplayObject(updatedData.m_ID);
	(*actual) = oldData;
	tool->SyncDisplayAndSceneObjects(updatedData.m_ID);
	tool->Notify(*tool);
}