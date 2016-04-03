#include <csignal>
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <map>
#include <utility>
#include <cmath>
#include <queue>
#include <set>

#include <bitset>
#include <SFML/Graphics.hpp>

#include "grid.h"
#include "shape.h"
#include "point.h"

Grid::Grid (int width, int height, int resolution) {
    this->width = width;
    this->height = height;
    this->resolution = resolution;

    nbPointWidth = (width / resolution) + 1;
    nbPointHeight = (height / resolution) + 1;
    nbPointGrid = nbPointHeight*nbPointWidth;

    this->listGridPoint = std::vector<float>(nbPointGrid, 0.0);
}

void logGrid(Grid* grid) 
{
    int id = 0;
    int nbPointWidth = grid->width / grid->resolution + 1;
    int nbPointHeight = grid->height / grid->resolution + 1;
    for (int j = 0; j < nbPointHeight; j++) {
        for (int i = 0; i < nbPointWidth; i++) {
            std::cout.precision(2);
            std::cout << grid->listGridPoint[id] << "\t";
            id++;   
        }
        std::cout << std::endl;
    }

}

float polygonClosestPoint(Shape* shape, Point p) 
{
        float dist = std::numeric_limits<float>::infinity(); 
        int nbEdges = shape->listEdges.size();
        for (int i = 0; i < nbEdges; i++) {
            Point a = shape->listEdges[i].p0;
            Point b = shape->listEdges[i].p1;
            float distToAB = minimumDistance(&a, &b, &p);
            dist = distToAB < dist ? distToAB : dist;
        }

    return dist < 0.0001 ? 0.0f : dist;
}

// utilisation de l'algorithm PNPoly
// http://www.codeproject.com/Tips/84226/Is-a-Point-inside-a-Polygon
float isPointInsideShape(Shape* shape, Point p)
{
    bool c = false;

    int nbEdges = shape->listEdges.size();
    for (int i = 0; i < nbEdges; i++) {
        Point a = shape->listEdges[i].p0;
        Point b = shape->listEdges[i].p1;
        if (((a.y > p.y) != (b.y > p.y)) &&
                (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x))
            c = !c;
    }

   return c; 
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void signedDistanceFieldCalculation(Grid* grid, std::vector<Shape>* shapes)
{
    int id = 0;
    int nbPointWidth = grid->width / grid->resolution + 1;
    int nbPointHeight = grid->height / grid->resolution + 1;
    int nbShapes = shapes->size();
    for (int j = 0; j < nbPointHeight; j++) {
        for (int i = 0; i < nbPointWidth; i++) {
            Point p { float(i * grid->resolution), float(j * grid->resolution) };
            float value = std::numeric_limits<float>::infinity(); 
            for (int k = 0; k < nbShapes; k++) 
            {
                Shape* s = &shapes->at(k);
                // + ou -
                float signe = isPointInsideShape(s, p) ? -1.0f : 1.0f;
                // distance point/edge
                float distance = polygonClosestPoint(s, p); 
                float newValue = signe * distance;
                if (sgn(value) != sgn(newValue))
                    value = std::min(value, newValue);
                else
                    value = fabs(value) < fabs(newValue) ? value : newValue;
            }
            grid->listGridPoint[id] = value;
            id++;   
        }
    }
    //logGrid(grid);
}

void drawVectorEdges(std::vector<Edge>* e, sf::RenderWindow* window)
{
    // on fixe la seed pour obtenir quasi les meme couleurs a chaque draw
    srand(1);
    int n = 2 * e->size();
    sf::VertexArray lines(sf::Lines, n);
    int c = 0;
    for (const auto &edge : *e) {
        sf::Color col = sf::Color::White;
        col.g = rand() % 255;
        col.r = rand() % 255;
        col.b = rand() % 255;
        lines[c+0].position = sf::Vector2f(edge.p0.x, edge.p0.y);
        lines[c+0].color = col;
        lines[c+1].position = sf::Vector2f(edge.p1.x, edge.p1.y);
        lines[c+1].color = col;
        c += 2;
    }

    window->draw(lines);
}

void drawEdges(Grid* g, EdgesList* e, sf::RenderWindow* window)
{
    int n = 2 * e->size();
    sf::VertexArray lines(sf::Lines, n);
    int c = 0;
    for (const auto &pair : *e) {
        int idPoint = pair.first.first;
        int w = g->resolution * (idPoint % g->nbPointWidth);
        int h = g->resolution * (idPoint / g->nbPointHeight);
        lines[c+0].position = sf::Vector2f(w, h);
        if (pair.first.second == DIRECTION::right)
            lines[c+1].position = sf::Vector2f(w + g->resolution, h);
        else if (pair.first.second == DIRECTION::down)
            lines[c+1].position = sf::Vector2f(w, h + g->resolution);
        c += 2;
    }

    window->draw(lines);
}

void incrementKey(EdgesList* map, KeyEdge key)
{
    auto search = map->find(key);
    if (search != map->end())
    {
        (*map)[key]++;
    }
    else 
    {
        (*map)[key] = 1;
    }
}

void detectComplexEdges(Grid* g, std::vector<Shape>* shapes, std::vector<Edge>* complexEdges)
{
    // get edges with intersections
    EdgesList intersections;

    for (const Shape &shape : *shapes) {
        for (const Edge &edge : shape.listEdges)
        {
            // bounding box du l'arrete
            int minX = std::floor(std::min(edge.p0.x, edge.p1.x) / g->resolution);
            int minY = std::floor(std::min(edge.p0.y, edge.p1.y) / g->resolution);
            int maxX = std::floor(std::max(edge.p0.x, edge.p1.x) / g->resolution);
            int maxY = std::floor(std::max(edge.p0.y, edge.p1.y) / g->resolution);
            for (int i = minY; i <= maxY; i++)
            {
                for (int j = minX; j <= maxX; j++)
                {
                    Point p;
                    bool intersect = getEdgeIntersection(j * g->resolution, i * g->resolution,
                        j * g->resolution, (i + 1) * g->resolution, 
                        edge.p0.x, edge.p0.y, edge.p1.x, edge.p1.y, &p, 0.00);
                    if (intersect) 
                    {
                        int id = i * g->nbPointWidth + j;
                        KeyEdge e1(id, DIRECTION::down);
                        incrementKey(&intersections, e1);
                    }
                    intersect = getEdgeIntersection(j * g->resolution, i * g->resolution,
                        (j + 1) * g->resolution, i * g->resolution, 
                        edge.p0.x, edge.p0.y, edge.p1.x, edge.p1.y, &p, 0.00);
                    if (intersect) 
                    {
                        int id = i * g->nbPointWidth + j;
                        KeyEdge e1(id, DIRECTION::right);
                        incrementKey(&intersections, e1);
                    }
                }
            }
        }
    }
    // keep only complex edges
    for (const auto &pair : intersections) {
        if (pair.second < 2) // l'arrete est intersectee une seule fois
            continue;
        int idP1 = pair.first.first;
        Point p1 = Point {
            float((idP1 % g->nbPointWidth) * g->resolution),
            float((idP1 / g->nbPointWidth) * g->resolution)
        };
        int idP2 = idP1;
        if (pair.first.second == DIRECTION::right)
            idP2++;
        else 
            idP2 += g->nbPointWidth;
        Point p2 = Point {
            float((idP2 % g->nbPointWidth) * g->resolution),
            float((idP2 / g->nbPointWidth) * g->resolution)
        };
        Edge e = { p1, p2 };
        complexEdges->push_back(e);
    }
}

void detectComplexCells(Grid* g, std::vector<Edge>* edges, std::map<int, int>* complexCellsMap)
{
    int nbCellsWidth = g->nbPointWidth - 1;
    for (const Edge e : *edges)
    {
        int idCellX = std::floor(e.p0.x / g->resolution);
        int idCellY = std::floor(e.p0.y / g->resolution);
        if (e.p0.y == e.p1.y) // arrete horizontale
        {
            if (e.p0.y > 0.001) // l'arrete n'est pas sur le bord du haut 
            {
                int id = idCellX + (idCellY - 1) * nbCellsWidth;
                (*complexCellsMap)[id]= 1;
            }
            if (e.p0.y < g->height - 0.001) // l'arrete n'est pas sur le bord du bas 
            {
                int id = idCellX + idCellY * nbCellsWidth;
				(*complexCellsMap)[id] = 1;
            }
        } else if (e.p0.x == e.p1.x) // arrete verticale
        {
            if (e.p0.x > 0.001) // l'arrete n'est pas sur le bord du haut 
            {
                int id = idCellX - 1 + idCellY * nbCellsWidth;
				(*complexCellsMap)[id] = 1;
            }
            if (e.p0.x < g->width - 0.001) // l'arrete n'est pas sur le bord du bas 
            {
                int id = idCellX + idCellY * nbCellsWidth;
				(*complexCellsMap)[id] = 1;
            }
        }
    }
}

void drawCells(Grid* g, std::vector<int>* cellsId, sf::RenderWindow* window, sf::Color color)
{
    int nbCellsWidth = g->nbPointWidth - 1;
    sf::VertexArray v(sf::Quads, cellsId->size() * 4);
    int c = 0;
    for (const int idCell : *cellsId)
    {
        int idX = idCell % nbCellsWidth;    
        int idY = idCell / nbCellsWidth;    
        v[c + 0].position = sf::Vector2f(idX * g->resolution, idY * g->resolution); 
        v[c + 1].position = sf::Vector2f((idX + 1) * g->resolution, idY * g->resolution); 
        v[c + 2].position = sf::Vector2f((idX + 1) * g->resolution, (idY + 1) * g->resolution); 
        v[c + 3].position = sf::Vector2f(idX * g->resolution, (idY + 1) * g->resolution); 
		v[c + 0].color = color;
        v[c + 1].color = color;
        v[c + 2].color = color;
        v[c + 3].color = color;
        c += 4;
    }
    window->draw(v);
}

void detectDeepCells(Grid* g, std::map<int, int>* complexCellsMap, std::vector<int>* deepCells)
{
	int nbCellsWidth = g->nbPointWidth - 1;
	int nbCellsHeight = g->nbPointHeight - 1;
	
	for (const auto c : *complexCellsMap) {
		int id = c.first;
		int idX = id % nbCellsWidth;
		int idY = id / nbCellsWidth;
		// cellules a gauches
		bool isDeep = true;
		if (idX > 0)
			isDeep &= complexCellsMap->find(id - 1) != complexCellsMap->end() ? true : false;
		if (idX > 1)
			isDeep &= complexCellsMap->find(id - 2) != complexCellsMap->end() ? true : false;
		if (idX < nbCellsWidth - 1)
			isDeep &= complexCellsMap->find(id + 1) != complexCellsMap->end() ? true : false;
		if (idX < nbCellsWidth - 2)
			isDeep &= complexCellsMap->find(id + 2) != complexCellsMap->end() ? true : false;
		if (isDeep) {
			deepCells->push_back(id);
			continue;
		}
		isDeep = true;
		if (idY > 0)
			isDeep &= complexCellsMap->find(id - nbCellsWidth) != complexCellsMap->end() ? true : false;
		if (idY > 1)
			isDeep &= complexCellsMap->find(id - 2*nbCellsWidth) != complexCellsMap->end() ? true : false;
		if (idY < nbCellsHeight - 1)
			isDeep &= complexCellsMap->find(id + nbCellsWidth) != complexCellsMap->end() ? true : false;
		if (idY < nbCellsHeight - 2)
			isDeep &= complexCellsMap->find(id + 2*nbCellsWidth) != complexCellsMap->end() ? true : false;
		if (isDeep)
			deepCells->push_back(id);
	}
}

void mapToVect(std::map<int, int>* map, std::vector<int>* vector) 
{
	for (const auto i : *map){
		vector->push_back(i.first);
	}
}

void extendDeepCells(Grid* g, std::vector<Edge>* complexEdges, std::map<int, int>* complexCellsMap, std::vector<int>* deepCells) 
{
	int nbCellWidth = g->nbPointWidth - 1;
	int nbCellHeight = g->nbPointHeight - 1;
	
	std::queue<int> queueDeepCells;
	for (const int q : *deepCells) {
		queueDeepCells.push(q);
		//std::cout << queueDeepCells.front() << "\n";
	}

	while (!queueDeepCells.empty()) {
		int id = queueDeepCells.front();
		int idCellX = id % nbCellWidth;
		int idCellY = id / nbCellWidth;
		bool isComplex = true;
		bool isAlreadyDeep = false;
		//std::cout << "ID deep cell : " << id << "\n";
		if (idCellX > 0)
			isComplex &= complexCellsMap->find(id - 1) != complexCellsMap->end() ? true : false;
		if (isComplex) {
			for (const int i : *deepCells) {
				if (i == id - 1) {
					isAlreadyDeep = true;
					break;
				}
			}
			if (!isAlreadyDeep) {
				if (complexEdgeBetweenCells(g, complexEdges, id, "gauche")) {
					queueDeepCells.push(id - 1);
					deepCells->push_back(id - 1);
				}
				
			}
		}
		//////////////////////////////////////
		isAlreadyDeep = false;
		isComplex = true;
		if (idCellX < nbCellWidth - 1)
			isComplex &= complexCellsMap->find(id + 1) != complexCellsMap->end() ? true : false;
		if (isComplex) {
			for (const int i : *deepCells) {
				if (i == id + 1) {
					isAlreadyDeep = true;
					break;
				}
			}
			if (!isAlreadyDeep) {
				if (complexEdgeBetweenCells(g, complexEdges, id, "droite")) {
					queueDeepCells.push(id + 1);
					deepCells->push_back(id + 1);
				}
				
			}
		}
		///////////////////////////////////////
		isAlreadyDeep = false;
		isComplex = true;
		if (idCellY > 0)
			isComplex &= complexCellsMap->find(id - nbCellWidth) != complexCellsMap->end() ? true : false;
		if (isComplex) {
			for (const int i : *deepCells) {
				if (i == id - nbCellWidth) {
					isAlreadyDeep = true;
					break;
				}
			}
			if (!isAlreadyDeep) {
				if (complexEdgeBetweenCells(g, complexEdges, id, "haut")) {
					queueDeepCells.push(id - nbCellWidth);
					deepCells->push_back(id - nbCellWidth);
				}
			}
		}
		///////////////////////////////////////////
		isAlreadyDeep = false;
		isComplex = true;
		if (idCellY < nbCellHeight - 1)
			isComplex &= complexCellsMap->find(id + nbCellWidth) != complexCellsMap->end() ? true : false;
		if (isComplex) {
			//v�rifier que la cellule est complexe mais pas profonde
			for (const int i : *deepCells) {
				if (i == id + nbCellWidth) {
					isAlreadyDeep = true;
					break;
				}
			}
			if (!isAlreadyDeep) {
				if (complexEdgeBetweenCells(g, complexEdges, id, "bas")) {
					queueDeepCells.push(id + nbCellWidth);
					deepCells->push_back(id + nbCellWidth);
				}
			}
		}

		queueDeepCells.pop();
	}
    //for (auto c : *deepCells)
    //    std::cout << c << " ";
    //std::cout << "\n";
}

bool complexEdgeBetweenCells(Grid* g, std::vector<Edge>* e, int idFirstCell, std::string relativePosSecondCell) {
	int nbCellWidth = g->nbPointWidth - 1;
	int posX0, posY0, posX1, posY1;
	int idCellX = idFirstCell % nbCellWidth;
	int idCellY = idFirstCell / nbCellWidth;
	if (relativePosSecondCell == "haut") {
		posX0 = idCellX * g->resolution;
		posY0 = idCellY * g->resolution;
		posX1 = posX0 + g->resolution;
		posY1 = posY0;
	}
	else if (relativePosSecondCell == "droite") {
		posX0 = idCellX * g->resolution + g->resolution;
		posY0 = idCellY * g->resolution;
		posX1 = posX0;
		posY1 = posY0 + g->resolution;
	}
	else if (relativePosSecondCell == "bas") {
		posX0 = idCellX * g->resolution;
		posY0 = idCellY * g->resolution + g->resolution;
		posX1 = posX0 + g->resolution;
		posY1 = posY0;
	}
	else if (relativePosSecondCell == "gauche") {
		posX0 = idCellX * g->resolution;
		posY0 = idCellY * g->resolution;
		posX1 = posX0;
		posY1 = posY0 + g->resolution;
	}

	for (const auto edge : *e) {
		if (edge.p0.x == posX0 && edge.p0.y == posY0 && edge.p1.x == posX1 && edge.p1.y == posY1) {
			return true;
		}
		else {
			continue;
		}
	}
	return false;
}

int getIdCornerTopLeft(Grid *g, int cellId)
{
	int nbCellsWidth = g->nbPointWidth - 1;
    int idX = cellId % nbCellsWidth;
    int idY = cellId / nbCellsWidth;

    return idX + idY * g->nbPointWidth; 
}

Point getPointFromId(Grid*g, int idPoint)
{
    int idX = idPoint % g->nbPointWidth;
    int idY = idPoint / g->nbPointWidth;
    return Point { float(idX * g->resolution), float(idY * g->resolution) };
}

Point getTopLeftPointFromCellId(Grid*g, int cellId)
{
    int idX = cellId % (g->nbPointWidth - 1);
    int idY = cellId / (g->nbPointWidth - 1);
    return Point { float(idX * g->resolution), float(idY * g->resolution) };
}

bool edgeIsInComplexCell(Grid* g, Edge* e, int cellId)
{
    Point tlCellPoint = getTopLeftPointFromCellId(g, cellId);
    return (e->p0.x >= tlCellPoint.x && e->p0.x <= (tlCellPoint.x + g->resolution) &&  
        e->p1.x >= tlCellPoint.x && e->p1.x <= (tlCellPoint.x + g->resolution) &&  
        e->p0.y >= tlCellPoint.y && e->p0.y <= (tlCellPoint.y + g->resolution) &&  
        e->p1.y >= tlCellPoint.y && e->p1.y <= (tlCellPoint.y + g->resolution));
}

void intersectEdgesAgainstComplexCells(Grid *g, std::vector<int> *cellsId, std::vector<Shape>* shapes,
         std::vector<std::vector<Edge>> *newEdges, IntersectionPoints *intersectionPointsEdges)
{
    int cptShape = -1, cptEdge = -1;
    std::vector<std::vector<Edge>> newEdgesShapes;
    for (const Shape &shape : *shapes) 
    {
        cptShape++;

        newEdges->push_back(std::vector<Edge>());

        for (const Edge &edge : shape.listEdges)
        {
            cptEdge++;

            bool insideComplexeCell = false;
            std::vector<Point> intersectionPoints;
            for (const int cellId : *cellsId)
            {
                Point tlCellPoint = getTopLeftPointFromCellId(g, cellId);
                const Edge topEdge = Edge { tlCellPoint, tlCellPoint + Point { float(g->resolution), 0 } };
                const Edge rightEdge = Edge { tlCellPoint + Point { float(g->resolution), 0 }, 
                    tlCellPoint + Point { float(g->resolution), float(g->resolution) } };
                const Edge bottomEdge = Edge { tlCellPoint + Point { 0, float(g->resolution) }, 
                    tlCellPoint + Point { float(g->resolution), float(g->resolution) } };
                const Edge leftEdge = Edge { tlCellPoint, tlCellPoint + Point { 0, float(g->resolution) } };

                Point p;
                bool intersect = getEdgeIntersection(&topEdge, &edge, &p, 0.00);
                // les points d'intersection rajoutés a la map peuvent p-e foirer
                // car ils peuvent etre écrasés par d'autre arrete
                if (intersect)
                {
                    intersectionPoints.push_back(p);
                    intersectionPointsEdges->emplace(std::make_pair(cellId, 'T'), p);
                }
                intersect = getEdgeIntersection(&rightEdge, &edge, &p, 0.00);
                if (intersect)
                {
                    intersectionPoints.push_back(p);
                    intersectionPointsEdges->emplace(std::make_pair(cellId, 'R'), p);
                }
                intersect = getEdgeIntersection(&bottomEdge, &edge, &p, 0.00);
                if (intersect)
                {
                    intersectionPoints.push_back(p);
                    intersectionPointsEdges->emplace(std::make_pair(cellId, 'B'), p);
                }
                intersect = getEdgeIntersection(&leftEdge, &edge, &p, 0.00);
                if (intersect)
                {
                    intersectionPoints.push_back(p);
                    intersectionPointsEdges->emplace(std::make_pair(cellId, 'L'), p);
                }

                if (edge.p0.x >= tlCellPoint.x && edge.p0.x <= (tlCellPoint.x + g->resolution) &&  
                    edge.p1.x >= tlCellPoint.x && edge.p1.x <= (tlCellPoint.x + g->resolution) &&  
                    edge.p0.y >= tlCellPoint.y && edge.p0.y <= (tlCellPoint.y + g->resolution) &&  
                    edge.p1.y >= tlCellPoint.y && edge.p1.y <= (tlCellPoint.y + g->resolution))
                {
                    insideComplexeCell = true;
                }
            }

            
            if (intersectionPoints.size() == 0) 
            {
                // si l'arrete est a l'exterieure de la cell on l'ajoute
                if (!insideComplexeCell)
                    newEdges->at(cptShape).push_back(edge);
                continue;
            }

            // nous avons pour cette arrete les differents points ou celle ci est decoupee
            // il faut trier ceux-ci et enlever les doublons
            intersectionPoints.push_back(edge.p0);
            intersectionPoints.push_back(edge.p1);
            if (edge.p0.x != edge.p1.x)
            {
                if (edge.p0.x < edge.p1.x) 
                {
                    std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](Point a, Point b)
                        {
                            return a.x < b.x; 
                        });
                }
                else 
                {
                    std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](Point a, Point b)
                        {
                            return a.x > b.x; 
                        });
                }
            } 
            else if (edge.p0.y != edge.p1.y)
            {
                if (edge.p0.y < edge.p1.y) 
                {
                    std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](Point a, Point b)
                        {
                            return a.y < b.y; 
                        });
                }
                else 
                {
                    std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](Point a, Point b)
                        {
                            return a.y > b.y; 
                        });
                }
            }
            else 
            {
                std::cout << "no sort";
            }
           
            // suppression des doublons
            intersectionPoints.erase(std::unique(intersectionPoints.begin(), 
                        intersectionPoints.end()), intersectionPoints.end());

            int nbNewEdges = intersectionPoints.size() - 1;
            for (int i = 0; i < nbNewEdges; i++)
            {
                newEdges->at(cptShape).push_back({intersectionPoints[i], intersectionPoints[i+1]});
            }
        }
        // drawVectorEdges(&newEdges->at(cptShape), window);
    }
}

void marchingSquares(Grid *g, std::vector<int> *cells, std::vector<Shape>* shapes, sf::RenderWindow *window)
{
    std::set<int> deepCells(std::make_move_iterator(cells->begin()),
            std::make_move_iterator(cells->end()));
    for (const int idCell : *cells)
    {
        int idTopLeftCorner = getIdCornerTopLeft(g, idCell);
        // on stock les infos de la cellule dans 4 bit pour le marching square
        // tl | tr | br | bl
        // ou 1 correspond a l'appartenance du coins de la cell a une forme
        int cellCode = (g->listGridPoint[idTopLeftCorner] <= 0) << 3 |
                (g->listGridPoint[idTopLeftCorner + 1] <= 0) << 2 |
                (g->listGridPoint[idTopLeftCorner + 1 + g->nbPointWidth] <= 0) << 1 |
                (g->listGridPoint[idTopLeftCorner + g->nbPointWidth] <= 0);

        if (cellCode == 0)
            deepCells.insert(idCell);
    }
    std::vector<int> deepCellsVect(deepCells.begin(), deepCells.end());

    std::vector<std::vector<Edge>> newEdges;
    IntersectionPoints intersectionPoints;
    intersectEdgesAgainstComplexCells(g, &deepCellsVect, shapes, &newEdges, &intersectionPoints);

    std::vector<Edge> finalEdges;

    for (const int idCell : deepCellsVect)
    {
        int idTopLeftCorner = getIdCornerTopLeft(g, idCell);
        // on stock les infos de la cellule dans 4 bit pour le marching square
        // tl | tr | br | bl
        // ou 1 correspond a l'appartenance du coins de la cell a une forme
        int cellCode = (g->listGridPoint[idTopLeftCorner] <= 0) << 3 |
                (g->listGridPoint[idTopLeftCorner + 1] <= 0) << 2 |
                (g->listGridPoint[idTopLeftCorner + 1 + g->nbPointWidth] <= 0) << 1 |
                (g->listGridPoint[idTopLeftCorner + g->nbPointWidth] <= 0);

        // chaque segment est defini par 2 char parmis
        // T: top | R: right | B: bottom | L: left
        // selon la config il peut y avoir 0, 2 ou 4 segments
        std::vector<char> vert;
        switch (cellCode)
        {
            case 0:
            case 15:
                break;
            case 1:
            case 14:
                vert.push_back('L');
                vert.push_back('B');
                break;
            case 2:
            case 13:
                vert.push_back('R');
                vert.push_back('B');
                break;
            case 3:
            case 12:
                vert.push_back('L');
                vert.push_back('R');
                break;
            case 4:
            case 11:
                vert.push_back('T');
                vert.push_back('R');
                break;
            case 5:
                vert.push_back('T');
                vert.push_back('L');
                vert.push_back('B');
                vert.push_back('R');
                break;
            case 6:
            case 9:
                vert.push_back('T');
                vert.push_back('B');
                break;
            case 7:
            case 8:
                vert.push_back('T');
                vert.push_back('L');
                break;
            case 10:
                vert.push_back('T');
                vert.push_back('R');
                vert.push_back('L');
                vert.push_back('B');
                break;
        }

        std::vector<Edge> newMarchedEdges;
        int nbNewPoints = vert.size();
        for (int i = 0 ; i < nbNewPoints; i += 2)
        {
            finalEdges.push_back(
                    Edge {
                        intersectionPoints[std::make_pair(idCell, vert[i])],
                        intersectionPoints[std::make_pair(idCell, vert[i+1])],
                        }
                        ); 
        }
    }

    // on recupere toues les arretes non presentes dans les cell complexes
    int nbShapes = shapes->size();
    for (int i = 0; i < nbShapes; i++) 
    {
        int nbEdges = newEdges[i].size();
        for (int j = 0; j < nbEdges; j++)
        {
            bool inComplexCell = false;
            for (const int cellId : *cells)
            {
                if (edgeIsInComplexCell(g, &newEdges[i][j], cellId))
                {
                    inComplexCell = true;
                    break;
                }
            }
            if (!inComplexCell)
                finalEdges.push_back(newEdges[i][j]); 
        }
    }
    drawVectorEdges(&finalEdges, window);
}
