#pragma once
#include <SFML/System/Clock.hpp>
#include <mutex>

class Timeline {
private:
    //std::mutex m; //if tics can change size and the game is multithreaded
    float start_time; //the time of the *anchor when created
    float elapsed_paused_time = 0;
    float last_paused_time = 0;
    float tic; //units of anchor timeline per step
    bool paused = false;
    sf::Clock* anchor; //for most general game time, system library pointer

public:
    Timeline(sf::Clock* anchor, float tic);
    long getTime(); //this can be game or system time implementation
    void togglePause();
    void pause();
    void unpause();
    void changeTic(float tic); //optional
    bool isPaused(); //optional
};
