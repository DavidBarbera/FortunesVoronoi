#pragma once

struct Point
{
public:
	double x, y;
	Point(double xx, double yy)
	{
		x = xx;
		y = yy;
	}

			
};

struct BreakPoint {
public:
	Point * leftSite;
	Point * rightSite;
};
