#pragma once
#include "raylib.h"

class Player {
public:
    Vector2 pos;
    Vector2 velocity;
    bool isGrounded;
    float radius;
    
    float spriteWidth;
    float spriteHeight;
    float facing;
    Texture2D texture;

    // 构造函数
    Player();
    
    // 处理玩家的键盘输入
    void HandleInput(float moveSpeed, float jumpForce);
    
    // 处理物理位移、重力以及基本的地面碰撞
    void UpdatePhysics(float deltaTime, float gravity, float groundY);
    
    // 渲染外卖员图片（包含镜像翻转）
    void Draw();
};