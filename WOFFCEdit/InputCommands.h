#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool rotRight;
	bool rotLeft;
	int mouse_x;
	int mouse_y;
	bool mouse_LB_Down;
	bool mouse_RB_Down;
	short scroll_wheel_delta;
	bool CTRL_Down;
	bool plane_x;
	bool plane_y;
	bool plane_z;
	bool handleHit;
	bool insertObject;
	bool deleteSelected;
};
