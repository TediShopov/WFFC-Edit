#include "ObjectTransformState.h"

#include "ObjectSelectionState.h"
#include "ToolMain.h"

void ObjectTransformState::Init(ToolMain* tool, const InputCommands& comms)
{
	this->MainTool = tool;
	if (this->MainTool->m_selectedObject.size() == 1)
	{
		if (comms.handleHit == true)
		{
			//Mouse button should be releases as well as any other transform realted button
			//to exit this state
			release_mouse_needed = true;
		}

		on_selection_commands = comms;
		GetLocalVectors(this->MainTool->m_selectedObject[0], selected_object_axes);
		GetLocalPlanes(
			this->MainTool->m_selectedObject[0],
			selected_object_planes);
	}
}

void ObjectTransformState::Update(const InputCommands& input)
{
	XMVECTOR plane;
	XMVECTOR axis;

	XMVECTOR newPosition;

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

	if (this->MainTool->m_selectedObject.size() == 1)
	{
		SceneObject& obj = this->MainTool->m_sceneGraph[this->MainTool->m_selectedObject[0]];

		POINT p;
		GetCursorPos(&p);
		XMVECTOR mouseWorldPos =
			this->MainTool->m_d3dRenderer.GetWorldRay(
				input.mouse_x,
				input.mouse_y,
				1000);
		if (on_selection_commands.CTRL_Down)
		{
			if (on_selection_commands.plane_x)
			{
				axis = selected_object_axes[1];
				plane = selected_object_planes[2];
			}
			else if (on_selection_commands.plane_y)
			{
				axis = selected_object_axes[2];
				plane = selected_object_planes[2];
			}
			else
			{
				axis = selected_object_axes[3];
				plane = selected_object_planes[1];
			}

			newPosition = MoveOnAxis(
				mouseWorldPos, plane, axis);
		}
		else
		{
			if (on_selection_commands.plane_x == true)
				plane = selected_object_planes[1];
			else if (on_selection_commands.plane_y == true)
				plane = selected_object_planes[2];
			else
				plane = selected_object_planes[3];
			newPosition = MoveOnPlane(
				mouseWorldPos, plane);
		}

		obj.posX = newPosition.m128_f32[0];
		obj.posY = newPosition.m128_f32[1];
		obj.posZ = newPosition.m128_f32[2];
		this->MainTool->m_d3dRenderer.UpdateDisplayElementTransform(
			this->MainTool->m_selectedObject[0], &this->MainTool->m_sceneGraph);
		//
		this->MainTool->Notify(*this->MainTool);
	}
	else
	{
		this->MainTool->ChangeState(new ObjectSelectionState());
	}
}

void ObjectTransformState::GetLocalVectors(int objectIndex, XMVECTOR vecs[4])
{
	XMMATRIX local = this->MainTool->m_d3dRenderer
		.GetObjectLocalMatrix(objectIndex);

	vecs[0] =
		XMVECTOR{ 0,0,0,1 };
	vecs[1] =
		XMVECTOR{ 1,0,0,1 };
	vecs[2] =
		XMVECTOR{ 0,1,0,1 };
	vecs[3] =
		XMVECTOR{ 0,0,1,1 };
	for (int i = 0; i < 4; ++i)
	{
		//Transform to local coordinates
		vecs[i] =
			XMVector3Transform(vecs[i], local);
	}
}

void ObjectTransformState::GetLocalPlanes(int objectIndex, DirectX::XMVECTOR planes[3])
{
	//GetLocalVectors(objectIndex, selected_object_axes);

	//Local vector are in Origin, X, Y, Z order

	//Pressing x key --> YZ plane
	planes[0] =
		DirectX::XMPlaneFromPoints(selected_object_axes[0], selected_object_axes[2], selected_object_axes[3]);
	//Pressing y key --> XZ plane
	planes[1] =
		DirectX::XMPlaneFromPoints(selected_object_axes[0], selected_object_axes[1], selected_object_axes[3]);
	//Pressing z key --> XY plane
	planes[2] =
		DirectX::XMPlaneFromPoints(selected_object_axes[0], selected_object_axes[1], selected_object_axes[2]);
}

XMVECTOR ObjectTransformState::MoveOnAxis(XMVECTOR mouseWorldRay, XMVECTOR plane, XMVECTOR rayOnPlane)
{
	XMVECTOR origin = selected_object_axes[0];

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
		this->MainTool->m_d3dRenderer.m_camPosition,
		mouseWorldRay
	);
	return intersectionPos;
}