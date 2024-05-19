#pragma once
#include "AxisBasedTransformState.h"
#include <DirectXMath.h>
class ObjectScaleState : public AxisBasedTransformState
{ 
public:
	ObjectScaleState();
	ObjectScaleState(AXES global_direction, bool a);


	void Init(ToolMain* tool, const InputCommands&) override;
	void Update(const InputCommands& input)override;


	XMVECTOR m_lastPosition;
	float GetWorldCoordinatesDelta(const InputCommands& commands);

	XMVECTOR ScaleOnSelectedAxis(const InputCommands& commands);



private:
	float m_scaleSpeed;
	float m_scaleRatio;
};

