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

	void UpdateCameraBasedOnKeyInput(const InputCommands& m_InputCommands);
	void UpdateCameraBasedOnMouseInput(const InputCommands& m_InputCommands);

	void Update(const InputCommands& input_commands);
	XMMATRIX GetView();

public:
	//
	bool isArcBallMode;
	Vector3 arcBallTarget;


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


	float GetDeltaXNDC();
	float GetDeltaYNDC();


	void ConvertToNDC(int x, int y, float& xNDC, float& yNDC);

private:
	float m_deltaXNDC;
	float m_deltaYNDC;
	float m_previousXNDC;
	float m_previousYNDC;


	XMMATRIX m_view;
};
