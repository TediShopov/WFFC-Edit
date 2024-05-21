#pragma once


#include "AxisBasedTransformState.h"
#include <DirectXMath.h>



class ObjectRotationState : public AxisBasedTransformState
{ 
public:
	ObjectRotationState();
	ObjectRotationState(AXES globalDirection, bool a);


	void Init(ToolMain* tool, const InputCommands&) override;
	void Update(const InputCommands& input)override;


	float GetWorldCoordinatesDelta(const InputCommands& commands);
	float GetMouseNDCDelta(const InputCommands& commands);

	XMVECTOR RotateAroundSelectedAxis(const InputCommands& commands);


private:
	float m_rotationSpeed;
	XMVECTOR m_lastPosition;
};

