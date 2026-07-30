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

    float baseMoveSpeed = 300.0f;    // Base movement speed in pixels per second
    float foodStatus = 100.0f;       // Food integrity percentage (100 = full delivery)
    float groundY = 350.0f;          // Ground Y coordinate (used for drone power-up positioning)

    float skatesTimer = 0.0f;
    float droneTimer = 0.0f;
    float catDebuffTimer = 0.0f;
    bool shieldActive = false;       // Shield status from BlackTechBox power-up

    // Constructor: Initializes Player with default values
    Player();

    // HandleInput: Processes player keyboard input for movement and jumping
    void HandleInput(float moveSpeed, float jumpForce);

    // UpdatePhysics: Handles physics simulation including velocity, gravity, and ground collision
    void UpdatePhysics(float deltaTime, float gravity, float groundY);

    // Draw: Renders the delivery player sprite with direction-based mirroring
    void Draw();    
    
    // ActivateSkates (legacy): Sets skate timer with speed boost parameter
    void ActivateSkates(float duration, float speedBoost)
    {
        skatesTimer = duration;
    }

    // UpdateTimers: Decrements all active power-up timers each frame
    void UpdateTimers(float deltaTime);
    // ActivateSkates: Triggered when player collects skate power-up
    void ActivateSkates(float duration);
    // ActivateDrone: Triggered when player collects drone power-up
    void ActivateDrone(float duration);
};