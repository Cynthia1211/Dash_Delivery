#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class StrayCat : public GameObject {
public:
    Texture2D texture; 

    // Creates stray cat 
    StrayCat(float x, float y, Texture2D tex) 
        : GameObject(x, y, 40.0f, 40.0f, ObjectType::STRAY_CAT) {
        texture = tex;
    }

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
            DrawRectangle(screenX, screenY, width, height, YELLOW);
            DrawText("CAT", screenX + 5, screenY + 10, 12, BLACK);
        }
    }

    // Handles collision with the player
    // If player has active shield, consume it. 
    // Otherwise, decrease speed and food status
    void OnCollision(Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false;
        } else {
            player.foodStatus -= 15.0f;
            player.catDebuffTimer = 5.0f;
        }
        isAlive = false;
    }
};