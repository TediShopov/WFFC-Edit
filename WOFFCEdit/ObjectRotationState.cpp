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

void ObjectRotationState::Init(ToolMain* tool, const InputCommands& input_commands)
{
	AxisBasedTransformState::Init(tool, input_commands);

	m_rotationSpeed = 50;
	XMVECTOR mouseWorldRay;
	GetMouseWorldRay(input_commands, mouseWorldRay);
	this->m_lastPosition = CardinalAxisIntersection(mouseWorldRay, axisType) - GetGlobalOrigin();
}

void ObjectRotationState::Update(const InputCommands& input)
{
	AxisBasedTransformState::Update(input);

	auto sel = MainTool->GetSelectedDisplayObjects();
	if (sel.size() == 1)
	{
		SimpleMath::Vector3 newOrientation = RotateAroundSelectedAxis(input);
		sel[0]->m_orientation = newOrientation;
		this->MainTool->SyncDisplayAndSceneObjects(sel[0]->m_ID);
		this->MainTool->Notify(*this->MainTool);
	}
	else
	{
		this->MainTool->ChangeState(new ObjectSelectionState());
	}
		
}


float ObjectRotationState::GetWorldCoordinatesDelta(const InputCommands& commands)
{
	XMVECTOR mouseWorldRay;
	GetMouseWorldRay(commands, mouseWorldRay);

	XMVECTOR worldPosition = CardinalAxisIntersection(mouseWorldRay, axisType);

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
	float delta = GetWorldCoordinatesDelta(commands);
	float angle = delta *m_rotationSpeed;
	bool move_on_axis;





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
