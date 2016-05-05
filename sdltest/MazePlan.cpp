#include "MazePlan.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

MazePlan::MazePlan(unsigned char p_cols, unsigned char p_rows, unsigned char p_blocksize, unsigned char p_startx = 0, unsigned char p_starty = 0, unsigned char p_endx = 0, unsigned char p_endy = 0, bool p_enableLoops = false , unsigned char p_lineWidth = 3)
	:cols(p_cols),rows(p_rows),blocksize(p_blocksize),startx(p_startx),starty(p_starty),endx(p_endx),endy(p_endy),enableLoops(p_enableLoops), lineWidth(p_lineWidth)
{
	hlines = new bool*[p_rows]();
	for (int i = 0;i < p_rows;++i)
		hlines[i] = new bool[p_cols - 1]();

	vlines = new bool*[p_cols]();
	for (int i = 0;i < p_cols;++i)
		vlines[i] = new bool[p_rows - 1]();
}

MazePlan::~MazePlan()
{
	for (int i = 0;i < rows;++i)
		delete[] hlines[i];
	delete[] hlines;

	for (int i = 0;i < cols;++i)
		delete[] vlines[i];
	delete[] vlines;
}

void MazePlan::generateMaze()
{
	srand(time(NULL));
	//recursive backtracking
	//vytvorim grid policok + ci boli navstivene
	bool** navstivene = new bool*[rows]();
	for (int i = 0;i < rows;++i)
		navstivene[i] = new bool[cols]();
	//zacnem na cielovom policku
	nextCell(endx, endy, navstivene);
	
	//uvolnim pamat
	for (int i = 0;i < rows;++i)
		delete[] navstivene[i];
	delete[] navstivene;
}

void MazePlan::nextCell(unsigned char x, unsigned char y, bool** navstivene, unsigned char lastDirection) {
	printf(" nextCell ");
	//zapisem sa ako prejdene
	navstivene[y][x] = true;
	//urcim, kde mozem ist
	bool possibilities[4];	//N,S,W,E
	char possibilityCount = 4;
	char nextDirection = 0;
	if (x == 0) {
		possibilities[2] = 0;	//vymazem W
		possibilityCount--;
	}
	if (x == (cols - 1)) {
		possibilities[3] = 0;	//vymazem E
		possibilityCount--;
	}
	if (y == 0) {
		possibilities[0] = 0;	//vymazem N
		possibilityCount--;
	}
	if (y == (rows - 1)) {
		possibilities[1] = 0;	//vymazem S
		possibilityCount--;
	}
	// aby sa nevracal na policko z ktoreho prisiel
	if (lastDirection >= 0 && lastDirection < 4) {
		possibilityCount--;
		switch (lastDirection) {
		case 0:	possibilities[1] = 0; break;
		case 1:	possibilities[0] = 0; break;
		case 2:	possibilities[3] = 0; break;
		case 3:	possibilities[2] = 0; break;
		}
	}
	//loop
	while (possibilityCount > 0) {
		//vyberiem nahodny smer
		int r = rand()%possibilityCount;
		int rand = r;
		for (int i = 0;i < 4; i++) {
			if (possibilities[i]) {
				if (r == 0) {
					nextDirection = i;
					break;
				}
				else
					r--;
			}
		}
		//otestujem ci mozem na dane policko ist
		unsigned char nextX, nextY;
		switch (nextDirection) {
		case 0:	//N
			nextX = x; nextY = y - 1;
			break;
		case 1:	//S
			nextX = x; nextY = y + 1;
			break;
		case 2:	//W
			nextX = x - 1; nextY = y;
			break;
		case 3:	//E
			nextX = x + 1; nextY = y;
			break;
		}
		//ano - zapisem cestu do h/v lines, zavolam funkciu na dalsie policko
		if (navstivene[nextY][nextX]==false) {
			switch (nextDirection) {
			case 0:	//N
				vlines[x][y-1] = true; break;
			case 1:	//S
				vlines[x][y] = true; break;
			case 2:	//W
				hlines[y][x-1] = true;break;
			case 3:	//E
				hlines[y][x] = true;break;
			}
			printf("cell %d %d, next %d %d , direction %d, rand %d\n", x, y, nextX, nextY, nextDirection,rand);
			nextCell(nextX, nextY, navstivene,nextDirection);
		}
		//nie - vyberiem iny smer
		possibilityCount--;
		possibilities[nextDirection] = 0;
		//ak dalsi smer uz nie je, ukoncim
	}
}

MazePlan * MazePlan::giveExample(unsigned char rows, unsigned char cols, unsigned char  bsize, unsigned char  lw)
{
	srand(time(NULL));
	int endx = rand() % rows;	//zistit preco niekedy hodi zly endy
	int endy = rand() % cols;	//
	MazePlan* example = new MazePlan( rows, cols, bsize, 0, 0, endx, endy, false, lw);	//rows,cols,bsize,starx,y,endx,y,loops, linewidth
	example->generateMaze();

	return example;
}
MazePlan * MazePlan::giveExample(bool demo)
{
	MazePlan* example = new MazePlan(5, 5, 200, 0, 0, 3, 2, false, 20);	//rows,cols,bsize,starx,y,endx,y,loops
	if (demo)
	{
		example->hlines[0] = new bool[4]{ true,true,false,true };
		example->hlines[1] = new bool[4]{ 1,0,1,0 };
		example->hlines[2] = new bool[4]{ 0,1,0,1 };
		example->hlines[3] = new bool[4]{ 1,0,1,0 };
		example->hlines[4] = new bool[4]{ 0,1,0,1 };

		example->vlines[0] = new bool[4]{ 0,1,1,1 };
		example->vlines[1] = new bool[4]{ 1,0,1,1 };
		example->vlines[2] = new bool[4]{ 0,1,0,1 };
		example->vlines[3] = new bool[4]{ 1,0,0,1 };
		example->vlines[4] = new bool[4]{ 0,1,1,1 };
	}
	else {
		example->generateMaze();
	}
	return example;
}