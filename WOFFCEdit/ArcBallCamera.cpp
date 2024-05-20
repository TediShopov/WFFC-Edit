#include "ArcBallCamera.h"

#include "InputCommands.h"

ArcBallCamera::ArcBallCamera() : Camera()
{
	m_camOrientation = Vector3(0, 0, 1);
	offsetFromTarget = 10;
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
		this->offsetFromTarget -= input_commands.scrollWheelDelta * 0.05f;
		
		if(this->offsetFromTarget <= 0.05f)
		{
			this->offsetFromTarget = 0.05f;
			
		}
	}


}

void ArcBallCamera::SetTarget(Vector3 target)
{ m_Target = target; }

