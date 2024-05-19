#include "ObjectTransformState.h"

#include "ObjectSelectionState.h"
#include "ToolMain.h"
#include  "PostionControlHandle.h"

#include "Camera.h"

ObjectTransformState::ObjectTransformState()
	: AxisBasedTransformState()
{
}

ObjectTransformState::ObjectTransformState(AXES axisType, bool a)
	: AxisBasedTransformState(axisType)
{
	this->move_on_axis = a;
}


void ObjectTransformState::Update(const InputCommands& input)
{
	AxisBasedTransformState::Update(input);

	world_axes_directions = GetWorldAxes(SelectedObject);
	world_planes = GetWorldPlanes(SelectedObject);
	XMVECTOR newPosition;
	auto sel = MainTool->GetSelectedObjects();
	if (sel.size() == 1)
	{
		SceneObject* obj = sel[0];

		XMVECTOR mouseWorldPos;
		GetMouseWorldRay(input, mouseWorldPos);

		if (move_on_axis)
		{
			newPosition = CardinalAxisIntersection(
				mouseWorldPos, axisType);
		}
		else

		{
			newPosition = PlaneIntersection(
				mouseWorldPos, plane);
		}

		obj->posX = newPosition.m128_f32[0];
		obj->posY = newPosition.m128_f32[1];
		obj->posZ = newPosition.m128_f32[2];
		
		this->MainTool->SyncDisplayAndSceneObjects(sel[0]->ID);
		this->MainTool->Notify(*this->MainTool);
	}
	else
	{
		this->MainTool->ChangeState(new ObjectSelectionState());
	}
}

void ObjectTransformState::Init(ToolMain* tool, const InputCommands& input_commands)
{
	AxisBasedTransformState::Init(tool, input_commands);
	if (move_on_axis)
	{
		global_direction = world_axes_directions.r[this->axisType];

		//Usually planes on the same index as an global_direction is
		//constructed from selected object origin and that global_direction as normal

		//Plane at 0 index is invalid by default, so get any index that
		//is not zero or global_direction index
		if (this->axisType == 3)
			plane = world_planes.r[2];
		else
			plane = world_planes.r[this->axisType + 1];
	}
}

void ObjectTransformState::FromInput(const InputCommands& input)
{
	move_on_axis = input.CTRL_Down;

	if (input.plane_x)
		axisType = X_AXIS;
	else if (input.plane_y)
		axisType = Y_AXIS;
	else if (input.plane_z)
		axisType = Z_AXIS;



	if (move_on_axis)
	{
		if (axisType == X_AXIS)
		{
			global_direction = world_axes_directions.r[1];
			plane = world_planes.r[2];
		}
		else if (axisType == Y_AXIS)
		{
			global_direction = world_axes_directions.r[2];
			plane = world_planes.r[2];
		}
		else
		{
			global_direction = world_axes_directions.r[3];
			plane = world_planes.r[1];
		}
	}
	else
	{
		if (axisType == X_AXIS)
			plane = world_planes.r[1];
		else if (axisType == Y_AXIS)
			plane = world_planes.r[2];
		else
			plane = world_planes.r[3];
	}
	return;
}


