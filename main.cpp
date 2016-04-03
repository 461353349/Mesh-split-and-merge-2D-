#include <csignal>
#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include "shape.h"
#include "grid.h"
#include "cell.h"

int main()
{
    srand(time(NULL));
	int resolution = 7;
	int height = 200, width = 200;
	sf::RenderWindow window(sf::VideoMode(height, width), "Mesh Split and Merge");

	Grid grid = Grid(width, height, resolution);
    std::vector<Shape> shapes;
	shapes.push_back(Shape("cercle20.txt"));
	shapes.push_back(Shape("cercle20.txt"));
	shapes.push_back(Shape("cercle20.txt"));

    //float offset = 42.0f;
    //shapes[0].centerPosition.y += offset;
    //for (int i = 0; i < shapes[0].listEdges.size(); i++) {
    //    shapes[0].listEdges[i].p0.y += offset;
    //    shapes[0].listEdges[i].p1.y += offset;
    //}
    //offset = 84.0f;
    //shapes[1].centerPosition.y += offset;
    //for (int i = 0; i < shapes[1].listEdges.size(); i++) {
    //    shapes[1].listEdges[i].p0.y += offset;
    //    shapes[1].listEdges[i].p1.y += offset;
    //}
	//shapes.push_back(Shape("quart1.txt"));
	//shapes.push_back(Shape("quart2.txt"));
	//shapes.push_back(Shape("quart3.txt"));
	//shapes.push_back(Shape("quart4.txt"));
	//shapes.push_back(Shape("half2.txt"));


	//std::cout << "nb points width:" << grid.nbPointWidth << " and nb points height:" << grid.nbPointHeight << std::endl;
	sf::Clock clock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
        std::vector<Edge> complexEdges;
		std::map<int, int> complexCellsMap;
		std::vector<int> deepCells;
		std::vector<int> complexCells;

        float elapsedTime = clock.restart().asSeconds();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            updateShapes(&grid, &shapes, elapsedTime * 5.0f);
			collisonDetection(&shapes);
            signedDistanceFieldCalculation(&grid, &shapes);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
        {
            sf::VertexArray g(sf::Points, grid.listGridPoint.size());

            int nbPoints = grid.listGridPoint.size();
            for (int i = 0; i < nbPoints; i++) {
                float value = grid.listGridPoint[i];
                if (value < 0.0f)
                    g[i].color = sf::Color::Red;
                else if (value >= -0.001f && value <= 0.001f)
                    g[i].color = sf::Color::Yellow;
                else
                    g[i].color = sf::Color::Green;

                g[i].position = sf::Vector2f((i%grid.nbPointWidth)*resolution, (i/grid.nbPointWidth)*resolution);
                //std::cout << g[i].position.x << "," << g[i].position.y << std::endl;
            }
            window.draw(g);
        }

        detectComplexEdges(&grid, &shapes, &complexEdges);
        detectComplexCells(&grid, &complexEdges, &complexCellsMap);
		detectDeepCells(&grid, &complexCellsMap, &deepCells);
		mapToVect(&complexCellsMap, &complexCells);
        //drawCells(&grid, &complexCells, &window, sf::Color::Yellow) ;
		extendDeepCells(&grid, &complexEdges, &complexCellsMap, &deepCells);
		//drawCells(&grid, &deepCells, &window, sf::Color::Red);
		//drawEdges(&complexEdges, &window);
		//drawShapes(&shapes, &window);
        marchingSquares(&grid, &complexCells, &shapes, &window);
        //marchingSquares(&grid, &deepCells, &shapes, &window);

		window.display();
	}
};
