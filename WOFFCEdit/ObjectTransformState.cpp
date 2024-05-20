#include "ObjectTransformState.h"

#include "ObjectSelectionState.h"
#include "ToolMain.h"
#include  "PostionControlHandle.h"

#include "Camera.h"

ObjectTransformState::ObjectTransformState()
	: AxisBasedTransformState()
{
}

ObjectTransformState::ObjectTransformState(AXES axisType, bool a)
	: AxisBasedTransformState(axisType)
{
	this->move_on_axis = a;
}


void ObjectTransformState::Update(const InputCommands& input)
{
	AxisBasedTransformState::Update(input);
	move_on_axis = input.ctrl;
	auto sel = MainTool->GetSelectedDisplayObjects();
	if (sel.size() == 1)
	{
		world_axes_directions = GetWorldAxes(SelectedObject);
		world_planes = GetWorldPlanes(SelectedObject);

		plane = world_planes.r[axisType];

		DisplayObject* obj = sel[0];

		XMVECTOR newPosition;
		XMVECTOR mouseWorldPos;
		GetMouseWorldRay(input, mouseWorldPos);

		if (move_on_axis)
		{
			newPosition = CardinalAxisIntersection(
				mouseWorldPos, axisType);
		}
		else

		{
			newPosition = PlaneIntersection(
				mouseWorldPos, plane);
		}

		if(obj->parentObject == nullptr)
			obj->m_position = newPosition;
		else
		{
			XMMATRIX parentInverseMatrix = 
				XMMatrixInverse(nullptr,obj->parentObject->GetWorldMatrix()); 
			newPosition = XMVector3Transform(newPosition, parentInverseMatrix);
			obj->m_position = newPosition;
		}


		this->MainTool->SyncDisplayAndSceneObjects(sel[0]->m_ID);
		this->MainTool->Notify(false,true,false);
	}
	else
	{
		this->MainTool->ChangeState(new ObjectSelectionState());
	}
}




