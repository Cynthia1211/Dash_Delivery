#include "Player.h"

Player::Player() {
    // 完美的同步你 main.cpp 中的初始数值
    pos = { 100.0f, 300.0f };
    velocity = { 0.0f, 0.0f };
    isGrounded = false;
    radius = 15.0f;
    
    spriteWidth = 80.0f;
    spriteHeight = 96.0f;
    facing = 1.0f; // 1.0f 代表朝右，-1.0f 代表朝左
}

void Player::UpdateTimers(float deltaTime) {
    if (skatesTimer > 0) skatesTimer -= deltaTime;
    if (droneTimer > 0) droneTimer -= deltaTime;
    if (catDebuffTimer > 0) catDebuffTimer -= deltaTime;
}

void Player::ActivateSkates(float duration) {
    skatesTimer = duration; // 设置倒计时为 5.0f 秒
}

void Player::HandleInput(float moveSpeed, float jumpForce) {
    velocity.x = 0.0f;

    float currentSpeed = baseMoveSpeed;
    if (skatesTimer > 0) currentSpeed *= 1.25f; // 旱冰鞋加成 [cite: 40]
    if (catDebuffTimer > 0) currentSpeed *= 0.5f; // 恶猫减速效果
    
    if (IsKeyDown(KEY_LEFT)) {
        velocity.x = -moveSpeed;
        facing = -1.0f; // 修复原本 main 中向左走不翻转的 Bug
    }
    if (IsKeyDown(KEY_RIGHT)) {
        velocity.x = moveSpeed;
        facing = 1.0f;
    }

    // 跳跃控制
    if (IsKeyPressed(KEY_SPACE) && isGrounded) {
        velocity.y = jumpForce;
        isGrounded = false;
    }
}

void Player::UpdatePhysics(float deltaTime, float gravity, float groundY) {
    // 应用重力
    if (!isGrounded) {
        velocity.y += gravity * deltaTime;
    } else {
        velocity.y = 0.0f;
    }

    // 垂直方向位置更新与地面碰撞
    pos.y += velocity.y * deltaTime;
    if (pos.y + radius >= groundY) {
        pos.y = groundY - radius;
        isGrounded = true;
    }
}

void Player::Draw() {
    // 裁剪源区域：通过乘以 facing 自动处理左右镜像翻转
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * facing, (float)texture.height };
    
    // 【修改核心】加入一个向下的透视偏移量值（比如 40.0f）
    float perspectiveOffsetY = 40.0f;

    // 渲染目标区域：对 Y 轴进行了微调 (-spriteHeight + radius)，确保小人的脚底死死踩在地面线
    Rectangle destRec = { pos.x - spriteWidth / 2.0f, pos.y  - spriteHeight + radius + perspectiveOffsetY, spriteWidth, spriteHeight };
    Vector2 origin = { 0.0f, 0.0f };
    
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
}