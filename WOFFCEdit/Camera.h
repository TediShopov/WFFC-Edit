#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

struct InputCommands;

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera
{
public:
	Camera();
	~Camera();

	void virtual Update(const InputCommands& input_commands);

	//Retrieve the calculated view matrx
	XMMATRIX GetView();

	//Get the difference between this frames normalized device coordinates and last frames
	float GetDeltaXNDC();
	float GetDeltaYNDC();

protected:
	void UpdateCameraBasedOnKeyInput(const InputCommands& m_InputCommands);
	void UpdateCameraBasedOnMouseInput(const InputCommands& m_InputCommands);
	void virtual CalculateViewMatrix();
	void ConvertToNDC(int x, int y, float& xNDC, float& yNDC);
public:
	//functionality
	float m_movespeed;
	float m_camRotateOnPress;
	float m_camRotateFromMouseDelta;

	//Window rect that camera project to
	RECT m_screenRect;

	//Camera State
	Vector3 m_camPosition;
	Vector3 m_camOrientation;
	Vector3 m_camLookAt;
	Vector3 m_camLookDirection;
	Vector3 m_camRight;

	//The cached view matrix
	XMMATRIX m_view;



private:
	float m_deltaXNDC;
	float m_deltaYNDC;
	float m_previousXNDC;
	float m_previousYNDC;


};
