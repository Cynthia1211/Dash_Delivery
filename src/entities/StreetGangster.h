// StreetGangster.h (街头流氓敌人)
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class StreetGangster : public GameObject {
public:
    Texture2D texture; // 存储流氓的图片

    StreetGangster(float x, Texture2D tex) 
        : GameObject(x, 40.0f, 60.0f, ObjectType::ROADBLOCK, RED) {
        texture = tex;
    }

    void Draw(float worldScrollOffset, float groundY) override {
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
            DrawText("GANG", screenX + 2, screenY + 10, 12, WHITE);
        }
    }

    void OnCollision(Player& player) override {
        // 直接抢走外卖！食物血量清空，触发 main.cpp 里的游戏结束逻辑
        player.foodStatus = 0.0f; 
        isAlive = false;
    }
};