#if !defined(BEZIER_H)
#define BEZIER_H

#include <Windows.h>

class CBezier
{
public:
	CBezier();
	~CBezier();
	POINT getNextPos();
	void create(int _point_num, POINT *_point, double _step);
	void recover();

private:
	double step;
	double step_now;
	unsigned int num_point;
	POINT *point;

	POINT hor(int degree, POINT *point, double t);  
};

#endif