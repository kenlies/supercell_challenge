#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include "Game.hpp"

class Game;

class Map {
	public:
		Map(Game *game);
		~Map();
		
		void saveMap(const std::string &path);
		void loadMap(const std::string &path);
		
	private:
		Game *_game;
};
