#ifndef Vertex_h
#define Vertex_h

#include "Point.h"
#include "HalfEdge.h"

class Vertex {
	Point* vertex;
	HalfEdge* incidentHalfEdge;
};

#endif