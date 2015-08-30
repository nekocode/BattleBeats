#include "Bezier.h"

void CBezier::create(int _point_num, POINT *_point, double _step) {
	num_point = _point_num;
	point = _point;

	step = 1.0/_step;
	step_now = 0.0;
}

CBezier::CBezier() {
	point = NULL;
}

CBezier::~CBezier() {
}

void CBezier::recover() {
	step_now = 0.0;
}

POINT CBezier::getNextPos() {
	POINT rlt = hor(num_point-1, point, step_now);
	step_now += step;
	if(step_now > 1.0)
		step_now = 0.0;
	return rlt;
}

POINT CBezier::hor(int degree, POINT *point, double t) {
	POINT aux;
	if(point==NULL)
		return aux;
	int i,choose_i;
	double fact,t1,x,y;
	t1=1.0-t;
	fact=1.0;
	choose_i=1;
	x=(double)point[0].x*t1;
	y=(double)point[0].y*t1;
	for(i=1;i<degree;i++) {
		fact=fact*t;
		choose_i=choose_i*(degree-i+1)/i;
		x=(x+fact*choose_i*point[i].x)*t1;
		y=(y+fact*choose_i*point[i].y)*t1;
	}
	x=x+fact*t*(double)point[degree].x;
	y=y+fact*t*(double)point[degree].y;
	aux.x=(long)x;
	aux.y=(long)y;
	return aux;
}