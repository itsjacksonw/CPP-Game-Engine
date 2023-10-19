#include "Platform.h"

GameObject::GameObject() :
    sf::RectangleShape() {
    setPosition(0, 0);
}

GameObject::GameObject(float width, float height)
: sf::RectangleShape(sf::Vector2f(width, height)) {
    setPosition(0, 0);
}

GameObject::GameObject(float width, float height, float x, float y)
: sf::RectangleShape(sf::Vector2f(width, height)) {
    setPosition(x, y);
}


void GameObject::setWidth(float width) {
    sf::Vector2f size = sf::Vector2f(width, getSize().y); 
    setSize(size);
    update();
}

void GameObject::setHeight(float height) {
    sf::Vector2f size = sf::Vector2f(getSize().x, height);
    setSize(size);
    update();
}

void GameObject::setY(float y) {
    sf::Vector2f location = sf::Vector2f(getPosition().x, -y);
    setPosition(location);
    update();
}

void GameObject::setX(float x) {
    sf::Vector2f location = sf::Vector2f(-x, getPosition().y);
    setPosition(location);
    update();
}


