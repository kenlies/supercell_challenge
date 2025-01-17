#include "Map.hpp"

Map::Map(Game *game) : _game(game){

}

Map::~Map() {

}

// load the map that was saved
void Map::loadMap(const std::string &path) {
    std::ifstream map_file(path); // read a file line by line using the ifstream class
    std::string line; // a individual line from the file is read to this string

	_game->getView().setRotation(0); // set the rotation to default for map loading
    // set the view to the default position for map loading
	_game->getView().setCenter(sf::Vector2f(_game->getWindowSize().x / 2, _game->getWindowSize().y / 2 ));

    // destroy all the current boxes
	std::vector<Box *> &boxes = _game->getBoxes(); // get all boxes
	for (Box* box : boxes) {
		_game->getWorld().DestroyBody(box->getBody()); // destory the physical boxes (box2d)
		delete box; // free box
	}
	boxes.clear(); // remove all references to the now deleted objects

    // skip the first 2 rows of the map file
	std::getline(map_file, line);
	std::getline(map_file, line);
    // read individual lines from the map file
    while (std::getline(map_file, line)) {
        // stream the string into different variables delimited by white space
        std::istringstream ss(line);
        float x, y;

        ss >> x >> y; // first value into x, second to y
        b2Vec2 position(x / SCALE, y / SCALE); // convert back
        _game->createBox(sf::Vector2i(x, y), sf::Color(50, 50, 50)); // create a box at this position
    }
}

// save mao
void Map::saveMap(const std::string &path) {
    std::ofstream map_file(path); // write into or create files

	map_file << "x y\n\n"; // insert "x y" and "\n" as the first two lines
    // go through every box and save the position into the file
    for (auto box : _game->getBoxes()) {
        b2Vec2 pos = box->getBody()->GetPosition(); // get position
        map_file << pos.x * SCALE 
                << " " 
                << pos.y * SCALE
				<< " "
                << "\n";
    }
}
