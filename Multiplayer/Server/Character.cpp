#include "Character.h"

float pixelLeniency = 4; // the amount of pixels a player can miss a jump and still make it

Character::Character(float width, float height, float x, float y, float speed)
: GameObject(width, height, x, y) {
    setFillColor(sf::Color(sf::Color::Blue));
    this->speed = speed;
}

void Character::walk(sf::Vector2f direction, float deltaTime) {

    sf::Vector2f movement = direction * speed * deltaTime;
    move(movement);
    update(); 
}

void Character::jump() {
    if (isGrounded) {
        move(sf::Vector2f(0, -pixelLeniency - 1));
        isGrounded = false;
        yVelocity = jumpSpeed;
    }
}

void Character::processFrame(GameObject platforms[], int numPlatforms, float deltaTime) {
    if (!Character::checkCollisions(platforms, numPlatforms)) { // check collisions, if not on ground, run y velocity
        Character::moveY(deltaTime);
        Character::gravity(deltaTime);
    }
    
}

// private functions

bool Character::checkCollisions(GameObject platforms[], int numPlatforms) {
    for (int i = 0; i < numPlatforms; i++) {
        sf::FloatRect bounds = getGlobalBounds();
        sf::FloatRect platformBounds = platforms[i].getGlobalBounds();
         
        if (bounds.intersects(platformBounds)) { // collision

            if (bounds.top + bounds.height > platformBounds.top // if character is colliding with the top of an object (ground)
                && bounds.top < platformBounds.top
                && bounds.top + bounds.height <= platformBounds.top + pixelLeniency) { // only if the player is very close to the surface, otherwise they are on a wall
                setPosition(getPosition().x, platformBounds.top - bounds.height);
                yVelocity = 0;
                isGrounded = true;
                return true;
            }

            else if (bounds.top < platformBounds.top + platformBounds.height // if the character is colliding the the bottom of an object
                && bounds.top > platformBounds.top
                && bounds.top + pixelLeniency >= platformBounds.top + platformBounds.height) { 
                setPosition(getPosition().x, platformBounds.top + platformBounds.height + pixelLeniency);
                if (yVelocity > 0) {
                    yVelocity = 0;
                }
                isGrounded = false;
            }

            else if (bounds.left < platformBounds.left + platformBounds.width // if the character is running into the left wall
                && bounds.left + bounds.width > platformBounds.left + platformBounds.width) {
                setPosition(platformBounds.left + platformBounds.width, getPosition().y);
                isGrounded = false;
            }

            else if (bounds.left + bounds.width > platformBounds.left // if the character runs into the right wall
                && bounds.left < platformBounds.left) {
                setPosition(platformBounds.left - bounds.width, getPosition().y);
                isGrounded = false;
            }

            else if (bounds.top + bounds.height > platformBounds.top 
                && bounds.left > platformBounds.left
                && bounds.left + bounds.width < platformBounds.left + platformBounds.width) { // extra check incase lagging
                setPosition(getPosition().x, platformBounds.top - bounds.height);
                yVelocity = 0;
                isGrounded = true;
                return true;
            }
        }
    }
    isGrounded = false;
    return false;
    update();
}

void Character::gravity(float deltaTime) {

    yVelocity += gravityAcceleration * deltaTime;

    if (yVelocity < maxFallSpeed) { // cap the max fall speed
        yVelocity = maxFallSpeed;
    }
}

void Character::moveY(float deltaTime) {
    move(sf::Vector2f(0, -yVelocity * deltaTime));
}