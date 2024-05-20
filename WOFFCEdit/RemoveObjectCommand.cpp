#include "RemoveObjectCommand.h"

#include "ToolMain.h"
#include "DisplayObject.h"
RemoveObjectCommand::RemoveObjectCommand(std::vector<int> ids)
{
	m_objectIds = ids;
}

void RemoveObjectCommand::Execute(ToolMain* tool)
{
	ExecuteSilent(tool);
	tool->AddCommandToStack(this);
	tool->Notify(*tool);
}

void RemoveObjectCommand::ExecuteSilent(ToolMain* tool)
{
	//For each of the presented ids
	for (int id : m_objectIds)
	{

		std::vector<int> ids;
		tool->GetHiearchyOf(id, &ids);

		for (int idInObjectHiearchy : ids)
		{
			auto subject = new DisplayObject();

			//Copy the information to hold in the command
			(*subject) =
			*tool->m_d3dRenderer.GetDisplayObject(idInObjectHiearchy);
			tool->DeleteById(idInObjectHiearchy);
			m_subjects.push_back(subject);
		}



		
	}
	tool->ClearSelection();

}

void RemoveObjectCommand::Revert(ToolMain* tool)
{
	for (DisplayObject* subject : m_subjects)
	{
		tool->InsertObject(subject);
		tool->AddToSelection(subject->m_ID);
	}
	m_subjects.clear();
	tool->Notify(*tool);
}
