#pragma once
#include "Point.h"

//to avoid floating point errors we add this information to calculate precisely circumcentres, or possible voronoi vertices.
// After time trying to find the culprit of the program not working I decided to adopt this way of storing edge information to avoid
//having floating point errors after using the conventional theoretically correct calculations, courtesy of
//http://blog.ivank.net/fortunes-algorithm-and-implementation.html

class EdgeInfo {

public:
	Point *	start;
	Point *	end;
	Point *	direction;
	Point *	left;
	Point *	right;

	double		f; //kind of slope
	double		g; //kind of function

	EdgeInfo * neighbour;

	//constuctor
	EdgeInfo( Point * s,  Point * l,  Point * r)
	{
		start = s;
		left = l;
		right = r;
		neighbour = 0;
		end = 0;

		f = (r->x - l->x) / (l->y - r->y);
		g = s->y - f * s->x;
		direction = new Point(r->y - l->y, -(r->x - l->x));

	}

	//destructor
	~EdgeInfo()
	{
		delete direction;
	}



};