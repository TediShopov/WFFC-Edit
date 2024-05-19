//#include "pch.h"
#include "ToolMain.h"
#include "DisplayObject.h"

DisplayObject::DisplayObject()
{
	m_model = NULL;
	m_texture_diffuse = NULL;
	m_orientation.x = 0.0f;
	m_orientation.y = 0.0f;
	m_orientation.z = 0.0f;
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_position.z = 0.0f;
	m_scale.x = 1.0f;
	m_scale.y = 1.0f;
	m_scale.z = 1.0f;
	m_render = true;
	m_wireframe = false;

	m_light_type = 0;
	m_light_diffuse_r = 0.0f;
	m_light_diffuse_g = 0.0f;
	m_light_diffuse_b = 0.0f;
	m_light_specular_r = 0.0f;
	m_light_specular_g = 0.0f;
	m_light_specular_b = 0.0f;
	m_light_spot_cutoff = 0.0f;
	m_light_constant = 0.0f;
	m_light_linear = 0.0f;
	m_light_quadratic = 0.0f;
}

DisplayObject::~DisplayObject()
{
	//	delete m_texture_diffuse;
}

DirectX::XMMATRIX DisplayObject::GetWorldMatrix() const
{
	const DirectX::XMVECTORF32 scale = {this->m_scale.x, this->m_scale.y, this->m_scale.z};
	const DirectX::XMVECTORF32 translate = {this->m_position.x, this->m_position.y, this->m_position.z};

	//convert degrees into radians for rotation matrix
	DirectX::XMVECTOR rotate =
		DirectX::SimpleMath::Quaternion::
		CreateFromYawPitchRoll(this->m_orientation.y * 3.1415 / 180,
		                       this->m_orientation.x * 3.1415 / 180,
		                       this->m_orientation.z * 3.1415 / 180);

	DirectX::XMMATRIX local =
		XMMatrixTransformation(
			DirectX::g_XMZero,
			DirectX::SimpleMath::Quaternion::Identity,
			scale, DirectX::g_XMZero, rotate, translate);
	if (this->parentObject != nullptr)
	{
		return local * this->parentObject->GetWorldMatrix();
		//return  this->parentObject->GetWorldMatrix() * local ;
	}
	return local;
}

Vector3 DisplayObject::GetWorldPosition() const
{
	return XMVector3TransformCoord(Vector3(0, 0, 0), GetWorldMatrix());
}
