#include "GameObject.h"

class Character : public GameObject {

private:
float speed;
float jumpSpeed = 20;
float yVelocity = 0;
float gravityAcceleration = -2;
float maxFallSpeed = -16;
bool isGrounded = false;

bool checkCollisions(GameObject platforms[], int numPlatforms); // returns true if the player hits the ground
void gravity(float deltaTime);
void moveY(float deltaTime);

public:

Character(float width, float height, float x, float y, float speed); // creates a character
void walk(sf::Vector2f direction, float deltaTime);
void jump();
void processFrame(GameObject platforms[], int numPlatforms, float deltaTime);

};