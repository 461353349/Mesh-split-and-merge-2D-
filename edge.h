#pragma once
#include <vector>
#include <iostream>

#include "point.h"

namespace sf
{
    class RenderWindow;
}


struct Edge {
	Point p0;
	Point p1;
};

bool getEdgeIntersection(const Edge* e1, const Edge* e2, Point* intersection, float epsilonExtremity); 
bool getEdgeIntersection(float e1p0x, float e1p0y, float e1p1x, float e1p1y, 
        float e2p0x, float e2p0y, float e2p1x, float e2p1y, 
        Point* intersection, float epsilonExtremity);

void drawEdges(std::vector<Edge>*, sf::RenderWindow*);
