#include "EventManager.h"

EventManager::EventManager(Timeline* timeline, zmq::socket_t* socket, char* id) {
    this->timeline = timeline;
    this->socket = socket;
    this->clientId = id;
}

EventManager::Event::Event() {}
EventManager::Event::Event(int type, int id, int priority, EventManager* manager) {
    this->type = type;
    this->id = id;
    this->priority = priority;
    this->manager = manager;
    this->socket = NULL;
    this->clientId = NULL;

}
EventManager::Event::Event(int type, int id, int priority, EventManager* manager, zmq::socket_t* socket, char* clientId) {
    this->type = type;
    this->id = id;
    this->priority = priority;
    this->manager = manager;
    this->socket = socket;
    this->clientId = clientId;
    
}

int EventManager::Event::getId() {
    return this->id;
}
int EventManager::Event::getPriority() {
    return this->priority;
}
void EventManager::Event::setPriority(int p)
{
    this->priority = p;
}
int EventManager::Event::getType() {
    return this->type;
}

EventManager::SpawnEvent::SpawnEvent() {}
EventManager::SpawnEvent::SpawnEvent(int id, int priority, Character* player, EventManager* manager)
    : Event(EVENT_TYPE_SPAWN, id, priority, manager) {
    
    this->player = player;
}

void EventManager::SpawnEvent::Spawn() {
    std::cout << "Respawning..." << std::endl;
    player->setPosition(player->getSpawn());
    player->setVelocity(sf::Vector2f(0,0));
    player->alive = true;
    return;
}


void EventManager::RegisterSpawnEvent(int id, int priority, Character* player) {

    SpawnEvent event = SpawnEvent(id, priority, player, this);
    spawnEvents[numSpawnEvents] = event;
    numSpawnEvents++;
}

EventManager::DeathEvent::DeathEvent() {}
EventManager::DeathEvent::DeathEvent(int id, int priority, Character* player, EventManager* manager, zmq::socket_t* socket, char* clientId)
    : Event(EVENT_TYPE_DEATH, id, priority, manager, socket, clientId) {

    this->player = player;
}

void EventManager::DeathEvent::Death() {
    std::cout << "test" << std::endl;
    if (!player->alive) {
        return;
    }
    player->alive = false;
    zmq::message_t start(6);
    std::string stringId = clientId;
    std::string mes = "E" + stringId;
    memcpy(start.data(), mes.c_str(), 6);

    socket->send(start, zmq::send_flags::none);
    zmq::message_t reply;
    socket->recv(reply, zmq::recv_flags::none);

    std::cout << "You died." << std::endl;
    char* response = (char*)reply.data();
    
    manager->ReceiveEvent(EVENT_TYPE_SPAWN, 0);
    return;
}

void EventManager::RegisterDeathEvent(int id, int priority, Character* player) {

    DeathEvent event = DeathEvent(id, priority, player, this, socket, clientId);
    deathEvents[numDeathEvents] = event;
    numDeathEvents++;
}

void EventManager::RegisterInputEvent(int id, int priority, std::string button, Character* player)
{
    InputEvent event = InputEvent(id, priority, button, player, this, timeline);
    inputEvents[numInputEvents] = event;
    
    numInputEvents++;
}

void EventManager::RegisterCollisionEvent(int id, int priority, Character* player, GameObject* platforms[], int numPlatforms, SpawnPoint* spawns[], int numSpawns, DeathZone* deaths[], int numDeaths, SideBoundary* scrolls[], int numScrolls)
{
    CollisionEvent event = CollisionEvent(id, priority, player, platforms, numPlatforms, spawns, numSpawns, deaths, numDeaths, scrolls, numScrolls, timeline, this);
    collisionEvents[numCollisionEvents] = event;

    
    numCollisionEvents++;
    
}

void EventManager::ReceiveEvent(int type, int id) {
    if (type == EVENT_TYPE_COLLISION) {
        
        for (int i = 0; i < numCollisionEvents; i++) {
            CollisionEvent e = collisionEvents[i];
            if (e.getId() == id) {
                e.setPriority(timeline->getTime() + e.getPriority());
                collisionQueued.push_back(e);
            }
        }
    }

    if (type == EVENT_TYPE_SPAWN) {
        for (int i = 0; i < numSpawnEvents; i++) {
            SpawnEvent e = spawnEvents[i];
            if (e.getId() == id) {
                e.setPriority(timeline->getTime() + e.getPriority());
                spawnQueued.push_back(e);
            }
        }
    }

    if (type == EVENT_TYPE_INPUT) {
        for (int i = 0; i < numInputEvents; i++) {
            InputEvent e = inputEvents[i];
            if (e.getId() == id) {
                e.setPriority(timeline->getTime() + e.getPriority());
                inputQueued.push_back(e);
            }
        }
    }

    if (type == EVENT_TYPE_DEATH) {
        for (int i = 0; i < numDeathEvents; i++) {
            DeathEvent e = deathEvents[i];
            if (e.getId() == id) {
                e.setPriority(timeline->getTime() + e.getPriority());
                deathQueued.push_back(e);
            }
        }
    }
    
}

void EventManager::DispatchEvents(long deltaTime) {

    for (int i = 0; i < collisionQueued.size(); i++) {
        CollisionEvent e = collisionQueued[i];
        if (timeline->getTime() >= e.getPriority()) {
            e.CheckCollisions();
            collisionQueued.erase(collisionQueued.begin() + i); // get rid of it
            i--;
        }
    }

    for (int i = 0; i < spawnQueued.size(); i++) {
        SpawnEvent e = spawnQueued[i];
        if (timeline->getTime() >= e.getPriority()) {
            
            deathQueued.clear();
            e.Spawn();
            spawnQueued.erase(spawnQueued.begin() + i); // get rid of it
            i--;
        }
    }

    for (int i = 0; i < deathQueued.size(); i++) {
        DeathEvent e = deathQueued[i];
        if (timeline->getTime() >= e.getPriority()) {
            e.Death();
            deathQueued.erase(deathQueued.begin() + i); // get rid of it
            i--;
        }
    }

    
    for (int i = 0; i < inputQueued.size(); i++) {
        
        InputEvent e = inputQueued[i];
        if (timeline->getTime() >= e.getPriority()) {
            e.Input(deltaTime);
            inputQueued.erase(inputQueued.begin() + i); // get rid of it
            i--;
        }
    }

}

EventManager::CollisionEvent::CollisionEvent(){}
EventManager::CollisionEvent::CollisionEvent(int id, int priority, Character* player, GameObject* platforms[],
    int numPlatforms, SpawnPoint* spawns[], int numSpawns, DeathZone* deaths[],
    int numDeaths, SideBoundary* scrolls[], int numScrolls, Timeline* timeline, EventManager* manager)
    : Event(EVENT_TYPE_COLLISION, id, priority, manager)
{
    this->player = player;
    this->platforms = platforms;
    this->numPlatforms = numPlatforms;
    this->spawns = spawns;
    this->numSpawns = numSpawns;
    this->deaths = deaths;
    this->numDeaths = numDeaths;
    this->scrolls = scrolls;
    this->numScrolls = numScrolls;

    this->timeline = timeline;
}

void EventManager::CollisionEvent::CheckCollisions()
{
    

    if (!checkPlatformCollisions(platforms, numPlatforms)) { // check collisions, if not on ground, run y velocity
        
    }
    //check for new spawnpoint
    checkSpawns(spawns, numSpawns);
    //check for deathzone
    checkDeaths(deaths, numDeaths);
    //check for screen boundaries
    SideBoundary boundary = checkScrolls(scrolls, numScrolls);
    if (boundary.getId() != -1) {

       

        boundary.moveScreen(platforms, numPlatforms);
        boundary.moveScreen((GameObject**)spawns, numSpawns);
        boundary.moveScreen((GameObject**)deaths, numDeaths);
        boundary.moveScreen((GameObject**)scrolls, numScrolls);

        player->move(sf::Vector2f(boundary.direction, 0) * boundary.distance);
        player->spawn += sf::Vector2f(boundary.direction, 0) * boundary.distance;
        player->room++;

    }
}

bool EventManager::CollisionEvent::checkPlatformCollisions(GameObject* platforms[], int numPlatforms) {

    for (int i = 0; i < numPlatforms; i++) {

        sf::FloatRect bounds = player->getGlobalBounds();
        sf::FloatRect platformBounds = (*platforms[i]).getGlobalBounds();

        if (bounds.intersects(platformBounds)) { // collision

            if (bounds.top + bounds.height > platformBounds.top // if character is colliding with the top of an object (ground)
                && bounds.top < platformBounds.top
                && bounds.top + bounds.height <= platformBounds.top + player->pixelLeniency) { // only if the player is very close to the surface, otherwise they are on a wall
                player->setPosition(player->getPosition().x, platformBounds.top - bounds.height);
                player->yVelocity = 0;
                player->isGrounded = true;
                return true;
            }

            else if (bounds.top < platformBounds.top + platformBounds.height // if the character is colliding the the bottom of an object
                && bounds.top > platformBounds.top
                && bounds.top + player->pixelLeniency >= platformBounds.top + platformBounds.height) {
                player->setPosition(player->getPosition().x, platformBounds.top + platformBounds.height + player->pixelLeniency);
                if (player->yVelocity > 0) {
                    player->yVelocity = 0;
                }
                player->isGrounded = false;
            }

            else if (bounds.left < platformBounds.left + platformBounds.width // if the character is running into the left wall
                && bounds.left + bounds.width > platformBounds.left + platformBounds.width) {
                player->setPosition(platformBounds.left + platformBounds.width, player->getPosition().y);
                player->isGrounded = false;
            }

            else if (bounds.left + bounds.width > platformBounds.left // if the character runs into the right wall
                && bounds.left < platformBounds.left) {
                player->setPosition(platformBounds.left - bounds.width, player->getPosition().y);
                player->isGrounded = false;
            }

            else if (bounds.top + bounds.height > platformBounds.top
                && bounds.left > platformBounds.left
                && bounds.left + bounds.width < platformBounds.left + platformBounds.width) { // extra check incase lagging
                player->setPosition(player->getPosition().x, platformBounds.top - bounds.height);
                player->yVelocity = 0;
                player->isGrounded = true;
                return true;
            }
        }
    }
    player->isGrounded = false;
    return false;
}

void EventManager::CollisionEvent::checkSpawns(SpawnPoint* spawns[], int numSpawns) {
    for (int i = 0; i < numSpawns; i++) {
        sf::FloatRect bounds = player->getGlobalBounds();
        sf::FloatRect spawnBounds = (*spawns[i]).getGlobalBounds();

        if (bounds.intersects(spawnBounds)) { // collision
            player->spawn = (*spawns[i]).getSpawn();
            return;
        }
    }
}

void EventManager::CollisionEvent::checkDeaths(DeathZone* deaths[], int numDeaths) {
    for (int i = 0; i < numDeaths; i++) {
        sf::FloatRect bounds = player->getGlobalBounds();
        sf::FloatRect deathBounds = (*deaths[i]).getGlobalBounds();

        if (bounds.intersects(deathBounds)) { // collision
            //player->setPosition(player->spawn);
            //player->yVelocity = 0;
            manager->ReceiveEvent(EVENT_TYPE_DEATH, 1); // raise death event
            return;
        }
    }
}

SideBoundary EventManager::CollisionEvent::checkScrolls(SideBoundary* scrolls[], int numScrolls) {
    for (int i = 0; i < numScrolls; i++) {
        sf::FloatRect bounds = player->getGlobalBounds();
        sf::FloatRect scrollBounds = (*scrolls[i]).getGlobalBounds();

        if (bounds.intersects(scrollBounds)) { // collision

            if (player->lastScroll != (*scrolls[i]).getId()) {
                //std::cout << lastScroll << " " << (*scrolls[i]).getId() << std::endl;
                player->lastScroll = (*scrolls[i]).getId();
                return (*scrolls[i]);
            }
        }
    }
    return SideBoundary(0, 0, 0, 0, 0, 0, -1);
}


EventManager::InputEvent::InputEvent() {}
EventManager::InputEvent::InputEvent(int id, int priority, std::string button, Character* player, EventManager* manager, Timeline* timeline)
    : Event(EVENT_TYPE_INPUT, id, priority, manager) {
    this->button = button;
    this->player = player;
    this->timeline = timeline;
}

void EventManager::InputEvent::Input(long deltaTime) {
    
    if (button == "left") {
        player->walk(sf::Vector2f(-1, 0), deltaTime);
    }
    if (button == "right") {
        player->walk(sf::Vector2f(1, 0), deltaTime);
    }
    if (button == "up") {
        player->jump();
    }
    if (button == "pause") {
        timeline->togglePause();
    }
    if (button == "fastSpeed") {
        timeline->changeTic(10);
    }
    if (button == "normalSpeed") {
        timeline->changeTic(20);
    }
    if (button == "slowSpeed") {
        timeline->changeTic(40);
    }
    return;
}