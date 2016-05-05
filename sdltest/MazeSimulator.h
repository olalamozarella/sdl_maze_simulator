#pragma once
#include "ISimulator.h"

class MazeSimulator :	public ISimulator
{
public:
	MazeSimulator(IRobot* subject, Position pos);
	MazeSimulator(IRobot* subject, Position pos, MazeAttributes	mattr);
	MazeSimulator(IRobot* subject, Position pos, bool demo);
	~MazeSimulator();
	virtual Position updateRobotPos(bool stopOnNewCell);
	Position updateRobotPos(char smer);
	virtual CellType giveCellType(Position pos);
	virtual void getWindowSize(int* width, int* height);
	MazePlan* getMazePlan();
	Position getRobotPosition();
private:
	MazePlan* plan;
	IRobot* testSubject;
	Position subjectPos;	//"logicka" pozicia - pouzita na rozhodovanie robota
	Position actualPos;		//pozicia pouzita na vykreslenie, tato sa bude menit po zlomkoch
	int actualMovementPhase = 0, actualRotationPhase = 0, rotationStep = 0;
	double translationStepX = 0, translationStepY = 0;
	bool finishedRotation = true, finishedTranslation = true, beginTranslation =true;
	void nextLogicalStep(bool interactive = false, char direction = 0);
	bool nextRotationStep();
	bool nextTranslationStep();
};

