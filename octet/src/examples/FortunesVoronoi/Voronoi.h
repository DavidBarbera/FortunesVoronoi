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
#include <tuple>
#include <iostream>

class Voronoi {
	typedef Point					Site;
	typedef std::list<Point *>		Sites;
	typedef std::list<Point *>		Diagram;
	typedef std::list<Vertex *>		Vertices;
	typedef std::list<Face *>		Faces;
	typedef std::list<HalfEdge *>	HalfEdges;

private:
	Topology topology;  // The  DCEL.
	BeachLineNode* root; // The Binary Search Tree.

	std::priority_queue<Event*, std::vector<Event*>, Event::CompareEvent> Queue; //The priority queue.
	std::set<Event*> falseAlarm; //We can't delete events in Queue so we store the ones we want removed in falseAlarm,
								 //and compare any circle event in Queue with falseAlarm, if it is in falseAlarm we won't process it.

	double SweepLine_y; // the y-component of the SweepLine.
	Sites * sites;		// Where we store the points of the cloud.		
	Diagram * diagram; //Where we will store the edges of the Voronoi Diagram. Reading as pairs of points each edge.

	//Constructor
	Voronoi()
	{
		diagram = 0;
		topology.vertexRecords = 0;
		topology.faceRecords = 0;
		topology.halfEdgeRecords = 0;
		root = 0;
	}
	void HandleCircleEvent(BeachLineNode* leaf);

	void VoronoiDiagram(::cloud::Sites * cloudPoints)
	{
		//Initialization	
		sites = cloudPoints;
		root = 0;

		if (!diagram) {
			diagram = new Diagram();
			topology.vertexRecords = new Vertices();
			topology.faceRecords = new Faces();
			topology.halfEdgeRecords = new HalfEdges;
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

		//Initializing Priority Queue
		for (Sites::iterator i = sites->begin(); i != sites->end(); ++i) {
			Queue.push(new Event(*i, false));//i is a pointer to a Point, which in this case represents sites (or places), i.e., the points
										   //around which we construct the Voronoi Diagram
		}


		//Main loop of the Algorithm:
		Event *currentEvent;
		while (!Queue.empty()) {
			currentEvent = Queue.top(); // event with highest y-component.
			Queue.pop(); //removing the event from the queue
			SweepLine_y = currentEvent->point->y; //sweepline where the current event to be processed is

			if (currentEvent->isCircleEvent == true) {
				//we check is not a false alarm
				//and use 'continue' to skip the event and get back to beginning of loop
			}

			if (currentEvent->isCircleEvent == true) {
				HandleCircleEvent(currentEvent->Arc);
			}
			else {
				HandleSiteEvent(currentEvent->point);
			}

		}//Main loop of the Algorithm
	}//VoronoiDiagram()

	void HandleCircleEvent(BeachLineNode* leaf)
	{

	}//HandleCircleEvent()

	void HandleSiteEvent(Site* p)
	{
		if (!root) {
			root = new BeachLineNode(p); // a new parabola
			return;
		}

		//Consider the list of degenerate events



	}//HandleSiteEvent

	BeachLineNode* ArcVerticallyAbove(Point* p)
	{
		BeachLineNode* leafNode = root;
		double x = 0;

		while (leafNode->isLeaf == false) {
			x = calculateXBreakPoint(leafNode, SweepLine_y);

		}


	}

	double calculateXBreakPoint(BeachLineNode* internalNode, double ly)
	{
		double leftx = internalNode->(std::get<left>(breakpoint))->x;
			//std::get<0>(breakpoint)->x;
	}

};