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
	XMVECTOR newPosition;
	auto sel = MainTool->GetSelectedObjects();
	if (sel.size() == 1)
	{
		SceneObject* obj = sel[0];

		POINT p;
		GetCursorPos(&p);
		XMVECTOR mouseWorldPos =
			this->MainTool->m_d3dRenderer.GetWorldRay(
				input.mouse_x,
				input.mouse_y,
				1000);

		if (move_on_axis)
		{
			newPosition = MoveOnAxis(
				mouseWorldPos, plane, axis);
		}
		else
		{
			newPosition = MoveOnPlane(
				mouseWorldPos, plane);
		}

		obj->posX = newPosition.m128_f32[0];
		obj->posY = newPosition.m128_f32[1];
		obj->posZ = newPosition.m128_f32[2];
		
		this->MainTool->m_d3dRenderer.UpdateDisplayElementTransform(
			sel[0]->ID - 1, &this->MainTool->m_sceneGraph);
		//
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
		axis = world_axes.r[this->axisType];

		//Usually planes on the same index as an axis is
		//constructed from selected object origin and that axis as normal

		//Plane at 0 index is invalid by default, so get any index that
		//is not zero or axis index
		if (this->axisType == 3)
			plane = world_planes.r[2];
		else
			plane = world_planes.r[this->axisType + 1];
	}
}

void ObjectTransformState::FromInput(const InputCommands& input)
{
	move_on_axis = input.CTRL_Down;

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

XMVECTOR ObjectTransformState::MoveOnAxis(XMVECTOR mouseWorldRay, XMVECTOR plane, XMVECTOR rayOnPlane)
{
	XMVECTOR origin = world_axes.r[0];

	XMVECTOR globalPlaneIntersection = MoveOnPlane(mouseWorldRay, plane);
	XMVECTOR localVectorToPoint = globalPlaneIntersection - origin;
	XMVECTOR localUnitAxis = XMVector3Normalize(rayOnPlane - origin);
	XMVECTOR dot = XMVector3Dot(localUnitAxis, localVectorToPoint);
	XMVECTOR res = localUnitAxis * dot.m128_f32[0] + origin;

	return res;
	//	return XMVector3Dot(originToPoint, rayOnPlane * 1000);
}

XMVECTOR ObjectTransformState::MoveOnPlane(XMVECTOR mouseWorldRay, XMVECTOR plane)
{
	XMVECTOR intersectionPos = XMPlaneIntersectLine(
		plane,
		this->MainTool->m_d3dRenderer.camera.m_camPosition,
		mouseWorldRay
	);
	return intersectionPos;
}
