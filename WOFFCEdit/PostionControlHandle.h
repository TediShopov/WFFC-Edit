#pragma once

#include "ControlHandle.h"

enum AXES
{
	X_AXIS = 1,
	Y_AXIS = 2,
	Z_AXIS = 3,
};

class PostionControlHandle :
	public ControlHandle
{
public:

	AXES Type;
	DirectX::XMVECTOR Color;
	DirectX::SimpleMath::Quaternion Rotation;
	static SimpleMath::Quaternion FromToQuaternion(DirectX::XMVECTOR from, DirectX::XMVECTOR to);
	//	PostionControlHandle(
	//		ToolMain* tool,
	//		float x, float y, float z,
	//		float x1, float y1, float z1,
	//		const SceneObject* mesh, DirectX::XMVECTOR color);
	PostionControlHandle(AXES axesType,
		ToolMain* tool, const SceneObject* mesh, DirectX::XMVECTOR color);

	DirectX::XMMATRIX GetWorldMatrix() const override;

	void Update() override;
	ToolStateBase* OnMouseClick() override;
};
