//#ifndef GameObject_H
#pragma once
#include <SFML/Graphics.hpp>


class GameObject : public sf::RectangleShape {

public:
    GameObject();

    GameObject(float width, float height);

    GameObject(float width, float height, float x, float y);


    // sets the width of the GameObject
    void setWidth(float width);
    // sets the height of the GameObject
    void setHeight(float height);
    // sets the y position of the GameObject
    void setY(float y);
    // sets the x position of the GameObject
    void setX(float x);
};

//#define GameObject_H
//#endif