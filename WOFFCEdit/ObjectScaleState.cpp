#include "ObjectScaleState.h"

#include "ToolMain.h"
#include <SimpleMath.h>

#include "ObjectSelectionState.h"
#include "PostionControlHandle.h"

ObjectScaleState::ObjectScaleState()
{
}

ObjectScaleState::ObjectScaleState(AXES global_direction, bool a)
{
}

void ObjectScaleState::Init(ToolMain* tool, const InputCommands& input_commands)
{
	AxisBasedTransformState::Init(tool, input_commands);

	//Scale speed is also dependent on the sstarting scale

	m_scaleRatio = 0.1;
	if (axisType == X_AXIS)
		m_scaleSpeed = m_scaleRatio * SelectedObject->m_scale.x;
	else if (axisType == Y_AXIS)
		m_scaleSpeed = m_scaleRatio * SelectedObject->m_scale.y;
	else
		m_scaleSpeed = m_scaleRatio * SelectedObject->m_scale.z;

	XMVECTOR mouseWorldRay;
	GetMouseWorldRay(input_commands, mouseWorldRay);
	this->m_lastPosition = CardinalAxisIntersection(mouseWorldRay, axisType) - GetGlobalOrigin();

}

void ObjectScaleState::Update(const InputCommands& input)
{
	AxisBasedTransformState::Update(input);
	auto sel = MainTool->GetSelectedDisplayObjects();
	if (sel.size() == 1)
	{
		SimpleMath::Vector3 newScale = ScaleOnSelectedAxis(input);
		sel[0]->m_scale = newScale;
		this->MainTool->SyncDisplayAndSceneObjects(sel[0]->m_ID);
		this->MainTool->Notify(*this->MainTool);
	}
	else
	{
		this->MainTool->ChangeState(new ObjectSelectionState());
	}
}



XMVECTOR ObjectScaleState::ScaleOnSelectedAxis(const InputCommands& commands)
{
	
	Vector3 current = SelectedObject->m_scale;
	//All other rotation use delta from X normalized device coordaintes
//	float delta = this->MainTool->m_d3dRenderer.camera.GetDeltaXNDC();
	float delta = GetWorldCoordinatesDelta(commands);
	float total = delta * m_scaleSpeed;
	if (axisType == X_AXIS)
		current.x += total;
	else if (axisType == Y_AXIS)
		current.y += total;
	else
		current.z += total;

//	float angle = delta * 10;
//	XMMATRIX matrixOfRotationAroundAxis=  XMMatrixRotationAxis(global_direction, angle);
//	current = XMVector3TransformNormal(current, matrixOfRotationAroundAxis);
	return current;
}

float ObjectScaleState::GetWorldCoordinatesDelta(const InputCommands& commands)
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
