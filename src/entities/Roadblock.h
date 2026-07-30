// Roadblock.h - Static roadblock obstacle that damages player
#pragma once
#include "GameObject.h"
#include "../entities/Player.h" // Player class defined in entities directory

class Roadblock : public GameObject {
public:
    Texture2D texture; // Reference to the roadblock image texture

    // Constructor: accepts Texture2D for custom rendering
    // x: X position, y: Y position in world coordinates (ground level = groundY)
    Roadblock(float x, float y, Texture2D tex) 
        : GameObject(x, y, 40.0f, 50.0f, ObjectType::ROADBLOCK) {
        texture = tex;
    }

    // Override Draw: renders roadblock texture with perspective offset
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f;
        float screenY = groundY - height + perspectiveOffsetY;

        // Render texture if valid, otherwise render red rectangle as fallback
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
            DrawRectangle(screenX, screenY, width, height, RED);
        }
    }

    void OnCollision(class Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false; // Shield absorbs the impact
        } else {
            player.foodStatus -= 10.0f; // Damage player food status on collision
        }
        isAlive = false; 
    }
};