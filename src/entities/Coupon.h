// Coupon.h - 优惠券道具：拾取后立即增加整体关卡剩余时间
#pragma once
#include "GameObject.h"
#include "../levelmanager.h"

class Coupon : public GameObject {
public:
    Texture2D texture; // 存储优惠券图片
    float timeBonus;   // 增加的时间秒数

    // 构造函数：增加 Texture2D 和 LevelManager 参数
    // timeBonus: 增加的时间秒数（默认 15 秒）
    Coupon(float x, Texture2D tex, LevelManager& lm, float bonus = 15.0f) 
        : GameObject(x, 30.0f, 30.0f, ObjectType::COUPON, GREEN) {
        texture = tex;
        timeBonus = bonus;
        levelManager = &lm;
    }

    // 重写 Draw 方法
    void Draw(float worldScrollOffset, float groundY) override {
        if (!isAlive) return;

        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f;
        float screenY = groundY - height + perspectiveOffsetY;

        // 如果图片有效，就画图；否则画绿色方块和文字兜底
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
            DrawText("COUPON", screenX, screenY - 15, 10, GREEN);
        }
    }

    // 碰撞后的即时效果：直接增加关卡的倒计时时间
    void OnCollision(Player& player) override {
        if (levelManager) {
            levelManager->AddCountdownTime(timeBonus); // 即时增加倒计时时间
        }
        isAlive = false; // 道具被吃掉，不再可见和触发碰撞
    }

private:
    LevelManager* levelManager = nullptr; // 指向 LevelManager 的指针
};
