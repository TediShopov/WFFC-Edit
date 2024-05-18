#include "ObjectSelectionState.h"

#include "ToolMain.h"
#include "ObjectTransformState.h"

void ObjectSelectionState::Init(ToolMain* tool, const InputCommands&)
{
	MainTool = tool;
}

void ObjectSelectionState::Update(const InputCommands& commands)
{
	if (MainTool->ShouldStartSelectDragging())
	{
		//this->MainTool->ChangeState(new ObjectSelectionState());
		auto newTransfomrStateHacky = new ObjectTransformState();
		//		newTransfomrStateHacky->Init(this->MainTool, commands);
		this->MainTool->ChangeState(newTransfomrStateHacky);
		return;
	}
	//std::vector<int>& sel = MainTool->m_selectedObject;
	auto sel = MainTool->GetSelectedObjects();
	if (commands.mouse_LB_Down)
	{
		int selectedId =
			MainTool->m_d3dRenderer.MousePicking();
		if (selectedId == -1)
		{
			MainTool->ClearSelection();
			
		}
		else
		{
			if (commands.CTRL_Down == true)
			{
				MainTool->AddToSelection(selectedId);
			}
			else
			{
				MainTool->ClearSelection();
				MainTool->AddToSelection(selectedId);
			}
			
		}

		this->MainTool->Notify(*MainTool);
	}
}