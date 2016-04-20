//*****************************************************************************************************************************************************
// A DCEL implementation to keep track of the topology of the Voronoi Diagram.
//*****************************************************************************************************************************************************

#pragma once

#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include <list>


class Topology {
	typedef std::list<Vertex *>		Vertices;
	typedef std::list<Face *>		Faces;
	typedef std::list<HalfEdge *>	HalfEdges;

public:
	Vertices*	 vertexRecords;
	Faces*		 faceRecords;
	HalfEdges*	 halfEdgeRecords;

};
