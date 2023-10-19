#include "Platform.h"

class MovingPlatform : public Platform {

private:
sf::Vector2f direction;
float speed;
float top;
float bottom;

public:

MovingPlatform(float width, float height, float x, float y,
    sf::Vector2f direction, float speed, float top, float bottom);

void frameMove(float deltaTime, GameObject player);
    
};