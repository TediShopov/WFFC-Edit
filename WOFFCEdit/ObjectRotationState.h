#pragma once


#include "AxisBasedTransformState.h"
#include <DirectXMath.h>



class ObjectRotationState : public AxisBasedTransformState
{ 
public:
	ObjectRotationState();
	ObjectRotationState(AXES global_direction, bool a);

	void Update(const InputCommands& input)override;

	void FromInput(const InputCommands& input) override;

	XMVECTOR m_lastPosition;
	float GetWorldCoordinatesDelta(const InputCommands& commands);
	float GetMouseNDCDelta(const InputCommands& commands);

	XMVECTOR RotateAroundSelectedAxis(const InputCommands& commands);


private:
	//If moving on global_direction or on plane
	bool move_on_axis;
};

