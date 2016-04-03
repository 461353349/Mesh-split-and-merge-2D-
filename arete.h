#pragma once
#include <SFML/Graphics.hpp>
#include "point.h"
namespace sf {
	class Arete : public sf::Drawable, sf::Transformable {
	public:
		Vertex line[2];
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			states.transform *= getTransform();

			target.draw(m_vertices, states);
		}
		sf::VertexArray m_vertices;
	};
}