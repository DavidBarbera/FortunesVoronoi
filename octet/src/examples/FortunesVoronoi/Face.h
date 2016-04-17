#pragma once

#include "Point.h"
#include "HalfEdge.h"

class Face {
public:
	Point* site;
	HalfEdge* incidentHalfEdge;
};
