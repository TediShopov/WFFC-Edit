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
		//return;
	}
	std::vector<int>& sel = MainTool->m_selectedObject;
	if (commands.mouse_LB_Down)
	{
		int selectedId =
			MainTool->m_d3dRenderer.MousePicking();
		if (commands.CTRL_Down == true)
		{
			//Add selection to the list of selections
			//only if it is unique
			if (std::find(
				sel.begin(),
				sel.end(),
				selectedId) == sel.end()) {
				sel.push_back(selectedId);
			}
		}
		else
		{
			//Reset selection and add newly selected
			sel.clear();
			if (selectedId != -1)
				sel.push_back(selectedId);
		}

		this->MainTool->Notify(*MainTool);
	}
}