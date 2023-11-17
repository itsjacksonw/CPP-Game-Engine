#pragma once
#include "GameObject.h"

class SpawnPoint : public GameObject {
private:
    sf::Vector2f spawn;

public:
    SpawnPoint(float width, float height, float x, float y, sf::Vector2f spawn);
    sf::Vector2f getSpawn();
};