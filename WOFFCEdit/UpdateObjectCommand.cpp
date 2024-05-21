#include "UpdateObjectCommand.h"



UpdateObjectCommand::UpdateObjectCommand(DisplayObject* old)
{
	m_oldData = *old;
	m_ptrToUnfinished = old;
}

void UpdateObjectCommand::FinishUpdate()
{
	m_updatedData = *m_ptrToUnfinished;
}

void UpdateObjectCommand::Execute(ToolMain* tool)
{
	ExecuteSilent(tool);
	tool->AddCommandToStack(this);
}

void UpdateObjectCommand::ExecuteSilent(ToolMain* tool)
{
	DisplayObject* actual =
	tool->m_d3dRenderer.GetDisplayObject(m_updatedData.m_ID);
	(*actual) = m_updatedData;
	tool->SynchroniseSceneToDisplayObject(m_updatedData.m_ID);
	bool hierarchyUpdated = m_oldData.parentObject != m_updatedData.parentObject;
	tool->Notify(false,true,hierarchyUpdated);
}

void UpdateObjectCommand::Revert(ToolMain* tool)
{
	//Find by id
	DisplayObject* actual =
	tool->m_d3dRenderer.GetDisplayObject(m_updatedData.m_ID);
	(*actual) = m_oldData;
	tool->SynchroniseSceneToDisplayObject(m_updatedData.m_ID);
	bool hierarchyUpdated = m_oldData.parentObject != m_updatedData.parentObject;
	tool->Notify(false,true,hierarchyUpdated);
}
