#pragma once

#include "Game.hpp"

class Game;

class Skeleton : public sf::Drawable, public sf::Transformable {
	public:
		Skeleton(Game *game, sf::Vector2i &spawnPos);
		~Skeleton();

		void updatePosition(sf::Vector2f playerPosition, float deltaTime);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		void isJump(float dt, float radians);

		// ---- getters ----
		b2Body*				getBody() const;
		sf::CircleShape*	getShape() const;
		

	private:
		Game*				_game;
		b2Body* 			_body;
		sf::CircleShape*	_shape;
		float 				_jumpInterval;
		float 				_jumpTimer = 0.0f;
};
