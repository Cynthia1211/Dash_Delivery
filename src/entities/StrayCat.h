// StrayCat.h (恶猫敌人)
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class StrayCat : public GameObject {
public:
    Texture2D texture; // 存储猫的图片

    StrayCat(float x, Texture2D tex) 
        : GameObject(x, 40.0f, 40.0f, ObjectType::ROADBLOCK, ORANGE) { // 借用现有类型或看是否有对应枚举
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
            DrawRectangle(screenX, screenY, width, height, color);
            DrawText("CAT", screenX + 5, screenY + 10, 12, BLACK);
        }
    }

    void OnCollision(Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false; // 护盾抵挡伤害
        } else {
            player.foodStatus -= 15.0f;
            player.catDebuffTimer = 5.0f;
        }
        isAlive = false;
    }
};