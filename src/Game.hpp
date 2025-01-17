#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include "external/include/box2d/box2d.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include "ResourceManager.h"
#include "Constants.h"
#include "Player.hpp"
#include "Box.hpp"
#include "Map.hpp"
#include "Throwable.hpp"
#include "Skeleton.hpp"

class Player;
class Box;
class Map;
class Throwable;
class Skeleton;

class Game : public b2ContactListener {
	public:
		Game();
		~Game();

		void run();
		void createBox(const sf::Vector2i &mousePos, const sf::Color &color);
		void removeBox(const sf::Vector2i &mousePos);

		// ---- getters ----
		b2World&			getWorld();
		sf::Vector2u&		getWindowSize();
		sf::View&			getView();
		std::vector<Box*>&	getBoxes();

		// ---- listeners ----
		void BeginContact(b2Contact* contact) override; // override the base class functions
		void EndContact(b2Contact* contact) override;	// override the base class functions

	private:
		// --- play ----
		void 	updatePlay();
		void 	explode();
		void	spawnSkeletons();
		void	restart();

		// --- editor ----
		void	updateEditor();
		void	draw_grid();
		void 	draw_box_at_cursor(const sf::Vector2i &mousePos);

		// --- texts ----
		sf::Font			_editorFont;
		sf::Text			_editorText;
		sf::Text			_timerText;
		sf::Text			_winLoseText;

		enum gameMode {
			Play,
			Editor
		};

		gameMode	_mode;
		sf::Time	_deltaTime;
		sf::Clock	_gameTimer;
		sf::Clock	_winClock;
		bool		_win = false;
		bool		_lose = false;

		// ---- window things ----
		sf::RenderWindow	_window;
		sf::Vector2u		_windowSize;
		sf::View			_view;

		// ---- physics things ----
		b2World	_world;
		b2Body* _toBeExplodedByJump = nullptr;
		b2Body* _toBeExplodedByLance = nullptr;
		bool 	_destroyThrowable = false;

		// ---- player stuff ----
		Player*		_player;
		b2Vec2		_spawnPos;
		sf::Clock 	_jumpCoolDownClock;
		sf::Clock	_throwableCoolDownClock;
		sf::Clock	_jumpDelayClock;
		bool 		_jump = false;
		bool		_jumpTrigger = false;	// need this to implement ground break below
		int 		_canJump = 0; 			// how many bodies touch the ball at a time: able to jump if > 0
		Throwable*	_throwable = nullptr;
		bool		_throw = false;

		// ---- skeletons stuff ----
		std::vector<Skeleton *> _skeletons;
		sf::Clock				_skeliSpawnRateClock;

		// ---- map things ----
		std::vector<Box*>	_boxes;
		Map*				_map;

		// ---- rotate the world ----
		bool	_rotateRight = false;
		bool	_rotateLeft = false;
		float	_rotVel = 0.f;

		// ---- move level editor view ----
		bool	_moveViewLeft = false;
		bool	_moveViewRight = false;
		bool	_moveViewUp = false;
		bool	_moveViewDown = false;

		// ---- music and sounds ----
		sf::Music*	_skeliMusic;
		sf::Music*	_skeliDeath;
		sf::Music*	_lanceThrow;

		// ---- textures and sprites ----
		sf::Sprite*		_hiverSprite;
		sf::Texture*	_hiverTexture;

		sf::Sprite*		_skeliSprite;
		sf::Texture*	_skeliTexture;
};