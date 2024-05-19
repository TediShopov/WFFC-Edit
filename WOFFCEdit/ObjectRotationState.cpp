#include "ObjectRotationState.h"

#include "ObjectSelectionState.h"
#include "ToolMain.h"
#include "PostionControlHandle.h"

ObjectRotationState::ObjectRotationState()
	:AxisBasedTransformState()
{
	m_lastPosition=Vector3();
}

ObjectRotationState::ObjectRotationState(AXES global_direction, bool a)
	:AxisBasedTransformState(axisType)
{
	m_lastPosition=Vector3();
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



		if (axisType == X_AXIS)
		{
			global_direction = world_axes_directions.r[2];
			plane = world_planes.r[3];
		}
		else if (axisType == Y_AXIS)
		{
			global_direction = world_axes_directions.r[1];
			plane = world_planes.r[2];
		}
		else
		{
			global_direction = world_axes_directions.r[3];
			plane = world_planes.r[1];
		}
	return;
}

float ObjectRotationState::GetWorldCoordinatesDelta(const InputCommands& commands)
{
	XMVECTOR mouseWorldRay;
	GetMouseWorldRay(commands, mouseWorldRay);

	XMVECTOR worldPosition = AxisIntersection(
		mouseWorldRay, plane, GetGlobalOrigin() + global_direction);

	XMVECTOR local = worldPosition - GetGlobalOrigin();



	//cos() to project one vector to the other and find delta
//	XMVECTOR dot = XMVector3Dot(worldPosition, m_lastPosition);
//	float d = dot.m128_f32[0];
	XMVECTOR direction = local - m_lastPosition;
	float d = XMVector3Length(direction).m128_f32[0];


	XMVECTOR projectToAxis = XMVector3Dot(direction, GetGlobalOrigin() + global_direction);
	if(projectToAxis.m128_f32[0]<0)
	{
		d = -d;
	}






	//Update the last position
	m_lastPosition = local;
	return d;
}

float ObjectRotationState::GetMouseNDCDelta(const InputCommands& commands)
{
	float xNDC = this->MainTool->m_d3dRenderer.camera.GetDeltaXNDC();
	return xNDC;


}

XMVECTOR ObjectRotationState::RotateAroundSelectedAxis(const InputCommands& commands)
{
	Vector3 current = SelectedObject->m_orientation;
	//All other rotation use delta from X normalized device coordaintes
//	float delta = this->MainTool->m_d3dRenderer.camera.GetDeltaXNDC();
	float delta = GetMouseNDCDelta(commands);
	float angle = delta * 100;





	if (axisType == X_AXIS)
		current.x += angle;
	else if (axisType == Y_AXIS)
		current.y += angle;
	else
		current.z += angle;

//	float angle = delta * 10;
//	XMMATRIX matrixOfRotationAroundAxis=  XMMatrixRotationAxis(global_direction, angle);
//	current = XMVector3TransformNormal(current, matrixOfRotationAroundAxis);
	return current;
}
