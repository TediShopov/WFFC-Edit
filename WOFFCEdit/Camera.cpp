#include "Camera.h"

#include "InputCommands.h"

Camera::Camera()
{
	//functional
	m_movespeed = 0.30;
	m_camRotateOnPress = 3.0;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;

	m_camOrientation.x = 0;
	m_camOrientation.y = 0;
	m_camOrientation.z = 0;

	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

	m_camOrientation.x = 0.0f;
	m_camOrientation.y = 0.0f;
	m_camOrientation.z = 0.0f;

	m_camRotateFromMouseDelta = 50.0f;

	m_view = Matrix::Identity;
}

Camera::~Camera()
{
}

void Camera::UpdateCameraBasedOnKeyInput(const InputCommands& m_InputCommands)
{
	if (m_InputCommands.rotRight)
	{
		m_camOrientation.y += m_camRotateOnPress;
	}
	if (m_InputCommands.rotLeft)
	{
		m_camOrientation.y -= m_camRotateOnPress;
	}

	m_camLookDirection.x = cosf(XMConvertToRadians(m_camOrientation.y)) * cosf(XMConvertToRadians(m_camOrientation.x));
	m_camLookDirection.y = sinf(XMConvertToRadians(m_camOrientation.x));
	m_camLookDirection.z = sinf(XMConvertToRadians(m_camOrientation.y)) * cosf(XMConvertToRadians(m_camOrientation.x));


	//create look direction from Euler angles in m_camOrientation
	//	m_camLookDirection.x = sin((m_camOrientation.y) * 3.1415 / 180);
	//	m_camLookDirection.z = cos((m_camOrientation.y) * 3.1415 / 180);
	m_camLookDirection.Normalize();

	//create right vector from look Direction
	m_camLookDirection.Cross(DirectX::SimpleMath::Vector3::UnitY, m_camRight);

	//process input and update stuff
	if (m_InputCommands.forward)
	{
		m_camPosition += m_camLookDirection * m_movespeed;
	}
	if (m_InputCommands.back)
	{
		m_camPosition -= m_camLookDirection * m_movespeed;
	}
	if (m_InputCommands.right)
	{
		m_camPosition += m_camRight * m_movespeed;
	}
	if (m_InputCommands.left)
	{
		m_camPosition -= m_camRight * m_movespeed;
	}
}

void Camera::UpdateCameraBasedOnMouseInput(const InputCommands& m_InputCommands)
{
	float xNDC;
	float yNDC;
	//Convert mouse coordinate to normalize device coordinates
	ConvertToNDC(
		m_InputCommands.mouse_x,
		m_InputCommands.mouse_y,
		xNDC, yNDC
	);
		m_deltaXNDC = xNDC - m_previousXNDC;
		m_deltaYNDC = yNDC - m_previousYNDC;
	if (m_InputCommands.mouse_RB_Down)
	{
		//		if (m_InputCommands.scroll_wheel_delta != 0)
		//		{
		//			if (m_InputCommands.mouse_RB_Down && m_InputCommands.CTRL_Down)
		//			{
		//				this->m_movespeed += (float)m_InputCommands.scroll_wheel_delta * 0.1;
		//				if (m_movespeed <= 0)
		//					m_movespeed = 0;
		//			}
		//			else if (m_InputCommands.mouse_RB_Down)
		//			{
		//				this->m_camRotateFromMouseDelta +=
		//					(float)m_InputCommands.scroll_wheel_delta * 0.001f;
		//			}
		//		}



		//Change in x rotate yaw
		m_camOrientation.y += m_deltaXNDC * m_camRotateFromMouseDelta;

		//Change in y rotate pitch
		m_camOrientation.x += m_deltaYNDC * m_camRotateFromMouseDelta;
	}


	//Store mouse coordinates
	this->m_previousXNDC = xNDC;
	this->m_previousYNDC = yNDC;

	//this->m_ca
}

void Camera::Update(const InputCommands& m_InputCommands)
{
	UpdateCameraBasedOnMouseInput(m_InputCommands);


	//camera motion is on a plane, so kill the 7 component of the look direction
	DirectX::SimpleMath::Vector3 planarMotionVector = m_camLookDirection;
	planarMotionVector.y = 0.0;
	UpdateCameraBasedOnKeyInput(m_InputCommands);

	//update lookat point
	m_camLookAt = m_camPosition + m_camLookDirection;
	CalculateViewMatrix();

}

void Camera::CalculateViewMatrix()
{
		m_view =
			Matrix::CreateLookAt(m_camPosition, m_camLookAt, DirectX::SimpleMath::Vector3::UnitY);
}

DirectX::XMMATRIX Camera::GetView()
{
	return m_view;
}

float Camera::GetDeltaXNDC()
{ return m_deltaXNDC; }

float Camera::GetDeltaYNDC()
{ return m_deltaYNDC; }

void Camera::ConvertToNDC(int x, int y, float& xNDC, float& yNDC)
{
	//Define screen dimensions
	float screenWidth = m_screenRect.right - m_screenRect.left;
	float screenHeight = m_screenRect.bottom - m_screenRect.top;

	//get NDC
	xNDC = (((2.0f * (float)x) / screenWidth) - 1);
	yNDC = -(((2.0f * (float)y) / screenHeight) - 1);
}
