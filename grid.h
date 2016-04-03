#pragma once
#include <vector>
#include <utility>
#include <map>
#include <string>

#include "edge.h"
#include "point.h"

struct Shape;
enum class DIRECTION { right = 0, down };
typedef std::pair<int, DIRECTION> KeyEdge;
typedef std::map<KeyEdge, int> EdgesList;

// clee compose de l'id de la cell et de l'arete (T/R/B/L)
typedef std::pair<int, char> KeyIntersection;
typedef std::map<KeyIntersection, Point> IntersectionPoints;

namespace sf
{
class RenderWindow;
}

struct Grid {
	int width;
	int height;
	int resolution;
	int nbPointWidth;
	int nbPointHeight;
	int nbPointGrid;
	std::vector<float> listGridPoint;

	Grid(int width, int height, int resolution);
};


void signedDistanceFieldCalculation(Grid*, std::vector<Shape>*);

void detectComplexEdges(Grid*, std::vector<Shape>*, std::vector<Edge>*);

void detectComplexCells(Grid*, std::vector<Edge>*, std::map<int, int>*);

void drawCells(Grid*, std::vector<int>*, sf::RenderWindow*, sf::Color);

void detectDeepCells(Grid*, std::map<int, int>*, std::vector<int>*);

void mapToVect(std::map<int, int>* Map, std::vector<int>* Vector);

void extendDeepCells(Grid*, std::vector<Edge>*, std::map<int, int>*, std::vector<int>*);

bool complexEdgeBetweenCells(Grid*, std::vector<Edge>*, int, std::string);

void marchingSquares(Grid*, std::vector<int>*, std::vector<Shape>*, sf::RenderWindow*);
