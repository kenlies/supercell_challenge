#include "Player.hpp"

Player::Player(Game *game) : _game(game) {
    b2BodyDef bodyDef; // bodydef is used to store the information to construct a rigid body
    bodyDef.type = b2_dynamicBody; // add the body type as a dynamic body, means that it reacts to other forces
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2((_game->getWindowSize().x / 2) / SCALE, (_game->getWindowSize().y / 16) / SCALE);
    // in box2d if a body stays still for some time, it becomes static (offline)
    // prevent that
    bodyDef.allowSleep = false;
    // construct the body using the bodydef
    b2Body* body = _game->getWorld().CreateBody(&bodyDef);

    b2CircleShape circleShape; // declare a shape
    circleShape.m_p.Set(0, 0); // set the center of mass be in the middle
    // set the radius of the circle (substact 1 for aesthetic reasons)
    // / SCALE: convert the radius from game units to Box2D's internal units
    circleShape.m_radius = (CIRCLE_RADIUS - 1) / SCALE;
    b2FixtureDef fixtureDef; // create a fixture (defines the properties and behavior of a fixture)
    fixtureDef.shape = &circleShape; // set the shape 
    // density determines how much that fixture contributes to the mass and inertia of the body it is attached to
    // inertia means how resistant it is to rotation
    // higher value makes the body heavier which makes it harder to move
	fixtureDef.density = 1.f;
	fixtureDef.restitution = 0.35f; // bounciness
    body->CreateFixture(&fixtureDef); // add the fixture to the body

	int* identifier; // identifier to evaluate which body is in contact with which
	identifier = new int(PLAYER); // set identifier
	body->SetUserData(identifier); // set identifier as userdata

    sf::CircleShape *circle = new sf::CircleShape(); // make the sfml shape that we actually draw
    circle->setRadius(CIRCLE_RADIUS); // set radius
    circle->setFillColor(sf::Color::Yellow); // set color
    circle->setOrigin({CIRCLE_RADIUS, CIRCLE_RADIUS}); // set origin

    // set the variables in the class attribute
	_body = body;
    _shape = circle;
}

// delete the necessary things in the destructor to prevent leaking
Player::~Player() {
	delete _shape;
    delete static_cast<int *>(_body->GetUserData());
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // linear damping reduces the velocity of the body over time
    // higher means it stops quicker
    _body->SetLinearDamping(1.6f);
    // set the shape positions and rotations based on the physics world positiojn and rotation
	_shape->setPosition(SCALE * _body->GetPosition().x, SCALE * _body->GetPosition().y);
	_shape->setRotation(_body->GetAngle() * 180 / b2_pi);
	//target.draw(*_shape);
    // commented out cuz we draw the sperite instead
}

// ---- getters ----
b2Body*	Player::getBody() const {
	return _body;
}

sf::CircleShape* Player::getShape() const {
	return _shape;
}