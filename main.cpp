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

    // 在加载 playerTexture 的下方添加：
    Texture2D foreTexture = LoadTexture("assets/foreground.png");

    // =================================================================
    // 1. 【新增】加载外卖员图片
    // =================================================================
    Texture2D playerTexture = LoadTexture("assets/player.png");

    // 你可以根据小人图片在屏幕上的实际视觉比例，任意调整这两个长宽数值
    float spriteWidth = 80.0f;
    float spriteHeight = 80.0f;

    // 用来记录小人当前的左右朝向（1.0f 代表朝右，-1.0f 代表朝左）
    float playerFacing = 1.0f;
    // =================================================================

    // 初始化外卖员状态
    Vector2 playerPos = { 100.0f, 300.0f };
    Vector2 playerVelocity = { 0.0f, 0.0f };
    bool isGrounded = false;
    float playerRadius = 15.0f;

    // 地面高度
    float groundY = 350.0f;

    // -----------------------------------------------------------------
    // 1. 【新增】远景图尺寸与缩放核心计算（调为 1/2 画面高度）
    // -----------------------------------------------------------------
    Texture2D backTexture = LoadTexture("assets/background.png"); // 假设你的远景图片叫 background.png

    float backHeight = SCREEN_HEIGHT / 1.0f; 
    float backScale = backHeight / (float)backTexture.height; // 保持原有宽高比
    float backRenderWidth = (float)backTexture.width * backScale; // 缩放后的实际宽度
    float backY = groundY - backHeight; // 让远景的底部刚好贴在地面上

    // 自定义一个 70% 明度的滤镜颜色 (R:178, G:178, B:178 是 255 的 70%)
    Color backTint = { 200, 200, 200, 100 };

    // -----------------------------------------------------------------
    // 1. 近景图（2851 x 1271）尺寸与缩放核心计算
    // -----------------------------------------------------------------

    // 要求高度为画面的 1/4
    float foreHeight = SCREEN_HEIGHT / 1.3f;

    // 根据高度计算缩放比例，保持原有宽高比不缩放变形
    float foreScale = foreHeight / (float)foreTexture.height; 

    // 计算出图片缩放后在屏幕上的实际渲染宽度
    float foreRenderWidth = (float)foreTexture.width * foreScale; 

    // 让近景图的底部刚好踩在地面（groundY）上
    float foreY = groundY - foreHeight;

    Color foreTint = { 255, 255, 255, 255 };

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
        ClearBackground(SKYBLUE); 

        // =================================================================
        // A. 【升级】绘制远景图片 (70% 明度 + 无缝平铺)
        // =================================================================
        // 传入 backTint 即可把图片亮度直接在硬件层降到 70%，省去了去 PS 调图的麻烦
        for (float xOffset = backBgScroll; xOffset < SCREEN_WIDTH; xOffset += backRenderWidth) {
            DrawTextureEx(backTexture, (Vector2){ xOffset, backY }, 0.0f, backScale, backTint);
        }

        // =================================================================
        // B. 绘制近景 (进阶：使用 for 循环无缝平铺铺满整个屏幕)
        // =================================================================
        // 因为图片缩放后变窄了，我们需要连续画几张来接力。
        // 从 foreBgScroll 开始，每隔一幅图的宽度画一次，直到画出屏幕右边界
        for (float xOffset = foreBgScroll; xOffset < SCREEN_WIDTH; xOffset += foreRenderWidth) {
            DrawTextureEx(foreTexture, (Vector2){ xOffset, foreY }, 0.0f, foreScale, foreTint);
        }
        // =================================================================

        // =================================================================
        // C. 【修改】绘制地面背景（灰色实心大矩形）
        // =================================================================
        // 从地平线（groundY）一直填满到屏幕最下方
        DrawRectangle(0, groundY, SCREEN_WIDTH, SCREEN_HEIGHT - groundY, GRAY);

        // =================================================================
        // D. 【核心改动】把红点换成小人图片，并进行左右翻转
        // =================================================================
        
        // 裁剪源区域：通过乘以 playerFacing (1.0f 或 -1.0f)，Raylib 会自动处理左右镜像翻转
        Rectangle sourceRec = { 0.0f, 0.0f, (float)playerTexture.width * playerFacing, (float)playerTexture.height };
        
        // 渲染目标区域：小人在屏幕上的实际 X, Y 坐标及缩放后的大小
        Rectangle destRec = { playerPos.x, playerPos.y, spriteWidth, spriteHeight };
        
        // 渲染原点：设置在图片左上角
        Vector2 origin = { 0.0f, 0.0f };
        
        // 调用高级纹理绘制函数渲染外卖小人
        DrawTexturePro(playerTexture, sourceRec, destRec, origin, 0.0f, WHITE);
        // =================================================================

        // E. 调试与界面信息展示
        DrawText("Dash Delivery - Stage 1 Target Achieved", 10, 10, 20, MAROON);
        DrawText(TextFormat("Player Screen X: %.1f | World Distance: %.1f", playerPos.x, worldScrollOffset), 10, 40, 16, LIGHTGRAY);
        // 绘制一条淡淡的辅助线，标出 1/3 锁定区的位置，方便你们写作业验证
        DrawLine(CAMERA_TRIGGER_X, 0, CAMERA_TRIGGER_X, groundY, LIGHTGRAY);

        EndDrawing();
    }
    
    // 在最下方卸载纹理
    UnloadTexture(playerTexture);
    UnloadTexture(foreTexture); 
    UnloadTexture(backTexture);

    CloseWindow();

    return 0;
}