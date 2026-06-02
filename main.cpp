#include "raylib.h"

// 屏幕尺寸常量
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

// 物理常量
const float GRAVITY = 1500.0f;     // 重力加速度
const float JUMP_FORCE = -600.0f;  // 跳跃初始速度
const float MOVE_SPEED = 300.0f;  // 左右移动速度

int main() {
    // 初始化窗口
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dash Delivery - Camera & Bounds Control");
    SetTargetFPS(60);

    // 初始化外卖员状态 (红点)
    Vector2 playerPos = { 100.0f, 300.0f };
    Vector2 playerVelocity = { 0.0f, 0.0f };
    bool isGrounded = false;
    float playerRadius = 15.0f;

    // 地面高度
    float groundY = 350.0f;

    // 屏幕 1/3 处的锚点坐标
    const float CAMERA_TRIGGER_X = SCREEN_WIDTH / 3.0f; 

    // 视差滚动核心变量 (代表世界滚动的总距离)
    float worldScrollOffset = 0.0f; 

    // 主游戏循环
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // ----------------- 1. 玩家输入控制 -----------------
        playerVelocity.x = 0.0f;
        if (IsKeyDown(KEY_LEFT))  playerVelocity.x = -MOVE_SPEED;
        if (IsKeyDown(KEY_RIGHT)) playerVelocity.x = MOVE_SPEED;

        // 跳跃控制
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

        // ----------------- 2. 核心逻辑：屏幕锁定与滚动控制 -----------------
        
        // 预估本帧如果单纯移动，玩家会在什么位置
        float nextPlayerX = playerPos.x + playerVelocity.x * deltaTime;

        // 条件判断：
        // 1. 玩家正在往右走 (playerVelocity.x > 0)
        // 2. 玩家即将或者已经到达了屏幕左侧的 1/3 锚点 (nextPlayerX >= CAMERA_TRIGGER_X)
        if (playerVelocity.x > 0 && nextPlayerX >= CAMERA_TRIGGER_X) {
            // 【满足条件】外卖员被固定在左 1/3 处不前移
            playerPos.x = CAMERA_TRIGGER_X;
            
            // 地图和背景开始向左滚动（worldScrollOffset 增加）
            worldScrollOffset += playerVelocity.x * deltaTime;
        } 
        else {
            // 【不满足条件】（即：往左走、原地跳跃、或者在开局还未走到 1/3 处时）
            // 外卖员自己在画框里走，地图和背景绝对不倒退 (worldScrollOffset 保持不变)
            playerPos.x = nextPlayerX;
        }

        // 垂直方向位置更新与地面碰撞
        playerPos.y += playerVelocity.y * deltaTime;
        if (playerPos.y + playerRadius >= groundY) {
            playerPos.y = groundY - playerRadius;
            isGrounded = true;
        }

        // ----------------- 3. 画框边缘限制 (玩家活动范围控制) -----------------
        // 无论何时，限制外卖员不能走出屏幕左边缘
        if (playerPos.x - playerRadius < 0) {
            playerPos.x = playerRadius;
        }
        // 限制外卖员不能走出屏幕右边缘
        if (playerPos.x + playerRadius > SCREEN_WIDTH) {
            playerPos.x = SCREEN_WIDTH - playerRadius;
        }


        // ----------------- 4. 计算视察背景滚动坐标 -----------------
        // 利用取模运算 (%) 让背景无缝循环。
        // 远景速度系数 0.1，近景速度系数 0.5。由于 worldScrollOffset 会一直增加，这里加负号让其向左退。
        float backBgScroll = -(worldScrollOffset * 0.1f);
        backBgScroll = (float)((int)backBgScroll % SCREEN_WIDTH);

        float foreBgScroll = -(worldScrollOffset * 0.5f);
        foreBgScroll = (float)((int)foreBgScroll % SCREEN_WIDTH);


        // ----------------- 5. 渲染部分 -----------------
        BeginDrawing();
        ClearBackground(DARKGRAY); 

        // A. 绘制远景 (随玩家右行而向左慢速滚动)
        DrawRectangle(backBgScroll, 100, 200, 150, GRAY);
        DrawRectangle(backBgScroll + SCREEN_WIDTH, 100, 200, 150, GRAY);
        DrawRectangle(backBgScroll + 400, 120, 250, 130, GRAY);
        DrawRectangle(backBgScroll + 400 + SCREEN_WIDTH, 120, 250, 130, GRAY);

        // B. 绘制近景 (随玩家右行而向左快速滚动)
        DrawCircle(foreBgScroll + 150, 280, 40, DARKGREEN);
        DrawCircle(foreBgScroll + 150 + SCREEN_WIDTH, 280, 40, DARKGREEN);
        DrawCircle(foreBgScroll + 600, 290, 30, DARKGREEN);
        DrawCircle(foreBgScroll + 600 + SCREEN_WIDTH, 290, 30, DARKGREEN);

        // C. 绘制路面 (一条横线)
        DrawLine(0, groundY, SCREEN_WIDTH, groundY, WHITE);

        // D. 绘制外卖员 (小红点)
        DrawCircleV(playerPos, playerRadius, RED);

        // E. 调试与界面信息展示
        DrawText("Dash Delivery - Stage 1 Target Achieved", 10, 10, 20, MAROON);
        DrawText(TextFormat("Player Screen X: %.1f | World Distance: %.1f", playerPos.x, worldScrollOffset), 10, 40, 16, LIGHTGRAY);
        // 绘制一条淡淡的辅助线，标出 1/3 锁定区的位置，方便你们写作业验证
        DrawLine(CAMERA_TRIGGER_X, 0, CAMERA_TRIGGER_X, groundY, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}