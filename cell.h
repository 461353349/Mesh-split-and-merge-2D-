#pragma once
#include "point.h"
#include <SFML/Graphics.hpp>
#include "edge.h"

struct Cell {
	Point* p0;
	Point* p1;
	Point* p2;
	Point* p3;
};

Edge getEdgeCell(Point* p0, Point* p1);