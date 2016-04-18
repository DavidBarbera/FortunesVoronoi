#pragma once

#include "Point.h"
#include "HalfEdge.h"

class Vertex {
	Point* vertex;
	HalfEdge* incidentHalfEdge;
};

