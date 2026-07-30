// DeliveryDrone.h - 配送无人机道具：拾取后玩家飞到空中1/3位置，持续5秒
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class DeliveryDrone : public GameObject {
public:
    Texture2D texture; // 存储无人机图片
    float duration;    // 飞行持续时间（秒）

    // 构造函数
    DeliveryDrone(float x, Texture2D tex) 
        : GameObject(x, 30.0f, 30.0f, ObjectType::DRONE, BLUE) {
        texture = tex;
        duration = 5.0f;
    }

    // 重写 Draw 方法 - 使用屏幕垂直中间位置
    void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float screenY = screenHeight > 0 ? GetVerticalMiddleY(height, screenHeight) : groundY - height + 40.0f;

        // 如果图片有效，就画图；否则画蓝色方块和文字兜底
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
            DrawText("DRONE", screenX, screenY - 15, 10, BLUE);
        }
    }

    // 碰撞后：让玩家飞到空中并持续5秒
    void OnCollision(Player& player) override {
        player.ActivateDrone(duration);
        isAlive = false; // 道具被拾取后消失
    }
};