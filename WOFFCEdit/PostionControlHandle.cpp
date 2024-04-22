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

PostionControlHandle::PostionControlHandle(AXES axesType, ToolMain* tool, const SceneObject* scene_obj,
	DirectX::XMVECTOR color)
{
	this->Type = axesType;
	Color = color;
	this->mainTool = tool;
	DisplayObject* mesh = tool->m_d3dRenderer.CreateDisplayObject(scene_obj);

	//Orient handle to the correct direction using Quaternions
	// the expected direction of the object is X positive
	XMVECTOR localDirection = ObjectTransformState::LocalAxes.r[((int)(axesType))];
	Rotation = FromToQuaternion(ObjectTransformState::LocalAxes.r[1], localDirection);

	m_model = mesh->m_model;
	m_texture_diffuse = mesh->m_texture_diffuse;
	m_orientation.x = mesh->m_orientation.x;
	m_orientation.y = mesh->m_orientation.y;
	m_orientation.z = mesh->m_orientation.z;
	m_position.x = mesh->m_position.x;
	m_position.y = mesh->m_position.y;
	m_position.z = mesh->m_position.z;
	m_scale.x = mesh->m_scale.x;
	m_scale.y = mesh->m_scale.y;
	m_scale.z = mesh->m_scale.z;
	m_render = mesh->m_render;
	m_wireframe = mesh->m_wireframe;

	m_light_type = 0;

	//Change the color to the one in the ctor
	m_light_diffuse_r = Color.m128_f32[0];
	m_light_diffuse_g = Color.m128_f32[1];
	m_light_diffuse_b = Color.m128_f32[2];

	m_light_specular_r = 0.0f;	m_light_specular_g = 0.0f;	m_light_specular_b = 0.0f;
	m_light_spot_cutoff = 0.0f;
	m_light_constant = 0.0f;
	m_light_linear = 0.0f;
	m_light_quadratic = 0.0f;
	this->m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetDiffuseColor(this->Color);
			}
		});
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
	return new ObjectTransformState(this->Type, true);
}