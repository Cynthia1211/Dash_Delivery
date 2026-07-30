// BlackTechBox.h - Black market technology box that activates player shield protection
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class BlackTechBox : public GameObject {
public:
    Texture2D texture; // Reference to the black tech box image texture

    // Constructor: creates black tech box at given position
    // x: X position, y: Y position in world coordinates (airborne items use GetVerticalMiddleY)
    BlackTechBox(float x, float y, Texture2D tex) 
        : GameObject(x, y, 40.0f, 40.0f, ObjectType::BLACK_TECH_BOX, BLUE) {
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
            DrawRectangle(screenX, screenY, width, height, color);
            DrawText("BLACK-TECH", screenX - 5, screenY - 15, 10, BLUE);
        }
    }

    // Collision effect: activate player shield protection
    void OnCollision(Player& player) override {
        player.shieldActive = true;
        isAlive = false; // Item collected, disappears after use
    }
};
