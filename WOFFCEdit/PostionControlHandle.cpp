#include "ToolMain.h"
#include "PostionControlHandle.h"
#include "ObjectTransformState.h"
using  DirectX::XMVECTOR;
using DirectX::SimpleMath::Quaternion;

Quaternion PostionControlHandle::FromToQuaternion(
	DirectX::XMVECTOR from,
	DirectX::XMVECTOR to)
{
	Quaternion q;
	XMVECTOR cross = XMVector3Cross(from, to);
	q.x = cross.m128_f32[0];
	q.y = cross.m128_f32[1];
	q.z = cross.m128_f32[2];

	//q.w = sqrt((v1.Length ^ 2) * (v2.Length ^ 2)) + dotproduct(v1, v2);
	q.w = sqrt((powf(XMVector3Length(from).m128_f32[0], 2)) *
		(powf(XMVector3Length(to).m128_f32[0], 2)) +
		XMVector3Dot(from, to).m128_f32[0]
	);

	q.Normalize();
	return q;
}

//PostionControlHandle::PostionControlHandle(
//	ToolMain* tool,
//	float x, float y, float z,
//	float x1, float y1, float z1,
//	const SceneObject* scene_object,
//	XMVECTOR color)
//{
//
//
//}

PostionControlHandle::PostionControlHandle(AXES axesType, ToolMain* tool, std::string model_path,
	DirectX::XMVECTOR color)
{
	this->Type = axesType;
	Color = color;
	this->mainTool = tool;

	//DisplayObject* mesh = new DisplayObject();
	this->mainTool->m_d3dRenderer
		.CreateHandleObject(this, model_path, Color);
	this->m_scale.x = 1.5f;
	this->m_scale.y = 0.1;
	this->m_scale.z = 0.1;

	//Orient handle to the correct direction using Quaternions
	// the expected direction of the object is X positive
	XMVECTOR localDirection = ObjectTransformState::LocalAxes.r[((int)(this->Type))];
	Rotation = FromToQuaternion(ObjectTransformState::LocalAxes.r[1], localDirection);
}

PostionControlHandle::PostionControlHandle(AXES axesType, AXES axesTypeTwo, ToolMain* tool, std::string model_path,
	DirectX::XMVECTOR color)
{
	move_on_axis = false;

	//Get the only remaining axis
	int indexOfRemainigAxes = 0;
	for (int i = 1; i <= 3; ++i)
	{
		if (axesType != (AXES)i && axesTypeTwo != (AXES)i)
		{
			indexOfRemainigAxes = i;
			break;
		}
	}
	this->Type = (AXES)indexOfRemainigAxes;
	Color = color;
	this->mainTool = tool;

	//DisplayObject* mesh = new DisplayObject();
	this->mainTool->m_d3dRenderer
		.CreateHandleObject(this, model_path, Color);

	auto axes = ObjectTransformState::LocalAxes;

	this->m_scale = (axes.r[axesType] + axes.r[axesTypeTwo]);
	this->m_scale *= 0.6;
	this->m_scale += (axes.r[this->Type] * 0.001);

	this->m_position = XMVector3Rotate(this->m_position, Rotation);
}

//
DirectX::XMMATRIX PostionControlHandle::GetWorldMatrix() const
{
	const DirectX::XMVECTORF32 scale = { this->m_scale.x, this->m_scale.y, this->m_scale.z };
	const DirectX::XMVECTORF32 translate = { this->m_position.x, this->m_position.y, this->m_position.z };

	DirectX::XMMATRIX local =
		XMMatrixTransformation(
			DirectX::g_XMZero,
			DirectX::SimpleMath::Quaternion::Identity,
			scale, DirectX::g_XMZero, Rotation, translate);
	if (this->parentObject != nullptr)
	{
		return   local * this->parentObject->GetWorldMatrix();
	}
	return local;
}
//
void PostionControlHandle::Update()
{
	if (this->mainTool->m_selectedObject.size() != 1)
	{
		this->m_render = false;
	}
	else
	{
		this->m_render = true;
		this->parentObject = this->mainTool->m_d3dRenderer.GetDisplayObject
		(this->mainTool->m_selectedObject[0]);
	}
}

ToolStateBase* PostionControlHandle::OnMouseClick()
{
	return new ObjectTransformState(this->Type, move_on_axis);
}