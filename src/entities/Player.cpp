#include "Player.h"

Player::Player() {
    // Initialize player to default values matching main.cpp
    pos = { 100.0f, 300.0f };
    velocity = { 0.0f, 0.0f };
    isGrounded = false;
    radius = 15.0f;
    
    spriteWidth = 80.0f;
    spriteHeight = 96.0f;
    facing = 1.0f; // 1.0f = facing right, -1.0f = facing left
}

void Player::UpdateTimers(float deltaTime) {
    // Update all active power-up timers by decreasing them with delta time
    if (skatesTimer > 0) skatesTimer -= deltaTime;
    if (droneTimer > 0) droneTimer -= deltaTime;
    if (catDebuffTimer > 0) catDebuffTimer -= deltaTime;
}

// ActivateSkates: Enables the skate power-up for the given duration
// This increases the player's movement speed by 25% while active
void Player::ActivateSkates(float duration) {
    skatesTimer = duration; // Set the skate power-up timer to the specified duration
}

// ActivateDrone: Enables the drone power-up, launching the player into the air
// The player is positioned at 1/4 of the screen from the bottom (3/4 height from ground)
void Player::ActivateDrone(float duration) {
    droneTimer = duration; // Set the drone flight timer to the specified duration
    // Position the player at 1/4 of the screen height from the bottom
    pos.y = groundY - radius - (groundY * 0.6f);
    isGrounded = false; // Set to airborne state
    velocity.y = 0.0f;  // Reset vertical velocity
}

// HandleInput: Processes keyboard input for player movement
// Reads arrow keys for horizontal movement and spacebar for jumping
// Applies speed modifiers from active power-ups and debuffs
void Player::HandleInput(float moveSpeed, float jumpForce) {
    velocity.x = 0.0f;

    float currentSpeed = baseMoveSpeed;
    if (skatesTimer > 0) currentSpeed *= 1.25f; // Skate power-up: 25% speed boost
    if (catDebuffTimer > 0) currentSpeed *= 0.75f; // Cat debuff: 25% speed reduction
    
    if (IsKeyDown(KEY_LEFT)) {
        velocity.x = -currentSpeed;
        facing = -1.0f;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        velocity.x = currentSpeed;
        facing = 1.0f;
    }

    // Jump control: Trigger jump when spacebar is pressed and player is grounded
    if (IsKeyPressed(KEY_SPACE) && isGrounded) {
        velocity.y = jumpForce;
        isGrounded = false;
    }
}

// UpdatePhysics: Updates player position, velocity, and gravity
// Handles drone flight state, ground collision, and physics simulation
void Player::UpdatePhysics(float deltaTime, float gravity, float groundY) {
    // Store groundY for ActivateDrone reference
    this->groundY = groundY;
    
    // Drone flight state: immune to gravity while drone timer is active
    if (droneTimer > 0) {
        // While flying: no gravity effect, maintain air position
        velocity.y = 0.0f;
        isGrounded = false;
    } else {
        // Apply normal gravity when not flying
        if (!isGrounded) {
            velocity.y += gravity * deltaTime;
        } else {
            velocity.y = 0.0f;
        }
    }

    // Update vertical position based on velocity
    pos.y += velocity.y * deltaTime;
    
    // Drone timer expired: player returns to ground
    if (droneTimer <= 0) {
        // Normal ground collision check
        if (pos.y + radius >= groundY) {
            pos.y = groundY - radius;
            isGrounded = true;
            velocity.y = 0.0f;
        }
    }
}

// Draw: Renders the player sprite with proper mirroring for facing direction
void Player::Draw() {
    // Source rectangle: auto-handles left/right mirroring by multiplying with facing direction
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * facing, (float)texture.height };
    
    // Perspective offset: shifts the sprite downward for visual depth (40.0f pixels)
    float perspectiveOffsetY = 40.0f;

    // Destination rectangle: Y-axis adjustment ensures the player's feet stay on the ground line
    Rectangle destRec = { pos.x - spriteWidth / 2.0f, pos.y  - spriteHeight + radius + perspectiveOffsetY, spriteWidth, spriteHeight };
    Vector2 origin = { 0.0f, 0.0f };
    
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
}