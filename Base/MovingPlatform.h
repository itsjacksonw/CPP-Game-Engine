#include "Platform.h"

class MovingPlatform : public Platform {

private:
sf::Vector2f direction;
float speed;
float top;
float bottom;
bool vertical = false;

public:

MovingPlatform();
MovingPlatform(float width, float height, float x, float y,
    sf::Vector2f direction, float speed, float top, float bottom);

MovingPlatform(float width, float height, float x, float y,
    sf::Vector2f direction, float speed, float top, float bottom, sf::Color color);

void frameMove(float deltaTime, GameObject player);
    
};