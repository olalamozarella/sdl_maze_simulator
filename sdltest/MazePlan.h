#pragma once
struct MazePlan
{
	unsigned char cols, rows, blocksize;
	unsigned char startx, starty, endx, endy;
	bool** hlines;
	bool** vlines;
	bool enableLoops;
	unsigned char lineWidth;

	MazePlan(unsigned char p_cols, unsigned  char p_rows, unsigned  char p_blocksize,
		unsigned  char p_startx, unsigned  char p_starty, unsigned  char p_endx, unsigned char p_endy,
		bool p_enableLoops, unsigned char p_lineWidth);
	~MazePlan();
	void generateMaze();
	static MazePlan* giveExample(unsigned char rows = 10, unsigned char cols = 10, unsigned char  bsize = 20, unsigned char  lw=15); //rows,cols,bsize,starx,y,endx,y,loops, linewidth
	static MazePlan* giveExample(bool demo);
private:
	void nextCell(unsigned char x, unsigned char y,bool** navstivene, unsigned char lastDirection=-1);
};

