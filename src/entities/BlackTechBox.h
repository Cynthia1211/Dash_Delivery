#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class BlackTechBox : public GameObject {
public:
    Texture2D texture; 

    // Creates black tech box 
    BlackTechBox(float x, float y, Texture2D tex) 
        : GameObject(x, y, 40.0f, 40.0f, ObjectType::BLACK_TECH_BOX) {
        texture = tex;
    }

    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        if (texture.id > 0) {
            DrawTexturePro(texture, 
                Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
                Rectangle{ screenX, screenY, width, height }, Vector2{ 0, 0 }, 0.0f, WHITE);
        } else {
            DrawRectangle(screenX, screenY, width, height, RED);
            DrawText("BLACK-TECH", screenX - 5, screenY - 15, 10, BLUE);
        }
    }

    // Handles collision with the player
    // Buff: ignore next debuff
    void OnCollision(Player& player) override {
        player.shieldActive = true;
        isAlive = false; 
    }
};
