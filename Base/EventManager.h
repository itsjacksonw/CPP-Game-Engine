#pragma once
#include <vector>
#include <string>
#include "Character.h"
#include "Timeline.h"

class EventManager {

    // character spawn = 1
    // character death = 2
    // user input = 3
    // character collision = 4
public:
    
    const static int EVENT_TYPE_SPAWN = 1;
    const static int EVENT_TYPE_DEATH = 2;
    const static int EVENT_TYPE_INPUT = 3;
    const static int EVENT_TYPE_COLLISION = 4;

    class Event {
    private:
        int id = -1;
        int type = -1;
        int priority = -1;
    public:

        Event();
        Event(int type, int id, int priority, EventManager* manager);
        int getId();
        int getPriority();
        void setPriority(int p);
        int getType();
        EventManager* manager;
    };


    class SpawnEvent : public Event {
    private:
        Character* player;
    public:
        SpawnEvent();
        SpawnEvent(int id, int priority, Character* player, EventManager* manager);
        void Spawn();
    };

    class DeathEvent : public Event {
    private:
        Character* player;
    public:
        DeathEvent();
        DeathEvent(int id, int priority, Character* player, EventManager* manager);
        void Death();
    };

    class InputEvent : public Event {
    private:
        Character* player;
        Timeline* timeline;
        
    public:
        InputEvent();
        InputEvent(int id, int priority, std::string button, Character* player, EventManager* manager, Timeline* timeline);
        void Input(long deltaTime);
        std::string button;
    };

    class CollisionEvent : public Event {
    private:
        Character* player;
        GameObject** platforms;
        int numPlatforms;
        SpawnPoint** spawns;
        int numSpawns;
        DeathZone** deaths;
        int numDeaths;
        SideBoundary** scrolls;
        int numScrolls;

        

        bool checkPlatformCollisions(GameObject* platforms[], int numPlatforms); // returns true if the player hits the ground
        void checkSpawns(SpawnPoint* spawns[], int numSpawns);
        void checkDeaths(DeathZone* deaths[], int numDeaths);
        SideBoundary checkScrolls(SideBoundary* scrolls[], int numScrolls);

    public:
        CollisionEvent();
        CollisionEvent(int id, int priority, Character* player, GameObject* platforms[], int numPlatforms,
            SpawnPoint* spawns[], int numSpawns, DeathZone* deaths[], int numDeaths,
            SideBoundary* scrolls[], int numScrolls, Timeline* timeline, EventManager* manager);
        void CheckCollisions();

        Timeline* timeline;
        long previousTime = 0;
    };


    EventManager(Timeline* timeline);
    //registration
    void RegisterSpawnEvent(int id, int priority, Character* player);
    void RegisterDeathEvent(int id, int priority, Character* player);
    void RegisterInputEvent(int id, int priority, std::string button, Character* player);
    void RegisterCollisionEvent(int id, int priority, Character* player, GameObject* platforms[], int numPlatforms,
        SpawnPoint* spawns[], int numSpawns, DeathZone* deaths[], int numDeaths,
        SideBoundary* scrolls[], int numScrolls);

    void ReceiveEvent(int type, int id); //raising
    void DispatchEvents(long deltaTime); //handling
    
    Timeline* timeline;
private:
    

    SpawnEvent spawnEvents[5];
    int numSpawnEvents = 0;
    std::vector<SpawnEvent> spawnQueued;

    DeathEvent deathEvents[5];
    int numDeathEvents = 0;
    std::vector<DeathEvent> deathQueued;

    InputEvent inputEvents[10];
    int numInputEvents = 0;
    std::vector<InputEvent> inputQueued;

    CollisionEvent collisionEvents[5];
    int numCollisionEvents = 0;
    std::vector<CollisionEvent> collisionQueued;

    
};
