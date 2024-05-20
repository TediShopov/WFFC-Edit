#pragma once
#include "Camera.h"

class ArcBallCamera :
    public Camera
{
public:
	ArcBallCamera();
	void CalculateViewMatrix() override;
    void Update(const InputCommands& input_commands)override;
    void SetTarget(Vector3 target);

private:
    Vector3 m_Target;
};

