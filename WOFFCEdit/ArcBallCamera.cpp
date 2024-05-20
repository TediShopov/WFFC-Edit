#include "ArcBallCamera.h"

ArcBallCamera::ArcBallCamera() : Camera()
{
	m_camOrientation = Vector3(0, 0, 1);
}

void ArcBallCamera::CalculateViewMatrix()
{
	Vector3 augmentedCamPosition = m_camLookDirection * 10 + m_Target;

	m_view =
			Matrix::CreateLookAt(
				augmentedCamPosition,
				m_Target,
				DirectX::SimpleMath::Vector3::UnitY);
}

void ArcBallCamera::Update(const InputCommands& input_commands)
{
	Camera::Update(input_commands);
}

void ArcBallCamera::SetTarget(Vector3 target)
{ m_Target = target; }

