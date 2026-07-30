#include "raylib.h"
#include "entities/Player.h"
#include "levelmanager.h"

// 背景音乐
Music backgroundMusic;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const float GRAVITY = 1500.0f;     
const float JUMP_FORCE = -600.0f;  
const float MOVE_SPEED = 300.0f;  
const float GROUND_Y = 350.0f;

// 全局视差滚动速度 (所有关卡共用)
const float BACK_SCROLL_SPEED = 0.1f;
const float FORE_SCROLL_SPEED = 0.5f;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dash Delivery - Modular Architecture");
    SetTargetFPS(60);

    // 初始化音频设备
    InitAudioDevice();

    // 加载并循环播放背景音乐 (LoadMusicStream 默认就是循环播放)
    backgroundMusic = LoadMusicStream("../assets/backgroundSond.mp3");
    PlayMusicStream(backgroundMusic);

    // 实例化模块
    Player player;
    player.texture = LoadTexture("../assets/player.png");

    // 在 main 函数内部的初始化部分添加：
    int currentStage = 1; // 记录当前是第几关

    // 定义游戏状态：0 = 游戏中，1 = 关卡间过渡（显示过关提示），2 = 游戏最终通关
    int gameState = 0; 

    // 记录过渡画面的计时器（比如让过关提示显示 3 秒钟）
    float stageTransitionTimer = 0.0f;

    LevelManager level(SCREEN_WIDTH, SCREEN_HEIGHT, GROUND_Y);
    level.SetParallaxScrollSpeed(BACK_SCROLL_SPEED, FORE_SCROLL_SPEED); // 设置全局视差滚动速度
    level.LoadAssets();
    level.SetupLevel(currentStage);

    const float CAMERA_TRIGGER_X = SCREEN_WIDTH / 3.0f; 
    float worldScrollOffset = 0.0f; 

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // 更新背景音乐流
        UpdateMusicStream(backgroundMusic);

        // =================================================================
        // 【核心逻辑控制】根据 gameState 分流
        // =================================================================
        if (gameState == 0) {
            // ----- 状态 0：正常游戏中 -----

            float deltaTime = GetFrameTime();

            // 1. 先更新计时器
            player.UpdateTimers(deltaTime);
            level.UpdateCountdown(deltaTime); // 更新关卡倒计时
            level.UpdateFoodDecay(deltaTime, player); // 更新食物完整度递减

            // 1. 玩家输入控制与物理更新
            player.HandleInput(MOVE_SPEED, JUMP_FORCE);
            player.UpdatePhysics(deltaTime, GRAVITY, GROUND_Y);

            // =================================================================
            // 2. 屏幕锁定与滚动控制（完美修复版）
            // =================================================================
            float nextPlayerX = player.pos.x + player.velocity.x * deltaTime;

            // 1. 计算地图能滚动的最大极限（终点线刚好贴在屏幕最右侧）
            float maxScrollLimit = level.currentLevel.maxDistance - SCREEN_WIDTH;

            // 2. 只有当地图还没滚到头，且玩家超过 1/3 触发线时，画面才滚动
            if (player.velocity.x > 0 && nextPlayerX >= CAMERA_TRIGGER_X && worldScrollOffset < maxScrollLimit) {
                
                // 让小人固定在屏幕 1/3 处
                player.pos.x = CAMERA_TRIGGER_X;
                
                // 画面正常滚动
                worldScrollOffset += player.velocity.x * deltaTime;
                
                // 【核心用处】如果这帧滚过头了，强制把它拉回到最大限制，卡死画面
                if (worldScrollOffset > maxScrollLimit) {
                    worldScrollOffset = maxScrollLimit;
                }
            } else {
                // 画面不动了，小人自己在屏幕里往右走（或者往左退）
                player.pos.x = nextPlayerX;
            }

            // 3. 边缘限制
            if (player.pos.x - player.radius < 0) player.pos.x = player.radius;
            if (player.pos.x + player.radius > SCREEN_WIDTH) player.pos.x = SCREEN_WIDTH - player.radius;

            // 4. 碰撞检测（AABB，世界坐标 X + 世界坐标 Y）
            float pWorldX = worldScrollOffset + player.pos.x;
            for (auto& obj : level.currentLevel.objects) {
                if (!obj->isAlive) continue;

                // X轴检测：玩家右边界 > 对象左边界 且 玩家左边界 < 对象右边界
                bool overlapX = pWorldX + player.radius > obj->worldX &&
                                pWorldX - player.radius < obj->worldX + obj->width;
                
                // Y轴检测：使用对象的worldY进行碰撞检测（支持空中道具）
                bool overlapY = player.pos.y + player.radius > obj->worldY &&
                                player.pos.y - player.spriteHeight + player.radius < obj->worldY + obj->height;

                if (overlapX && overlapY) {
                    obj->OnCollision(player);
                }
            }

            // 5. 食物状态归零 → 任务失败
            if (player.foodStatus <= 0.0f) {
                player.foodStatus = 0.0f;
                gameState = 3;
            }

            // 5b. 倒计时归零 → 任务失败
            if (level.currentLevel.countdownTimer <= 0.0f) {
                gameState = 3;
            }

            // 6. 检查是否触碰终点线（完美修复：小人亲脚踩到终点线才算过关）
            float playerWorldX = pWorldX;
            if (playerWorldX + player.radius >= level.currentLevel.maxDistance) {
                player.shieldActive = false;  // 过关时重置护盾
                if (currentStage < 3) {
                    // 如果还没到第 3 关，进入关卡过渡状态
                    gameState = 1; 
                    stageTransitionTimer = 3.0f; // 提示显示 3 秒
                } else {
                    // 如果已经是第 3 关，说明全剧终，直接通关
                    gameState = 2;
                }
            }
        } 
        else if (gameState == 1) {
            // ----- 状态 1：关卡过渡中（自动倒计时切换） -----
            stageTransitionTimer -= deltaTime;
            
            if (stageTransitionTimer <= 0.0f) {
                // 3秒倒计时结束，自动切入下一关！
                currentStage++;
                worldScrollOffset = 0.0f;        // 核心：重置地图滚动距离

                player.pos.x = 100.0f;
                player.pos.y = GROUND_Y - player.radius;
                player.velocity = { 0.0f, 0.0f }; // 速度清零
                player.isGrounded = true;
                player.foodStatus = 100.0f;       // 重置食物完整度
                player.shieldActive = false;      // 重置护盾
                
                level.SetupLevel(currentStage);   // 核心：一键加载新关卡的数据（速度、天气、障碍物、倒计时）
                gameState = 0;                    // 切回正常游戏状态
            }
        }

        // =================================================================
        // 5. 渲染部分
        // =================================================================
        BeginDrawing();
        ClearBackground(SKYBLUE); 

        // 无论什么状态，都先把背景、地面和小人画出来
        level.Draw(worldScrollOffset);
        player.Draw();

        // 根据不同状态，在屏幕最上层盖上不同的文字提示 UI（张月负责的部分）
        if (gameState == 0) {
            level.DrawHUD(player, worldScrollOffset);
        } 
        else if (gameState == 1) {
            // 关卡通过，等待自动开启下一关的黑色半透明遮罩
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
            DrawText(TextFormat("STAGE %d CLEAR!", currentStage), 260, 180, 40, GREEN);
            DrawText(TextFormat("Next Stage starts in %d seconds...", (int)stageTransitionTimer), 240, 240, 20, WHITE);
        } 
        else if (gameState == 2) {
            // 最终大通关
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
            DrawText("CONGRATULATIONS!", 180, 160, 40, GOLD);
            DrawText("You Delivered All Orders On Time!", 200, 220, 20, WHITE);
        }
        else if (gameState == 3) {
            // 任务失败
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
            DrawText("MISSION FAILED", 210, 160, 40, RED);
            DrawText("Food status dropped to 0%!", 240, 220, 20, WHITE);
            DrawText("Press R to Restart", 290, 260, 20, LIGHTGRAY);

            if (IsKeyPressed(KEY_R)) {
                currentStage = 1;
                worldScrollOffset = 0.0f;
                player.pos = { 100.0f, GROUND_Y - player.radius };
                player.velocity = { 0.0f, 0.0f };
                player.isGrounded = true;
                player.foodStatus = 100.0f;
                player.skatesTimer = 0.0f;
                player.droneTimer = 0.0f;
                player.catDebuffTimer = 0.0f;
                player.shieldActive = false;      // 重置护盾
                level.SetupLevel(currentStage);
                gameState = 0;
            }
        }

        DrawLine(CAMERA_TRIGGER_X, 0, CAMERA_TRIGGER_X, GROUND_Y, LIGHTGRAY);

        EndDrawing();
    }
    
    // 停止并卸载背景音乐
    StopMusicStream(backgroundMusic);
    UnloadMusicStream(backgroundMusic);
    
    // 关闭音频设备
    CloseAudioDevice();

    // 清理资源
    UnloadTexture(player.texture);
    level.UnloadAssets();
    CloseWindow();

    return 0;
}