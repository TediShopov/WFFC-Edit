#include "AddObjectCommand.h"

#include "ToolMain.h"
#include "DisplayObject.h"

AddObjectCommand::AddObjectCommand(DisplayObject* subject)
{
	//Subject =  new DisplayObject(*subject);
	Subject =  subject;
}

void AddObjectCommand::Execute(ToolMain* tool)
{
	ExecuteSilent(tool);
	tool->AddCommandToStack(this);
}

void AddObjectCommand::ExecuteSilent(ToolMain* tool)
{
	Subject = tool->InsertObject(Subject);
	tool->Notify(true,true,true);
}

void AddObjectCommand::Revert(ToolMain* tool)
{
	tool->RemoveFromSelection(Subject->m_ID);
	tool->DeleteById(Subject->m_ID);
	tool->Notify(true,true,true);

}
