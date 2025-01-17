#include "Skeleton.hpp"

// this is the constructor of the class Skeleton
// it takes in a pointer to the game object and a reference vecto to spawn position
Skeleton::Skeleton(Game *game, sf::Vector2i &spawnPos) : _game(game) { // initialier list sets the _game pointer
    b2BodyDef bodyDef; // BodyDef contains all information to construct a rigid body
    bodyDef.type = b2_dynamicBody; // set the bodytype to be a dynamic body, which means it reacts to forces
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2(spawnPos.x / SCALE, spawnPos.y / SCALE);
    bodyDef.allowSleep = false; // makes so that the objects don't freeze
    b2Body* body = _game->getWorld().CreateBody(&bodyDef); // create the body and save it to b2Body pointer

    b2CircleShape circleShape; // create a shape that holds the collition points
    circleShape.m_p.Set(0, 0);  // set the center to be in in the middle
    circleShape.m_radius = (CIRCLE_RADIUS - 1) / SCALE; // set the radious, substarct 1 to make it more aesthetic
    b2FixtureDef fixtureDef; // add a ficture definition to then add that to the body
    fixtureDef.shape = &circleShape; // add the shape to fixture body
	fixtureDef.density = 1.f;  // set density to 1: density basically sets the weight: how easy it reacts to forces like inertia
    // which means how resistant it is to rotation
	fixtureDef.restitution = 0.35f; // bounciness 
    body->CreateFixture(&fixtureDef); // create the fixture in the body

	int* identifier; // pointer to an identifier to identify later in the contact listener
	identifier = new int(SKELETON); // set the identifyer to point to an integer
	body->SetUserData(identifier); // set it as user data: box2d doesn't do anything with this
    // we can just retreive the data and do something with it ourselves

    // make the sfml circle shape which lets us see the actual thing
    sf::CircleShape *circle = new sf::CircleShape(); // circle shape
    circle->setRadius(CIRCLE_RADIUS); // set radius
    circle->setFillColor(sf::Color::Red); // set color
    circle->setOrigin({CIRCLE_RADIUS, CIRCLE_RADIUS}); // set origin

    // set the variables in the class attributes
	_body = body;
    _shape = circle;

    // the idea of this is the same that of moudlus (%) that it tailors the value to be at specified range, but since
    // we are working with floating point values, we cannot use modulus.

    // 1. std::rand() creates a random integer in the range [0, RAND_MAX]
    // 2. divide that value by RAND_MAX to normalize the value to be between 0 and 1
    // 3. multiply by 3 scales the values to be between 0 and 3
    // 4. add 1 to shift the values to be in range of 1 and 4

	_jumpInterval = 1.0f + static_cast<float>(std::rand()) / RAND_MAX * 3.0f; // random interval between 1 and 4 seconds
}

// destrucyor of skeleton
Skeleton::~Skeleton() {
	delete _shape;
    delete static_cast<int *>(_body->GetUserData());
}

// move the skeleton towards the player
// take in the vecotr player position and delta time
void Skeleton::updatePosition(sf::Vector2f playerPosition, float deltaTime) {
    b2Vec2 skeletonPos = _body->GetPosition(); // get the current skeleton position in box2d cordinates

    // check if the player is in the left or right side of skeleton
    // also convert out of box2d positions by multiplying by SCALE
    float directionX = playerPosition.x - skeletonPos.x * SCALE;

    // if skeleton is in the right side of player then directionX will be less than 0
    float moveDirection = directionX > 0 ? 1.0f : -1.0f;

	// only move the skeletons on the x axis
    b2Vec2 velocity(moveDirection * SKELETON_MOVE_SPEED, _body->GetLinearVelocity().y);
    _body->SetLinearVelocity(velocity);
}


void Skeleton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	_shape->setPosition(SCALE * _body->GetPosition().x, SCALE * _body->GetPosition().y);
	_shape->setRotation(_body->GetAngle() * 180 / b2_pi);
	//target.draw(*_shape);
	// commented cuz we draw the sprite instead
}

// check if its for the skeli to jump
// takes in deltatime
// takes in radians of the current angle of the view/world, because we don't want the world rotation
// to affect which way the skeleton jumps
void Skeleton::isJump(float dt, float radians) {
	_jumpTimer += dt; // increase jump timer by deltatime
    // if jump timer is bigger that jumpinterval and if the body is not already falling: its time to jump
	if (_jumpTimer > _jumpInterval && _body->GetLinearVelocity().y == 0) {
        b2Vec2 force;
        // the goal is to split the total jump force to x and y using trigonometry
        // sine and cosine are used to caluclate these based on angle
        // if we take off the sin and cos, then the skeletons wouldn't take in account the current rotation of the world
        // when jumping and it would result in weird behaviour
        force.x = -JUMP_FORCE * sin(radians);
        force.y = JUMP_FORCE * cos(radians); 
		_body->ApplyLinearImpulseToCenter(force, false);

		_jumpTimer = 0.0f;
        // the idea of this is the same that of moudlus (%) that it tailors the value to be at specified range, but since
        // we are working with floating point values, we cannot use modulus.

        // 1. std::rand() creates a random integer in the range [0, RAND_MAX]
        // 2. divide that value by RAND_MAX to normalize the value to be between 0 and 1
        // 3. multiply by 7 scales the values to be between 0 and 7
        // 4. add 3 to shift the values to be in range of 3 and 10
		_jumpInterval = 3.0f + static_cast<float>(std::rand()) / RAND_MAX * 7.0f;
	}
}

// ---- getters ----
b2Body*	Skeleton::getBody() const {
	return _body;
}

sf::CircleShape* Skeleton::getShape() const {
	return _shape;
}
