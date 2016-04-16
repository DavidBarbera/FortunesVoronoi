//*****************************************************************************************************************************************************
//Binary Search Balanced Tree implementation
//*****************************************************************************************************************************************************
#pragma once

#include <tuple>
#include "Point.h"
#include "HalfEdge.h"

enum {left,right};

class BeachLineNode {
public:
	// if the Node is internal (breakpoint), store 2 halfEdges. If it is schedule to disappear, a pointer to the circle event.
	std::tuple<Point*, Point*> breakpoint;
	HalfEdge* halfEdge1;
	HalfEdge* halfEdge2;
	Event* circleEvent;

	//if the Node is a leaf (parabola), keeps the site
	Point* site;
	bool isLeaf;

	//Pointers for the Tree
	BeachLineNode* parent;
	BeachLineNode* left;
	BeachLineNode* right;

	//Rebalancing operations are needed
	
	
};
