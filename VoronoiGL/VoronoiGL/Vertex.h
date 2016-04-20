#pragma once

#include "Point.h"
#include "HalfEdge.h"

class Vertex {
public:

	Point* vertex;
	HalfEdge* incidentHalfEdge;

	//constructor
	Vertex(Point* p, HalfEdge* he)
	{
		vertex = p;
		incidentHalfEdge = he;
	}
};

