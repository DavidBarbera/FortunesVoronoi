//*****************************************************************************************************************************************************
//Binary Search Balanced Tree implementation
//*****************************************************************************************************************************************************
#pragma once

//#include <tuple>
#include "Point.h"
#include "HalfEdge.h"

enum {left,right};



class BeachLineNode {
public:
	// if the Node is internal (breakpoint), store 2 halfEdges. If it is schedule to disappear, a pointer to the circle event.
	BreakPoint breakpoint; // stores sites coordinates <left site,right site> better make an struct out of this
	HalfEdge* halfEdge;
							//HalfEdge* halfEdge2; you can access the twin through halfEdge.
	Event* circleEvent; //Pointer to a future circle event,that makes this arc disappear(if it is an arc).

	//if the Node is a leaf (parabola), keeps the site
	Point* site;
	bool isLeaf;

	//Pointers for the Binary Tree
	BeachLineNode* parent;
	BeachLineNode* left;
	BeachLineNode* right;

	//Rebalancing operations are needed


	//constructor for a breakpoint
	BeachLineNode() //!!!
	{
		site = 0;
		isLeaf = false;
		circleEvent = 0;
		parent = 0;
		left = 0;
		right = 0;
		halfEdge = 0;
		breakpoint.leftSite = 0;
		breakpoint.rightSite = 0;
	}

	//constructor for a leaf
	BeachLineNode(Point* p)
	{
		isLeaf = true;
		site = p;
		circleEvent = 0;
		parent = 0;
		left = 0;
		right = 0;
		halfEdge = 0;
		breakpoint.leftSite = 0;
		breakpoint.rightSite = 0;

	}




	
	
};
