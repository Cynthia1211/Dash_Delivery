// BlackTechBox.h (黑市技术盒：抵挡下一次伤害/碰撞)
#pragma once
#include "GameObject.h"
#include "../entities/Player.h"

class BlackTechBox : public GameObject {
public:
    Texture2D texture; // 存储盒子图片

    BlackTechBox(float x, Texture2D tex) 
        : GameObject(x, 40.0f, 40.0f, ObjectType::BLACK_TECH_BOX, BLUE) {
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
            DrawText("BLACK-TECH", screenX - 5, screenY - 15, 10, BLUE);
        }
    }

    // 碰撞后：激活玩家保护盾
    void OnCollision(Player& player) override {
        player.shieldActive = true;
        isAlive = false; // 道具被拾取后消失
    }
};
