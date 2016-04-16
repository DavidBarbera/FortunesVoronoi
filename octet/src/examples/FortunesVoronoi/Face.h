#ifndef Face_h
#define Face_h

#include "Point.h"
#include "HalfEdge.h"

class Face {
public:
	Point* site;
	HalfEdge* incidentHalfEdge;
};

#endif