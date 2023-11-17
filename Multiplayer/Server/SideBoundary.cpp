#include "SideBoundary.h"
#include <iostream>

using namespace std;
SideBoundary::SideBoundary(float width, float height, float x, float y, int direction, float distance, int id)
    : GameObject(width, height, x, y) {
    this->direction = direction;
    this->distance = distance;
    this->id = id;
}

void SideBoundary::moveScreen(GameObject* objects[], int numObjects) {
    for (int i = 0; i < numObjects; i++) {
        if ((*objects[i]).getPosition().x == -25) {
            break;
        }
        (*objects[i]).move(sf::Vector2f(direction, 0) * distance);
    }
}

int SideBoundary::getId() {
    return this->id;
}