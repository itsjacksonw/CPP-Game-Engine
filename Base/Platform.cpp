#include "Platform.h"

Platform::Platform() {

}
Platform::Platform(float width, float height, float x, float y)
: GameObject(width, height, x, y) {
    
}

Platform::Platform(float width, float height, float x, float y, sf::Color color)
    : GameObject(width, height, x, y) {
    setFillColor(color);
}


