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
	ObjectTransformState(AXES axis, bool a);

	void Update(const InputCommands& input)override;
	void Init(ToolMain* tool, const InputCommands&) override;
	void FromInput(const InputCommands& input) override;

	XMVECTOR MoveOnAxis(
		XMVECTOR mouseWorldRay,
		XMVECTOR plane,
		XMVECTOR rayOnPlane);

	XMVECTOR MoveOnPlane(
		XMVECTOR mouseWorldRay,
		XMVECTOR plane);
private:
	//If moving on axis or on plane
	bool move_on_axis;
};
