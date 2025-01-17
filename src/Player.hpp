#pragma once

#include "Game.hpp"

class Game;

class Player : public sf::Drawable, public sf::Transformable {
	public:
		Player(Game *game);
		~Player();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- getters ----
		b2Body*				getBody() const;
		sf::CircleShape*	getShape() const;
		

	private:
		Game*				_game;
		b2Body* 			_body;
		sf::CircleShape*	_shape;
};
