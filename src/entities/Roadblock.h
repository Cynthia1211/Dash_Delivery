// Roadblock.h (路障)
#pragma once
#include "GameObject.h"
#include "../entities/Player.h" // 假设你的Player类定义在entities

class Roadblock : public GameObject {
public:
    Texture2D texture; // 存储那张固定的路障图片

    // 构造函数：增加一个 Texture2D 参数
    Roadblock(float x, Texture2D tex) 
        : GameObject(x, 40.0f, 50.0f, ObjectType::ROADBLOCK, RED) {
        texture = tex;
    }

    // 重写 Draw 方法
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f;
        float screenY = groundY - height + perspectiveOffsetY;

        // 如果图片有效（id不为0），就画图；否则画红色方块兜底
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
            DrawRectangle(screenX, screenY, width, height, color);
        }
    }

    void OnCollision(class Player& player) override {
        if (player.shieldActive) {
            player.shieldActive = false; // 护盾抵挡伤害
        } else {
            player.foodStatus -= 10.0f; // 撞到扣血
        }
        isAlive = false; 
    }
};