#pragma once

#include "Point.h"
#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include "BeachLineNode.h"
#include "Topology.h"
#include "Event.h"
#include "EdgeInfo.h"

#include <list>
#include <queue>
#include <set>
#include <functional>
#include <tuple>
#include <iostream>
#include <vector>

class BeachLineNode;
class EdgeInfo;

#define FACTOR 100

namespace vor
{
	/*
	to be able to use naming with more intuition
	*/

	typedef std::list<Point *>		Vertics;
	typedef std::list<EdgeInfo *>	Edges;

	typedef Point					Site;
	typedef std::list<Point *>		Sites;
	typedef std::list<Point *>		Diagram;
	typedef std::list<Vertex *>		Vertices;
	typedef std::list<Face *>		Faces;
	typedef std::list<HalfEdge *>	HalfEdges;

	/*
	Class for generating the Voronoi diagram
	*/
	class Voronoi {


	private:
		Topology topology;  // The  DCEL.
		BeachLineNode* root; // The Binary Search Tree.

		std::priority_queue<Event*, std::vector<Event*>, Event::CompareEvent> Queue; //The priority queue.
		std::set<Event*> falseAlarmEvents; //We can't delete events in Queue so we store the ones we want removed in falseAlarm,
				//and compare any circle event in Queue with falseAlarm, if it is in falseAlarm we won't process it.

		double SweepLine_y; // the y-component of the SweepLine.
		Sites * sites;		// Where we store the points of the cloud.		
		Diagram * diagram; //Where we will store the edges of the Voronoi Diagram. Reading as pairs of points each edge.

		Edges * edges;//store info on the go about the edges to avoid incurring in floating point errors for the calculations.
		Vertics points; //for debugging

		double width, height; //for the bounding box
	public:
		//Constructor
		Voronoi()
		{
			diagram = 0;
			topology.vertexRecords = 0;
			topology.faceRecords = 0;
			topology.halfEdgeRecords = 0;
			root = 0;
		}


		Edges* VoronoiDiagram(Sites * cloudPoints, double w, double h)
		{
			//Initialization	
			sites = cloudPoints;
			root = 0;
			width = w;
			height= h;

			if (!diagram) {
				diagram = new Diagram();
				topology.vertexRecords = new Vertices();
				topology.faceRecords = new Faces();
				topology.halfEdgeRecords = new HalfEdges;
				edges = new Edges();
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
				//Extro edge info for floating point errors avoidance
				for (Edges::iterator i = edges->begin(); i != edges->end(); ++i) delete (*i);
				edges->clear();

				for (Vertics::iterator i = points.begin(); i != points.end(); ++i) delete (*i);
				points.clear();
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

					if (falseAlarmEvents.find(currentEvent) != falseAlarmEvents.end()) {// means this circle event is among the false alarm ones.
						delete( currentEvent );
						falseAlarmEvents.erase(currentEvent);
						currentEvent = 0;
						continue;
					}
					//we check is not a false alarm
					//and use 'continue' to skip the event and get back to beginning of loop

				if (currentEvent->isCircleEvent == true) {
					HandleCircleEvent(currentEvent);
				}
				else {
					HandleSiteEvent(currentEvent->point);
				}
				delete (currentEvent); //Already processed, no longer needed.

			}//Main loop of the Algorithm

			 //Bounding box, (for the infinite edges)
			 //1. complete vertex info for each halfedge record  2. fill
		//ApplyBoundingBox();

		//	prepareEdges();
		//	return diagram;



			//from http://blog.ivank.net/fortunes-algorithm-and-implementation.html
			//It works faster, but we can't do Delaunay afterwards as there is no DCEL.
			FinishEdge(root);

			for (Edges::iterator i = edges->begin(); i != edges->end(); ++i)
			{
				if ((*i)->neighbour)
				{
					(*i)->start = (*i)->neighbour->end;
					delete (*i)->neighbour;
				}
			}

			return edges;

		}//VoronoiDiagram()

		void prepareEdges()
		{
			for (HalfEdges::iterator i = topology.halfEdgeRecords->begin(); i != topology.halfEdgeRecords->end(); ++i) {
				Point* origin = new Point((*i)->origin->x, (*i)->origin->y);
				Point* destination = new Point((*i)->destination->x, (*i)->destination->y);
				diagram->push_back( origin ); 
				diagram->push_back( destination );
			}

		/*	for (Sites::iterator i = sites->begin(); i != sites->end(); ++i)
				diagram->push_back((*i));*/
		}

		void ApplyBoundingBox()
		{
			for (HalfEdges::iterator i = topology.halfEdgeRecords->begin(); i != topology.halfEdgeRecords->end(); ++i) {
					if ((*i)->destination == 0 || (*i)->origin == 0) {
						if ((*i)->destination == 0) {
							(*i)->destination = calculateBBoxIntersection((*i)->origin, (*i)->orientation);
							(*i)->twin->origin = (*i)->destination;
						}
						else {
							(*i)->origin = calculateBBoxIntersection((*i)->destination, (*i)->orientation);
							(*i)->twin->destination = (*i)->origin;
						}
					}

			/*	if ((*i)->destination == 0) {
					(*i)->destination = calculateBBoxIntersection((*i)->origin, (*i)->orientation);
					(*i)->twin->origin = (*i)->destination;
				}
				if ((*i)->origin == 0) {
					(*i)->origin = calculateBBoxIntersection((*i)->destination, (*i)->orientation);
					(*i)->twin->destination = (*i)->origin;
				}*/
			}

		}

		

		Point* calculateBBoxIntersection(Point* v, Orientation orientation)
		{
			Site* s1 = orientation.leftSite;
			Site* s2 = orientation.rightSite;
			Point* V = new Point(0, 0);

			//1. Determining quadrant:
			Point m((s1->x + s2->x) / 2, (s1->y + s2->y) / 2);
			Point s( s2->x-s1->x,s2->y-s1->y ); // s2-s1
			Point u( -s.y, s.x); // perpendicular left-wise
			
			//	Point u(m.x - v->x, m.y - v->y);
		

			if (u.x > 0 && u.y > 0) {
				if (u.x == 0 || u.y == 0) {
					if (u.x == 0) { V->x = v->x; V->y = FACTOR; }
					if (u.y == 0) { V->x = FACTOR; V->y = v->y; }
				}
				else {
					Point d(FACTOR - v->x, FACTOR - v->y); // d: vector from v to (FACTOR,FACTOR), top right corner.
					double slopeD = d.y / d.x; //d.x would be zero only if one of the sites is out of the bbox, which we constructed including all sites.
					double slopeU = u.y / u.x;
					if (slopeD == slopeU) { V->x = FACTOR; V->y = FACTOR; }
					if (slopeD > slopeU) { //hits right side of the box
						V->x = FACTOR; V->y = slopeU*(V->x) + m.y - slopeU*m.x;
					}
					else { //hits top of the box
						V->y = FACTOR; V->x = (V->y - m.y + slopeU*m.x) / slopeU;
					}
				}
			}
			if (u.x < 0 && u.y>0) {
				if (u.x == 0 || u.y == 0) {
					if (u.x == 0) { V->x = v->x; V->y = FACTOR; }
					if (u.y == 0) { V->x = 0; V->y = v->y; }
				}
				else {
					Point d(0 - v->x, FACTOR - v->y); // d: vector from v to (0,FACTOR), top left corner.
					double slopeD = d.y / d.x; //d.x would be zero only if one of the sites is out of the bbox, which we constructed including all sites.
					double slopeU = u.y / u.x;
					if (slopeD == slopeU) { V->x = 0; V->y = FACTOR; }
					if (slopeD > slopeU) { //hits top of the box
						V->y = FACTOR; V->x = (V->y - m.y + slopeU*m.x) / slopeU;
					}
					else { //hits left of the box
						V->x = 0; V->y = slopeU*(V->x) + m.y - slopeU*m.x;
					}
				}

			}
			if (u.x < 0 && u.y < 0) {
				if (u.x == 0 || u.y == 0) {
					if (u.x == 0) { V->x = v->x; V->y = 0; }
					if (u.y == 0) { V->x = 0; V->y = v->y; }
				}
				else {
					Point d(0 - v->x, 0 - v->y); // d: vector from v to (0,0), bottom left corner.
					double slopeD = d.y / d.x; //d.x would be zero only if one of the sites is out of the bbox, which we constructed including all sites.
					double slopeU = u.y / u.x;
					if (slopeD == slopeU) { V->x = 0; V->y = 0; }
					if (slopeD > slopeU) { //hits left of the box

						V->x = 0; V->y = slopeU*(V->x) + m.y - slopeU*m.x;
					}
					else { //hits bottom of the box
						V->y = 0; V->x = (V->y - m.y + slopeU*m.x) / slopeU;
					}
				}
			}
			if (u.x > 0 && u.y < 0) {
				if (u.x == 0 || u.y == 0) {
					if (u.x == 0) { V->x = v->x; V->y = 0; }
					if (u.y == 0) { V->x = FACTOR; V->y = v->y; }
				}
				else {
					Point d(FACTOR - v->x, 0 - v->y); // d: vector from v to (FACTOR,0), bottom right corner.
					double slopeD = d.y / d.x; //d.x would be zero only if one of the sites is out of the bbox, which we constructed including all sites.
					double slopeU = u.y / u.x;
					if (slopeD == slopeU) { V->x = FACTOR; V->y = 0; }
					if (slopeD > slopeU) { //hits right of the box

						V->x = FACTOR; V->y = slopeU*(V->x) + m.y - slopeU*m.x;
					}
					else { //hits bottom of the box
						V->y = 0; V->x = (V->y - m.y + slopeU*m.x) / slopeU;
					}
				}
			}

			return V;
		}

		void HandleCircleEvent(Event* e)
		{
			BeachLineNode* leaf = e->Arc;

			BeachLineNode* leftBreakPoint = leaf->GetLeftParent(leaf);
			BeachLineNode* rightBreakPoint = leaf->GetRightParent(leaf);

			BeachLineNode* leftLeaf = leftBreakPoint->GetLeftChild(leftBreakPoint);
			BeachLineNode* rightLeaf = rightBreakPoint->GetRightChild(rightBreakPoint);

			//side arcs may have cirlce events of the future, now they are false alarms. need to record it.
			if (leftLeaf->circleEvent) { 
				//falseAlarmEvents.insert(leftLeaf->circleEvent); leftLeaf->circleEvent = 0;
			} // is this mean that all triplets that trigger a circleEvent have a pointer 
			if (rightLeaf->circleEvent) { 
				//falseAlarmEvents.insert(rightLeaf->circleEvent); rightLeaf->circleEvent = 0;
 } // is this mean that all triplets that trigger a circleEvent have a pointer 

			

			//Point* vertexPoint = leaf->circleEvent->circleCentre; // a voronoi vertex
			Point* vertexPoint = new Point(e->point->x, GetY(leaf->site, e->point->x));
			points.push_back(vertexPoint);//3

			BeachLineNode* highestBreakPoint = 0;
			BeachLineNode* node = leaf;

			leftBreakPoint->edge->end = vertexPoint;
			rightBreakPoint->edge->end = vertexPoint;

			//locate first our highest breakpoint which we will reuse for our new breakpoint.
			while (node != root) // the top node could be our breakpoint. y not (node->parent != 0)?
			{
				node = node->parent;
				if (node == leftBreakPoint) highestBreakPoint = leftBreakPoint;
				if (node == rightBreakPoint) highestBreakPoint = rightBreakPoint;
			}
			//for the calulations on the go. More precise, no floating point errors.
			highestBreakPoint->edge = new EdgeInfo(vertexPoint, leftLeaf->site, rightLeaf->site);
			edges->push_back(highestBreakPoint->edge);

			// use highest breakpoint for our new breakpoint when the arc disappears.
			highestBreakPoint->breakpoint.leftSite = leftLeaf->site;
			highestBreakPoint->breakpoint.rightSite = rightLeaf->site;
			updateTopologyWithVertex(leftLeaf->site, rightLeaf->site, vertexPoint, leaf->site);



			//relocating pointers in the tree
			BeachLineNode* granParent = leaf->parent->parent;
			if (leaf->parent->Left() == leaf) {
				if (granParent->Left() == leaf->parent) granParent->SetLeft( leaf->parent->Right() );
				if (granParent->Right() == leaf->parent) granParent->SetRight( leaf->parent->Right() );
			}
			else {
				if (granParent->Left() == leaf->parent) granParent->SetLeft( leaf->parent->Left() );
				if (granParent->Right() == leaf->parent) granParent->SetRight( leaf->parent->Left() );

			}

			//delete the leaf and the breakpoint (lowest) no longer in use.
			delete leaf->parent;
			delete leaf;

			//Check for circle events considering now, leftLeaf and rightLeaf
			CheckForCircleEvents(leftLeaf);
			CheckForCircleEvents(rightLeaf);

		}//HandleCircleEvent()

		void updateTopologyWithVertex(Site* s3, Site* s2, Point* voronoiVertex, Site* s1)
		{
			bool face3isNew = true;
			HalfEdge* e32 = new HalfEdge(s3, s2);
			HalfEdge* e23 = new HalfEdge(s2, s3);
			e32->twin = e23;
			e23->twin = e32;

			Vertex* vertex = new Vertex(voronoiVertex, e32);
			topology.vertexRecords->push_back(vertex);

			HalfEdge* e12=0;
			HalfEdge* e21=0;
			HalfEdge* e13=0;
			HalfEdge* e31=0;

			//locate halfedges involved with the new voronoi vertex
			for (HalfEdges::iterator i = topology.halfEdgeRecords->begin(); i != topology.halfEdgeRecords->end(); ++i) {
				if ((*i)->orientation.leftSite == s1 && (*i)->orientation.rightSite == s2) e12 = (*i);
				if ((*i)->orientation.leftSite == s2 && (*i)->orientation.rightSite == s1) e21 = (*i);
				if ((*i)->orientation.leftSite == s1 && (*i)->orientation.rightSite == s3) e13 = (*i);
				if ((*i)->orientation.leftSite == s3 && (*i)->orientation.rightSite == s1) e31 = (*i);
			}

			// updating halfedge info due to the new vertex
			e21->destination = voronoiVertex;
			e13->destination = voronoiVertex;
			e32->destination = voronoiVertex;
			e12->origin = voronoiVertex;
			e31->origin = voronoiVertex;
			e23->origin = voronoiVertex;

			e32->next = e31;
			e13->next = e12;
			e21->next = e23;

			e12->previous = e13;
			e31->previous = e32;
			e23->previous = e21;

			Face* face3 = new Face(s3, e32);

			for (Faces::iterator i = topology.faceRecords->begin(); i != topology.faceRecords->end(); ++i) {
				if ((*i)->site == face3->site) face3isNew = false;
			}
			if (face3isNew) {
				topology.faceRecords->push_back(face3);
			}

			e32->face = face3;
			e23->face = e21->face;

			topology.halfEdgeRecords->push_back(e32);
			topology.halfEdgeRecords->push_back(e23);
		}

		void HandleSiteEvent(Site* p)
		{
			if (!root) {
				root = new BeachLineNode(p); // a new parabolic arc
				return;
			}

			//Consider the list of degenerate events
			//Strange case, degenerate event where root's site and new site have same y-coordinate
		if (root->isLeaf && root->site->y - p->y < 1) //degenerate event //Both the lower space at the same height
			{
				Point * fp = root->site;
				root->isLeaf = false;
				root->SetLeft(new BeachLineNode(fp));
				root->SetRight(new BeachLineNode(p));
				Point * s = new Point((p->x + fp->x) / 2, height); //The start edges of the middle sites
																	 //height of the bbox, so we got the first point of a voronoi edge, right on the upper side of the bbox.
				points.push_back(s);//1
				
				if (p->x > fp->x) root->edge = new EdgeInfo(s, fp, p);//Decide which left to right
				else root->edge = new EdgeInfo(s, p, fp);
				edges->push_back(root->edge);

				return;
			}
			

			BeachLineNode* arcAbove = ArcVerticallyAbove(p);

			if (arcAbove->circleEvent) { //if it is a futre circle event we must mark it as false alarm.
				falseAlarmEvents.insert(arcAbove->circleEvent); //Queue will skip this circle event when will find it in falsAlarmEvents.
				arcAbove->circleEvent = 0;
			}

			//inserting subtree...  and Handling halfedges, topology records (DCEL)....
			BeachLineNode* newArc = insertSubTree(arcAbove, p);

			//check for circle events.

			CheckForCircleEvents( newArc->GetLeftLeaf() );  //check triple where newArc is rightest
			CheckForCircleEvents( newArc->GetRightLeaf() ); //check triple where newArc is leftiest

		}//HandleSiteEvent

		void CheckForCircleEvents(BeachLineNode* mid)		 // check triplets where leaf is at the centre of the triplet
		{
			BeachLineNode* leftBP = mid->GetLeftParent(mid); //left breakpoint of mid
			BeachLineNode* rightBP = mid->GetRightParent(mid);//right breakpoint of mid

			BeachLineNode* left = mid->GetLeftChild(leftBP); //left leaf of mid
			BeachLineNode* right = mid->GetRightChild(rightBP); //left leaf of mid


			if (!right || !mid || !left) return; // no three consecutive arcs, so no chance of a circle.
			if ((right->site->x == left->site->x) && (right->site->y == left->site->y)) return; // no intersection in this case. this is case m1=m2

			Point * point = BisectorsIntersection(leftBP->edge, rightBP->edge);

			if (point == 0) return;
			//Calculate radius
			

			double radius = distance(point, left->site);
			
			double reach = (point->y - radius);

			if (SweepLine_y <= reach)  return;// the tangent is not below the sweep line
															// delete point?
															//!!! we need to check for a degenerate case in here: new site = lowest tangent point.
			//if ( checkPoint(point) ) return;
			Point* lowestTangent = new Point(point->x, point->y - radius);
			


			//insertCircleEvent(point, lowestTangent, mid);
			Event* event = new Event(lowestTangent, true); // , point, mid);
			points.push_back(event->point);//5

			mid->circleEvent = event;
			event->Arc = mid;
			Queue.push(event);

		}

		bool checkPoint(Point* point)
		{ // for debugging purposes
			bool isCalculatedBefore = false;
			for (Vertices::iterator i = topology.vertexRecords->begin(); i != topology.vertexRecords->end(); ++i) {
				if ((*i)->vertex->x == point->x && (*i)->vertex->y == point->y) isCalculatedBefore = true;
				if((*i)->vertex == point ) isCalculatedBefore = true;
			}
			return isCalculatedBefore;
		}

		//too little code to deserve another function. 
		void insertCircleEvent(Point* centreCircle, Point* lowestTangent, BeachLineNode* disappearingArc)// insert circle event in Queue, making sure
		{																					// disappearing arc points to the circle event.
			Event* event = new Event(lowestTangent, true); //, centreCircle, disappearingArc);
			//points.push_back(lowestTangent);//5

			//disappearingArc->circleEvent = event;
			//event->Arc = disappearingArc;
		//	Queue.push(event);
		}

		double distance(Point* p1, Point* p2)
		{
			double dx = p1->x - p2->x;
			double dy = p1->y - p2->y;

			return std::sqrt(dx*dx + dy*dy);
		}

		Point* BisectorsIntersection(EdgeInfo* l, EdgeInfo* r)
		{ //from http://blog.ivank.net/fortunes-algorithm-and-implementation.html
			
			double x = (r->g - l->g) / (l->f - r->f);
			double y = l->f * x + l->g;

			if ((x - l->start->x) / l->direction->x < 0) return 0;
			if ((y - l->start->y) / l->direction->y < 0) return 0;

			if ((x - r->start->x) / r->direction->x < 0) return 0;
			if ((y - r->start->y) / r->direction->y < 0) return 0;

			Point * p = new Point(x, y);
			points.push_back(p);//6
			return p;

			/* Excessive floating point errors this way!!!!
			double dx1 = b->x - a->x;
			double dy1 = b->y - a->y;
			double dx2 = c->x - b->x;
			double dy2 = c->y - b->y;
			double m1;
			double m2;
			double p1;
			double p2;

			if (b->y < a->y && b->y < c->y) return 0; // intersection is above, so no interest.

			Point mid1((a->x + b->x) / 2, (a->y + b->y) / 2);		//midpoint between a and b
			Point mid2((c->x + b->x) / 2, (c->y + b->y) / 2); //midpoint between b and c
			Point* intersectionPoint = new Point(0, 0);

			if (dx1 != 0 && dx2 != 0) {

				m1 = -dy1 / dx1;
				m2 = -dy2 / dx2;

				p1 = mid1.y - m1*mid1.x; // p1 belongs to r1 (bisector between a and b)
				p2 = mid2.y - m2*mid2.x; // p2 belongs to r2 (bisector line between b and c)

				intersectionPoint->x = (p2 - p1) / (m1 - m2);
				intersectionPoint->y = m1*(intersectionPoint->x) + p1;

				return intersectionPoint;
			}

			if (dx1 == 0 && dx2 == 0) return 0; //two vertical parallel lines: no intersections.
			if (dy1 == 0 && dy1 == 0) return 0; //two horizontal lines: no intersections.

			if (dx1 == 0) {
				intersectionPoint->x = mid1.x;
				m2 = -dy2 / dx2;
				p2 = mid2.y - m2*mid2.x;
				intersectionPoint->y = m2*(intersectionPoint->x) + p2;
			}

			if (dx2 == 0) {
				intersectionPoint->x = mid2.x;
				m1 = -dy1 / dx1;
				p1 = mid1.y - m1*mid1.x;
				intersectionPoint->y = m1*(intersectionPoint->x) + p1;
			}
			*/
		}

		BeachLineNode* insertSubTree(BeachLineNode* arc, Site* newSite)//Arc is a leaf node.
		{													  //                     <2,1>
															  //					/    \
															  //				<1,2>    a2
															  //               /    \
															  //x-view:       a0    a1
			Point * start = new Point(newSite->x, GetY(arc->site, newSite->x));//Gets intersection point between parabola above and vertical ray(degenerated parabola) with x as coordinates
			points.push_back(start);//2

			EdgeInfo * el = new EdgeInfo(start, arc->site, newSite);
			EdgeInfo * er = new EdgeInfo(start, newSite, arc->site);

			el->neighbour = er;
			edges->push_back(el);

			//Insert edge in the internal node(not sure is the best way)
			arc->edge = er;
			arc->isLeaf = false;




			BeachLineNode* a0 = new BeachLineNode(arc->site);
			BeachLineNode* a1 = new BeachLineNode(newSite);
			BeachLineNode* a2 = new BeachLineNode(arc->site);
			BeachLineNode* breakpoint12 = new BeachLineNode();

			arc->SetRight(a2);
			arc->SetLeft(breakpoint12);
			arc->Left()->edge = el;

			arc->Left()->SetLeft(a0);
			arc->Left()->SetRight(a1);

			arc->breakpoint.leftSite = newSite;
			arc->breakpoint.rightSite = arc->site;

			breakpoint12->breakpoint.leftSite = a0->site;
			breakpoint12->breakpoint.rightSite = newSite;

			
			// Update DCEL, with new halfedges.//!!! put it in a new function (updateTopology-like)

			//Creating the information:
			updateTopology(newSite, arc->site);



			return a1; // so we can check for circle events right and left of this new arc we just added to the beachline.
						// a1 is the new arc created
		}

		double	GetY(Point * p, double x) // given a site above a new site with x component, gives intersection with the parabola above, y-componet.
		{
			double dp = 2 * (p->y - SweepLine_y);
			double a1 = 1 / dp;
			double b1 = -2 * p->x / dp;
			double c1 = SweepLine_y + dp / 4 + p->x * p->x / dp;

			return(a1*x*x + b1*x + c1);
		}

		void updateTopology(Site* s2, Site* s1)
		{
			bool face1isNew = true;
			bool face2isNew = true;
			HalfEdge* halfedge21 = new HalfEdge(s2, s1);
			Face* face2 = new Face(s2, halfedge21);

			HalfEdge* halfedge12 = new HalfEdge(s1, s2);
			Face* face1 = new Face(s1, halfedge12);

			halfedge21->twin = halfedge12;
			halfedge12->twin = halfedge21;

			halfedge21->face = face2;
			halfedge12->face = face1;

			//Inserting info into our records (DCEL)
			//halfedge records: both edges e12, e21
			topology.halfEdgeRecords->push_back(halfedge12);
			topology.halfEdgeRecords->push_back(halfedge21);

			//Adding faces to our face records, only the new ones. if they are already there they won't be inserted.
			for (Faces::iterator i = topology.faceRecords->begin(); i != topology.faceRecords->end(); ++i) {
				if ((*i)->site == face1->site) face1isNew = false;
				if ((*i)->site == face2->site) face2isNew = false;
			}
			if (face1isNew) topology.faceRecords->push_back(face1);
			if (face2isNew)  topology.faceRecords->push_back(face2);

		}

		BeachLineNode* ArcVerticallyAbove(Point* p)
		{
			BeachLineNode* leafNode = root;
			double x = 0;

			while (leafNode->isLeaf == false) {
				x = calculateXBreakPoint(leafNode, SweepLine_y);//Since it is not leaf, is an internal node. This calculates x-coordinates of the breakpoints of the beachline
				if (x < p->x) {
					leafNode = leafNode->Right();
				}
				else {
					leafNode = leafNode->Left();
				}
			}

			return leafNode;
		}

		double calculateXBreakPoint(BeachLineNode* internalNode, double ly)
		{
			//Calculates the intersection points between two parabolas defined by sites l and r. And decides which intersection
			//belongs to the breakpoint.
/*			Point* l = internalNode->breakpoint.leftSite;
			Point* r = internalNode->breakpoint.rightSite;

			double a = (r->y - ly) - (l->y - ly);
			double b = -2 * (l->x)*(r->y - ly) + 2 * (r->x)*(l->y - ly);
			double c = ( (r->y - ly)*((l->x)*(l->x) + (l->y)*(l->y) - (ly)*(ly)) ) - ( (l->y)*((r->x)*(r->x) + (r->y)*(r->y) - (ly)*(ly)) );

			double x1 = (-b - std::sqrt(b*b - 4 * a*c)) / (2 * a);
			double x2 = (-b + std::sqrt(b*b - 4 * a*c)) / (2 * a);

			if (l->x < r->x) {
				return std::max(x1, x2);
			}
			else {
				return std::min(x1, x2);
			}
		*/ // doesnt seem to work !!!!!!!!!!!!!!!!!!!!!

			// This works. courtesy of http://blog.ivank.net/fortunes-algorithm-and-implementation.html
			Point* p = internalNode->breakpoint.leftSite;
			Point* r = internalNode->breakpoint.rightSite;

			double dp = 2.0 * (p->y - ly);
			double a1 = 1.0 / dp;
			double b1 = -2.0 * p->x / dp;
			double c1 = ly + dp / 4 + p->x * p->x / dp;

			dp = 2.0 * (r->y - ly);
			double a2 = 1.0 / dp;
			double b2 = -2.0 * r->x / dp;
			double c2 = ly + dp / 4 + r->x * r->x / dp;

			double a = a1 - a2;
			double b = b1 - b2;
			double c = c1 - c2;

			double disc = b*b - 4 * a * c;
			double x1 = (-b + std::sqrt(disc)) / (2 * a);
			double x2 = (-b - std::sqrt(disc)) / (2 * a);

			double ry;
			if (p->y < r->y) return std::max(x1, x2);
			else return std::min(x1, x2);

		}

		void	FinishEdge(BeachLineNode * n)
		{//Collection info to finish up infinite edges. 
			//from http://blog.ivank.net/fortunes-algorithm-and-implementation.html
			if (n->isLeaf) { delete n; return; }
			double mx;

			//This looks baad very bad: but it works!
			if (n->edge->direction->x > 0.0)	mx = std::max(width, n->edge->start->x + 10); //These numbers have to be generalized to constants.
			else							mx = std::min(0.0, n->edge->start->x - 10);

			Point * end = new Point(mx, mx * n->edge->f + n->edge->g);
			n->edge->end = end;
			points.push_back(end);//4

			FinishEdge(n->Left());
			FinishEdge(n->Right());
			delete n;
		}


	};
}