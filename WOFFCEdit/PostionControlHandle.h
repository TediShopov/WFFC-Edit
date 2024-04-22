#pragma once

#include "ControlHandle.h"
class PostionControlHandle :
	public ControlHandle
{
public:
	DirectX::SimpleMath::Quaternion Rotation;
	DirectX::XMVECTOR Color;
	PostionControlHandle(DirectX::XMVECTOR handleOrientation,
		DisplayObject* mesh, DirectX::XMVECTOR color);

	DirectX::XMMATRIX GetWorldMatrix() const override;

	void Update() override;
	ToolStateBase* OnMouseClick() override;
};
