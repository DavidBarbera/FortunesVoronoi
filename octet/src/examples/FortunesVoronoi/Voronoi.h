#pragma once

#include "Point.h"
#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include "BeachLineNode.h"
#include "Topology.h"
#include "Event.h"
#include "Cloud.h"
#include "FortunesVoronoi.h"

#include <list>
#include <queue>
#include <set>
#include <functional>
#include <tuple>
#include <iostream>

namespace octet {
	class Voronoi : public resource {
		typedef Point					Site;
		typedef std::list<Point *>		Sites;
		typedef std::list<Point *>		Diagram;
		typedef std::list<Vertex *>		Vertices;
		typedef std::list<Face *>		Faces;
		typedef std::list<HalfEdge *>	HalfEdges;


	public:
		Topology topology;  // The  DCEL.
		BeachLineNode* root; // The Binary Search Tree.

		std::priority_queue<Event*, std::vector<Event*>, Event::CompareEvent> Queue; //The priority queue.
		std::set<Event*> falseAlarmEvents; //We can't delete events in Queue so we store the ones we want removed in falseAlarm,
									 //and compare any circle event in Queue with falseAlarm, if it is in falseAlarm we won't process it.

		double SweepLine_y; // the y-component of the SweepLine.
		Sites * sites;		// Where we store the points of the cloud.		
		Diagram * diagram; //Where we will store the edges of the Voronoi Diagram. Reading as pairs of points each edge.

		dynarray<vec2> edges;

		//Constructor
		Voronoi()
		{
			diagram = 0;
			topology.vertexRecords = 0;
			topology.faceRecords = 0;
			topology.halfEdgeRecords = 0;
			root = 0;
		}

		void VoronoiDraw()
		{
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glLineWidth();
			glBegin(GL_LINES);

			for (unsigned int i = 0; i < edges.size(); i++) {
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f(edges[i].x(),edges[i].y());
			}

			glEnd();
		}

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

				//Bounding box, (for the infinite edges)
				//1. complete vertex info for each halfedge record  2. fill
				ApplyBoundingBox();

				prepareEdges();

			}//Main loop of the Algorithm
		}//VoronoiDiagram()

		void prepareEdges()
		{
			for (HalfEdges::iterator i = topology.halfEdgeRecords->begin(); i != topology.halfEdgeRecords->end(); ++i) {
				edges.push_back(vec2((*i)->origin->x, (*i)->destination->y)); i++;
			}
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
			}
		}

		Point* calculateBBoxIntersection(Point* v, Orientation orientation)
		{
			Site* s1 = orientation.leftSite;
			Site* s2 = orientation.rightSite;
			Point* V = new Point(0, 0);

			//1. Determining quadrant:
			Point m((s1->x + s2->x) / 2, (s1->y + s2->y) / 2);
			Point u(m.x - v->x, m.y - v->y);

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

		void HandleCircleEvent(BeachLineNode* leaf)
		{
			if (falseAlarmEvents.find(leaf->circleEvent) != falseAlarmEvents.end()) {// means this circle event is among the false alarm ones.
				delete(leaf->circleEvent);
				falseAlarmEvents.erase(leaf->circleEvent);
				leaf->circleEvent = 0;
				return;
			}

			BeachLineNode* leftBreakPoint = leaf->GetLeftParent(leaf);
			BeachLineNode* rightBreakPoint = leaf->GetRightParent(leaf);

			BeachLineNode* leftLeaf = leftBreakPoint->GetLeftChild(leftBreakPoint);
			BeachLineNode* rightLeaf = rightBreakPoint->GetLeftChild(rightBreakPoint);

			BeachLineNode* highestBreakPoint;
			BeachLineNode* node = leaf;

			Point* vertexPoint = leaf->circleEvent->circleCentre; // a voronoi vertex

			//locate first our highest breakpoint which we will reuse for our new breakpoint.
			while (node != root) // the top node could be our breakpoint. y not (node->parent != 0)?
			{
				node = node->parent;
				if (node == leftBreakPoint) highestBreakPoint = leftBreakPoint;
				if (node == rightBreakPoint) highestBreakPoint = rightBreakPoint;
			}

			// use highest breakpoint for our new breakpoint when the arc disappears.
			highestBreakPoint->breakpoint.leftSite = leftLeaf->site;
			highestBreakPoint->breakpoint.rightSite = rightLeaf->site;
			updateTopologyWithVertex(leftLeaf->site, rightLeaf->site, vertexPoint, leaf->site);



			//relocating pointers in the tree
			BeachLineNode* granParent = leaf->parent->parent;
			if (leaf->parent->left == leaf) {
				if (granParent->left == leaf->parent) granParent->left = leaf->parent->right;
				if (granParent->right == leaf->parent) granParent->right = leaf->parent->right;
			}
			else {
				if (granParent->left == leaf->parent) granParent->left = leaf->parent->left;
				if (granParent->right == leaf->parent) granParent->right = leaf->parent->left;

			}

			//delete the leaf and the breakpoint (lowest) no longer in use.
			delete leaf->parent;
			delete leaf;

			//Check for circle events considering now, leftLeaf and rightLeaf
			CheckTripleMid(leftLeaf);
			CheckTripleMid(rightLeaf);

		}//HandleCircleEvent()

		void updateTopologyWithVertex(Site* s3, Site* s2, Point* voronoiVertex, Site* s1)
		{
			HalfEdge* e32 = new HalfEdge(s3, s2);
			HalfEdge* e23 = new HalfEdge(s2, s3);
			e32->twin = e23;
			e23->twin = e32;

			Vertex* vertex = new Vertex(voronoiVertex, e32);
			topology.vertexRecords->push_back(vertex);

			HalfEdge* e12;
			HalfEdge* e21;
			HalfEdge* e13;
			HalfEdge* e31;

			//locate halfedges involved with the new voronoi vertex
			for (HalfEdges::iterator i = topology.halfEdgeRecords->begin(); i != topology.halfEdgeRecords->end(); ++i) {
				if ((*i)->orientation.leftSite == s1 && (*i)->orientation.leftSite == s2) e12 = (*i);
				if ((*i)->orientation.leftSite == s2 && (*i)->orientation.leftSite == s1) e21 = (*i);
				if ((*i)->orientation.leftSite == s1 && (*i)->orientation.leftSite == s3) e13 = (*i);
				if ((*i)->orientation.leftSite == s3 && (*i)->orientation.leftSite == s1) e31 = (*i);
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
			topology.faceRecords->push_back(face3);
			e32->face = face3;
			e23->face = e21->face;

			topology.halfEdgeRecords->push_back(e32);
			topology.halfEdgeRecords->push_back(e23);


		}

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

			//inserting subtree...  and Handling halfedges, topology records (DCEL)....
			BeachLineNode* newArc = insertSubTree(arcAbove, p);

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
			if ((right->site->x == left->site->x) && (right->site->y == left->site->y)) return; // no intersection in this case. this is case m1=m2

			Point * point = BisectorIntersection(left->site, mid->site, right->site);

			if (point == 0) return;
			//Calculate radius

			double radius = distance(point, leaf->site);

			if (SweepLine_y <= (point->y - radius))  return;// the tangent is not below the sweep line
			// delete point?
			//!!! we need to check for a degenerate case in here: new site = lowest tangent point.

			Point* lowestTangent = new Point(point->x, point->y - radius);

			insertCircleEvent(point, lowestTangent, mid); //  mid arc will disappear

		}

		void CheckTripleMid(BeachLineNode* leaf) // check triple where leaf is at the left of the triple
		{
			BeachLineNode* left = leaf->GetLeftLeaf();
			BeachLineNode* mid;
			BeachLineNode* right = leaf->GetRightLeaf();

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

		void insertCircleEvent(Point* centreCircle, Point* lowestTangent, BeachLineNode* Arc)// insert circle event in Queue, making sure
		{																					// disappearing arc points to the circle event.
			Event* event = new Event(lowestTangent, centreCircle, Arc);
			Arc->circleEvent = event;
			Queue.push(event);
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

			Point mid1((a->x + b->x) / 2, (a->y + b->y) / 2);		//midpoint between a and b
			Point mid2((c->x + b->x) / 2, (c->y + b->y) / 2); //midpoint between b and c
			Point* intersectionPoint = new Point(0, 0);

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
			updateTopology(newSite, arc1->site);



			return newArc2; // so we can check for circle events right and left of this new arc we just added to the beachline.

		}

		void updateTopology(Site* s2, Site* s1)
		{
			HalfEdge* halfedge21 = new HalfEdge(s2, s1);
			Face* face2 = new Face(s2, halfedge21);

			HalfEdge* halfedge12 = new HalfEdge(s1, s2);
			Face* face1 = new Face(s1, halfedge12);

			halfedge21->twin = halfedge12;
			halfedge12->twin = halfedge21;

			halfedge21->face = face2;
			halfedge12->face = face1;

			//Inserting info into our records (DCEL)
			topology.halfEdgeRecords->push_back(halfedge12);
			topology.halfEdgeRecords->push_back(halfedge21);

			topology.faceRecords->push_back(face1);
			topology.faceRecords->push_back(face2);
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
			double c = (r->y - ly)*((l->x)*(l->x) + (l->y)*(l->y) - (ly)*(ly)) - (l->y)*((r->x)*(r->x) + (r->y)*(r->y) - (ly)*(ly));

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
}