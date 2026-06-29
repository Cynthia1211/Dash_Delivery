#include "raylib.h"
#include "entities/Player.h"
#include "levelmanager.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const float GRAVITY = 1500.0f;     
const float JUMP_FORCE = -600.0f;  
const float MOVE_SPEED = 300.0f;  
const float GROUND_Y = 350.0f;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dash Delivery - Modular Architecture");
    SetTargetFPS(60);

    // 实例化模块
    Player player;
    player.texture = LoadTexture("../assets/player.png");

    LevelManager level(SCREEN_WIDTH, SCREEN_HEIGHT, GROUND_Y);
    level.LoadAssets();

    const float CAMERA_TRIGGER_X = SCREEN_WIDTH / 3.0f; 
    float worldScrollOffset = 0.0f; 

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // 1. 玩家输入控制与物理更新
        player.HandleInput(MOVE_SPEED, JUMP_FORCE);
        player.UpdatePhysics(deltaTime, GRAVITY, GROUND_Y);

        // 2. 核心逻辑：屏幕锁定与滚动控制
        float nextPlayerX = player.pos.x + player.velocity.x * deltaTime;

        if (player.velocity.x > 0 && nextPlayerX >= CAMERA_TRIGGER_X) {
            player.pos.x = CAMERA_TRIGGER_X;
            worldScrollOffset += player.velocity.x * deltaTime;
        } 
        else {
            player.pos.x = nextPlayerX;
        }

        // 3. 屏幕画框左右边缘限制
        if (player.pos.x - player.radius < 0)              player.pos.x = player.radius;
        if (player.pos.x + player.radius > SCREEN_WIDTH)   player.pos.x = SCREEN_WIDTH - player.radius;

        // 4. 渲染阶段
        BeginDrawing();
        ClearBackground(SKYBLUE); 

        // 调用模块绘制背景与地面
        level.Draw(worldScrollOffset);

        // 调用模块绘制玩家
        player.Draw();

        // 界面调试文字与辅助线
        DrawText("Dash Delivery - Stage 1 Target Achieved", 10, 10, 20, MAROON);
        DrawText(TextFormat("Player Screen X: %.1f | World Distance: %.1f", player.pos.x, worldScrollOffset), 10, 40, 16, LIGHTGRAY);
        DrawLine(CAMERA_TRIGGER_X, 0, CAMERA_TRIGGER_X, GROUND_Y, LIGHTGRAY);

        EndDrawing();
    }
    
    // 清理资源
    UnloadTexture(player.texture);
    level.UnloadAssets();
    CloseWindow();

    return 0;
}