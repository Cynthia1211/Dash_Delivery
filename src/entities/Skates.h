// Skates.h - Speed boost power-up item that increases player movement speed
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class Skates : public GameObject {
public:
    Texture2D texture; // Reference to the skates image texture

    // Constructor: accepts Texture2D for custom rendering
    // x: X position, y: Y position in world coordinates (airborne items use GetVerticalMiddleY)
    Skates(float x, float y, Texture2D tex) 
        : GameObject(x, y, 30.0f, 30.0f, ObjectType::SKATES) {
        texture = tex;
    }

    // Override Draw: renders skates texture at screen vertical middle position
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        // Render texture if valid, otherwise render gold rectangle with text as fallback
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
            DrawRectangle(screenX, screenY, width, height, GOLD);
            DrawText("SKATES", screenX, screenY - 15, 10, GOLD);
        }
    }
 
    void OnCollision(Player& player) override {
        player.ActivateSkates(5.0f); // Activate player speed boost for 5 seconds
        isAlive = false;            // Item collected, remove from scene and disable collision
    }
};