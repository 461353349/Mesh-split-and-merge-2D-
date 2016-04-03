#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "point.h"
#include "edge.h"
#include "grid.h"
#include <iostream>
#include <fstream>
#include <string>

struct Shape {
	std::vector<Point> listPoints;
	std::vector<Edge> listEdges;
	Shape(std::string shapeFile);
	sf::Vector2f velocity;
	Point centerPosition;
    float mass;
    float radius;
};


void drawShapes(std::vector<Shape>* s, sf::RenderWindow* window);

void updateShapes(Grid*, std::vector<Shape>*, float);

void collisonDetection(std::vector<Shape>*);

float getMaxPointX(std::vector<Edge>*);

float getMinPointX(std::vector<Edge>*);

float getMaxPointY(std::vector<Edge>*);

float getMinPointY(std::vector<Edge>*);

