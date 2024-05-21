#pragma once
#include <DirectXMath.h>

#include "AxisBasedTransformState.h"
#include "ToolStateBase.h"

using namespace DirectX;
enum AXES;
class ObjectTransformState : public AxisBasedTransformState
{
public:
	ObjectTransformState();
	ObjectTransformState(AXES globalDirection, bool a);

	void Update(const InputCommands& input)override;

private:
	//If moving on globalDirection or on plane
	bool move_on_axis;
};
