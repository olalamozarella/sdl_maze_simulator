#pragma once
#include "IRobot.h"

class MouseV1 :	public IRobot
{
public:
	virtual bool positionUpdate(CellType cell, double* d_move, int* d_angle);
};

