#pragma once
#include "GameObject.h"
#include "../levelmanager.h"

class Coupon : public GameObject {
public:
    Texture2D texture;  
    float timeBonus; 

    // Creates a coupon
    Coupon(float x, float y, Texture2D tex, LevelManager& lm, float bonus = 15.0f) 
        : GameObject(x, y, 30.0f, 30.0f, ObjectType::COUPON) {
        texture = tex;
        timeBonus = bonus;
        levelManager = &lm;
    }

    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        if (texture.id > 0) {
            DrawTexturePro(
                texture,
                Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
                Rectangle{ screenX, screenY, width, height }, Vector2{ 0, 0 }, 0.0f, WHITE );
        } else {
            DrawRectangle(screenX, screenY, width, height, GREEN);
            DrawText("COUPON", screenX, screenY - 15, 10, GREEN);
        }
    }

    // Handles collision with the player
    // Buff: extra time
    void OnCollision(Player& player) override {
        if (levelManager) {
            levelManager->AddCountdownTime(timeBonus); 
        }
        isAlive = false; 
    }

private:
    LevelManager* levelManager = nullptr; 
    
};
