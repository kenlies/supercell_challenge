#include "Game.hpp"

Game::Game() : 
    // he doesn't walk you know (can still jump though)
    // conext setting is set to antialaising level 8, which softens the edges
    _window({ 1024, 768 }, "supercell_challenge", sf::Style::Close, sf::ContextSettings(0, 0, 8)),
    // set the window size
    _windowSize(_window.getSize()),
    // set the spawn position to be in the middle and almost on top
    // SCALE explained here: https://seanballais.com/blog/box2d-and-the-pixel-per-meter-ratio/
    // scale down when calculating physics and when drawn scale up
    _spawnPos(b2Vec2((_windowSize.x / 2) / SCALE, (_windowSize.y / 16) / SCALE)),
    // set the view to be in the middle, also I set the size to be effectively 0 so the view can't see anything,
    // which may cause renderig issues, I should have set it to sf::Vector2f(_windowSize.x, _windowSize.y)
	_view(sf::Vector2f(_windowSize.x / 2, _windowSize.y / 2), sf::Vector2f()),
    // create the box2d world and set the gravity in y direction to GRAVITY_MAGNITUDE
	_world(b2Vec2(0.f, GRAVITY_MAGNITUDE)),
    // set mode to to the enum Play
    // enums are a way to define integral constants and assign meaningful names to them
	_mode(Play)
{
	// ---- set window & view things ----
    _window.setFramerateLimit(60); // 60 is a good framerate limit for casual games
    _view.setSize(sf::Vector2f(_windowSize.x, _windowSize.y)); // yeah I set the view size here actually lol

    // ---- load fonts and setup texts ----
    if (!_editorFont.loadFromFile(ResourceManager::getFilePath("BebasNeue-Regular.ttf"))) {
        exit(EXIT_FAILURE); // EXIT_FAILURE is a predefined macto usually 1
        // a predefined macro is defined by the compiler and cannot be redefined by the user
    }
    // try to use powers of 2 for performance (though most compilers are quick enough these days)
    _editorText.setPosition({5, static_cast<float>(_windowSize.y / 64)}); // set position to top left
    _editorText.setString("Editor");
    _editorText.setFillColor(sf::Color::Red);
    _editorText.setFont(_editorFont);
    // get the height of the text by getLocalBounds()
    sf::FloatRect textBounds = _editorText.getLocalBounds();
    // set the origin in the y axis to be in the middle, leave the x to the left
    _editorText.setOrigin(0, textBounds.height / 2.f);

    // set the timer poisiton
    _timerText.setPosition({static_cast<float>(_windowSize.x - 50), static_cast<float>(_windowSize.y / 64)}); // set positiom to top right
    _timerText.setString("60"); // set it to initially be 60
    _timerText.setFillColor(sf::Color::Yellow);
    _timerText.setFont(_editorFont);
    textBounds = _timerText.getLocalBounds(); // get localbounds
    _timerText.setOrigin(0, textBounds.height / 2.f); // set origin 

    // initialize some text properties like before
    _winLoseText.setFillColor(sf::Color::Yellow);
    _winLoseText.setFont(_editorFont);
    _winLoseText.setOutlineThickness(2);
    textBounds = _winLoseText.getLocalBounds(); // left this in here, whoops
    _winLoseText.scale(sf::Vector2f(1.5, 1.5)); // make it a bit bigger

    // ---- load textures and sprites ----
    // using dynamic memory because it is more intuitive and I wanted to show
    // that I can do it lol
    // also because the lifespan needs to extend beyond the scope in which they are created??
    // might be better to do it on the stack if possible or use smart pointers
    _hiverTexture = new sf::Texture; // allocating a new texture
    _hiverSprite = new sf::Sprite; // allocating a new sprite
    if (!_hiverTexture->loadFromFile(ResourceManager::getFilePath("player.png"))) {
        exit(EXIT_FAILURE);
    }
    _hiverSprite->setTexture(*_hiverTexture); // takes a reference so dereferencing the pointer is necessary
    _hiverSprite->setOrigin(sf::Vector2f(8, 12)); // set origin somewhat in the middle (tested)
    _hiverSprite->setScale({1.5, 1.5}); // scale a bit bigger

    _skeliTexture = new sf::Texture;
    _skeliSprite = new sf::Sprite;
    if (!_skeliTexture->loadFromFile(ResourceManager::getFilePath("vampire.png"))) {
        exit(EXIT_FAILURE);
    }
    _skeliSprite->setTexture(*_skeliTexture);
    _skeliSprite->setOrigin(sf::Vector2f(8, 12));
    _skeliSprite->setScale({1.5, 1.5});


    // ---- load and play music
    _skeliMusic = new sf::Music;
    if (!_skeliMusic->openFromFile(ResourceManager::getFilePath("skeli-song.ogg"))) {
        exit(EXIT_FAILURE);
    }
    _skeliMusic->setVolume(10); // set volume a little higher
    _skeliMusic->setLoop(true); // looping song
    _skeliMusic->play(); // play it

    // ---- load skeli death sound ----
    _skeliDeath = new sf::Music;
    if (!_skeliDeath->openFromFile(ResourceManager::getFilePath("skeli-death.ogg"))) {
        exit(EXIT_FAILURE);
    }
    _skeliDeath->setVolume(15);

    // ---- load lance throw sound ----
    _lanceThrow = new sf::Music;
    if (!_lanceThrow->openFromFile(ResourceManager::getFilePath("lance-throw.ogg"))) {
        exit(EXIT_FAILURE);
    }
    _lanceThrow->setVolume(15); 

    // ---- crate player ball ----
    // allocating memory for the Player object to which the _player pointer points to
    _player = new Player(this);
    // starting the jumping cooldown timer
    _jumpCoolDownClock.restart();
    // starting the throwable cooldown timer
    _throwableCoolDownClock.restart();

    // ---- create map ----
    // allocating memory for the Map object to which the _map pointer points to
    _map = new Map(this);
    // start the timer which holds the time until win
    _gameTimer.restart(); // start timer
    
    // ---- start spawning skeletons ----
    // start the spawning
    _skeliSpawnRateClock.restart();

    // ---- start contact listener ----
    // set up the contact listeners
    // passing in 'this' works because Game inherits from b2ContactListener and implements the listeners
    _world.SetContactListener(this);
	// ---- create map ----
	for (auto box : MAP) {
		createBox(box, sf::Color(50, 50, 50)); // gray color
	}
    // ---- seed rand ----
    // seed rand by time in sconds since unix epoch in Jan 1 1970
    std::srand(static_cast<unsigned>(std::time(0)));
}

Game::~Game() {
    // delete all allocated things
	delete _player;
    delete _map;
    delete _throwable;
    delete _skeliMusic;
    delete _skeliDeath;
    delete _hiverSprite;
    delete _hiverTexture;
    delete _skeliSprite;
    delete _skeliTexture;
    delete _lanceThrow;
    for (Box* box : _boxes) {
        delete box; 
    }
    for (Skeleton* skeli : _skeletons) {
        delete skeli;
    }
    // remove dangling pointers from the vectors
    _boxes.clear();
    _skeletons.clear();
}

// this the gateway to the Game, it is a public function that holds the main game loop
void Game::run() {
    sf::Clock frameClock;

    // main game loop
    while (_window.isOpen()) {
        // ---- handle events ----
        // poll events: if there are any unprocessed events waiting then they are handled
        for (auto event = sf::Event(); _window.pollEvent(event);) {
            // switch on the ecent type 
            switch (event.type) {
                // ---- on window closed ----
                case sf::Event::Closed:
                    _window.close();
                    break;

                // ---- on key pressed ----
                // this is all pretty self-explanatory
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q) {
                        _window.close();
                    } else if (event.key.code == sf::Keyboard::Space) {
                        _jump = true;
                    } else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                        _mode == Editor ? _moveViewLeft = true : _rotateLeft = true;
                    } else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                        _mode == Editor ? _moveViewRight = true : _rotateRight = true;
                    } else if ((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) && _mode == Editor) {
                        _moveViewUp = true;
                    } else if ((event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) && _mode == Editor) {
                        _moveViewDown = true;
                    } else if (event.key.code == sf::Keyboard::M) {
                        _mode == Play ? _mode = Editor : _mode = Play; // toggle between editor and play modes
                    } else if (event.key.code == sf::Keyboard::Comma && _mode == Editor) {
                        std::cout << "Saving map\n";
                        _map->saveMap("newMap");
                    }
                    else if (event.key.code == sf::Keyboard::Period && _mode == Editor) {
                        std::cout << "Loading map\n";
                        _map->loadMap("newMap");
                    }
                    break;
				// ---- on key release -----
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::Space) {
                        //_jump = false;
                    } else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                        _mode == Editor ? _moveViewLeft = false : _rotateLeft = false;
                    } else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                        _mode == Editor ? _moveViewRight = false : _rotateRight = false;
                    } else if ((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) && _mode == Editor) {
                        _moveViewUp = false;
                    } else if ((event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) && _mode == Editor) {
                        _moveViewDown = false;
                    }
                    break;
                case sf::Event::MouseWheelScrolled:
					break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left && _mode == Play) {
                        //std::cout << "left click pressed\n";
                        _throw = true;
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left && _mode == Play) {
                        //std::cout << "left click released\n";
                        _throw = false;
                    }
                    break;
                default:
                    break;
            }
        }
        // take the time since one main game loop
        _deltaTime = frameClock.restart();

        // check which mode we are currently in and execute that
        switch(_mode) {
            case Play:
                updatePlay();
                break;
            case Editor:
                updateEditor();
                break;
        }
    }
}

void Game::updatePlay() {
    // ---- rotate the view ----
    // while _rotateLeft is on we keep incrementing _rotVel
    // while -rotateRight is on we keep decrementing _rotVel
    // did this because I wanted smooth feeling rotation
    if (_rotateLeft) {
        _rotVel += ROTATE_SPEED;
    }
    if (_rotateRight) {
        _rotVel -= ROTATE_SPEED;
    }
    // get the current rotation of the view in degrees
	float currRot = _view.getRotation();
    // ---- automatically reset view to default pos ----
    // if the current rotation is less than 12 and more than 0 which means
    // it is a situtation where the view is rotated counter clockwise or to the left
	if ((currRot <= 12 && currRot > 0 && !_rotateLeft && static_cast<int>(_view.getRotation()) != 0)) {
		_view.rotate(-6.f * _deltaTime.asSeconds());
	}
    // this checks if the current rotation is less than 360 but more than 347 which means
    // it is a situation where the view is rotated clockwise or to the right 
    else if ((currRot < 360 && currRot >= 348 && !_rotateRight && static_cast<int>(_view.getRotation() != 0))) {
		_view.rotate(6.f * _deltaTime.asSeconds());
	}
    // ---- player rotates view
    // allow the player to rotate only 10 degrees to both directions
	if ((currRot <= 10 && currRot >= 0) || (currRot <= 360 && currRot >= 350)) {
    	_view.rotate(_rotVel * _deltaTime.asSeconds()); // frame rate independent
	}
	
	// ---- restrict player movement speed ----
    // if the current velocity on the x is more than max speed
    if (std::abs(_player->getBody()->GetLinearVelocity().x) > MAX_MOVE_SPEED) {
		b2Vec2 newVel;
		if (_player->getBody()->GetLinearVelocity().x < 0)
			newVel = b2Vec2(-MAX_MOVE_SPEED, _player->getBody()->GetLinearVelocity().y);
		else
			newVel = b2Vec2(MAX_MOVE_SPEED, _player->getBody()->GetLinearVelocity().y);
		_player->getBody()->SetLinearVelocity(newVel); // set the new max velocity
	}

	_window.setView(_view); // set the newly rotated 
    _rotVel *= 0.8; // smoothing rotation
    // this is actually redundant and waste of memory, we can just use the currRot variable that was defined earlier
    float rotation = _view.getRotation();
    float radians = rotation * (b2_pi / 180.0f); // convert rotation angle to radians for trigonometric calculations
    // calculate gravity direction in world space to always point to window bottom
    b2Vec2 gravity;
    // use trigonometry to get how much force should be applied to each axis
    // the negative sign is there because in Box2d positive means to the right, we want to invert it...
    gravity.x = -GRAVITY_MAGNITUDE * sin(radians);
    gravity.y = GRAVITY_MAGNITUDE * cos(radians);
    // setting the gravity
	_world.SetGravity(gravity);

    // ---- jump ----
    // 1. if the jump key is pressed
    // 2. if _canJump is more than 0 which means that we are actually touching the floor
    // 3. if we are outside of the jump cooldown
    // 4. we are not already falling
    if (_jump && _canJump > 0 && _jumpCoolDownClock.getElapsedTime().asSeconds() > 0.7f
        && _player->getBody()->GetLinearVelocity().y < 0.001) {
        _jump = false; // set the current jump to be false: we are not jumping
        // start the delay:
        // need a small delay before the jump so that there is a way to first jump, then later destory the ground below
        _jumpDelayClock.restart();
        // get the force, take into account current view
        b2Vec2 force;
        force.x = -JUMP_FORCE * sin(radians);
        force.y = JUMP_FORCE * cos(radians); 
        // apply linear impulse which means that the force disappears over time: its push
        _player->getBody()->ApplyLinearImpulseToCenter(force, false);
        // start the cooldown
        _jumpCoolDownClock.restart();
        //std::cout << "jumped\n";
        // use a trigger to go into destroying to block of off which the player jumped from
        _jumpTrigger = true;
    }

    if (_jump && _jumpCoolDownClock.getElapsedTime().asSeconds() < 0.4f && _player->getBody()->GetLinearVelocity().y < 0.001) {
        _jump = false;
        b2Vec2 force;
        force.x = -JUMP_FORCE * sin(radians);
        force.y = -JUMP_FORCE / 4 * cos(radians); 
        // apply linear impulse which means that the force disappears over time: its push
        _player->getBody()->ApplyLinearImpulseToCenter(force, false);
    }

    // ---- create throwable ----
    // 1. the mouse was pressed
    // 2. there is notalready a throwable in the world
    // 3. we are not in the cooldown still of 1 second
    if (_throw && !_throwable && _throwableCoolDownClock.getElapsedTime().asSeconds() > 1.f) {
        // create the throwable
        _throwable = new Throwable(this, _player, &_window);
        if (!_throwable)
            std::cout << "Failed to create throwable\n";
        _throwableCoolDownClock.restart();
        _lanceThrow->play(); // paly the sound effect of the lance
        //std::cout << "threw throwable\n";
    }

    // ---- explode throwable ----
    // set in the contact listener
    if (_toBeExplodedByLance) {
        explode();
        _destroyThrowable = true;
    }

    // ---- exlpode ground after jump ----
    // destory the block below if
    // 1. we have jumped
    // 2. _toBeExolodedByJump is not nullptr, meaning we jumped off of something (set in the contact listener)
    // 3. 100 milliseconds have passed
    if (_jumpTrigger && _toBeExplodedByJump && _jumpDelayClock.getElapsedTime().asMilliseconds() > 100) {
        explode();
       _jumpTrigger = false;
    }

    // ---- destroy throwable ----
    // 1. the _destroyThrowable is set or if the throwable cooldown is over 3 sec
    // 2. there is a throwable

    // this is written a bit dumb you could shorten this
    if ((_destroyThrowable && _throwable) || (_throwable && _throwableCoolDownClock.getElapsedTime().asSeconds() > 3.f)) {
        _world.DestroyBody(_throwable->getBody()); // destory the physical body
        delete _throwable; // delete the throwable object
        _throwable = nullptr; // set it as nullptr
        _destroyThrowable = false; // we are not destroying it anymore
    }

    // ---- make boxoes over you fall out ----
    for (auto box : _boxes) {
        if (box->getShape()->getPosition().y < _player->getShape()->getPosition().y - 30) {
            box->getBody()->SetType(b2_dynamicBody);
            box->getBody()->GetFixtureList()->SetSensor(true);
            _view.move(sf::Vector2f(0, 0.05)); // move view down
        }
    }

    // ---- spawn skeletons ----
    spawnSkeletons();

    // ---- update skeletons ----
    for (auto skeli : _skeletons) {
        // move the skeletons towards the player
        skeli->updatePosition(_player->getShape()->getPosition(), _deltaTime.asSeconds());

        // check if its for skeleton to jump
        skeli->isJump(_deltaTime.asSeconds(), radians);

        // if lance hits skeleton
        // 1. throwable exists
        // 2. the throawble shape intersects with the skeli shape
        if (_throwable && _throwable->getShape()->getGlobalBounds().intersects(skeli->getShape()->getGlobalBounds())) {
            // make it the skeli body to not react to any collision
            skeli->getBody()->GetFixtureList()->SetSensor(true);
            // didn't use the current rotation of the world view for this one
            // probably because I was tired of coding and it was such a small movement
            b2Vec2 force;
            force.x = 0;
            force.y = -0.1;
            skeli->getBody()->ApplyLinearImpulseToCenter(force, true);
            skeli->getBody()->SetBullet(true); // just using this for kinda disabling this so the dead skelis dont kill player
            _skeliDeath->play();
        }
        // if skeli hits player
        // 1. player intersects with skeli
        // 2. the bullet is not set
        if (_player->getShape()->getGlobalBounds().intersects(skeli->getShape()->getGlobalBounds())
            && !skeli->getBody()->IsBullet()) {
            // getting the vel of skeli
            b2Vec2 force = skeli->getBody()->GetLinearVelocity();   
            force.y = 5; // shoulve made this a constant
            _player->getBody()->ApplyLinearImpulseToCenter(force, true);
        }

    }

    // ---- check death by void ----
    // 1. if we we corssed the y axis at 1200
    if (_player->getShape()->getPosition().y > 1200) {
        _lose = true;
        // set the winlose text
        _winLoseText.setString("YOU LOSE!");
        // get the lcoal bounds to set the origin
        sf::FloatRect textBounds = _winLoseText.getLocalBounds();
        // from the left bound we add half of the width to get to center same goes for height
        _winLoseText.setOrigin(textBounds.left + textBounds.width / 2, 
                            textBounds.top + textBounds.height / 2);
        // if the y corsses 2500 we respawn and load the map again
        if (_player->getShape()->getPosition().y > 2500) {
            restart();
            _lose = false; // we are not losing anymore since the game just restarted
        }
    }

    // should be named 'remaining_time' instead
    int elapsed_time = 60 - static_cast<int>(_gameTimer.getElapsedTime().asSeconds());
    // ---- check win condition ----
    // 1. the time has passed
    // 2. we didn't already win
    // 3. we didn't already lose
    if (elapsed_time < 0 && !_win && !_lose) {
        _winLoseText.setString("YOU WIN!"); // set the string
        // set the localbounds to set origin
        sf::FloatRect textBounds = _winLoseText.getLocalBounds();
        _winLoseText.setOrigin(textBounds.left + textBounds.width / 2, 
                            textBounds.top + textBounds.height / 2);
        _win = true; // we have won
        _winClock.restart(); // start the winning timer, so there is a delay after which the new game will begin
    // stop updating the timer if we won or lost
    } else if (!_win && !_lose) {
        // get the center and then substact half to get the left most cordinate of the view
        // do instead of fixes cordinates because the view might move and I want the text to follow the view
        // then add little offsets of x150 and y70
        _timerText.setPosition({_view.getCenter().x - _view.getSize().x / 2 + 150, _view.getCenter().y - _view.getSize().y / 2 + 70});
        // set the string to show the current elapsed time, or time remaining
        _timerText.setString(std::to_string(elapsed_time));
    }

    // ---- restart the game if won ----
    // 1. we won
    // 2. win time delay is met
    if (_win && _winClock.getElapsedTime().asSeconds() > 3) {
        restart();
        _win = false; // we are not winning anymore
    }

    // ---- update the physics ----
    if (!_win) // stop them if win
	    _world.Step(_deltaTime.asSeconds(), 8, 3); // take a step in the physics world

    _window.clear(); // clear the buffers so the frames displayed don't stay
    // ---- draw timer ----
    _window.draw(_timerText); // draw timer, maybe could have 

	// ---- draw player ----
    // draw player using the overwrite draw functiom, because player inherits from drawable
	_player->draw(_window, sf::RenderStates::Default);
    _hiverSprite->setPosition(_player->getShape()->getPosition()); // draw the hiver sprite instead of the player shape
    _window.draw(*_hiverSprite); // draw te hiver sprite

	// ---- draw map/boxes ----
	for (auto box : _boxes) {
		box->draw(_window, sf::RenderStates::Default);
	}

    // ---- draw throwable ----
    if (_throwable)
        _throwable->draw(_window, sf::RenderStates::Default);
    
    // ---- draw skeletons ----
    for (auto skeli : _skeletons) {
        // smae thing here as in player
        _skeliSprite->setPosition(skeli->getShape()->getPosition());
        _window.draw(*_skeliSprite);
        skeli->draw(_window, sf::RenderStates::Default);
    }

    // ---- draw winLose text ----
    if (_win || _lose) {
        _winLoseText.setPosition(_view.getCenter());
        _window.draw(_winLoseText);
    }
    // swap backbuffer to frontbuffer, making things visible
    _window.display();
}

void Game::updateEditor() {
    // depending on which is true, just move the view to the correcy direction
    // no smoothing or anything more complex is implemeted
    if (_moveViewLeft) {
        _view.move({-5, 0});
    }
    if (_moveViewRight) {
        _view.move({5, 0});
    }
    if (_moveViewUp) {
        _view.move({0, -5});
    }
    if (_moveViewDown) {
        _view.move({0, 5});
    }
    _window.setView(_view); // set the newly moved view

    sf::Vector2i mousePos = sf::Mouse::getPosition(_window); // getting mouse pos

    // if the left click is pressed the place block there
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        createBox(mousePos, sf::Color(50, 50, 50)); // gray
    }

    // if the right click is pressed remove a block from the positino
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        removeBox(mousePos);
    }

    // keep the editor text in the upper left corner of the screen
    _editorText.setPosition({_view.getCenter().x - _view.getSize().x / 2 + 10, _view.getCenter().y - _view.getSize().y / 2 + 10}); // add padding

    _window.clear();
    // ---- draw map/boxes ----
	for (auto box : _boxes) {
		box->draw(_window, sf::RenderStates::Default);
	}
    draw_grid();
    draw_box_at_cursor(mousePos); // draw the box to be placed
    _window.draw(_editorText);

    _window.display();
}


void Game::draw_grid() {
    sf::Vector2f topLeft = _window.mapPixelToCoords(sf::Vector2i(0, 0));
    sf::Vector2f bottomRight = _window.mapPixelToCoords(sf::Vector2i(_windowSize.x, _windowSize.y));

    // store the lines to be drawed in a vertex array
    // vertex array is a collection of vertices that can represent geometric primitives
    // every two vertices will form a line
    sf::VertexArray gridLines(sf::Lines);

    // calculate the vertical lines to be drawn
    // first get x which is the world cordinate where the line should be drawn:
    // topLeft.x / BOX_WIDTH gets us the gird index of which tile we are at
    // then static_cast to int to basically floor that value 1.9 -> 1.0
    // then multiplay by BOX_WIDTH to get the position where the line should be drawn
    // do this while x <= the most right position
    // always increase x by BOX_WIDTH to have each line separated by exactly one BOX
    for (int x = static_cast<int>(topLeft.x / BOX_WIDTH) * BOX_WIDTH; x <= bottomRight.x; x += BOX_WIDTH) {
        // append a vertex point to the top positon
        gridLines.append(sf::Vertex(sf::Vector2f(x, topLeft.y), sf::Color(50, 50, 50)));
        // append a vertex point to the bottom position
        gridLines.append(sf::Vertex(sf::Vector2f(x, bottomRight.y), sf::Color(50, 50, 50)));
    }

    // calculate the horizontal lines to be drawn
    for (int y = static_cast<int>(topLeft.y / BOX_WIDTH) * BOX_WIDTH; y <= bottomRight.y; y += BOX_WIDTH) {
        gridLines.append(sf::Vertex(sf::Vector2f(topLeft.x, y), sf::Color(50, 50, 50)));
        gridLines.append(sf::Vertex(sf::Vector2f(bottomRight.x, y), sf::Color(50, 50, 50)));
    }

    // Draw all lines in one call
    _window.draw(gridLines);
}

void Game::createBox(const sf::Vector2i &mousePos, const sf::Color &color) {   
    bool available = true;
    // converts the pixel positions to world positions with the current view
    sf::Vector2f mousePosConverted = _window.mapPixelToCoords(mousePos, _view);

    // divide mouse by 16 because each box is 16 pixels
    // then floor to round down example:
    // if mouseX = 31 then 31 / 16 ≈ 1.9
    // then we take floor(1.9) which gives us 1, so we are in the second tile
    float mouseX = std::floor(mousePosConverted.x / BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / BOX_WIDTH);
    // mouseX/Y is the index of which block is it in the grid:
    // lets say it is 1, then we are in the second block then we multiply it by BOX_WIDTH and
    // add half of BOX_WIDTH on it, we end up in the second block
    b2Vec2 checkPos((mouseX * BOX_WIDTH + BOX_WIDTH / 2) / SCALE, (mouseY * BOX_WIDTH + BOX_WIDTH / 2) / SCALE);

    // check that the spot isn't already occupied or
    // if there is already a spawn box in the world
	for (int i = 0; i < _boxes.size(); i++) {
		if (_boxes[i]->getBody() && _boxes[i]->getBody()->GetPosition() == checkPos) {
			available = false;
			break;
		}
	}

    // create the box
    if (available) {
        Box *box = new Box(this, checkPos, color);
        _boxes.push_back(box);
    }
}

void Game::removeBox(const sf::Vector2i &mousePos) {
    // convert the pixel cordinates to world cordinates in the world using the current view
    sf::Vector2f mousePosConverted = _window.mapPixelToCoords(mousePos, _view);
    
    // check which tile was pressed on by:
    // first dividing the position by BOX_WIDTH because each box is that big
    // the floor down so for example 1.9 would still mean that we are in the second block (1)
    float mouseX = std::floor(mousePosConverted.x / BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / BOX_WIDTH);

    // when we know th of which tile we are at, its time to get the center cordinates of the tile by:
    // multiplaying the psition with BOX_WIDTH so if if the tile number was 1 we get only the BOX_WIDTH
    // then we add half of the BOX_WIDH to that so we end up in the center of the second tile
    // then we still divide with SCALE since we are working with Box2D units which uses MKS (metre, kilo, seconds)
    // if we didn't divide, everything would just be so slow
    b2Vec2 checkPos((mouseX * BOX_WIDTH + BOX_WIDTH / 2) / SCALE, (mouseY * BOX_WIDTH + BOX_WIDTH / 2) / SCALE);

    // go through the boxes to find the one we will remove
    for (int i = 0; i < _boxes.size(); i++) {
        // if we find it
        if (_boxes[i]->getBody()->GetPosition() == checkPos) {
            _world.DestroyBody(_boxes[i]->getBody()); // destroy the physical body
            delete _boxes[i]; // destory the object in the vector
            _boxes[i] = _boxes.back(); // make just deleted pointer to point to the same object that last pointer points to
            _boxes.pop_back(); // then just pop the last pointer
            //std::cout << "remove block\n";
            break;
        }
    }
}

void Game::draw_box_at_cursor(const sf::Vector2i &mousePos) {
    sf::RectangleShape rectangle;
    
    // basic stuff
    rectangle.setFillColor(sf::Color(50, 50, 50));
    rectangle.setOrigin(BOX_WIDTH / 2, BOX_WIDTH / 2);
    rectangle.setSize({BOX_WIDTH, BOX_WIDTH});
    // need to use mapPixelToCoords to account for distrortion on location after using _view.move()
    rectangle.setPosition(_window.mapPixelToCoords(mousePos));

    _window.draw(rectangle);
}

void Game::explode() {
    float rotation = _view.getRotation(); // get the current view
    float radians = rotation * (b2_pi / 180.0f); // turn rotation into radians for trigonomeric calcs
    b2Vec2 force;
    // calculate the explosion force taking into account the current rotation
    force.x = EXPLOSION_FORCE * sin(radians);
    force.y = -EXPLOSION_FORCE * cos(radians);
    b2Filter filter;
    filter.maskBits = 0x0000; // set maskBits to zero to ignore all collisions
    // if we should explode ground by lance
    if (_toBeExplodedByLance) {
        _toBeExplodedByLance->GetFixtureList()->SetFilterData(filter);
        _toBeExplodedByLance->GetFixtureList()->SetSensor(true); // is be redundant since filter is applied earlier
        _toBeExplodedByLance->SetType(b2_dynamicBody); // set the body as dynamic so it falls
        _toBeExplodedByLance->ApplyLinearImpulseToCenter(force, true); // add the force
        _toBeExplodedByLance->ApplyTorque(100, true); // add some rotation
        _toBeExplodedByLance = nullptr; // make it this nullptr
    // if should explode ground by jumping
    } else {
        _toBeExplodedByJump->GetFixtureList()->SetFilterData(filter);
        _toBeExplodedByJump->GetFixtureList()->SetSensor(true);
        _toBeExplodedByJump->SetType(b2_dynamicBody);
        _toBeExplodedByJump->ApplyLinearImpulseToCenter(force, true);
        _toBeExplodedByJump->ApplyTorque(100, true);
        _toBeExplodedByJump = nullptr; 
    }
    //std::cout << "Block destroyed\n";
}


void Game::spawnSkeletons() {
    // if 1.75 seconds has passed: spawn a skeleton
    if (_skeliSpawnRateClock.getElapsedTime().asSeconds() > 1.75) {
        static bool flip = true;
        int spawnX;
        
        // every other spawns closer to player to prevent camping xD
        if (flip) {
            // take middle of window and substarct a fouth from it and add a random portion up to half of window
            spawnX = (_windowSize.x / 2 - _windowSize.x / 4) + rand() % _windowSize.x / 2;
            flip = false;
        } else {
            // take the player position and substract a sixth and add rand up to third
            spawnX = (_player->getShape()->getPosition().x - _windowSize.x / 6 + rand() % _windowSize.x / 3);
            flip = true;
        }
        // spawn position where y is 0 so they fall from the sky
        sf::Vector2i spawnPos = sf::Vector2i(spawnX, 0);
        Skeleton *skeli = new Skeleton(this, spawnPos); // create a skeli at that position
        _skeletons.push_back(skeli); // add that skeli to the vector
        _skeliSpawnRateClock.restart(); // restart the clock
        //std::cout << "skeleton spawned\n";
    }
}

void Game::restart() {
    // set the view to the center of the window to load the map correctly
    _view.setCenter(sf::Vector2f(_windowSize.x / 2, _windowSize.y / 2));
    // set the rotation to 0 to load the map correctly
    _view.setRotation(0);
    // destory every current box
    for (auto box : _boxes) {
        _world.DestroyBody(box->getBody());
        delete box;
    }
    // destroy every skeleton
    for (auto skeli : _skeletons) {
        _world.DestroyBody(skeli->getBody());
        delete skeli;
    }
    // clear dangling pointers
    _boxes.clear();
    _skeletons.clear();
    // warp the player to spawn position
    _player->getBody()->SetTransform(_spawnPos, 0);
    // create new boxes in new map
    for (auto box : MAP) {
        createBox(box, sf::Color(50, 50, 50)); // gray color
    }
    // reset the current velocity of the player
    _player->getBody()->SetLinearVelocity(b2Vec2(0, 0)); // reset velocity on spawn
    // reset the current rotation velocity
    _player->getBody()->SetAngularVelocity(0);
    _gameTimer.restart(); // restart game timer
    _skeliSpawnRateClock.restart(); // restart skeleton spawn clock
}

// ---- listeners ----
void Game::BeginContact(b2Contact* contact) {
    // get the userdata of both parties of the contact
    int* fixtureA_UserData = static_cast<int*>(contact->GetFixtureA()->GetBody()->GetUserData());
    int* fixtureB_UserData = static_cast<int*>(contact->GetFixtureB()->GetBody()->GetUserData());

    // if its something else than the player
    if ((contact->GetFixtureB()->GetBody() != _player->getBody() 
        && contact->GetFixtureA()->GetBody() != _player->getBody())) {
        // ------------ IF LANCE HITS GROUND ------------
        // if block is userdata A
        if (fixtureA_UserData && *fixtureA_UserData == BOX
            && fixtureB_UserData && *fixtureB_UserData == THROWABLE) {
            _toBeExplodedByLance = contact->GetFixtureA()->GetBody();
        // if block is userdata B
        } else if (fixtureB_UserData && *fixtureB_UserData == BOX
            && fixtureA_UserData && *fixtureA_UserData == THROWABLE) {
            _toBeExplodedByLance = contact->GetFixtureB()->GetBody();
        }
    // if something hits player
    } else {
        // was segfaulting if there was jump at the same time as throwable hit ground
        // but there is a problem with this if check :
        // _toBeExplodedByJump isn't udpating when there is a throwable
        if (!_throwable) {
            if (fixtureA_UserData && *fixtureA_UserData == PLAYER) {
                _toBeExplodedByJump = contact->GetFixtureB()->GetBody();
            } else if (fixtureB_UserData && *fixtureB_UserData == PLAYER) {
                _toBeExplodedByJump = contact->GetFixtureA()->GetBody();
            }
        }
        // increment can jump if we are touching a surface
        _canJump++;
    }
}

void Game::EndContact(b2Contact* contact) {
    // don't do anything if the contact doesn't involve player
    if ((contact->GetFixtureB()->GetBody() != _player->getBody() 
        && contact->GetFixtureA()->GetBody() != _player->getBody())) {
        return ;
    } else {
        // decrements when we end contact with a surface
        _canJump--;
    }
}

// --- getters ----
// these can't be const because some none const methods
// are called on the return of these
b2World &Game::getWorld() {
    return _world;
}

sf::Vector2u &Game::getWindowSize() {
    return _windowSize;
}

sf::View &Game::getView() {
    return _view;
}

std::vector<Box*> &Game::getBoxes() {
    return _boxes;
}
