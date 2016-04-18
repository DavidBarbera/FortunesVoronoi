#pragma once

#include "Point.h"
#include "Face.h"

typedef BreakPoint Orientation;

class HalfEdge {
public:
	Orientation orientation; // with face(site) always at its left where halfedge is pointing.
	Point*		origin;
	Point*		destination;
	HalfEdge*	twin;
	Face*		face;
	HalfEdge*	next;
	HalfEdge*	previous;

	//Constructor
	HalfEdge(Point* site1, Point* site2)
	{
		orientation.leftSite = site1;
		orientation.rightSite = site2;

	}

};

