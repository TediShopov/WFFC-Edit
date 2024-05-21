#include "AddObjectCommand.h"

#include "ToolMain.h"
#include "DisplayObject.h"

AddObjectCommand::AddObjectCommand(DisplayObject* subject)
{
	m_Subject =  subject;
}

void AddObjectCommand::Execute(ToolMain* tool)
{
	ExecuteSilent(tool);
	tool->AddCommandToStack(this);
}

void AddObjectCommand::ExecuteSilent(ToolMain* tool)
{
	m_Subject = tool->InsertObject(m_Subject);
	tool->Notify(true,true,true);
}

void AddObjectCommand::Revert(ToolMain* tool)
{
	tool->RemoveFromSelection(m_Subject->m_ID);
	tool->DeleteById(m_Subject->m_ID);
	tool->Notify(true,true,true);

}
