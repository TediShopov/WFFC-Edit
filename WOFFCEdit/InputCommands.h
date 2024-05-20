#pragma once

struct InputCommands
{
	//States - based on actual state of button
	bool forward;
	bool back;
	bool right;
	bool left;
	bool rotRight;
	bool rotLeft;
	int mouseX;
	int mouseY;
	bool mouseLB;
	bool mouseRB;
	short scrollWheelDelta;
	bool shift;
	bool ctrl;
	bool planeX;
	bool planeY;
	bool planeZ;
	bool handleHit;

	//Actions - triggered only for a frame after RELEASE
	bool undoAction;
	bool redoAction;
	bool switchToTranslateAction;
	bool switchToScaleAction;
	bool switchToRotateAction;
	bool insertObjectAction;
	bool deleteSelectedAction;
};
