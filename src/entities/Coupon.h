// Coupon.h - Coupon Item: Collectible that instantly increases the level's remaining time
#pragma once
#include "GameObject.h"
#include "../levelmanager.h"

class Coupon : public GameObject {
public:
    Texture2D texture;  // Texture for rendering the coupon image
    float timeBonus;    // Time bonus in seconds added to the level timer

    // Constructor: Creates a coupon item at the specified position
    // Parameters:
    //   x, y: World coordinates for the coupon position
    //   tex: Texture2D for rendering the coupon sprite
    //   lm: Reference to LevelManager for time bonus application
    //   bonus: Time bonus in seconds (default: 15.0f seconds)
    Coupon(float x, float y, Texture2D tex, LevelManager& lm, float bonus = 15.0f) 
        : GameObject(x, y, 30.0f, 30.0f, ObjectType::COUPON, GREEN) {
        texture = tex;
        timeBonus = bonus;
        levelManager = &lm;
    }

    // Draw: Renders the coupon at the vertical middle of the screen
    // Parameters:
    //   worldScrollOffset: Offset for scrolling effect
    //   groundY: Ground Y coordinate (fallback position)
    //   screenHeight: Screen height for vertical positioning
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        // Render coupon sprite if texture is valid; otherwise render fallback green rectangle with text
        if (texture.id > 0) {
            DrawTexturePro(
                texture,
                Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
                Rectangle{ screenX, screenY, width, height },
                Vector2{ 0, 0 },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangle(screenX, screenY, width, height, color);
            DrawText("COUPON", screenX, screenY - 15, 10, GREEN);
        }
    }

    // OnCollision: Handles collision with the player
    // Effect: Immediately adds the time bonus to the LevelManager and deactivates the coupon
    void OnCollision(Player& player) override {
        if (levelManager) {
            levelManager->AddCountdownTime(timeBonus); // Instantly increase the countdown timer
        }
        isAlive = false; // Deactivate the item so it is no longer visible or collidable
    }

private:
    LevelManager* levelManager = nullptr; // Pointer to LevelManager for time bonus updates
};
