#include "sleep.h"
extern float hz;

bool mytime::sleep(int time)       //  timeÎª1/1000Ãë
{
	count++;
	if(count>(hz*time))
	{
		count = 0.0;
		return true;
	}
	return false;
}

void mytime::clear()
{
	count=0;
}