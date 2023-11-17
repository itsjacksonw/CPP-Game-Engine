#include "SpawnPoint.h"

SpawnPoint::SpawnPoint(float width, float height, float x, float y, sf::Vector2f spawn)
    : GameObject(width, height, x, y) {
    this->spawn = spawn;
}

sf::Vector2f SpawnPoint::getSpawn() {
    return this->spawn;
}