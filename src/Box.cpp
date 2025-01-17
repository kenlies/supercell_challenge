#include "Box.hpp"

// constructor of Box takes in a pointer to game object, a position in box2d cordinates and a color
Box::Box(Game *game, b2Vec2 &checkPos, const sf::Color &color) : _game(game) {

    // ---- physics attributes ----
	// bodydef holds all the data to construct a rigid body
	// shpaes are added to body after construciton
    b2BodyDef bodyDef;
    bodyDef.position = checkPos; // set the position
	bodyDef.type = b2_staticBody; // set body type
	
	b2Body* body = _game->getWorld().CreateBody(&bodyDef); // create body
	
	// every fixture has a shape which is used to check for collisions with other fixtures as it moves around the scene

	b2PolygonShape Shape; // create a shape
	Shape.SetAsBox((BOX_WIDTH / 2) / SCALE, (BOX_WIDTH / 2) / SCALE); // set shape to be a cube
	b2FixtureDef fixtureDef; // create the fixture

	fixtureDef.shape = &Shape; // pointer to the shape above
	fixtureDef.density = 1.f;
	fixtureDef.restitution = .2f; // bounciness

	body->CreateFixture(&fixtureDef); // add the fixture to the body

	// ---- set color identifier -----
	int* identifier; // identifier for checking what collided with what in the contact listener
	identifier = new int(BOX); // add integer to identify
	body->SetUserData(identifier); // set the identifier as userdata
	// box2d doesn't do anything with the data, it just stores it and gives it to you when you ask for it

	// ---- drawing attributes ----
	// this seciton is just for what the player can see
	sf::RectangleShape *rectangle = new sf::RectangleShape(); // set the shape to be a rectagle
	rectangle->setOrigin(BOX_WIDTH / 2, BOX_WIDTH / 2); // set it origin
	rectangle->setSize({BOX_WIDTH, BOX_WIDTH}); // set its size
	rectangle->setFillColor(color); // set its color
	rectangle->setOutlineThickness(1.5); // set the outline thickness
	rectangle->setOutlineColor(sf::Color(60, 60, 60)); // set the outline color (lighter gray)

	// ---- store them in one data structure ----
	// now we store the body in the the class attribute _body which is going to be used for all the physics and how the
	// object moves and collides with other objects
	// we store the shape in the class attribute _shape to then draw the shape at the position where the _body is
	_body = body; // set body
	_shape = rectangle; // set shape
}

// Box inherits from Drawable so we can overwrite the draw function to draw the shape associated with the Box at the
// location in the box2d (physical) world
void Box::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// set the shapes position according to where it is located in the physical world (box2d world)
	// we need to multiply it with the SCALE because thats how box2d works and thats how we convert positions between
	// sfml and box2d
	_shape->setPosition(SCALE * _body->GetPosition().x, SCALE * _body->GetPosition().y);
	// set rotation of the shape
	// we can convert radians to degrees by: radians * 180 / pi
	_shape->setRotation(_body->GetAngle() * 180 / b2_pi);
	// draw the shape to the render target
	target.draw(*_shape);
}

// Box destructor
Box::~Box() {
	// deletes shape: it is dynamically allocated
	delete _shape;
	// delete user data: it is dynamically allocated
	delete static_cast<int *>(_body->GetUserData());
}

// ---- getters ----

// get the Box body
// defined const: it doesn't modify any member attribures
b2Body*	Box::getBody() const {
	return _body;
}

// get the Box shape 
// defined const: it doesn't modify any member attribures
sf::RectangleShape* Box::getShape() const {
	return _shape;
}
