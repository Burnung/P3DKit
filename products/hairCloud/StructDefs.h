
#ifndef STRUCTDEFS_H
#define	STRUCTDEFS_H

#include "main.h"

#define Pi 3.1415926

class CPoint3D
{
public:
	float	x, y, z;

	CPoint3D ( );					// normal constructor
	CPoint3D ( int, int, int );			// normal constructor
	CPoint3D ( float, float, float );		// normal constructor
	CPoint3D ( double, double, double );		// normal constructor
	virtual ~CPoint3D ();

	float	operator & ( CPoint3D& v );		// dot product
	CPoint3D	operator + ( CPoint3D& v );
	CPoint3D	operator - ( CPoint3D& v );
	CPoint3D	operator * ( CPoint3D& v );		// cross_product
	CPoint3D	operator * ( int k );			// scale by int
	CPoint3D	operator * ( float k );			// scale by float
	CPoint3D	operator * ( double k );		// scale by double

	void operator +=(CPoint3D &v);
	bool operator ==(CPoint3D& v);

	//		int	operator = ( CPoint3D& v );
	void	unify();
	void	RangeUnify(float min,float max);
	float	length();

	CPoint3D ProjectToNewCoordinate(CPoint3D& coordX,CPoint3D& coordY,CPoint3D& coordZ);

	/*VectorR3 cvt2VectorR3()
	{
	VectorR3 r(this->x,this->y,this->z);
	return r;
	}*/
};

#endif
