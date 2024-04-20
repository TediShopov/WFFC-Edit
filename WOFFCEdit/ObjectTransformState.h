#pragma once
#include <DirectXMath.h>

#include "ToolStateBase.h"

using namespace DirectX;
class ObjectTransformState : public ToolStateBase
{
public:
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
};
