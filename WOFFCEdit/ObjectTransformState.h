#pragma once
#include <DirectXMath.h>

#include "ToolStateBase.h"

using namespace DirectX;
enum AXES;
class ObjectTransformState : public ToolStateBase
{
public:
	static XMMATRIX LocalAxes;

	XMVECTOR plane;
	XMVECTOR axis;
	AXES axisType;

	ObjectTransformState();
	ObjectTransformState(AXES axis, bool a);
	//ObjectTransformState(XMVECTOR axis, XMVECTOR axis_two);

	void Init(ToolMain* tool, const InputCommands&) override;
	void Update(const InputCommands& input)override;

	InputCommands on_selection_commands;

	XMVECTOR  selected_object_axes[4];
	XMVECTOR  selected_object_planes[3];

	void GetLocalVectors(int objectIndex,
		XMVECTOR vecs[4]);

	void GetLocalPlanes(
		int objectIndex,
		DirectX::XMVECTOR planes[3]);

	XMVECTOR MoveOnAxis(
		XMVECTOR mouseWorldRay,
		XMVECTOR plane,
		XMVECTOR rayOnPlane);

	XMVECTOR MoveOnPlane(
		XMVECTOR mouseWorldRay,
		XMVECTOR plane);
private:
	bool release_mouse_needed = false;
	//If from handle or from input
	bool from_handle = false;
	//If moving on axis or on plane
	bool move_on_axis;
};
