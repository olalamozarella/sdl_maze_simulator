#include "MazeSimulator.h"
#include <math.h>
#define PI 3.14159265358979323846
#define ANGLESTEPS 10
#define MOVEMENTSTEPS 10

MazeSimulator::MazeSimulator(IRobot* subject, Position pos)
{
	plan = MazePlan::giveExample();
	//priradi subject a pos do premennych
	testSubject = subject;
	subjectPos = pos;
	actualPos = pos;
}

MazeSimulator::MazeSimulator(IRobot* subject, Position pos, MazeAttributes	mattr)
{
	plan = MazePlan::giveExample(mattr.rows, mattr.cols, mattr.bsize, mattr.lw);
	//priradi subject a pos do premennych
	testSubject = subject;
	subjectPos = pos;
	actualPos = pos;
}

MazeSimulator::MazeSimulator(IRobot* subject, Position pos, bool demo)
{
	plan = MazePlan::giveExample(demo);
	//priradi subject a pos do premennych
	testSubject = subject;
	subjectPos = pos;
	actualPos = pos;
}

MazeSimulator::~MazeSimulator()
{
	plan->~MazePlan();
}

Position MazeSimulator::updateRobotPos(bool stopOnNewCell)
{
//vyrataj dalsi "logicky" krok
	if ((stopOnNewCell == false) && finishedRotation && finishedTranslation) {
		nextLogicalStep();
	}
// najprv sa otocim (ak treba aby som sa otocil)
	if (finishedRotation == false) {
		finishedRotation = nextRotationStep();
	}
// ak som sa dootacal (a stlacil klavesu), zacnem sa posuvat
	if (finishedRotation && stopOnNewCell == false) beginTranslation = true;
// posuvam sa dalej
	if (finishedTranslation == false && beginTranslation == true) {
		finishedTranslation = nextTranslationStep();
	}
	return actualPos;
}

Position MazeSimulator::updateRobotPos(char smer) {
	//INTERACTIVE MODE
	//vyrataj dalsi "logicky" krok
	if (smer != '0' && finishedRotation && finishedTranslation) {
		nextLogicalStep(true, smer);
	}
	// najprv sa otocim (ak treba aby som sa otocil)
	if (finishedRotation == false) {
		finishedRotation = nextRotationStep();
	}
	// ak som sa dootacal, zacnem sa posuvat
	if (finishedRotation) beginTranslation = true;
	// posuvam sa dalej
	if (finishedTranslation == false && beginTranslation == true) {
		finishedTranslation = nextTranslationStep();
	}
	return actualPos;
}

void MazeSimulator::nextLogicalStep(bool interactive, char direction)
{
	double d_move = 0;
	int d_angle = 0;
	if (interactive == false) {
		CellType cell = giveCellType(subjectPos);	//zisti na akom je policku		
		testSubject->positionUpdate(cell, &d_move, &d_angle);		//zavola positionUpdate() - robot vrati o kolko sa posunul a otocil				

	//upravim subjectPos podla hodnot co som dostal
		subjectPos.angle += d_angle;
		if (subjectPos.angle >= 360) subjectPos.angle -= 360;
		if (subjectPos.angle < 0) subjectPos.angle += 360;
		subjectPos.x += (sin(subjectPos.angle*PI / 180)*d_move);
		subjectPos.y -= (cos(subjectPos.angle*PI / 180)*d_move);
	}
	else {	//ak je v interactive mode - hybe sa na zaklade sipok
		d_move = 1;
		switch (direction) {
		case 'N': subjectPos.y -= 1; subjectPos.angle = 0 ; break;
		case 'S': subjectPos.y += 1; subjectPos.angle = 180; break;
		case 'W': subjectPos.x -= 1; subjectPos.angle = 270; break;
		case 'E': subjectPos.x += 1; subjectPos.angle = 90; break;
		}
		d_angle = subjectPos.angle - actualPos.angle;
		if (d_angle > 180) d_angle -= 360;
		if (d_angle < -180) d_angle += 360;
	}

//vyratam velkosti krokov posunutia/otocenia
	translationStepX = (subjectPos.x - actualPos.x) / MOVEMENTSTEPS;
	translationStepY = (subjectPos.y - actualPos.y) / MOVEMENTSTEPS;
	rotationStep = d_angle / ANGLESTEPS;
	actualMovementPhase = 0;
	actualRotationPhase = 0;
	
//urcim booleany, ci treba posuvat/otacat
	if (d_angle == 0) finishedRotation = true;
	else finishedRotation = false;
	if (d_move == 0) finishedTranslation = true;
	else finishedTranslation = false;
	beginTranslation = false;
}

bool MazeSimulator::nextRotationStep()
{
	if (actualRotationPhase < ANGLESTEPS) {
		actualPos.angle += rotationStep;
		actualRotationPhase++;
		return false;
	}
	else {
		actualPos.angle = subjectPos.angle;
		return true;
	}
}

bool MazeSimulator::nextTranslationStep()
{
	if (actualMovementPhase < MOVEMENTSTEPS) {
		actualPos.x += translationStepX;
		actualPos.y += translationStepY;
		actualMovementPhase++;
		return false;
	}
	else {
		actualPos.x = subjectPos.x;
		actualPos.y = subjectPos.y;
		return true;
	}
}

CellType MazeSimulator::giveCellType(Position pos)
{
	if (fabs(pos.x - plan->endx) < 0.01 && fabs(pos.y - plan->endy) < 0.01) return c_finish;
	unsigned char nswe = 0;
	//vrati celltype vzhladom na polohu&orientaciu robota
	bool isOnLeft, isOnRight, isInFront, isBehind;

	//celltype urcim pomocou h/vlines z policka, tak si ich najdem (catche su kvoli array out of bounds)
	//x a y pricitam 0.5 kvoli truncate pri premene na int
	bool vl1, vl2, hl1, hl2;
  	if (pos.y < 0.01) vl1 = false;
	else vl1 = plan->vlines[(int)(pos.x+0.5)][(int)(pos.y-0.5)];
	if (((plan->rows - 1) - pos.y) < 0.01) vl2 = false;
	else vl2 = plan->vlines[(int)(pos.x + 0.5)][(int)(pos.y + 0.5)];
	if (pos.x < 0.01) hl1 = false;
	else hl1 = plan->hlines[(int)(pos.y + 0.5)][(int)(pos.x - 0.5)];
	if (((plan->cols - 1) - pos.x) < 0.01) hl2 = false;
	else hl2 = plan->hlines[(int)(pos.y + 0.5)][(int)(pos.x + 0.5)];

	while (pos.angle >= 360) pos.angle -= 360;
	//zmena perspektivy podla uhlu natocenia
	if (pos.angle < 45) nswe = 'N';
	else if (pos.angle < 135) nswe = 'E';
	else if (pos.angle < 225) nswe = 'S';
	else if (pos.angle < 315) nswe = 'W';
	else nswe = 'N';

	switch (nswe) {
	case 'N': isInFront = vl1; isBehind = vl2; isOnLeft = hl1; isOnRight = hl2; break;
	case 'S': isInFront = vl2; isBehind = vl1; isOnLeft = hl2; isOnRight = hl1; break;
	case 'W': isInFront = hl1; isBehind = hl2; isOnLeft = vl2; isOnRight = vl1; break;
	case 'E': isInFront = hl2; isBehind = hl1; isOnLeft = vl1; isOnRight = vl2; break;
	}

	//pomocou h/vlines vyplujem celltype
	if (isInFront == true && isBehind == true && isOnLeft == false && isOnRight == false) return c_straight;
	if (isInFront == false && isBehind == true && isOnLeft == false && isOnRight == true) return c_right;
	if (isInFront == false && isBehind == true && isOnLeft == true && isOnRight == false) return c_left;
	if (isInFront == true && isBehind == true && isOnLeft == true && isOnRight == true) return c_cross;
	if (isInFront == false && isBehind == true && isOnLeft == true && isOnRight == true) return c_tjun;
	if (isInFront == true && isBehind == true && isOnLeft == true && isOnRight == false) return c_tleft;
	if (isInFront == true && isBehind == true && isOnLeft == false && isOnRight == true) return c_tright;
	if (isInFront == false && isBehind == true && isOnLeft == false && isOnRight == false) return c_deadend;	//deadend hore
	if (isInFront == true && isBehind == false && isOnLeft == false && isOnRight == false) return f_deadend;	//deadend dolu
	if (isInFront == false && isBehind == false && isOnLeft == true && isOnRight == false) return f_deadend;	//deadend vlavo
	if (isInFront == false && isBehind == false && isOnLeft == false && isOnRight == true) return f_deadend;	//deadend vpravo
	if (isInFront == true && isBehind == false && isOnLeft == false && isOnRight == true) return f_right;
	if (isInFront == true && isBehind == false && isOnLeft == true && isOnRight == false) return f_left;
	if (isInFront == true && isBehind == false && isOnLeft == true && isOnRight == true) return f_tjun;
	if (isInFront == false && isBehind == false && isOnLeft == true && isOnRight == true) return f_straight;
	return f_lost;
}

void MazeSimulator::getWindowSize(int* width, int* height)
{
	*width = plan->blocksize*plan->cols;
	*height = plan->blocksize*plan->rows;
}

MazePlan * MazeSimulator::getMazePlan()
{
	return plan;
}

Position MazeSimulator::getRobotPosition()
{
	return actualPos;
}