#include "Game.hpp"

/*
	prioritized "show us what you can do" rather than pretty code
*/

/*
    the main.cpp just serves as a notebook for me from now on:

    ideas:

		* a game where you play as a blob and need to jump on skeletons to survive *
			- rotate world to move
			- kill skeletons by jumping on them
			- map editor?
			- add physics
			- add lighting to the bombs with candle
			- make skeleton push you instead of kill <-- this

    problems:
		- sometimes player gets stuck to corners

	suggestions:
		additions:
		- make an input handler in separate file to take handle user inputs and events
		- when player dies after dropping floors there is a weird flicker lag when the map reloads
		- make the skeleton sprites have rotations when they die

		changes:
		- make the jump for the player a class member function like it is done in Skeleton
 */

int main(int argc, char* argv[])
{   
	ResourceManager::init(argv[0]);

    Game game;
    game.run();
    return 0;
}
