#include "MovingPlatform.h"


MovingPlatform::MovingPlatform() {

}
MovingPlatform::MovingPlatform(float width, float height, float x, float y, sf::Vector2f direction, float speed, float top, float bottom)
: Platform(width, height, x, y) {

    this->direction = direction;
    this->speed = speed;
    this->top = top;
    this->bottom = bottom;
    if (direction.y != 0) {
        vertical = true;
    }
}

MovingPlatform::MovingPlatform(float width, float height, float x, float y, sf::Vector2f direction, float speed, float top, float bottom, sf::Color color)
    : Platform(width, height, x, y, color) {

    this->direction = direction;
    this->speed = speed;
    this->top = top;
    this->bottom = bottom;
    if (direction.y != 0) {
        vertical = true;
    }
}

void MovingPlatform::frameMove(float deltaTime, GameObject player) {

    if (getPosition().x < 0 || getPosition().x > 800) {
        return;
    }

    if (vertical) {
        if (getPosition().y <= top) {
            direction = sf::Vector2f(direction.x, -1);
        }
        if (getPosition().y >= bottom) {
            direction = sf::Vector2f(direction.x, 1);
        }
    }

    else {
        if (getPosition().x >= top) {
            direction = sf::Vector2f(1, direction.y);
        }
        if (getPosition().x <= bottom) {
            direction = sf::Vector2f(-1, direction.y);
        }
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
    

