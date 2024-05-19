#pragma once
#include "pch.h"
//#include "ToolMain.h"
//#include <memory>
//#include <SimpleMath.h>
//#include <DirectXMath.h>
//#include <memory>
//#include <d3d11.h>
//#include <SimpleMath.h>
#include "DirectXMath.h"

namespace DirectX
{
	class Model;
}

struct ID3D11ShaderResourceView;

class DisplayObject
{
public:
	DisplayObject();
	~DisplayObject();


	std::string model_path;
	std::string texture_path;

	std::shared_ptr<DirectX::Model> m_model; //main Mesh
	ID3D11ShaderResourceView* m_texture_diffuse; //diffuse texture

	int m_ID;

	const DisplayObject* parentObject = nullptr;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_orientation;
	DirectX::SimpleMath::Vector3 m_scale;
	bool m_render;
	bool m_wireframe;

	int m_light_type;
	float m_light_diffuse_r, m_light_diffuse_g, m_light_diffuse_b;
	float m_light_specular_r, m_light_specular_g, m_light_specular_b;
	float m_light_spot_cutoff;
	float m_light_constant;
	float m_light_linear;
	float m_light_quadratic;

	DirectX::SimpleMath::Vector3 GetWorldPosition() const;
	virtual DirectX::XMMATRIX GetWorldMatrix() const;

};
