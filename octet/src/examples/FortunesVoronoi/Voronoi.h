#pragma once

#include "Point.h"
#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include "BeachLine.h"
#include "Topology.h"
#include "Event.h"
#include "Cloud.h"

#include <list>
#include <queue>
#include <set>
#include <functional>

class Voronoi {
	typedef std::list<Point *> Diagram;
	typedef std::list<Vertex *>		Vertices;
	typedef std::list<Face *>		Faces;
	typedef std::list<HalfEdge *>	HalfEdges;

private:
	Topology topology;  // DCEL
	BeachLineNode* root; // Binary Search Tree

	std::priority_queue<Event*, std::vector<Event*>, Event::CompareEvent> Queue;
	std::set<Event*> falseAlarm; //We can't delete events in Queue so we store the ones we want removed in falseAlarm,
								 //and compare any circle event in Queue with falseAlarm, if it is in falseAlarm we won't process it.

	double SweepLine_y;
	::cloud::Sites * sites;
	Diagram * diagram;

	void VoronoiDiagram(::cloud::Sites * cloudPoints)
	{
		sites = cloudPoints;
		root = 0;

		if (!diagram) {
			diagram = new Diagram();
		}
		else {
			//Resetting/Initalizing diagram data structure
			for (Diagram::iterator i = diagram->begin(); i != diagram->end(); ++i) delete (*i);
			diagram->clear();
			//Resetting/Initializing DCEL
			for (Vertices::iterator i = topology.vertexRecords->begin(); i != topology.vertexRecords->end(); ++i) delete (*i);
			topology.vertexRecords->clear();
			for (Faces::iterator i = topology.faceRecords->begin(); i != topology.faceRecords->end(); ++i) delete (*i);
			topology.faceRecords->clear();
			for (HalfEdges::iterator i = topology.halfEdgeRecords->begin(); i != topology.halfEdgeRecords->end(); ++i) delete (*i);
			topology.halfEdgeRecords->clear();
		}
	}

};