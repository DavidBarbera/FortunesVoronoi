//*****************************************************************************************************************************************************
//Binary Search Balanced Tree implementation
//*****************************************************************************************************************************************************
#pragma once

//#include <tuple>
#include "Point.h"
#include "HalfEdge.h"
#include "Event.h"
#include "EdgeInfo.h"


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

	EdgeInfo * edge; //store info about edges to about floating point errors

	//Pointers for the Binary Tree
	BeachLineNode* parent;
private:	
	BeachLineNode* left;
	BeachLineNode* right;

public:
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
	//Setters
	void SetLeft(BeachLineNode * n)
	{
		left = n;
		n->parent = this;
	}

	void SetRight(BeachLineNode * n)
	{
		right = n;
		n->parent = this;
	}

	BeachLineNode * Left()
	{
		return left;
	}

	BeachLineNode * Right()
	{
		return right;
	}

	// Some useful functions.

	BeachLineNode* GetLeftLeaf() // gets closest left leaf of 'this'-node (x-wise). 'this' is always a leaf.
	{							
		return GetLeftChild(GetLeftParent(this));
	}

	BeachLineNode* GetRightLeaf() // gets closest right leaf of 'this'-node (x - wise). 'this' is always a leaf.
	{
		return GetRightChild(GetRightParent(this));
	}
	
	BeachLineNode* GetLeftParent(BeachLineNode* n) // Returns closest left parent (breakpoint) (x-wise)
	{														//Here n is always a leaf
		BeachLineNode* parentNode = n->parent;
		BeachLineNode* lastNode = n;

		while (parentNode->left == lastNode)
		{
			if (!parentNode->parent) return 0; // Case parent is the root of the tree.
			lastNode = parentNode;
			parentNode = parentNode->parent;
		}

		return parentNode;
	}

	BeachLineNode* GetRightParent(BeachLineNode* n)// Returns closest right parent (breakpoint) (x-wise)
	{														//Here n is always a leaf
		BeachLineNode* parentNode = n->parent;
		BeachLineNode* lastNode = n;

		while (parentNode->right == lastNode)
		{
			if (!parentNode->parent) return 0; // Case parent is the root of the tree.
			lastNode = parentNode;
			parentNode = parentNode->parent;
		}

		return parentNode;
	}

	BeachLineNode* GetLeftChild(BeachLineNode* n) // Get closest left leaf (x-wise)
	{											  // Here n is always an internal node (i.e. not a leaf).
		if (!n) return 0;					  // if n is the root
		BeachLineNode* child = n->left;
		while (!child->isLeaf) child = child->right;
			
		return child;
	}

	BeachLineNode* GetRightChild(BeachLineNode* n) // Get closest right leaf (x-wise)
	{											  // Here n is always an internal node (i.e. not a leaf).
		if (!n) return 0;					  // if n is the root
		BeachLineNode* child = n->right;
		while (!child->isLeaf) child = child->left;

		return child;
	}

};
