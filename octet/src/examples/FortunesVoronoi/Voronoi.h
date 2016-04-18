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
	std::set<Event*> falseAlarmEvents; //We can't delete events in Queue so we store the ones we want removed in falseAlarm,
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
		BeachLineNode* arcAbove = ArcVerticallyAbove(p);
		
		if (arcAbove->circleEvent) { //if it is a futre circle event we must mark it as false alarm.
			falseAlarmEvents.insert(arcAbove->circleEvent); //Queue will skip this circle event when will find it in falsAlarmEvents.
			arcAbove->circleEvent = 0;
		}

		//inserting subtree...  and Handling halfedges, topology records....
		BeachLineNode* newArc = insertSubTree(arcAbove,p);

		//check for circle events.

		CheckForCircleEvents(newArc);




	}//HandleSiteEvent

	void CheckForCircleEvents(BeachLineNode* leaf)
	{
		CheckTripleLefty(leaf);
		CheckTripleRighty(leaf);

	}

	void CheckTripleLefty(BeachLineNode* leaf) // check triple where leaf is at the left of the triple
	{
		BeachLineNode* left = leaf;
		BeachLineNode* mid = leaf->GetRightLeaf();
		BeachLineNode* right = mid->GetRightLeaf();

		if (!right || !mid) return; // no three consecutive arcs, so no chance of a circle.
		if ((right->site->x == left->site->x) && (right->site->y == left->site->y)) return; // no intersection in this case. this is case m1=m2

		Point * point = BisectorIntersection(left->site, mid->site, right->site);

		if (point == 0) return;
		//Calculate radius

		double radius = distance(point, leaf->site);

		if (SweepLine_y <= (point->y - radius))  return;// the tangent is not below the sweep line
														// delete point?
														//!!! we need to check for a degenerate case in here: new site = lowest tangent point.

		Point* lowestTangent = new Point(point->x, point->y - radius);

		insertCircleEvent(point, lowestTangent, mid);
		


	}

	void CheckTripleRighty(BeachLineNode* leaf) // check triple where leaf is at the right of the triple
	{
		BeachLineNode* right = leaf;
		BeachLineNode* mid = leaf->GetLeftLeaf();
		BeachLineNode* left = mid->GetLeftLeaf();

		if (!left || !mid) return; // no three consecutive arcs, so no chance of a circle.
		if ((right->site->x == left->site->x) && (right->site->y == left->site->y )) return; // no intersection in this case. this is case m1=m2
		
		Point * point = BisectorIntersection(left->site, mid->site, right->site);

		if (point == 0) return;
		//Calculate radius

		double radius = distance(point, leaf->site);

		if (SweepLine_y <= (point->y - radius))  return;// the tangent is not below the sweep line
		// delete point?
		//!!! we need to check for a degenerate case in here: new site = lowest tangent point.
		
		Point* lowestTangent = new Point(point->x, point->y - radius);

		insertCircleEvent( point, lowestTangent, mid );

		
	}

	void insertCircleEvent(Point* centreCircle, Point* lowestTangent,BeachLineNode* Arc)
	{
		Event* circleEvent = new Event( lowestTangent, centreCircle,Arc );
		Queue.push(circleEvent);
	}

	double distance(Point* p1, Point* p2)
	{
		double dx = p1->x - p2->x;
		double dy = p2->y - p2->y;

		return sqrt(dx*dx + dy*dy);
	}

	Point* BisectorIntersection(Point* a, Point* b, Point* c)
	{
		double dx1 = b->x - a->x;
		double dy1 = b->y - a->y;
		double dx2 = c->x - b->x;
		double dy2 = c->y - b->y;
		double m1;
		double m2;
		double p1;
		double p2;

		if (b->y < a->y && b->y < c->y) return 0; // intersection is above, so no interest.

		Point mid1((a->x+b->x)/2,(a->y+b->y)/2);		//midpoint between a and b
		Point mid2((c->x + b->x) / 2, (c->y + b->y) / 2); //midpoint between b and c
		Point* intersectionPoint= new Point(0, 0);

		if (dx1 != 0 && dx2 != 0) {
			
			m1 = dy1 / dx1;
			m2 = dy2 / dx2;

			p1 = mid1.y - m1*mid1.x; // p1 belongs to r1 (bisector between a and b)
			p2 = mid2.y - m2*mid2.x; // p2 belongs to r2 (bisector line between b and c)

			intersectionPoint->x = (p2 - p1) / (m1 - m2);
			intersectionPoint->y = m1*(intersectionPoint->x) + p1;

			return intersectionPoint;
		}

		if (dx1 == 0 && dx2 == 0) return 0; //two vertical parallel lines: no intersections.

		if (dx1 == 0) {
			intersectionPoint->x = mid1.x;
			m2 = dy2 / dx2;
			p2 = mid2.y - m2*mid2.x;
			intersectionPoint->y = m2*(intersectionPoint->x) + p2;
		}

		if (dx2 == 0) {
			intersectionPoint->x = mid2.x;
			m1 = dy1 / dx1;
			p1 = mid1.y - m1*mid1.x;
			intersectionPoint->y = m1*(intersectionPoint->x) + p1;
		}
		
	}

	BeachLineNode* insertSubTree(BeachLineNode* arc1, Site* newSite)//Arc is a leaf node.
	{													  //                     <2,1>
														  //					/    \
														  //				<1,2>    arc1
														  //               /    \
														  //x-view:     arc12   arc2
		BeachLineNode* breakpoint21 = new BeachLineNode;
		BeachLineNode* breakpoint12 = new BeachLineNode;
		BeachLineNode* newArc2 = new BeachLineNode(newSite);
		BeachLineNode* newArc12 = new BeachLineNode(newSite);

		breakpoint21->parent = arc1->parent; 
		arc1->parent = breakpoint21;
		breakpoint21->breakpoint.rightSite = arc1->site;
		breakpoint21->breakpoint.leftSite = newSite;
		breakpoint21->right = arc1;
		breakpoint21->left = breakpoint12;

		breakpoint12->parent = breakpoint21;
		breakpoint12->breakpoint.leftSite = arc1->site;
		breakpoint12->breakpoint.rightSite = newSite;
		breakpoint12->left = newArc12; newArc12->parent = breakpoint12;
		breakpoint12->right = newArc2; newArc2->parent = breakpoint12;

		newArc12->site = arc1->site;
		newArc2->site = newSite;


	    // Update DCEL, with new halfedges.//!!! put it in a new function (updateTopology-like)

		//Creating the information:
		HalfEdge* halfedge21 = new HalfEdge(newSite, arc1->site);
		Face* face2 = new Face(newSite, halfedge21);

		HalfEdge* halfedge12 = new HalfEdge(arc1->site, newSite);
		Face* face1 = new Face(arc1->site, halfedge12);

		halfedge21->twin = halfedge12;
		halfedge12->twin = halfedge21;

		halfedge21->face = face2;
		halfedge12->face = face1;

		//Inserting info into our records (DCEL)
		topology.halfEdgeRecords->push_back(halfedge12);
		topology.halfEdgeRecords->push_back(halfedge21);

		topology.faceRecords->push_back(face1);
		topology.faceRecords->push_back(face2);


		return newArc2; // so we can check for circle events right and left of this new arc we just added to the beachline.

	}

	BeachLineNode* ArcVerticallyAbove(Point* p)
	{
		BeachLineNode* leafNode = root;
		double x = 0;

		while (leafNode->isLeaf == false) {
			x = calculateXBreakPoint(leafNode, SweepLine_y);//Since it is not leaf, is an internal node. This calculates x-coordinates of the breakpoints of the beachline
			if (x < p->x) leafNode = leafNode->right;
			else leafNode = leafNode->left;
		}

		return leafNode;
	}

	double calculateXBreakPoint(BeachLineNode* internalNode, double ly)
	{
		//Calculates the intersection points between two parabolas defined by sites l and r. And decides which intersection
		//belongs to the breakpoint.
		Point* l = internalNode->breakpoint.leftSite;
		Point* r = internalNode->breakpoint.rightSite;
		
		double a = (r->y - ly) - (l->y - ly);
		double b = -2 * (l->x)*(r->y - ly) + 2 * (r->x)*(l->y - ly);
		double c = (r->y - ly)*( (l->x)*(l->x) + (l->y)*(l->y) - (ly)*(ly) )  -  (l->y)*( (r->x)*(r->x) + (r->y)*(r->y) - (ly)*(ly) );

		double x1 = (-b - sqrt(b*b - 4 * a*c)) / (2 * a);
		double x2 = (-b + sqrt(b*b - 4 * a*c)) / (2 * a);

		if (l->x < r->x) {
			return std::max(x1, x2);
		}
		else {
			return std::min(x1, x2);
		}
	}

};