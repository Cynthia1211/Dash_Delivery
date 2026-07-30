#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class Skates : public GameObject {
public:
    Texture2D texture; 

    // Create skates
    Skates(float x, float y, Texture2D tex) 
        : GameObject(x, y, 30.0f, 30.0f, ObjectType::SKATES) {
        texture = tex;
    }

    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        if (texture.id > 0) {
            DrawTexturePro(
                texture,
                Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
                Rectangle{ screenX, screenY, width, height }, Vector2{ 0, 0 }, 0.0f, WHITE);
        } else {
            DrawRectangle(screenX, screenY, width, height, GOLD);
            DrawText("SKATES", screenX, screenY - 15, 10, GOLD);
        }
    }
 
    // Handles collision with the player
    // Buff: Speed up 5s
    void OnCollision(Player& player) override {
        player.ActivateSkates(5.0f); 
        isAlive = false;        
    }
};