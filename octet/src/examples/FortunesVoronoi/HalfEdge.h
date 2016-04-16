#pragma once

#include "Point.h"
#include "Face.h"

class HalfEdge {
public:
	Point*		origin;
	Point*		destination;
	HalfEdge*	twin;
	Face*		face;
	HalfEdge*	next;
	HalfEdge*	previous;
};

