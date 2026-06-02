#include "raylib.h"

// 屏幕尺寸常量
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

// 重力与跳跃物理常量
const float GRAVITY = 1500.0f;     // 重力加速度
const float JUMP_FORCE = -600.0f;  // 跳跃初始速度（向上为负）
const float MOVE_SPEED = 300.0f;  // 左右移动速度

int main() {
    // 1. 初始化窗口和音频（准备工作）
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dash Delivery - Prototype Phase 1");
    SetTargetFPS(60);

    // 2. 初始化外卖员状态 (红点)
    Vector2 playerPos = { 100.0f, 300.0f };
    Vector2 playerVelocity = { 0.0f, 0.0f };
    bool isGrounded = false;
    float playerRadius = 15.0f;

    // 3. 定义地面高度
    float groundY = 350.0f;

    // 4. 初始化背景滚动变量 (Yue 的基础视差工作)
    float backBgScroll = 0.0f;  // 远景滚动速度
    float foreBgScroll = 0.0f;  // 近景滚动速度

    // 主游戏循环
    while (!WindowShouldClose()) {
        // --- 帧率/时间增量 ---
        float deltaTime = GetFrameTime();

        // --- 玩家输入与物理更新 (Mingda 的任务范围) ---
        
        // 左右移动控制
        playerVelocity.x = 0.0f;
        if (IsKeyDown(KEY_LEFT))  playerVelocity.x = -MOVE_SPEED;
        if (IsKeyDown(KEY_RIGHT)) playerVelocity.x = MOVE_SPEED;

        // 跳跃控制（只有在地面上时才能跳）
        if (IsKeyPressed(KEY_SPACE) && isGrounded) {
            playerVelocity.y = JUMP_FORCE;
            isGrounded = false;
        }

        // 应用重力
        if (!isGrounded) {
            playerVelocity.y += GRAVITY * deltaTime;
        } else {
            playerVelocity.y = 0.0f;
        }

        // 更新玩家位置
        playerPos.x += playerVelocity.x * deltaTime;
        playerPos.y += playerVelocity.y * deltaTime;

        // 地面碰撞检测 (简单的 Y 轴边界检查)
        if (playerPos.y + playerRadius >= groundY) {
            playerPos.y = groundY - playerRadius;
            isGrounded = true;
        }

        // 防止玩家走出屏幕左边界
        if (playerPos.x - playerRadius < 0) {
            playerPos.x = playerRadius;
        }

        // --- 背景视差滚动更新 (Yue 的任务范围) ---
        // 只有当玩家在移动时，背景才会向反方向滚动，制造移动错觉
        if (playerVelocity.x != 0) {
            backBgScroll -= playerVelocity.x * 0.1f * deltaTime;  // 远景动得慢
            foreBgScroll -= playerVelocity.x * 0.5f * deltaTime;  // 近景动得快
        }

        // 循环背景坐标，防止数值无限变大
        if (backBgScroll <= -SCREEN_WIDTH) backBgScroll = 0;
        if (backBgScroll >= SCREEN_WIDTH)  backBgScroll = 0;
        if (foreBgScroll <= -SCREEN_WIDTH) foreBgScroll = 0;
        if (foreBgScroll >= SCREEN_WIDTH)  foreBgScroll = 0;


        // --- 渲染部分 ---
        BeginDrawing();
        ClearBackground(DARKGRAY); // 深色天空背景

        // A. 绘制远景 (例如: 远处的微弱方块群，代表高楼)
        DrawRectangle(backBgScroll, 100, 200, 150, GRAY);
        DrawRectangle(backBgScroll + SCREEN_WIDTH, 100, 200, 150, GRAY);
        DrawRectangle(backBgScroll + 400, 120, 250, 130, GRAY);
        DrawRectangle(backBgScroll + 400 + SCREEN_WIDTH, 120, 250, 130, GRAY);

        // B. 绘制近景 (例如: 稍近一些的简单线条或小障碍，动得较快)
        DrawCircle(foreBgScroll + 150, 280, 40, DARKGREEN);
        DrawCircle(foreBgScroll + 150 + SCREEN_WIDTH, 280, 40, DARKGREEN);
        DrawCircle(foreBgScroll + 600, 290, 30, DARKGREEN);
        DrawCircle(foreBgScroll + 600 + SCREEN_WIDTH, 290, 30, DARKGREEN);

        // C. 绘制路面 (一条横线)
        DrawLine(0, groundY, SCREEN_WIDTH, groundY, WHITE);

        // D. 绘制外卖员 (小红点)
        DrawCircleV(playerPos, playerRadius, RED);

        // E. 界面文字提示
        DrawText("Dash Delivery Prototype", 10, 10, 20, MAROON);
        DrawText("Controls: Left/Right Arrow to Move | Space to Jump", 10, 40, 16, LIGHTGRAY);

        EndDrawing();
    }

    // 清理关闭窗口
    CloseWindow();
    return 0;
}