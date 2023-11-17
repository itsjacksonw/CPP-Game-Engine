#pragma once
#include "GameObject.h"

class Platform : public GameObject {

public:

Platform();
Platform(float width, float height, float x, float y);
Platform(float width, float height, float x, float y, sf::Color color);

};