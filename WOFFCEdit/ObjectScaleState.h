#pragma once
#include "AxisBasedTransformState.h"
#include <DirectXMath.h>
class ObjectScaleState : public AxisBasedTransformState
{ 
public:
	ObjectScaleState();
	ObjectScaleState(AXES globalDirection, bool a);

	void Init(ToolMain* tool, const InputCommands&) override;
	void Update(const InputCommands& input)override;

	float GetWorldCoordinatesDelta(const InputCommands& commands);

	XMVECTOR ScaleOnSelectedAxis(const InputCommands& commands);
private:
	float m_scaleSpeed;
	float m_scaleRatio;
	XMVECTOR m_lastPosition;
};

