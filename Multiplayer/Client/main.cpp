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
#include <cmath>
#include <zmq.hpp>
#include <windows.h>

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

    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::req);

    std::cout << "Connecting to server..." << std::endl;
    socket.connect("tcp://localhost:5555");

    zmq::message_t start(5);
    memcpy(start.data(), "Join", 5);
    socket.send(start, zmq::send_flags::none);
    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);
    char* id = (char*)reply.data();
    cout << "Connected Id: " << id << endl;

    int connections = 0;

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
    MovingPlatform movingPlatform = MovingPlatform(200, 25, 450, 450, sf::Vector2f(0, 1), 0, 200, 400);
    movingPlatform.setFillColor(sf::Color(sf::Color::Green));

    // create the player 
    Character player = Character(25, 50, 100, 300, 6);

    Character otherPlayers[3] = { Character(25, 50, 100, 300, 6),
        Character(25, 50, 100, 300, 6), Character(25, 50, 100, 300, 6) };

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


        // contact the server
        string position;
        int x = round(player.getPosition().x);
        int y = round(player.getPosition().y);
        string sx = to_string(x);
        string sy = to_string(y);
        position = "," + sx + "." + sy; //c_str() to convert to chars
        position = id + position;
        const char* message = position.c_str();
        zmq::message_t request(strlen(message) + 1);
        memcpy(request.data(), message, strlen(message) + 1);
        //cout << (char*)request.data() << endl;
        socket.send(request, zmq::send_flags::none);
        
        // now get data back from the server
        char buffer[1024];
        int size = zmq_recv(socket, buffer, 1024, 0);

        if (buffer[0] == '1') { // this means there is only one connection, so only one more player to render
            connections = 1;
            string mes = buffer;
            //cout << mes << endl;
            string mx = mes.substr(2, mes.find(".") - 2);
            string my = mes.substr(mes.find(".") + 1, mes.find("/") - 6);
            
            movingPlatform.setPosition(sf::Vector2f(stoi(mx), stoi(my)));
            string next = mes.substr(mes.find("/") + 1, mes.length() - 1);
            //cout << next << endl;
            string p1x = next.substr(0, next.find("."));
            string p1y = next.substr(next.find(".") + 1, next.find("/") - 3);
            //cout << p1x << " " << p1y << endl;
            otherPlayers[0].setPosition(sf::Vector2f(stoi(p1x), stoi(p1y)));
        }

        if (buffer[0] == '2') { // two clients are connected (including this one)
            connections = 2;
            string mes = buffer;
            //cout << mes << endl;
            string mx = mes.substr(2, mes.find(".") - 2);
            string my = mes.substr(mes.find(".") + 1, mes.find("/") - 6);

            movingPlatform.setPosition(sf::Vector2f(stoi(mx), stoi(my)));
            string next = mes.substr(mes.find("/") + 1, mes.length() - 1);
            //cout << next << endl;
            string p1x = next.substr(0, next.find("."));
            string p1y = next.substr(next.find(".") + 1, next.find("/") - 3);
            //cout << p1x << " " << p1y << endl;
            otherPlayers[0].setPosition(sf::Vector2f(stoi(p1x), stoi(p1y)));

            string next2 = next.substr(next.find("/") + 1, next.length() - 1);
            string p2x = next2.substr(0, next2.find("."));
            string p2y = next2.substr(next2.find(".") + 1, next2.find("/") - 3);

            //cout << next2 << " " << p2x << " " << p2y << endl;
            otherPlayers[1].setPosition(sf::Vector2f(stoi(p2x), stoi(p2y)));
        }

        if (buffer[0] == '3') { // three clients are connected (including this one)
            connections = 3;
            string mes = buffer;
            //cout << mes << endl;
            string mx = mes.substr(2, mes.find(".") - 2);
            string my = mes.substr(mes.find(".") + 1, mes.find("/") - 6);

            movingPlatform.setPosition(sf::Vector2f(stoi(mx), stoi(my)));
            string next = mes.substr(mes.find("/") + 1, mes.length() - 1);
            //cout << next << endl;
            string p1x = next.substr(0, next.find("."));
            string p1y = next.substr(next.find(".") + 1, next.find("/") - 3);
            //cout << p1x << " " << p1y << endl;
            otherPlayers[0].setPosition(sf::Vector2f(stoi(p1x), stoi(p1y)));

            string next2 = next.substr(next.find("/") + 1, next.length() - 1);
            string p2x = next2.substr(0, next2.find("."));
            string p2y = next2.substr(next2.find(".") + 1, next2.find("/") - 3);

            //cout << next2 << " " << p2x << " " << p2y << endl;
            otherPlayers[1].setPosition(sf::Vector2f(stoi(p2x), stoi(p2y)));

            string next3 = next2.substr(next2.find("/") + 1, next2.length() - 1);
            string p3x = next3.substr(0, next3.find("."));
            string p3y = next3.substr(next3.find(".") + 1, next3.find("/") - 3);
            otherPlayers[2].setPosition(sf::Vector2f(stoi(p3x), stoi(p3y)));
        }

        // draw
        if (connections > 0) {
            window.draw(otherPlayers[0]);
        }
        if (connections > 1) {
            window.draw(otherPlayers[1]);
        }
        if (connections > 2) {
            window.draw(otherPlayers[2]);
        }

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