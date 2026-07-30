#pragma once
#include "raylib.h"


enum class ObjectType { ROADBLOCK, SKATES, BLACK_TECH_BOX, COUPON, DRONE, STRAY_CAT, GANGSTER };

class GameObject {
public:
    float worldX;
    float worldY; 
    float width;
    float height;
    ObjectType type;
    bool isAlive; // Whether object is active

    GameObject(float x, float y, float w, float h, ObjectType t)
        : worldX(x), worldY(y), width(w), height(h), type(t), isAlive(true) {}

    virtual ~GameObject() = default;

    // Each subclass implements its own collision behavior
    // Receives Player reference to modify player state
    virtual void OnCollision(class Player& player) = 0; 
    
    // Object position
    virtual void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) {
        if (!isAlive) return;
        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f;
        float screenY = groundY - height + perspectiveOffsetY;
        
    }

    // Y coordinate for air items
    static float GetVerticalMiddleY(float height, int screenHeight) {
        return screenHeight * 0.55f - height / 2.0f;
    }
};
