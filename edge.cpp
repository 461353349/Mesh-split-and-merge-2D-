#include <SFML/Graphics.hpp>
#include <iostream>

#include "edge.h"
#include "point.h"

// vereifie si deux arretes s'intersectent et remplie la variable intersection
// si c'est le cas. base sur :
// http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect#565282
bool getEdgeIntersection(float e1p0x, float e1p0y, float e1p1x, float e1p1y, 
        float e2p0x, float e2p0y, float e2p1x, float e2p1y, 
        Point* intersection, float epsilonExtremity)
{
    float epsilon = 0.0001;
    float s1_x, s1_y, s2_x, s2_y; // vecteurs correspondants aux arretes 
    s1_x = e1p1x - e1p0x;
    s1_y = e1p1y - e1p0y;
    s2_x = e2p1x - e2p0x;
    s2_y = e2p1y - e2p0y;

    float s, t, det;
    det = - s2_x * s1_y + s1_x * s2_y;
    if (det < epsilon && det > epsilon) return 0;

    s = (-s1_y * (e1p0x - e2p0x) + s1_x * (e1p0y - e2p0y)) / det;
    t = ( s2_x * (e1p0y - e2p0y) - s2_y * (e1p0x - e2p0x)) / det;

    if (s >= epsilonExtremity && s <= (1.0 - epsilonExtremity) && t >= epsilonExtremity && t <= (1.0 - epsilonExtremity)) {
        // collision
        intersection->x = e1p0x + t * s1_x;
        intersection->y = e1p0y + t * s1_y;
        return 1;
    }
    return 0;
}

bool getEdgeIntersection(const Edge* e1, const Edge* e2, Point* intersection, float epsilonExtremity) {
    return getEdgeIntersection(e1->p0.x, e1->p0.y, e1->p1.x, e1->p1.y, 
        e2->p0.x, e2->p0.y, e2->p1.x, e2->p1.y, intersection, epsilonExtremity); 
}

void drawEdges(std::vector<Edge>* edges, sf::RenderWindow* window)
{
    int nbLines = edges->size();
    sf::VertexArray v(sf::Lines, nbLines * 2);
    for (int i = 0; i < nbLines; i++) {
        v[i * 2].position = sf::Vector2f(edges->at(i).p0.x, edges->at(i).p0.y);
        v[(i * 2) + 1].position = sf::Vector2f(edges->at(i).p1.x, edges->at(i).p1.y);
    }
    window->draw(v);
}
