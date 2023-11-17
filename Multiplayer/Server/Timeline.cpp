#include "Timeline.h"

Timeline::Timeline(sf::Clock* anchor, float tic) {
    this->anchor = anchor;
    this->tic = tic;
    start_time = anchor->getElapsedTime().asMilliseconds(); 
}

long Timeline::getTime() { //returns the gametime
    if (isPaused()) { // if the game is paused, just return the last time it was paused, minus the previous pause times
        return (last_paused_time - elapsed_paused_time) / tic;
    }

    float anchorTime = anchor->getElapsedTime().asMilliseconds();
    long time = ((anchorTime - start_time - elapsed_paused_time) / tic);
    return time;
}

void Timeline::togglePause() {
    if (last_pasued_button_time + 0.5f > anchor->getElapsedTime().asSeconds()) { // stops from holding button breaking game
        return;
    }
    last_pasued_button_time = anchor->getElapsedTime().asSeconds();
    if (paused)
        unpause();
    else
        pause();
}

void Timeline::pause() {
    last_paused_time = anchor->getElapsedTime().asMilliseconds();
    paused = true;
}
void Timeline::unpause() {
    elapsed_paused_time += anchor->getElapsedTime().asMilliseconds() - last_paused_time;
    paused = false;
}
void Timeline::changeTic(float tic) { //changes the gametime tic speed
    anchor->restart();
    start_time = anchor->getElapsedTime().asMilliseconds();
    if (isPaused())
        unpause();
    last_paused_time = 0;
    elapsed_paused_time = 0;
    last_pasued_button_time = 0;
    this->tic = tic;
}
bool Timeline::isPaused() {
    return this->paused;
}
