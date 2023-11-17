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
#include <zmq.hpp>

using namespace std;

class ClientCommunication {

    int i; // an identifier
    std::mutex* mutex; // the object for mutual exclusion of execution
    Character** players;
    bool** connectedPlayers;
    sf::Clock** playerTimer;
    zmq::socket_t* socket;
    MovingPlatform* movingPlatform;
    MovingPlatform* movingPlatform2;
    EventManager* eventManager;

public:
    ClientCommunication(int i, std::mutex* mutex, zmq::socket_t* socket, Character* players[4],
        bool* connectedPlayers[4], sf::Clock* playerTimer[4], MovingPlatform* movingPlatform, MovingPlatform* movingPlatform2, EventManager* eventManager) {
        this->i = i;
        this->mutex = mutex;
        this->socket = socket;
        this->players = players;
        this->connectedPlayers = connectedPlayers;
        this->playerTimer = playerTimer;
        this->movingPlatform = movingPlatform;
        this->movingPlatform2 = movingPlatform2;
        this->eventManager = eventManager;

        
    }

    void run() {
        // actual thread code
        try {
            while (true) {
                int rec = 0;
                while (rec != -1) {
                    char message[1024] = "\0";
                    mutex->lock();
                    rec = zmq_recv(*socket, message, 1024, ZMQ_DONTWAIT);
                    mutex->unlock();
                    if (rec != -1) {
                        if (strcmp(message, "Join") == 0) {
                            zmq::message_t reply(2);
                            string sid;
                            
                            for (int i = 0; i < 4; i++) { // check which player slot is open
                                if (*connectedPlayers[i] == false) {
                                    *connectedPlayers[i] = true;
                                    playerTimer[i]->restart();
                                    *players[i] = Character(25, 50, 50, 300, 6);
                                    sid = to_string(i);
                                    break;
                                }
                            }

                            const char* id = sid.c_str();
                            memcpy(reply.data(), id, 2);
                            cout << "Client connected with id: " << (char*)reply.data() << endl;
                            mutex->lock();
                            socket->send(reply, zmq::send_flags::none);
                            mutex->unlock();
                        }

                        else if (message[0] == 'E') { // event
                            
                            zmq::message_t reply(2);
                            memcpy(reply.data(), "s", 2);

                            int clientId = message[1] - 48;
                            
                            mutex->lock();
                            eventManager->ReceiveEvent(eventManager->EVENT_TYPE_DEATH, clientId);
                            mutex->unlock();

                            mutex->lock();
                            socket->send(reply, zmq::send_flags::none);
                            mutex->unlock();
                        }

                        else {
                            int mx = round(movingPlatform->getPosition().x);
                            int my = round(movingPlatform->getPosition().y);
                            int mx2 = round(movingPlatform2->getPosition().x);
                            int my2 = round(movingPlatform2->getPosition().y);
                            string sreport = to_string(mx) + "." + to_string(my) + "-" + to_string(mx2) + "," + to_string(my2) + "/";

                            if (message[0] == '0') { // otherPlayer #0
                                playerTimer[0]->restart();

                                string mes = message;
                                string x = mes.substr(2, mes.find(".") - 2);
                                string y = mes.substr(mes.find(".") + 1, mes.length());
                                players[0]->setPosition(sf::Vector2f(stoi(x), stoi(y)));

                                for (int i = 0; i < 4; i++) { // check which players are connected
                                    if (i != 0 && *connectedPlayers[i]) { // if this player is connected
                                        int x = round(players[i]->getPosition().x);
                                        int y = round(players[i]->getPosition().y);
                                        sreport += to_string(i) + ":" + to_string(x) + "," + to_string(y) + "/";
                                    }
                                }

                            }
                            if (message[0] == '1') { // otherPlayer #0
                                playerTimer[1]->restart();

                                string mes = message;
                                string x = mes.substr(2, mes.find(".") - 2);
                                string y = mes.substr(mes.find(".") + 1, mes.length());
                                players[1]->setPosition(sf::Vector2f(stoi(x), stoi(y)));

                                for (int i = 0; i < 4; i++) { // check which players are connected
                                    if (i != 1 && *connectedPlayers[i]) { // if this player is connected
                                        int x = round(players[i]->getPosition().x);
                                        int y = round(players[i]->getPosition().y);
                                        sreport += to_string(i) + ":" + to_string(x) + "," + to_string(y) + "/";
                                    }
                                }
                            }
                            if (message[0] == '2') { // otherPlayer #0
                                playerTimer[2]->restart();

                                string mes = message;
                                string x = mes.substr(2, mes.find(".") - 2);
                                string y = mes.substr(mes.find(".") + 1, mes.length());
                                players[2]->setPosition(sf::Vector2f(stoi(x), stoi(y)));

                                for (int i = 0; i < 4; i++) { // check which players are connected
                                    if (i != 2 && *connectedPlayers[i]) { // if this player is connected
                                        int x = round(players[i]->getPosition().x);
                                        int y = round(players[i]->getPosition().y);
                                        sreport += to_string(i) + ":" + to_string(x) + "," + to_string(y) + "/";
                                    }
                                }
                            }
                            if (message[0] == '3') { // otherPlayer #0
                                playerTimer[3]->restart();

                                string mes = message;
                                string x = mes.substr(2, mes.find(".") - 2);
                                string y = mes.substr(mes.find(".") + 1, mes.length());
                                players[3]->setPosition(sf::Vector2f(stoi(x), stoi(y)));

                                for (int i = 0; i < 4; i++) { // check which players are connected
                                    if (i != 3 && *connectedPlayers[i]) { // if this player is connected
                                        int x = round(players[i]->getPosition().x);
                                        int y = round(players[i]->getPosition().y);
                                        sreport += to_string(i) + ":" + to_string(x) + "," + to_string(y) + "/";
                                    }
                                }
                            }

                            const char* report = sreport.c_str();
                            zmq::message_t reply(1024);
                            memcpy(reply.data(), report, 1024);
                            mutex->lock();
                            socket->send(reply, zmq::send_flags::none);
                            mutex->unlock();

                        }
                    }
                }
            }

        }
        catch (...) {
            std::cerr << "Thread " << i << " caught exception." << std::endl;
        }
    }

};

void run_thread(ClientCommunication* c)
{
    c->run();
}


int main() {

    //  Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:5555");

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

    
    //Create four generic players
    Character p1 = Character(25, 50, 50, 300, 6);
    Character p2 = Character(25, 50, 50, 300, 6);
    Character p3 = Character(25, 50, 50, 300, 6);
    Character p4 = Character(25, 50, 50, 300, 6);
    Character* players[4] = { &p1, &p2, &p3, &p4 };
    bool cp1 = false;
    bool cp2 = false;
    bool cp3 = false;
    bool cp4 = false;
    bool* connectedPlayers[4] = { &cp1, &cp2, &cp3, &cp4 };
    sf::Clock tp1 = sf::Clock();
    sf::Clock tp2 = sf::Clock();
    sf::Clock tp3 = sf::Clock();
    sf::Clock tp4 = sf::Clock();
    sf::Clock* playerTimer[4] = { &tp1, &tp2, &tp3, &tp4 };
    float disconnectTime = 2.0f;

    std::mutex mutex;

    // register events
    eventManager.RegisterDeathEvent(0, 5, players[0]);
    eventManager.RegisterDeathEvent(1, 5, players[1]);
    eventManager.RegisterDeathEvent(2, 5, players[2]);
    eventManager.RegisterDeathEvent(3, 5, players[3]);

    ClientCommunication clientCommunication(0, &mutex, &socket, players, connectedPlayers, playerTimer, &movingPlatform, &movingPlatform2, &eventManager);
    std::thread thread0 = std::thread(run_thread, &clientCommunication);
    ClientCommunication clientCommunication2(1, &mutex, &socket, players, connectedPlayers, playerTimer, &movingPlatform, &movingPlatform2, &eventManager);
    std::thread thread1 = std::thread(run_thread, &clientCommunication2);
    ClientCommunication clientCommunication3(2, &mutex, &socket, players, connectedPlayers, playerTimer, &movingPlatform, &movingPlatform2, &eventManager);
    std::thread thread2 = std::thread(run_thread, &clientCommunication3);
    ClientCommunication clientCommunication4(3, &mutex, &socket, players, connectedPlayers, playerTimer, &movingPlatform, &movingPlatform2, &eventManager);
    std::thread thread3 = std::thread(run_thread, &clientCommunication4);

    //register events

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

        movingPlatform.frameMove(deltaTime);
        movingPlatform2.frameMove(deltaTime);

        //mutex.lock();
        eventManager.DispatchEvents(deltaTime);
        //mutex.unlock();

        window.draw(platform1);
        window.draw(platform2);
        window.draw(platform3);
        window.draw(wall);
        window.draw(wall2);
        window.draw(ceiling);
        window.draw(movingPlatform);
        window.draw(movingPlatform2);

        for (int i = 0; i < 4; i++) { // check which player slots are connected
            if (playerTimer[i]->getElapsedTime().asSeconds() > disconnectTime && *connectedPlayers[i]) {
                cout << "Client disconnected: " << i << endl;
                *connectedPlayers[i] = false;
            }

            if (*connectedPlayers[i] == true) {
                window.draw(*players[i]);
            }
        }
        
        //std::cout << player.getPosition().x << " " << player.getPosition().y << endl;
        // end the current frame
        window.display();
    }

    thread0.join();
    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}