#pragma once
#include "IRobot.h"
#include "MazePlan.h"
#include "defines.h"

class ISimulator
{
public:
	virtual Position updateRobotPos(bool stopOnNewCell) = 0;	/* vrati poziciu robota na vykreslenie = actualPos,
	tu bude doratany pohyb medzi "krokmi", kazdych n cyklov zavola getNextPos() */
	virtual CellType giveCellType(Position pos) = 0;		//vrati typ policka na ktorom robot stoji - dorata pomocou MazePlan
	virtual void getWindowSize(int* width, int* height) = 0;
private:
};

