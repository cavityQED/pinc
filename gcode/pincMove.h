#ifndef PINC_MOVE_H
#define PINC_MOVE_H

#include <cmath>

#include "c/stepper/stepper_common.h"

template <typename T>
struct pincVec
{
	T x;
	T y;
	T z;

	pincVec<T> operator*(T c)					{return pincVec<T>(x*c, y*c, z*c);}
	pincVec<T> operator+(const pincVec<T>& vec)	{return pincVec<T>(x+vec.x, y+vec.y + z+vec.z);}
	pincVec<T> operator-(const pincVec<T>& vec)	{return pincVec<T>(x-vec.x, y-vec.y + z-vec.z);}

	p_cartesian step()	{return p_cartesian(x, y, z);}

	double distanceTo(const pincVec<T>& vec) const
	{
		double x2 = vec.x - x;
		x2 *= x2;

		double y2 = vec.y - y;
		y2 *= y2;

		double z2 = vec.z - z;
		z2 *= z2;

		return std::sqrt(x2 + y2 + z2);
	}
};


struct pincMove
{
	uint8_t	mode;
	
	pincVec<double>	cur;
	pincVec<double> end;
	pincVec<double> ctr;

	bool	cw;
	double	feed;
	double	radius;
};


#endif