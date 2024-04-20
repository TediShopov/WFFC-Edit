#pragma once
#include "pch.h"

class DisplayObject
{
public:
	DisplayObject();
	~DisplayObject();

	std::shared_ptr<DirectX::Model>						m_model;							//main Mesh
	ID3D11ShaderResourceView* m_texture_diffuse;					//diffuse texture

	int m_ID;

	const DisplayObject* parentObject = nullptr;
	DirectX::SimpleMath::Vector3			m_position;
	DirectX::SimpleMath::Vector3			m_orientation;
	DirectX::SimpleMath::Vector3			m_scale;
	bool									m_render;
	bool									m_wireframe;

	int		m_light_type;
	float	m_light_diffuse_r, m_light_diffuse_g, m_light_diffuse_b;
	float	m_light_specular_r, m_light_specular_g, m_light_specular_b;
	float	m_light_spot_cutoff;
	float	m_light_constant;
	float	m_light_linear;
	float	m_light_quadratic;

	DirectX::XMMATRIX GetWorldMatrix() const
	{
		const DirectX::XMVECTORF32 scale = { this->m_scale.x, this->m_scale.y, this->m_scale.z };
		const DirectX::XMVECTORF32 translate = { this->m_position.x, this->m_position.y, this->m_position.z };

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
			return   local * this->parentObject->GetWorldMatrix();
		}
		return local;
	}
};
