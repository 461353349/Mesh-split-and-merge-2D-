#include <iostream>
#include <csignal>
#include "shape.h"

Shape::Shape(std::string shapeFile) {
    std::fstream file(shapeFile);
    float x, y;
    while (file >> x >> y) {
        listPoints.push_back({x, y});
    }
    int nbEdges = listPoints.size() - 1;
    for (int i = 0; i < nbEdges; i++) {
        listEdges.push_back({ listPoints[i], listPoints[i + 1] });
    }
    listEdges.push_back({ listPoints.back(), listPoints.front() });

    // TODO: a calculer dynamiquement
    radius = 20.0f;
    mass = 1.0f;
    centerPosition.x = 100.0f;
    centerPosition.y = 100.0f;

    // randopm velocity
    int maxSpeed = 10;
    velocity.x = (rand() % maxSpeed * 2000) / 1000.0f - maxSpeed / 2.0f;
    velocity.y = (rand() % maxSpeed * 2000) / 1000.0f - maxSpeed / 2.0f;
    std::cout << "velocity" << velocity.x << " : " << velocity.y;
}

void drawShapes(std::vector<Shape>* shapes, sf::RenderWindow* window) {
    for (const Shape &s : *shapes) 
    {
        sf::VertexArray v(sf::LinesStrip, s.listPoints.size() + 1);
		for (int i = 0; i < s.listEdges.size(); i++)
		{
			v[i].position = sf::Vector2f(s.listEdges[i].p0.x, s.listEdges[i].p0.y);
		}
		v[s.listEdges.size()].position = sf::Vector2f(s.listEdges[0].p0.x, s.listEdges[0].p0.y);

        /*for (int i = 0; i < s.listPoints.size(); i++) {
            v[i].position = sf::Vector2f(s.listPoints[i].x, s.listPoints[i].y);
        }
        v[s.listPoints.size()].position = sf::Vector2f(s.listPoints[0].x, s.listPoints[0].y);*/
        window->draw(v);
    }
}

void updateShapes(Grid* g, std::vector<Shape>* shapes, float elapsedTime) {
	//sf::Vector2f velocity = sf::Vector2f(10, 10);
    float scale = 0.00001f;
	for (Shape &s : *shapes)
	{
        std::vector<sf::Vector2f> forces;
        
        // gravity
        //forces.push_back(sf::Vector2f(0.0f, 9.78f * scale));

        // somme des forces
        sf::Vector2f f;
        for (auto force : forces)
            f += force;

        // test collisions avec la fenetre
		if (getMaxPointX(&s.listEdges) >= g->width && s.velocity.x > 0.0f)
			s.velocity.x *= -1;
			
		if (getMinPointX(&s.listEdges) <= 0 && s.velocity.x < 0.0f)
			s.velocity.x *= -1;
		
		if (getMaxPointY(&s.listEdges) >= g->height && s.velocity.y > 0.0f)
        {
			s.velocity.y *= -1;
        }

		if (getMinPointY(&s.listEdges) <= 0 && s.velocity.y < 0.0f)
			s.velocity.y *= -1;

        // calculs et application des forces
        // on considere dans un premier temps que toutes les arretes ont la même vitesse
        s.velocity += elapsedTime * f / s.mass;
        sf::Vector2f tmp = (f / s.mass) / (2.0f * elapsedTime * elapsedTime);
        sf::Vector2f x = s.velocity * elapsedTime + tmp; 
        //std::cout << s.velocity.x << " " << s.velocity.y << std::endl;
        //std::cout << tmp.x << " " << tmp.y << std::endl;
        //std::cout << x.x << " " << x.y << std::endl;
        s.centerPosition.x += x.x;
        s.centerPosition.y += x.y;
		for (int i = 0; i < s.listEdges.size(); i++) {
            /*s.listEdges[i].p0.x += x.x;
            s.listEdges[i].p0.y += x.y;
            s.listEdges[i].p1.x += x.x;
            s.listEdges[i].p1.y += x.y;*/
			s.listEdges[i].p0.x += s.velocity.x *float(elapsedTime);
			s.listEdges[i].p0.y += s.velocity.y *float(elapsedTime);
			s.listEdges[i].p1.x += s.velocity.x *float(elapsedTime);
			s.listEdges[i].p1.y += s.velocity.y *float(elapsedTime);
		}
	}    
}

void collisonDetection(std::vector<Shape>* shapes) {
	for (int i = 0; i < shapes->size(); i++)
	{
		Shape* si = &shapes->at(i);
		for (int j = 0; j < shapes->size(); j++) {
			Shape* sj = &shapes->at(j);

			if (i == j)
				continue;
			// Si collision
			if (dist(&si->centerPosition, &sj->centerPosition) < (si->radius + sj->radius)) {
				float newVelx1 = (si->velocity.x * (si->mass - sj->mass) + (2 * sj->mass * sj->velocity.x)) / (si->mass + sj->mass);
				float newVely1 = (si->velocity.y * (si->mass - sj->mass) + (2 * sj->mass * sj->velocity.y)) / (si->mass + sj->mass);
				float newVelx2 = (sj->velocity.x * (sj->mass - si->mass) + (2 * si->mass * si->velocity.x)) / (si->mass + sj->mass);
				float newVely2 = (sj->velocity.y * (sj->mass - si->mass) + (2 * si->mass * si->velocity.y)) / (si->mass + sj->mass);
			
				si->velocity = sf::Vector2f(newVelx1, newVely1);
				sj->velocity = sf::Vector2f(newVelx2, newVely2);

				si->centerPosition.x += newVelx1;
				si->centerPosition.y += newVely1;
				for (int i = 0; i < si->listEdges.size(); i++) {
					si->listEdges[i].p0.x += newVelx1;
					si->listEdges[i].p0.y += newVely1;
					si->listEdges[i].p1.x += newVelx1;
					si->listEdges[i].p1.y += newVely1;
				}
				sj->centerPosition.x += newVelx2;
				sj->centerPosition.y += newVely2;
				for (int i = 0; i < sj->listEdges.size(); i++) {
					sj->listEdges[i].p0.x += newVelx2;
					sj->listEdges[i].p0.y += newVely2;
					sj->listEdges[i].p1.x += newVelx2;
					sj->listEdges[i].p1.y += newVely2;
				}
			}
		}
	}
}

float getMaxPointX(std::vector<Edge>* edges) {
	float maxInX = 0;
	for (const Edge e : *edges)
	{
		maxInX = std::max(maxInX, e.p0.x);
	}
	return maxInX;
}

float getMaxPointY(std::vector<Edge>* edges) {
	float maxInY = 0;
	for (const Edge e : *edges)
	{
		maxInY = std::max(maxInY, e.p0.y);
	}
	return maxInY;
}

float getMinPointX(std::vector<Edge>* edges) {
	float minInX = std::numeric_limits<float>::infinity();;
	for (const Edge e : *edges)
	{
		minInX = std::min(minInX, e.p0.x);
	}
	return minInX;
}

float getMinPointY(std::vector<Edge>* edges) {
	float minInY = std::numeric_limits<float>::infinity();;
	for (const Edge e : *edges)
	{
		minInY = std::min(minInY, e.p0.y);
	}
	return minInY;
}
