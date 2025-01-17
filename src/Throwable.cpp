#include "Throwable.hpp"

// this is the constructor of the class Throwable
// takes in a pointer to game object, player and to a render window object
// initializes game pointer and player pointer
Throwable::Throwable(Game *game, Player *player, sf::RenderWindow *window) : _game(game), _player(player) {
    b2BodyDef bodyDef; // this is the body definition which hols all the information to construct a rigid body
    bodyDef.type = b2_dynamicBody; // set the body type to be a dynamic body which means it will react to forces

    // starting position of the throwable is the player's position with a tiny offset
    b2Vec2 startPosition = _player->getBody()->GetPosition();
	startPosition.y -= 0.2f; // set the spawn position to be on top of the player
    bodyDef.position = startPosition; // set it

	// get mouse pos
    sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
    // convert the cordinates from the target (view) to world cordinates using mapPixelToCoords()
    sf::Vector2f mouseWorldPosition = window->mapPixelToCoords(mousePosition, _game->getView());

    // ---- calculate direction vector ----
    // calculate the vector pointing from the throwable to the mouse
    b2Vec2 direction(mouseWorldPosition.x / SCALE - startPosition.x, mouseWorldPosition.y / SCALE - startPosition.y);

    // ---- normalize and Scale the Direction Vector ----
    float speed = THROWABLE_FORCE;

    // calc the length or (magnitude) of the vector using the pythagorean theorem
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // avoid devision by zero: check that the length isn't 0, meaning we haven't clicked exactly at the players position
    if (length != 0.0f) {
        // normalize the vector, why?:
        // a raw vector has both direction and magnitude
        // to only preserce the direciton, divide by length (magnitude)
        // then scale by constant speed
        direction.x = (direction.x / length) * speed;
        direction.y = (direction.y / length) * speed;
    }

    // set the initial linear velocity toward the mouse position
    bodyDef.linearVelocity = direction;
    bodyDef.allowSleep = false; // make the object not freeze when stationary

    b2Body* body = _game->getWorld().CreateBody(&bodyDef); // create the body

    b2PolygonShape rectangleShape; // create the collision shape
    rectangleShape.SetAsBox((THROWABLE_WIDTH - 5) / SCALE, THROWABLE_HEIGHT / SCALE); // with these values

    // create the fixture definition which holds all the attributes of the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &rectangleShape; // add the shape
    fixtureDef.density = 1.f; // make density one, which means how reactive the body is to external forces like inertia
    fixtureDef.restitution = 0.35f; // bouncines
    body->CreateFixture(&fixtureDef); // create the ficture in the body

    // have a pointer identifier to distiguish between objects when they collide
    int* identifier = new int(THROWABLE); // integer
    body->SetUserData(identifier); // set the user data, which box2d doesn't do anything with

    sf::RectangleShape* rectangle = new sf::RectangleShape(); // create the sfml shape that actually gets drawn
    rectangle->setSize(sf::Vector2f(THROWABLE_WIDTH, THROWABLE_HEIGHT + 2)); // set its size
    rectangle->setFillColor(sf::Color::Blue); // set its color
    rectangle->setOrigin(THROWABLE_WIDTH / 2, THROWABLE_HEIGHT / 2); // set its origin

    // set the class attributes
    _body = body;
    _shape = rectangle;

    // start the life time clock to make sure that the object gets destroyed eventually
    // even if it never collides
    _lifeTimeClock.restart();
}

// destructor
Throwable::~Throwable() {
	delete _shape;
    delete static_cast<int *>(_body->GetUserData());
}

// draw the throwable
void Throwable::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	_shape->setPosition(SCALE * _body->GetPosition().x, SCALE * _body->GetPosition().y); // get the position from the physical world
	_shape->setRotation(_body->GetAngle() * 180 / b2_pi); // get the angle from the physical world
	target.draw(*_shape); // draw the shape
}

// ---- getters ----
b2Body*	Throwable::getBody() const {
	return _body;
}

sf::RectangleShape* Throwable::getShape() const {
	return _shape;
}

sf::Clock &Throwable::getLifeTimeClock() {
	return _lifeTimeClock;
}