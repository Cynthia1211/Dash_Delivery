#include "levelmanager.h"

LevelManager::LevelManager(int sWidth, int sHeight, float gY) {
    screenWidth = sWidth;
    screenHeight = sHeight;
    groundY = gY;

    // 初始化滤镜颜色
    backTint = { 200, 200, 200, 100 };
    foreTint = { 255, 255, 255, 255 };
    
    backScale = 0.0f;
    backRenderWidth = 0.0f;
    backY = 0.0f;
    foreScale = 0.0f;
    foreRenderWidth = 0.0f;
    foreY = 0.0f;
}

void LevelManager::LoadAssets() {
    backTexture = LoadTexture("../assets/background.png");
    foreTexture = LoadTexture("../assets/foreground.png");

    // 完美复刻原 main.cpp 中的远景计算公式
    float backHeight = screenHeight / 1.0f; 
    backScale = backHeight / (float)backTexture.height;
    backRenderWidth = (float)backTexture.width * backScale;
    backY = groundY - backHeight;

    // 完美复刻原 main.cpp 中的近景计算公式
    float foreHeight = screenHeight / 1.3f;
    foreScale = foreHeight / (float)foreTexture.height; 
    foreRenderWidth = (float)foreTexture.width * foreScale; 
    foreY = groundY - foreHeight;
}

void LevelManager::UnloadAssets() {
    UnloadTexture(backTexture);
    UnloadTexture(foreTexture);
}

void LevelManager::Draw(float worldScrollOffset) {
    // 核心视差混滚算法公式
    float backBgScroll = -(worldScrollOffset * 0.1f);
    backBgScroll = (float)((int)backBgScroll % screenWidth);

    float foreBgScroll = -(worldScrollOffset * 0.5f);
    foreBgScroll = (float)((int)foreBgScroll % screenWidth);

    // A. 绘制远景平铺
    for (float xOffset = backBgScroll; xOffset < screenWidth; xOffset += backRenderWidth) {
        DrawTextureEx(backTexture, (Vector2){ xOffset, backY }, 0.0f, backScale, backTint);
    }

    // B. 绘制近景平铺
    for (float xOffset = foreBgScroll; xOffset < screenWidth; xOffset += foreRenderWidth) {
        DrawTextureEx(foreTexture, (Vector2){ xOffset, foreY }, 0.0f, foreScale, foreTint);
    }

    // C. 绘制灰色实心大矩形地面
    DrawRectangle(0, groundY, screenWidth, screenHeight - groundY, GRAY);
}