// StreetGangster.h - Hostile gangster enemy that confiscates player delivery
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class StreetGangster : public GameObject {
public:
    Texture2D texture;  // Texture for rendering the street gangster sprite

    // Constructor: Creates a street gangster enemy at the specified world position
    // Parameters:
    //   x, y: World coordinates for the gangster position
    //   tex: Texture2D for rendering the gangster sprite
    StreetGangster(float x, float y, Texture2D tex) 
        : GameObject(x, y, 40.0f, 60.0f, ObjectType::GANGSTER) {
        texture = tex;
    }

    // Draw: Renders the street gangster sprite at ground level
    // Applies scrolling offset for world movement and perspective offset for visual depth
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;
        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f;
        float screenY = groundY - height + perspectiveOffsetY;

        if (texture.id > 0) {
            DrawTexturePro(texture, 
                Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
                Rectangle{ screenX, screenY, width, height }, Vector2{ 0, 0 }, 0.0f, WHITE);
        } else {
            DrawRectangle(screenX, screenY, width, height, RED);
            DrawText("GANG", screenX + 2, screenY + 10, 12, WHITE);
        }
    }

    // OnCollision: Handles collision with the player
    // If player has active shield, consume it. Otherwise, confiscate the delivery (set foodStatus to 0)
    void OnCollision(Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false; // Shield absorbs the attack
        } else {
            // Confiscate delivery: clear food status, triggering game over in main.cpp
            player.foodStatus = 0.0f; 
        }
        isAlive = false;
    }
};