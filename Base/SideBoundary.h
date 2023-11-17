#pragma once
#include "GameObject.h"

class SideBoundary : public GameObject {
private:
    
    int id = -1;

public:
    SideBoundary(float width, float height, float x, float y, int direction, float distance, int id);
    void moveScreen(GameObject* objects[], int numObjects);
    int getId();
    int direction;
    float distance;
};