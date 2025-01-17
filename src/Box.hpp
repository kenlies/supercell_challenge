#pragma once

#include "Game.hpp"

class Game;

class Box : public sf::Drawable, public sf::Transformable {
	public:
		Box(Game *game, b2Vec2 &checkPos, const sf::Color &color);
		~Box();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- getters ----
		b2Body*				getBody() const;
		sf::RectangleShape*	getShape() const;
		
	private:
		Game*				_game;
		b2Body* 			_body;
		sf::RectangleShape*	_shape;
};
