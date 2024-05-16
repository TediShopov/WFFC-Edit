#include "AxisBasedTransformState.h"

#include "ObjectSelectionState.h"
#include "ToolMain.h"
#include  "PostionControlHandle.h"

XMMATRIX AxisBasedTransformState::LocalAxes =
{
	XMVECTOR{ 0,0,0,0 },
	XMVECTOR{ 1,0,0,0 },
	XMVECTOR{ 0,1,0,0 },
	XMVECTOR{ 0,0,1,0 }
};
AxisBasedTransformState::AxisBasedTransformState()
{
	from_handle = false;
}

AxisBasedTransformState::AxisBasedTransformState(AXES axisType)
{
	from_handle = true;
	release_mouse_needed = true;
	this->axisType = axisType;
}

void AxisBasedTransformState::Init(ToolMain* tool, const InputCommands& comms)
{
	this->MainTool = tool;
	on_selection_commands = comms;
	if (this->MainTool->m_selectedObject.size() != 1) return;

	SelectedObject = this->MainTool->m_d3dRenderer.GetDisplayObject(
		this->MainTool->m_selectedObject[0]);
	world_axes = this->GetWorldAxes(SelectedObject);
	world_planes = this->GetWorldPlanes(SelectedObject);
	if (!from_handle)
	{
		FromInput(comms);
	}

	this->plane = world_planes.r[axisType];

	if (comms.handleHit == true)
	{
		//Mouse button should be releases as well as any other transform realted button
		//to exit this state
		release_mouse_needed = true;
	}

	//	if (on_selection_commands.CTRL_Down)
	//	{
	//		if (on_selection_commands.plane_x)
	//		{
	//			axis = world_axes.r[1];
	//			plane = world_planes.r[2];
	//		}
	//		else if (on_selection_commands.plane_y)
	//		{
	//			axis = world_axes.r[2];
	//			plane = world_planes.r[2];
	//		}
	//		else
	//		{
	//			axis = world_axes.r[3];
	//			plane = world_planes.r[1];
	//		}
	//	}
}

void AxisBasedTransformState::Update(const InputCommands& input)
{
	world_axes = GetWorldAxes(SelectedObject);
	world_planes = GetWorldPlanes(SelectedObject);
	if (MainTool->ShouldStartSelectDragging() == false)
	{
		if (release_mouse_needed)
		{
			if (input.mouse_LB_Down == false)
			{
				this->MainTool->ChangeState(new ObjectSelectionState());
			}
		}
		else
		{
			this->MainTool->ChangeState(new ObjectSelectionState());
		}
	}
}

void AxisBasedTransformState::FromInput(const InputCommands& input)
{
	//		if (move_on_axis)
	//		{
	//			if (axisType == X_AXIS)
	//			{
	//				axis = world_axes.r[1];
	//				plane = world_planes.r[2];
	//			}
	//			else if (axisType == Y_AXIS)
	//			{
	//				axis = world_axes.r[2];
	//				plane = world_planes.r[2];
	//			}
	//			else
	//			{
	//				axis = world_axes.r[3];
	//				plane = world_planes.r[1];
	//			}
	//		}
	//		else
	//		{
	//			if (axisType == X_AXIS)
	//				plane = world_planes.r[0];
	//			else if (axisType == Y_AXIS)
	//				plane = world_planes.r[1];
	//			else
	//				plane = world_planes.r[2];
	//		}
	//		return;
}

XMMATRIX AxisBasedTransformState::GetWorldAxes(const DisplayObject* selected) const
{
	XMMATRIX world = selected->GetWorldMatrix();
	XMMATRIX axes;

	axes.r[0] = { 0,0,0,0 };
	axes.r[1] = XMVector3TransformNormal(LocalAxes.r[1], world);
	axes.r[2] = XMVector3TransformNormal(LocalAxes.r[2], world);
	axes.r[3] = XMVector3TransformNormal(LocalAxes.r[3], world);
	return axes;
	//return XMMatrixMultiply(LocalAxes, world);
}

XMMATRIX AxisBasedTransformState::GetWorldPlanes(const DisplayObject* selected) const
{
	XMMATRIX planes;
	//Empty vector at position zero
	planes.r[0] = XMVECTOR();

	//Local vector are in Origin, X, Y, Z order
	//Pressing x key --> YZ plane
	planes.r[1] =
		XMPlaneFromPointNormal(world_axes.r[0], world_axes.r[1]);
	//Pressing y key --> XZ plane
	planes.r[2] =
		XMPlaneFromPointNormal(world_axes.r[0], world_axes.r[2]);
	//Pressing z key --> XY plane
	planes.r[3] =
		XMPlaneFromPointNormal(world_axes.r[0], world_axes.r[3]);
	return planes;
}