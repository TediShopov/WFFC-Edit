#include "ObjectSelectionState.h"

#include "ToolMain.h"
#include "ObjectTransformState.h"
#include "DisplayObject.h"

void ObjectSelectionState::Init(ToolMain* tool, const InputCommands&)
{
	MainTool = tool;
}

void ObjectSelectionState::Update(const InputCommands& commands)
{
	if (MainTool->IsTransformActionInputted())
	{
		//this->MainTool->ChangeState(new ObjectSelectionState());
		//auto newTransfomrStateHacky = new ObjectTransformState();
		//		newTransfomrStateHacky->Init(this->MainTool, commands);
		this->MainTool->ChangeState(this->MainTool->GetToolStateFromEditType());
		return;
	}
	//std::vector<int>& sel = MainTool->m_selectedObject;
	auto sel = MainTool->GetSelectedObjects();
	if (commands.mouseLB)
	{
		auto selected =
			MainTool->m_d3dRenderer.MousePicking();
		if (selected == nullptr)
		{
			MainTool->ClearSelection();
			
		}
		else
		{
			if (commands.ctrl == true)
			{
				MainTool->AddToSelection(selected->m_ID);
			}
			else
			{
				MainTool->ClearSelection();
				MainTool->AddToSelection(selected->m_ID);
			}
			
		}

		this->MainTool->Notify(true,false,false);
	}
}