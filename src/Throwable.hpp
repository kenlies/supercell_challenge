#pragma once

#include "Game.hpp"

class Game;
class Player;

class Throwable : public sf::Drawable, public sf::Transformable {
	public:
		Throwable(Game *game, Player *player, sf::RenderWindow *window);
		~Throwable();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- getters ----
		b2Body*				getBody() const;
		sf::RectangleShape*	getShape() const;
		sf::Clock			&getLifeTimeClock();

	private:
		sf::Clock			_lifeTimeClock;
		Game*				_game;
		Player*				_player;
		b2Body* 			_body;
		sf::RectangleShape*	_shape;
};
