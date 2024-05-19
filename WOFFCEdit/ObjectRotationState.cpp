#include "ObjectRotationState.h"

#include "ObjectSelectionState.h"
#include "ToolMain.h"
#include "PostionControlHandle.h"

ObjectRotationState::ObjectRotationState()
{
}

ObjectRotationState::ObjectRotationState(AXES axis, bool a)
{
}

void ObjectRotationState::Update(const InputCommands& input)
{
	AxisBasedTransformState::Update(input);

	auto sel = MainTool->GetSelectedObjects();
	if (sel.size() == 1)
	{
		SimpleMath::Vector3 newOrientation = RotateAroundSelectedAxis(input);

		sel[0]->rotX = newOrientation.x;
		sel[0]->rotY = newOrientation.y;
		sel[0]->rotZ = newOrientation.z;


		this->MainTool->SyncDisplayAndSceneObjects(sel[0]->ID);
		this->MainTool->Notify(*this->MainTool);
	}
	else
	{
		this->MainTool->ChangeState(new ObjectSelectionState());
	}
		
}

void ObjectRotationState::FromInput(const InputCommands& input)
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
			axis = world_axes.r[1];
			plane = world_planes.r[2];
		}
		else if (axisType == Y_AXIS)
		{
			axis = world_axes.r[2];
			plane = world_planes.r[2];
		}
		else
		{
			axis = world_axes.r[3];
			plane = world_planes.r[1];
		}
	}
	else
	{
		if (axisType == X_AXIS)
			plane = world_planes.r[0];
		else if (axisType == Y_AXIS)
			plane = world_planes.r[1];
		else
			plane = world_planes.r[2];
	}
	return;
}

XMVECTOR ObjectRotationState::RotateAroundSelectedAxis(const InputCommands& commands)
{
	Vector3 current = SelectedObject->m_orientation;
	//All other rotation use delta from X normalized device coordaintes
	float delta = this->MainTool->m_d3dRenderer.camera.GetDeltaXNDC();
	float angle = delta * 100;

	if (axisType == X_AXIS)
		current.x += angle;
	else if (axisType == Y_AXIS)
		current.y += angle;
	else
		current.z += angle;

//	float angle = delta * 10;
//	XMMATRIX matrixOfRotationAroundAxis=  XMMatrixRotationAxis(axis, angle);
//	current = XMVector3TransformNormal(current, matrixOfRotationAroundAxis);
	return current;
}
