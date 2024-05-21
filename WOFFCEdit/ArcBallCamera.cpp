#include "ArcBallCamera.h"

#include "InputCommands.h"

ArcBallCamera::ArcBallCamera() : Camera()
{
	m_camOrientation = Vector3(0, 0, 1);
	offsetFromTarget = 10;
	m_minAllowedOffset = 0.05f;
	m_mouseWheeOffsetChange = 0.05f;
}

void ArcBallCamera::CalculateViewMatrix()
{
	Vector3 augmentedCamPosition = m_camLookDirection * offsetFromTarget + m_Target;

	m_view =
			Matrix::CreateLookAt(
				augmentedCamPosition,
				m_Target,
				DirectX::SimpleMath::Vector3::UnitY);
}

void ArcBallCamera::Update(const InputCommands& input_commands)
{
	Camera::Update(input_commands);
	if(input_commands.scrollWheelDelta != 0)
	{
		this->offsetFromTarget -= input_commands.scrollWheelDelta * m_mouseWheeOffsetChange;
		
		if(this->offsetFromTarget <= m_minAllowedOffset)
		{
			this->offsetFromTarget = m_minAllowedOffset;
		}
	}
}

void ArcBallCamera::SetTarget(Vector3 target)
{ m_Target = target; }

