//*****************************************************************************************************************************************************
//Binary Search Balanced Tree implementation
//*****************************************************************************************************************************************************
#pragma once

#include <tuple>
#include "Point.h"
#include "HalfEdge.h"

enum {left,right};

struct BreakPoint {
	Point * leftSite;
	Point * rightSite;
};

class BeachLineNode {
public:
	// if the Node is internal (breakpoint), store 2 halfEdges. If it is schedule to disappear, a pointer to the circle event.
	std::tuple<Point*, Point*> breakpoint; // stores sites coordinates <left site,right site> better make an struct out of this
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


	//constructor for a breakpoint
	BeachLineNode()
	{
		isLeaf = false;
	}

	//constructor for a leaf
	BeachLineNode(Point* p)
	{
		isLeaf = true;
		site = p;
	}




	
	
};
