#pragma once
#include "raylib.h"

class Player
{
public:
    Vector2 pos;
    Vector2 velocity;
    bool isGrounded;
    float radius;

    float spriteWidth;
    float spriteHeight;
    float facing;
    Texture2D texture;

    float baseMoveSpeed = 300.0f; 
    float foodStatus = 100.0f; 
    float groundY = 350.0f; 

    float skatesTimer = 0.0f;
    float droneTimer = 0.0f;
    float catDebuffTimer = 0.0f;
    bool shieldActive = false; 

    // Initializes Player with default values
    Player();

    // Keyboard input for movement and jumping
    void HandleInput(float moveSpeed, float jumpForce);

    // Physics simulation including velocity, gravity, and ground collision
    void UpdatePhysics(float deltaTime, float gravity, float groundY);

    // Renders the delivery player sprite with direction-based mirroring
    void Draw();    

    // Decrements all active power-up timers
    void UpdateTimers(float deltaTime);

    // Triggered when player collects skate power-up
    void ActivateSkates(float duration);

    // Triggered when player collects drone power-up
    void ActivateDrone(float duration);

};