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
#include "SpawnPoint.h"
#include "DeathZone.h"
#include "EventManager.h"

using namespace std;

class InputThread {

    bool busy; // a member variable used to indicate thread "status"
    int i; // an identifier
    //ThreadExample *other; // a reference to the "other" thread in this example
    std::mutex* _mutex; // the object for mutual exclusion of execution
    long* previousTime;
    EventManager* e;

public:
    InputThread(int i, std::mutex* _mutex, long* previousTime, EventManager* e) {
        this->busy = false;
        this->i = i;
        if (i == 0) { this->busy = true; }
        this->_mutex = _mutex;
        this->previousTime = previousTime;
        this->e = e;
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
                //_player->walk(sf::Vector2f(1, 0), _deltaTime);
                
                e->ReceiveEvent(e->EVENT_TYPE_INPUT, 5);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                e->ReceiveEvent(e->EVENT_TYPE_INPUT, 4);
                //_player->walk(sf::Vector2f(-1, 0), _deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                e->ReceiveEvent(e->EVENT_TYPE_INPUT, 6);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                e->ReceiveEvent(e->EVENT_TYPE_INPUT, 7);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                if (e->timeline->tic != 10) {
                    e->ReceiveEvent(e->EVENT_TYPE_INPUT, 8);
                    *previousTime = 0;
                }
                
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Slash)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                if (e->timeline->tic != 40) {
                    e->ReceiveEvent(e->EVENT_TYPE_INPUT, 10);
                    *previousTime = 0;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period)) {
                std::unique_lock<std::mutex> cv_lock(*this->_mutex);
                if (e->timeline->tic != 20) {
                    e->ReceiveEvent(e->EVENT_TYPE_INPUT, 9);
                    *previousTime = 0;
                }
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
   
    EventManager* e;

public:
    CollisionThread(int i, std::mutex* _mutex, EventManager* e) {
        this->busy = false;
        this->i = i;
        if (i == 0) { this->busy = true; }
        this->_mutex = _mutex;
        this->e = e;
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
            //_player->processFrame(_platforms, _numPlatforms, _spawns, _numSpawns, _deaths, _numDeaths, _scrolls, _numScrolls, _deltaTime);
            e->ReceiveEvent(e->EVENT_TYPE_COLLISION, 2);
            
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

    float halfSpeed = 40.0f;
    float normalSpeed = 20.0f;
    float doubleSpeed = 10.0f;

    float tic = 20.0f;
    Timeline timeline = Timeline(&clock, tic);

    EventManager eventManager = EventManager(&timeline);

    // create objects
    Platform platform1 = Platform(350, 100, 0, 500, sf::Color::Magenta);
    Platform platform2 = Platform(725, 100, 450, 500, sf::Color::Cyan);
    Platform platform3 = Platform(350, 100, 1275, 500, sf::Color::Green);
    Platform wall = Platform(25, 600, -25, 0);
    Platform wall2 = Platform(25, 600, 775 + 800, 0);
    Platform ceiling = Platform(800, 50, 0, 0);
    MovingPlatform movingPlatform = MovingPlatform(200, 25, 450, 450, sf::Vector2f(0, 1), 1, 200, 400, sf::Color::Green);
    MovingPlatform movingPlatform2 = MovingPlatform(200, 25, 100, 250, sf::Vector2f(1, 0), 1, 225, 100, sf::Color::Yellow);

    // checkpoints
    SpawnPoint spawn1 = SpawnPoint(100, 500, 25, 0, sf::Vector2f(50, 450));
    SpawnPoint spawn2 = SpawnPoint(100, 500, 675, 0, sf::Vector2f(725, 450));
    SpawnPoint spawn3 = SpawnPoint(100, 500, 800, 0, sf::Vector2f(80, 450));

    DeathZone death = DeathZone(1600, 100, 0, 600); // create death zone at bottom of screen

    // side scrolling
    SideBoundary scroll1 = SideBoundary(1, 600, 800, 0, -1, 800, 1);
    


    // create the player 
    Character player = Character(25, 50, 50, 300, 6);

    //register events
    eventManager.RegisterSpawnEvent(0, 5, &player); // create spawn event for player id = 0
    eventManager.RegisterDeathEvent(1, 1, &player); // create death event for player id = 1

    eventManager.RegisterInputEvent(4, 0, "left", &player); // id = 4
    eventManager.RegisterInputEvent(5, 0, "right", &player);
    eventManager.RegisterInputEvent(6, 0, "up", &player);
    eventManager.RegisterInputEvent(7, 0, "pause", &player);
    eventManager.RegisterInputEvent(8, 0, "fastSpeed", &player);
    eventManager.RegisterInputEvent(9, 0, "normalSpeed", &player);
    eventManager.RegisterInputEvent(10, 0, "slowSpeed", &player);
    

    int numPlatforms = 8;
    GameObject* platforms[8] = { &platform1, &platform2, &platform3, &movingPlatform, &movingPlatform2, &wall2, &ceiling, &wall }; // list of platforms to collide with
    int numSpawns = 3;
    SpawnPoint* spawns[3] = { &spawn1, &spawn2, &spawn3 };
    int numDeaths = 1;
    DeathZone* deaths[1] = { &death };
    int numScrolls = 1;
    SideBoundary* scrolls[1] = { &scroll1 };

    eventManager.RegisterCollisionEvent(2, 0, &player, platforms, numPlatforms, spawns, numSpawns, deaths, numDeaths, scrolls, numScrolls);

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
            InputThread inputThread(0, &mutex, &previousTime, &eventManager);
            thread0 = std::thread(run_input, &inputThread);
            ranInput = true;
        }
        

        // collision
        //player.processFrame(platforms, numPlatforms, deltaTime);
        

        CollisionThread collisionThread(0, &mutex, &eventManager);
        std::thread thread1(run_collision, &collisionThread);

        

        // join the threads
        if (ranInput) {
            thread0.join();
        }
        thread1.join();

        eventManager.DispatchEvents(deltaTime);

        player.moveY(deltaTime);
        player.gravity(deltaTime);

        window.draw(platform1);
        window.draw(platform2);
        window.draw(platform3);
        window.draw(wall);
        window.draw(wall2);
        window.draw(ceiling);

        movingPlatform.frameMove(deltaTime, player);
        movingPlatform2.frameMove(deltaTime, player);
        window.draw(movingPlatform);
        window.draw(movingPlatform2);

        window.draw(player);
        
        //std::cout << player.getPosition().x << " " << player.getPosition().y << endl;
        // end the current frame
        window.display();
    }

    return 0;
}