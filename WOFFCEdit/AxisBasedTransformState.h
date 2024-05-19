#pragma once
#include <DirectXMath.h>

#include "ToolStateBase.h"

class DisplayObject;
using namespace DirectX;
enum AXES;
class AxisBasedTransformState : public ToolStateBase
{
public:

	static XMMATRIX LocalAxes;
	XMVECTOR plane;
	XMVECTOR global_direction;
	AXES axisType;
	InputCommands on_selection_commands;

	//Base constructor when state is called from other classes
	AxisBasedTransformState();
	AxisBasedTransformState(AXES selected_axis);

	void virtual Init(ToolMain* tool, const InputCommands&) override;
	void virtual Update(const InputCommands& input)override;
	void virtual FromInput(const InputCommands& input);

protected:
	DisplayObject* SelectedObject;
	XMMATRIX  world_axes_directions;
	XMMATRIX  world_planes;
	XMMATRIX GetWorldAxes(const DisplayObject* selected) const;
	XMVECTOR GetGlobalOrigin() const;


	XMMATRIX GetWorldPlanes(const DisplayObject* selected) const;
XMVECTOR PlaneIntersection(XMVECTOR mouseWorldRay, XMVECTOR plane);
XMVECTOR AxisIntersection(XMVECTOR mouseWorldRay, XMVECTOR plane, XMVECTOR rayOnPlane);

	void GetMouseWorldRay(const InputCommands& input, XMVECTOR& mouseWorldPos);

	//	void GetWorldPlanes(
	//		int objectIndex,
	//		DirectX::XMVECTOR planes[3]);

	bool release_mouse_needed = false;
	//If from handle or from input
	bool from_handle = false;
};
