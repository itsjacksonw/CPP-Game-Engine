#pragma once
#include "GameObject.h"
#include "SpawnPoint.h"
#include "DeathZone.h"
#include "SideBoundary.h"
#include <iostream>

class Character : public GameObject {

private:
float speed;
float jumpSpeed = 20;

float gravityAcceleration = -2;
float maxFallSpeed = -16;


bool checkCollisions(GameObject* platforms[], int numPlatforms); // returns true if the player hits the ground
void checkSpawns(SpawnPoint* spawns[], int numSpawns);
void checkDeaths(DeathZone* deaths[], int numDeaths);




public:



Character(float width, float height, float x, float y, float speed); // creates a character
void walk(sf::Vector2f direction, float deltaTime);
void jump();
void processFrame(GameObject* platforms[], int numPlatforms,
    SpawnPoint* spawns[], int numSpawns, DeathZone* deaths[], int numDeaths, SideBoundary* scrolls[], int numScrolls, float deltaTime);
SideBoundary checkScrolls(SideBoundary* scrolls[], int numScrolls);
void setVelocity(sf::Vector2f velocity);
sf::Vector2f getSpawn();
void gravity(float deltaTime);
void moveY(float deltaTime);
sf::Vector2f spawn;
int lastScroll = -1;
float pixelLeniency = 4; // the amount of pixels a player can miss a jump and still make it
bool isGrounded = false;
float yVelocity = 0;
};