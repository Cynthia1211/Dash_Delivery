#pragma once
#include "raylib.h"

// Object type enum for distinguishing different game entity types (used for logic and rendering)
enum class ObjectType { ROADBLOCK, SKATES, BLACK_TECH_BOX, COUPON, DRONE, STRAY_CAT, GANGSTER };

class GameObject {
public:
    float worldX;
    float worldY;          // Y position in world coordinates
    float width;
    float height;
    ObjectType type;
    Color color; // Temporary test color (for debugging)
    bool isAlive; // Whether object is active (items disappear after being collected)

    GameObject(float x, float y, float w, float h, ObjectType t, Color c)
        : worldX(x), worldY(y), width(w), height(h), type(t), color(c), isAlive(true) {}

    virtual ~GameObject() = default;

    // Each subclass implements its own collision behavior
    // Receives Player reference to modify player state (speed, food status, life cycle, etc.)
    virtual void OnCollision(class Player& player) = 0; 
    
    // Custom rendering logic (override if specific text or images need custom drawing)
    virtual void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) {
        if (!isAlive) return;
        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f; // Perspective offset for visual depth effect
        float screenY = groundY - height + perspectiveOffsetY;
        
        DrawRectangle(screenX, screenY, width, height, color);
    }

    // Utility method: Calculate Y coordinate at vertical middle-lower position on screen (reachable by player jump)
    static float GetVerticalMiddleY(float height, int screenHeight) {
        return screenHeight * 0.55f - height / 2.0f;
    }
};
