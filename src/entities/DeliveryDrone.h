// DeliveryDrone.h - Delivery drone power-up that flies player to 1/3 height position for 5 seconds
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class DeliveryDrone : public GameObject {
public:
    Texture2D texture; // Reference to the drone image texture
    float duration;    // Flight duration in seconds

    // Constructor: creates delivery drone at given position
    // x: X position, y: Y position (airborne items use GetVerticalMiddleY)
    DeliveryDrone(float x, float y, Texture2D tex) 
        : GameObject(x, y, 30.0f, 30.0f, ObjectType::DRONE) {
        texture = tex;
        duration = 5.0f;
    }

    // Override Draw: renders drone texture at screen vertical middle position
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        // Render texture if valid, otherwise render blue rectangle with text as fallback
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
            DrawRectangle(screenX, screenY, width, height, BLUE);
            DrawText("DRONE", screenX, screenY - 15, 10, BLUE);
        }
    }

    // Collision effect: activate drone flight for 5 seconds
    void OnCollision(Player& player) override {
        player.ActivateDrone(duration);
        isAlive = false; // Item collected, disappears after pickup
    }
};