//*****************************************************************************************************************************************************
// A DCEL implementation to keep track of the topology of the Voronoi Diagram.
//*****************************************************************************************************************************************************

#pragma once

#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include <list>


class Topology {
public:
	std::list<Vertex*>	 vertexRecords;
	std::list<Face*>	 faceRecords;
	std::list<HalfEdge*> halfEdgeRecords;

};
