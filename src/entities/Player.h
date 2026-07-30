#pragma once
#include "raylib.h"

class Player
{
public:
    Vector2 pos;
    Vector2 velocity;
    bool isGrounded;
    float radius;

    float spriteWidth;
    float spriteHeight;
    float facing;
    Texture2D texture;

    float baseMoveSpeed = 300.0f;    // 基础移动速度
    float foodStatus = 100.0f; // 食物完整度 [cite: 18]
    float groundY = 350.0f;  // 地面Y坐标（用于无人机道具）

    float skatesTimer = 0.0f;
    float droneTimer = 0.0f;
    float catDebuffTimer = 0.0f;
    bool shieldActive = false;  // 黑市技术盒护盾状态
    



    // 构造函数
    Player();

    // 处理玩家的键盘输入
    void HandleInput(float moveSpeed, float jumpForce);

    // 处理物理位移、重力以及基本的地面碰撞
    void UpdatePhysics(float deltaTime, float gravity, float groundY);

    // 渲染外卖员图片（包含镜像翻转）
    void Draw();    
    
    // 激活道具的函数
    void ActivateSkates(float duration, float speedBoost)
    {
        skatesTimer = duration;
    }

    void UpdateTimers(float deltaTime); // 每帧用来更新倒计时的函数
    void ActivateSkates(float duration); // 吃到旱冰鞋时触发
    void ActivateDrone(float duration); // 吃到无人机时触发
};
