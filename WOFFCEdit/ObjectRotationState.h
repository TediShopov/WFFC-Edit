#pragma once


#include "AxisBasedTransformState.h"
#include <DirectXMath.h>



class ObjectRotationState : public AxisBasedTransformState
{ 
public:
	ObjectRotationState();
	ObjectRotationState(AXES axis, bool a);

	void Update(const InputCommands& input)override;

	void FromInput(const InputCommands& input) override;


	XMVECTOR RotateAroundSelectedAxis(const InputCommands& commands);


private:
	//If moving on axis or on plane
	bool move_on_axis;
};

