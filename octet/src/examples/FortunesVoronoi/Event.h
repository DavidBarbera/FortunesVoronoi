#pragma once

#include "Point.h"
#include "BeachLine.h"
#include "Vertex.h"

#include <iostream>

class Event {
public:
	Point*		point;
	bool		isCircleEvent;
	Point*		circleCentre;
	BeachLineNode*	Arc; //if it is Site Event, Arc points to the parabola immediately above. 
						 //if it is Circle Event Arc is the disappearing Arc.
	
	double		x;
	double		y;
	
	//Constructor
	Event(Point* lowestTangent, Point* circle, BeachLineNode* disappearingArc ) 
	{
		point = lowestTangent;
		circleCentre = circle;
		Arc = disappearingArc;
		isCircleEvent = true;
		x = lowestTangent->x;
		y = lowestTangent->y;
	}

	Event(Point* p, bool circle) // for site events, circle is false.
	{
		point = p;
		x = p->x;
		y = p->y;
		isCircleEvent = circle;
		Arc = 0;
	}

	struct CompareEvent : public std::binary_function<Event*, Event*, bool>
	{
		bool operator()(const Event* lhs, const Event* rhs) const
		{
			if (lhs->y == rhs->y) {
				return (lhs->x < rhs->x);
			} else {
				return (lhs->y < rhs->y);
			}
		}


	};
};