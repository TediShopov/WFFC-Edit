#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

struct InputCommands;

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera
{
	//m_camera

public:
	Camera();
	~Camera();

	void UpdateCameraBasedOnKeyInput(const InputCommands& m_InputCommands);
	void UpdateCameraBasedOnMouseInput(const InputCommands& m_InputCommands);

	void virtual Update(const InputCommands& input_commands);
	void virtual CalculateViewMatrix();
	XMMATRIX GetView();
	float GetDeltaXNDC();
	float GetDeltaYNDC();

	void ConvertToNDC(int x, int y, float& xNDC, float& yNDC);
public:


	//functionality
	float m_movespeed;
	float m_camRotateOnPress;
	float m_camRotateFromMouseDelta;


	RECT m_screenRect;

	//Camera State
	Vector3 m_camPosition;
	Vector3 m_camOrientation;
	Vector3 m_camLookAt;
	Vector3 m_camLookDirection;
	Vector3 m_camRight;


	XMMATRIX m_view;



private:
	float m_deltaXNDC;
	float m_deltaYNDC;
	float m_previousXNDC;
	float m_previousYNDC;


};
