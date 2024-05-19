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
	ObjectTransformState(AXES global_direction, bool a);

	void Update(const InputCommands& input)override;
	void Init(ToolMain* tool, const InputCommands&) override;
	void FromInput(const InputCommands& input) override;

private:
	//If moving on global_direction or on plane
	bool move_on_axis;
};
