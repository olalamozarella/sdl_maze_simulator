#pragma once
#include "ISimulator.h"
#include "defines.h"

class IRobot
{
public:
	virtual bool positionUpdate(CellType cell, double* d_move, int* d_angle) = 0;
};

