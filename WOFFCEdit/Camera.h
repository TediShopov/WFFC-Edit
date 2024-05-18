#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

struct InputCommands;

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera
{
	//camera

public:
	Camera();
	~Camera();

	void Update(const InputCommands& input_commands);
	XMMATRIX GetView();

public:
	//functionality
	float m_movespeed;
	float m_camRotRate;

	//Camera State
	Vector3 m_camPosition;
	Vector3 m_camOrientation;
	Vector3 m_camLookAt;
	Vector3 m_camLookDirection;
	Vector3 m_camRight;

private:
	XMMATRIX m_view;
};
