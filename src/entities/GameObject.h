#pragma once
#include "raylib.h"

// 区分类型，方便后续特定逻辑或渲染判断
enum class ObjectType { ROADBLOCK, SKATES, BLACK_TECH_BOX, COUPON, DRONE, STRAY_CAT, GANGSTER };

class GameObject {
public:
    float worldX;
    float width;
    float height;
    ObjectType type;
    Color color; // 临时测试颜色
    bool isAlive; // 是否激活（比如道具吃完就消失）

    GameObject(float x, float w, float h, ObjectType t, Color c)
        : worldX(x), width(w), height(h), type(t), color(c), isAlive(true) {}

    virtual ~GameObject() = default;

    // 每一个子类去实现自己被碰撞后的特殊逻辑
    // 传入 Player 引用，直接修改玩家的状态（比如速度、食物完整度、生命周期等）
    virtual void OnCollision(class Player& player) = 0; 
    
    // 自定义渲染逻辑（如果需要绘制特定的提示文字或图片）
    virtual void Draw(float worldScrollOffset, float groundY, int screenHeight = 0) {
        if (!isAlive) return;
        float screenX = worldX - worldScrollOffset;
        float perspectiveOffsetY = 40.0f; // 保持你目前的视觉偏移
        float screenY = groundY - height + perspectiveOffsetY;
        
        DrawRectangle(screenX, screenY, width, height, color);
    }

    // 工具方法：计算屏幕垂直中间偏下位置的Y坐标（玩家跳跃可触及）
    static float GetVerticalMiddleY(float height, int screenHeight) {
        return screenHeight * 0.55f - height / 2.0f;
    }
};
