#pragma once
struct Position {
	double x, y, angle;
};
enum CellType {	
	//hocake policko moze byt len jeden z osmych druhov + finish
	c_straight, c_right, c_left, c_cross, c_tjun, c_tleft, c_tright, c_deadend, c_finish, 
	//toto moze najst ak strati stopu
	f_deadend, f_right, f_left, f_tjun, f_straight,
	f_lost	//ak uz fakt nevie co
};
struct MazeAttributes {
	unsigned char rows, cols, bsize, lw;
};
struct Color {
	unsigned char r, g, b, a;
};