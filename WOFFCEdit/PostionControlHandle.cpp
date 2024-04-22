#include "ToolMain.h"
#include "PostionControlHandle.h"
#include "ObjectTransformState.h"
using  DirectX::XMVECTOR;
using DirectX::SimpleMath::Quaternion;
PostionControlHandle::PostionControlHandle(
	ToolMain* tool,
	DirectX::XMVECTOR handleOrientation, DisplayObject* mesh,
	XMVECTOR color)
{
	this->mainTool = tool;
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
	m_light_diffuse_r = 0.0f;	m_light_diffuse_g = 0.0f;	m_light_diffuse_b = 0.0f;
	m_light_specular_r = 0.0f;	m_light_specular_g = 0.0f;	m_light_specular_b = 0.0f;
	m_light_spot_cutoff = 0.0f;
	m_light_constant = 0.0f;
	m_light_linear = 0.0f;
	m_light_quadratic = 0.0f;
	XMVECTOR expectedOrientation =
		XMVECTOR{ 1,0,0,0 };

	Rotation = Quaternion::Slerp(
		XMVECTOR{ 1,0,0,0 }, handleOrientation, 1);
	Color = color;
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
	//return nullptr;
	return new ObjectTransformState();
}