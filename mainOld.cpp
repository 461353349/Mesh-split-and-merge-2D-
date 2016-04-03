#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include "edge.h"
#include "point.h"
#include "shape.h"

float getMaxPointX(sf::VertexArray array);
float getMaxPointY(sf::VertexArray array);
float getMinPointX(sf::VertexArray array);
float getMinPointY(sf::VertexArray array);
void renderingThread(sf::RenderWindow* window);
Edge* setEdges(Point* listPoint);


void rendering(sf::RenderWindow* window, sf::VertexArray* array) {
	while (window->isOpen()) {
		window->clear();
		window->draw(*array);
		window->display();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main()
{

	

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	float height = 200, width = 200;
	sf::RenderWindow window(sf::VideoMode(height, width), "SFML works!");
	//sf::VertexArray triangle(sf::Triangles, 3);

	//Point P0, P1, P2, P3, P4, P5;
	//
	//P0.vertex.position = sf::Vector2f(100, 200); P0.velocity = sf::Vector2f(-1, 1);
	//P1.vertex.position = sf::Vector2f(50, 75); P1.velocity = sf::Vector2f(-1, -1);
	//P2.vertex.position = sf::Vector2f(60, 120); P2.velocity = sf::Vector2f(1, 1);
	//P3.vertex.position = sf::Vector2f(100, 200); P3.velocity = sf::Vector2f(-1, 1);
	//P4.vertex.position = sf::Vector2f(60, 30);
	//P5.vertex.position = sf::Vector2f(50, 75);


	//sf::VertexArray lines(sf::LinesStrip);
	//lines.append(P0.vertex);
	//lines.append(P1.vertex);
	//lines.append(P2.vertex);
	//lines.append(P3.vertex);

	///*lines[0] = P0.vertex;
	//lines[1] = P1.vertex;
	//lines[2] = P2.vertex;
	//lines[3] = P3.vertex;
	//lines[4] = P4.vertex;
	//lines[5] = P5.vertex;*/
	
	sf::Vector2f velocity = sf::Vector2f(1, 1);

	window.setActive(false);

	std::thread renderThread;
	renderThread = std::thread(&rendering, &window, &lines);
	//renderThread.join();

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Time elapsedTime = clock.getElapsedTime();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		//Droite/Gauche de fenetre
		if (getMaxPointX(lines) >= width || getMinPointX(lines) <= 0) {
			velocity.x *= -1;
		}
		//Bas/Haut de fenetre
		if (getMaxPointY(lines) >= height || getMinPointY(lines) <= 0) {
			velocity.y *= -1;
		}

		for (int i = 0; i < lines.getVertexCount(); i++)
		{
			lines[i].position = lines[i].position + velocity * float(elapsedTime.asMicroseconds()/1000.0);
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			
		}
		
		clock.restart();
	}	

	return 0;
}

Edge* setEdges(Point* listPoint) {

	return 0;
}

float getMaxPointX(sf::VertexArray array) {
	float maxInX = 0;
	for (int i= 0; i < array.getVertexCount(); i++)
	{
		maxInX = std::max(maxInX, array[i].position.x);
	}
	return maxInX;
}

float getMaxPointY(sf::VertexArray array) {
	float maxInY = 0;
	for (int i = 0; i < array.getVertexCount(); i++)
	{
		maxInY = std::max(maxInY, array[i].position.y);
	}
	return maxInY;
}

float getMinPointX(sf::VertexArray array) {
	float minInX = array[0].position.x;
	for (int i = 0; i < array.getVertexCount(); i++)
	{
		minInX = std::min(minInX, array[i].position.x);
	}
	return minInX;
}

float getMinPointY(sf::VertexArray array) {
	float minInY = array[0].position.y;
	for (int i = 0; i < array.getVertexCount(); i++)
	{
		minInY = std::min(minInY, array[i].position.y);
	}
	return minInY;
}
