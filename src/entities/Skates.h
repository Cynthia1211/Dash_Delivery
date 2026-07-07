// Skates.h (旱冰鞋道具)
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class Skates : public GameObject {
public:
    Texture2D texture; // 存储固定的旱冰鞋图片

    // 构造函数：增加 Texture2D 参数
    Skates(float x, Texture2D tex) 
        : GameObject(x, 30.0f, 30.0f, ObjectType::SKATES, GOLD) {
        texture = tex;
    }

    // 重写 Draw 方法
    void Draw(float worldScrollOffset, float groundY) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f;
        float screenY = groundY - height + perspectiveOffsetY;

        // 如果图片有效，就画图；否则画金色方块和文字兜底
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
            DrawText("SKATES", screenX, screenY - 15, 10, GOLD);
        }
    }

    void OnCollision(Player& player) override {
        player.ActivateSkates(5.0f); // 触发玩家的加速，持续 5 秒
        isAlive = false;            // 道具被吃掉，不再可见和触发碰撞
    }
};