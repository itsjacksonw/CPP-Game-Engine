#include "MovingPlatform.h"

MovingPlatform::MovingPlatform(float width, float height, float x, float y, sf::Vector2f direction, float speed, float top, float bottom)
: Platform(width, height, x, y) {

    this->direction = direction;
    this->speed = speed;
    this->top = top;
    this->bottom = bottom;
}

void MovingPlatform::frameMove(float deltaTime, GameObject player) {

    if (getPosition().y <= top) {
         direction = sf::Vector2f(0, -1);
    }
    if (getPosition().y >= bottom) {
        direction = sf::Vector2f(0, 1);
    }

    sf::Vector2f movement = direction * -speed * deltaTime;
    move(movement);

    sf::FloatRect bounds = getGlobalBounds();
    sf::FloatRect playerBounds = player.getGlobalBounds();
         
    if (bounds.intersects(playerBounds) && playerBounds.top > bounds.top) { // collision with player, move player with platform
        player.move(movement);
    }

    update(); 
}
    

