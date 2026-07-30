#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class StreetGangster : public GameObject {
public:
    Texture2D texture;  

    // Creates a street gangster enemy
    StreetGangster(float x, float y, Texture2D tex) 
        : GameObject(x, y, 40.0f, 60.0f, ObjectType::GANGSTER) {
        texture = tex;
    }

    // Renders the street gangster at ground level
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
            DrawText("STREET GANG", screenX + 2, screenY + 10, 12, WHITE);
        }
    }

    // Handles collision with the player
    // If player has active shield, consume it. Otherwise, confiscate the delivery
    void OnCollision(Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false;
        } else {

            player.foodStatus = 0.0f; 
        }
        isAlive = false;
    }
};