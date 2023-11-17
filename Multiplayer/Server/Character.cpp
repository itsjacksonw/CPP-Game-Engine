#include "Character.h"



using namespace std;

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

void Character::processFrame(GameObject* platforms[], int numPlatforms,
    SpawnPoint* spawns[], int numSpawns, DeathZone* deaths[], int numDeaths,
    SideBoundary* scrolls[], int numScrolls, float deltaTime) {
    if (!Character::checkCollisions(platforms, numPlatforms)) { // check collisions, if not on ground, run y velocity
        Character::moveY(deltaTime);
        Character::gravity(deltaTime);
    }
    //check for new spawnpoint
    checkSpawns(spawns, numSpawns);
    //check for deathzone
    checkDeaths(deaths, numDeaths);
    //check for screen boundaries
    SideBoundary boundary = checkScrolls(scrolls, numScrolls);
    if (boundary.getId() != -1) {
        
        boundary.moveScreen(platforms, numPlatforms);
        boundary.moveScreen((GameObject**)spawns, numSpawns);
        boundary.moveScreen((GameObject**)deaths, numDeaths);
        boundary.moveScreen((GameObject**)scrolls, numScrolls);

        move(sf::Vector2f(boundary.direction, 0) * boundary.distance);
        spawn += sf::Vector2f(boundary.direction, 0) * boundary.distance;

    }
    
}

// private functions

bool Character::checkCollisions(GameObject* platforms[], int numPlatforms) {

    for (int i = 0; i < numPlatforms; i++) {

        sf::FloatRect bounds = getGlobalBounds();
        sf::FloatRect platformBounds = (*platforms[i]).getGlobalBounds();
         
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

void Character::checkSpawns(SpawnPoint* spawns[], int numSpawns) {
    for (int i = 0; i < numSpawns; i++) {
        sf::FloatRect bounds = getGlobalBounds();
        sf::FloatRect spawnBounds = (*spawns[i]).getGlobalBounds();

        if (bounds.intersects(spawnBounds)) { // collision
            this->spawn = (*spawns[i]).getSpawn();
            return;
        }
    }
}

void Character::checkDeaths(DeathZone* deaths[], int numDeaths) {
    for (int i = 0; i < numDeaths; i++) {
        sf::FloatRect bounds = getGlobalBounds();
        sf::FloatRect deathBounds = (*deaths[i]).getGlobalBounds();

        if (bounds.intersects(deathBounds)) { // collision
            setPosition(this->spawn);
            this->yVelocity = 0;
            //eventManager->RecieveEvent(0); // raise spawn event
            return;
        }
    }
}

SideBoundary Character::checkScrolls(SideBoundary* scrolls[], int numScrolls) {
    for (int i = 0; i < numScrolls; i++) {
        sf::FloatRect bounds = getGlobalBounds();
        sf::FloatRect scrollBounds = (*scrolls[i]).getGlobalBounds();

        if (bounds.intersects(scrollBounds)) { // collision
            
            if (lastScroll != (*scrolls[i]).getId()) {
                lastScroll = (*scrolls[i]).getId();
                return (*scrolls[i]);
            }
        }
    }
    return SideBoundary(0, 0, 0, 0, 0, 0, -1);
}

void Character::setVelocity(sf::Vector2f velocity)
{
    yVelocity = velocity.y;
}

sf::Vector2f Character::getSpawn()
{
    return spawn;
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