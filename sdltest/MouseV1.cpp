#include "MouseV1.h"

bool MouseV1::positionUpdate(CellType cell, double* d_move, int* d_angle)
{
	/*na adresu danu parametrom vrati:
	o kolko policok sa posunie (d_move) - 0..1 policka
	o kolko stupnov sa ma otocit (d_angle) - -360..0..360
	vrati true ak je v cieli, false ak nie je
	*/
	//tu bude AI robota
   	switch (cell) {
	case CellType::c_straight:
		*d_move = 1;
		*d_angle = 0;
		break;
	case CellType::c_right:
		*d_move = 1;
		*d_angle = 90;
		break;
	case CellType::c_left:
		*d_move = 1;
		*d_angle = -90;
		break;
	case CellType::c_cross:
		*d_move = 1;
		*d_angle = -90;	//left hand on the wall
		break;
	case CellType::c_tjun:
		*d_move = 1;
		*d_angle = -90;	//left hand on the wall
		break;
	case CellType::c_tleft:
		*d_move = 1;
		*d_angle = -90;	//left hand on the wall
		break;
	case CellType::c_tright:
		*d_move = 1;
		*d_angle = 0;	//left hand on the wall
		break;
	case CellType::c_deadend:
		*d_move = 1;
		*d_angle = 180;	//otocka
		break;
	case CellType::c_finish:
		*d_move = 0;
		*d_angle = 0;
		return true;
	case CellType::f_lost:
		*d_move = 0;
		*d_angle = 90;
		break;
	default:
		*d_move = 0;
		*d_angle = 90;	//otocka vpravo
	}
	return false;
}
