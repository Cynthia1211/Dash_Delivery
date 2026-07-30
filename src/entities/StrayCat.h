// StrayCat.h - Stray cat enemy that reduces player speed and food status
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class StrayCat : public GameObject {
public:
    Texture2D texture; // Reference to the cat image texture

    // Constructor: creates stray cat at given position
    // x: X position, y: Y position in world coordinates (ground level = groundY)
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

    void OnCollision(Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false; // Shield absorbs the impact
        } else {
            player.foodStatus -= 15.0f;
            player.catDebuffTimer = 5.0f;
        }
        isAlive = false;
    }
};