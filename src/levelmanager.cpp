#include "levelmanager.h"

LevelManager::LevelManager(int sWidth, int sHeight, float gY) {
    screenWidth = sWidth;
    screenHeight = sHeight;
    groundY = gY;

    // 初始化滤镜颜色
    backTint = { 200, 200, 200, 100 };
    foreTint = { 255, 255, 255, 255 };
    
}

void LevelManager::LoadAssets() {
    backTexture = LoadTexture("../assets/background.png");
    foreTexture = LoadTexture("../assets/foreground.png");

    float backHeight = screenHeight / 1.0f; 
    backScale = backHeight / (float)backTexture.height;
    backRenderWidth = (float)backTexture.width * backScale;
    backY = groundY - backHeight;

    float foreHeight = screenHeight / 1.3f;
    foreScale = foreHeight / (float)foreTexture.height; 
    foreRenderWidth = (float)foreTexture.width * foreScale; 
    foreY = groundY - foreHeight;
}

void LevelManager::UnloadAssets() {
    UnloadTexture(backTexture);
    UnloadTexture(foreTexture);
}

// 【核心改动】爽快的 switch-case 结构
void LevelManager::SetupLevel(int levelNumber) {
    currentLevel.objects.clear(); // 切换关卡前，必须先把上一关的物体清空
    currentLevel.levelNumber = levelNumber;

    switch (levelNumber) {
        case 1:
            // ------- 第一关配置 -------
            currentLevel.maxDistance = 2000.0f;     // 关卡长度
            currentLevel.backScrollSpeed = 0.1f;    // 视差速度
            currentLevel.foreScrollSpeed = 0.5f;
            currentLevel.weather = WeatherType::SUNNY;

            // 摆放障碍物（张月负责调整 worldX）
            currentLevel.objects.push_back({ 600.0f, 40.0f, 50.0f, 0, RED });    // 基础方块障碍
            currentLevel.objects.push_back({ 1200.0f, 30.0f, 30.0f, 1, GOLD });  // 道具：旱冰鞋
            break;

        case 2:
            // ------- 第二关配置 -------
            currentLevel.maxDistance = 4000.0f;
            currentLevel.backScrollSpeed = 0.15f;   // 速度稍微加快
            currentLevel.foreScrollSpeed = 0.7f;
            currentLevel.weather = WeatherType::NIGHT;

            // 摆放第二关的敌人和道具
            currentLevel.objects.push_back({ 500.0f, 40.0f, 40.0f, 3, ORANGE }); // 敌人：流氓
            currentLevel.objects.push_back({ 1500.0f, 30.0f, 30.0f, 4, BLACK }); // 敌人：恶猫
            currentLevel.objects.push_back({ 2500.0f, 40.0f, 20.0f, 2, BLUE });  // 道具：无人机
            break;

        case 3:
            // ------- 第三关配置 -------
            currentLevel.maxDistance = 6000.0f;
            currentLevel.backScrollSpeed = 0.2f;    // 极速飙车
            currentLevel.foreScrollSpeed = 0.9f;
            currentLevel.weather = WeatherType::RAIN;

            // 用循环密集摆放第三关的障碍物
            for (float x = 800.0f; x < 5500.0f; x += 600.0f) {
                currentLevel.objects.push_back({ x, 40.0f, 60.0f, 0, RED });
            }
            break;

        // 万一以后你们想加第 4 关，直接在这里写：
        // case 4:
        //     ...
        //     break;

        default:
            break;
    }
}

void LevelManager::Draw(float worldScrollOffset) {
// 根据当前关卡配置的速度计算滚动
    float backBgScroll = -(worldScrollOffset * currentLevel.backScrollSpeed);
    backBgScroll = (float)((int)backBgScroll % screenWidth);

    float foreBgScroll = -(worldScrollOffset * currentLevel.foreScrollSpeed);
    foreBgScroll = (float)((int)foreBgScroll % screenWidth);

    // 绘制远景和近景
    for (float xOffset = backBgScroll; xOffset < screenWidth; xOffset += backRenderWidth) {
        DrawTextureEx(backTexture, (Vector2){ xOffset, backY }, 0.0f, backScale, backTint);
    }
    for (float xOffset = foreBgScroll; xOffset < screenWidth; xOffset += foreRenderWidth) {
        DrawTextureEx(foreTexture, (Vector2){ xOffset, foreY }, 0.0f, foreScale, foreTint);
    }

    DrawRectangle(0, groundY, screenWidth, screenHeight - groundY, GRAY);

    // 根据天气渲染全局环境特效
    switch (currentLevel.weather) {
        case WeatherType::RAIN:
            DrawText("WEATHER: HEAVY RAIN (Food status drops faster!)", 10, 70, 16, BLUE);
            // 以后可以在这里加动态下雨特效
            break;
        case WeatherType::NIGHT:
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 20, 120 }); // 黑色半透明夜幕
            DrawText("WEATHER: NIGHT (Limited Visibility!)", 10, 70, 16, PURPLE);
            break;
        default:
            break;
    }

    // 绘制当前关卡的所有物体
    for (const auto& obj : currentLevel.objects) {
        float screenX = obj.worldX - worldScrollOffset;
        if (screenX + obj.width > 0 && screenX < screenWidth) {

            // 1. 声明与 Player 相同的视觉透视偏移量
            float perspectiveOffsetY = 40.0f;
            float screenY = groundY - obj.height + perspectiveOffsetY;;
            DrawRectangle(screenX, screenY, obj.width, obj.height, obj.color);
            
            if (obj.type == 1) DrawText("SKATES", screenX, screenY - 15, 10, GOLD);
            if (obj.type == 2) DrawText("DRONE", screenX, screenY - 15, 10, BLUE);
        }
    }

    // 绘制终点线
    float finishScreenX = currentLevel.maxDistance - worldScrollOffset;
    if (finishScreenX > 0 && finishScreenX < screenWidth) {
        DrawRectangle(finishScreenX, 0, 20, groundY, GREEN);
        DrawText("FINISH", finishScreenX - 15, 20, 12, GREEN);
    }
}

bool LevelManager::CheckWin(float worldScrollOffset) {
    return worldScrollOffset >= currentLevel.maxDistance;
}