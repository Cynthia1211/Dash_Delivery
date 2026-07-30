#include "Player.h"

Player::Player() {

    // Initialize player to default values
    pos = { 100.0f, 300.0f };
    velocity = { 0.0f, 0.0f };
    isGrounded = false;
    radius = 15.0f;
    
    spriteWidth = 80.0f;
    spriteHeight = 96.0f;
    facing = 1.0f; // 1.0f = facing right, -1.0f = facing left
}

// Update active power-up timers by decreasing them with delta time
void Player::UpdateTimers(float deltaTime) {
    if (skatesTimer > 0) skatesTimer -= deltaTime;
    if (droneTimer > 0) droneTimer -= deltaTime;
    if (catDebuffTimer > 0) catDebuffTimer -= deltaTime;
}

// Enables the skate power-up for the given duration
void Player::ActivateSkates(float duration) {
    skatesTimer = duration; 
}

// Enables the drone power-up for the given duration
void Player::ActivateDrone(float duration) {
    droneTimer = duration;

    // Put the player into the air
    pos.y = groundY - radius - (groundY * 0.6f);
    isGrounded = false; 
    velocity.y = 0.0f; 
}

// Processes keyboard input for player movement
// Arrow keys for movement and spacebar for jumping
void Player::HandleInput(float moveSpeed, float jumpForce) {
    velocity.x = 0.0f;

    float currentSpeed = baseMoveSpeed;

    // Skate power-up: 25% speed boost
    if (skatesTimer > 0) currentSpeed *= 1.25f; 

    // Cat debuff: 25% speed reduction
    if (catDebuffTimer > 0) currentSpeed *= 0.75f; 
    
    if (IsKeyDown(KEY_LEFT)) {
        velocity.x = -currentSpeed;
        facing = -1.0f;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        velocity.x = currentSpeed;
        facing = 1.0f;
    }

    if (IsKeyPressed(KEY_SPACE) && isGrounded) {
        velocity.y = jumpForce;
        isGrounded = false;
    }
}

// Updates player position, velocity, and gravity
void Player::UpdatePhysics(float deltaTime, float gravity, float groundY) {

    this->groundY = groundY;
    
    // Drone flight state
    if (droneTimer > 0) {

        velocity.y = 0.0f;
        isGrounded = false;
    } else {

        if (!isGrounded) {
            velocity.y += gravity * deltaTime;
        } else {
            velocity.y = 0.0f;
        }
    }

    pos.y += velocity.y * deltaTime;
    
    // Non Drone status
    if (droneTimer <= 0) {

        if (pos.y + radius >= groundY) {
            pos.y = groundY - radius;
            isGrounded = true;
            velocity.y = 0.0f;
        }
    }
}

// Renders the player with facing direction
void Player::Draw() {
    
    // Define the source rectangle from the texture
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * facing, (float)texture.height };
    
    // Put player in the ground
    float perspectiveOffsetY = 40.0f;
    Rectangle destRec = { pos.x - spriteWidth / 2.0f, pos.y  - spriteHeight + radius + perspectiveOffsetY, spriteWidth, spriteHeight };
    Vector2 origin = { 0.0f, 0.0f };
    
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
}