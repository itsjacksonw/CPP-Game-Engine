#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include "GameObject.h"
#include "Platform.h" 
#include "MovingPlatform.h"
#include "Character.h"
#include "Timeline.h"

using namespace std;

class InputThread {

    bool busy; // a member variable used to indicate thread "status"
    int i; // an identifier
    //ThreadExample *other; // a reference to the "other" thread in this example
    std::mutex* _mutex; // the object for mutual exclusion of execution
    Character* _player;
    float _deltaTime;

public:
    InputThread(int i, std::mutex* _mutex, Character* _player, long _deltaTime) {
        this->busy = false;
        this->i = i;
        if (i == 0) { this->busy = true; }
        this->_mutex = _mutex;
        this->_player = _player;
        this->_deltaTime = _deltaTime;
    }

    bool isBusy()
    {
        //std::lock_guard<std::mutex> lock(*_mutex);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
        return busy;
    }

    void run() {
        // player inputs
        try {
            //std::unique_lock<std::mutex> cv_lock(*this->_mutex);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                _player->walk(sf::Vector2f(1, 0), _deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                _player->walk(sf::Vector2f(-1, 0), _deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                _player->jump();
            }
            
        }
        catch (...) {
            std::cerr << "Thread " << i << " caught exception." << std::endl;
        }
    }

};

class CollisionThread {

    bool busy; // a member variable used to indicate thread "status"
    int i; // an identifier
    //ThreadExample *other; // a reference to the "other" thread in this example
    std::mutex* _mutex; // the object for mutual exclusion of execution
    Character* _player;
    GameObject* _platforms;
    int _numPlatforms;
    float _deltaTime;

public:
    CollisionThread(int i, std::mutex* _mutex, Character* _player, GameObject _platforms[], int _numPlatforms, long _deltaTime) {
        this->busy = false;
        this->i = i;
        if (i == 0) { this->busy = true; }
        this->_mutex = _mutex;
        this->_player = _player;
        this->_platforms = _platforms;
        this->_numPlatforms = _numPlatforms;
        this->_deltaTime = _deltaTime;
    }

    bool isBusy()
    {
        //std::lock_guard<std::mutex> lock(*_mutex);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
        return busy;
    }

    void run() {
        // player inputs
        try {
            std::unique_lock<std::mutex> cv_lock(*this->_mutex);
            _player->processFrame(_platforms, _numPlatforms, _deltaTime);

            
        }
        catch (...) {
            std::cerr << "Thread " << i << " caught exception." << std::endl;
        }
    }

};

/**
 * Wrapper function because threads can't take pointers to member functions.
 */
void run_input(InputThread* th)
{
    th->run();
}

void run_collision(CollisionThread* th)
{
    th->run();
}


int main() {


    // creates a window 800x600 pixels
    sf::RenderWindow window;
    window.create(sf::VideoMode(800, 600), "Game Window", sf::Style::Resize); //sf::Style::Close | sf::Style::Resize
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    // create a clock to keep track of time. Use timeline with the clock
    sf::Clock clock = sf::Clock();
    long previousTime = 0;

    float halfSpeed = 50.0f;
    float normalSpeed = 25.0f;
    float doubleSpeed = 12.5f;

    float tic = 25.0f;
    Timeline timeline = Timeline(&clock, tic);

    // create objects
    Platform platform = Platform(800, 100, 0, 500);
    Platform wall = Platform(25, 600, 0, 0);
    Platform wall2 = Platform(25, 600, 775, 0);
    Platform ceiling = Platform(800, 50, 0, 0);
    MovingPlatform movingPlatform = MovingPlatform(200, 25, 450, 450, sf::Vector2f(0, 1), 1, 200, 400);
    movingPlatform.setFillColor(sf::Color(sf::Color::Green));

    // create the player 
    Character player = Character(25, 50, 100, 300, 6);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.scancode == sf::Keyboard::Scan::P) {
                    timeline.togglePause();
                }

                if (event.key.scancode == sf::Keyboard::Scan::Comma) {
                    timeline.changeTic(halfSpeed);
                    previousTime = 0;
                }
                if (event.key.scancode == sf::Keyboard::Scan::Slash) {
                    timeline.changeTic(doubleSpeed);
                    previousTime = 0;
                }
                if (event.key.scancode == sf::Keyboard::Scan::Period) {
                    timeline.changeTic(normalSpeed);
                    previousTime = 0;
                }
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // calculate time passed
        long deltaTime = timeline.getTime() - previousTime;
        previousTime = timeline.getTime();

        // player input
        std::mutex mutex;
        std::condition_variable cv;

        bool ranInput = false;
        std::thread thread0;
        if (window.hasFocus()) {
            InputThread inputThread(0, &mutex, &player, deltaTime);
            thread0 = std::thread(run_input, &inputThread);
            ranInput = true;
        }
        

        // collision
        //player.processFrame(platforms, numPlatforms, deltaTime);
        int numPlatforms = 5;
        GameObject platforms[5] = { platform, movingPlatform, wall, wall2, ceiling }; // list of platforms to collide with

        CollisionThread collisionThread(0, &mutex, &player, platforms, numPlatforms, deltaTime);
        std::thread thread1(run_collision, &collisionThread);

        if (player.getPosition().y > 900) { // respawn if fall off screen
            player.setPosition(sf::Vector2f(100, 300));
        }

        // join the threads
        if (ranInput) {
            thread0.join();
        }
        thread1.join();


        

        window.draw(platform);
        window.draw(wall);
        window.draw(wall2);
        window.draw(ceiling);

        movingPlatform.frameMove(deltaTime, player);
        window.draw(movingPlatform);

        window.draw(player);


        // end the current frame
        window.display();
    }

    return 0;
}