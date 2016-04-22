#pragma once

#include "Point.h"
//#include "BeachLineNode.h"

class BeachLineNode;

#include <iostream>

class Event {
public:
	Point*		point;
	bool		isCircleEvent;
	//Point*		circleCentre;
	BeachLineNode*	Arc;			//if it is Site Event, Arc points to the parabola immediately above. 
	double		x;					//if it is Circle Event Arc is the disappearing Arc.
	double		y;
	
	
	//Constructor
/*	Event(Point* lowestTangent, bool circle) //, Point* circle ) 
	{
		point = lowestTangent;
		//circleCentre = circle;
		Arc = 0;
		isCircleEvent = circle;
		x = lowestTangent->x;
		y = lowestTangent->y;
	}*/

	Event(Point* p, bool isCircle) // for site events, circle is false.
	{
		point = p;
		x = p->x;
		y = p->y;
		isCircleEvent = isCircle;
		Arc = 0;
	}

	~Event() {}

	struct CompareEvent : public std::binary_function<Event*, Event*, bool>
	{
		bool operator()(const Event* lhs, const Event* rhs) const
		{
			if (lhs->y == rhs->y) {
				return (lhs->x < rhs->x);
			} else {
				return (lhs->y < rhs->y);
			}
			//return (lhs->y < rhs->y);
		}

	};
};