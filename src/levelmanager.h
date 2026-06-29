#pragma once
#include "raylib.h"

class LevelManager {
public:
    // 远景和近景纹理
    Texture2D backTexture;
    Texture2D foreTexture;

    // 屏幕和渲染尺寸参数
    int screenWidth;
    int screenHeight;
    float groundY;

    float backScale;
    float backRenderWidth;
    float backY;
    Color backTint;

    float foreScale;
    float foreRenderWidth;
    float foreY;
    Color foreTint;

    // 构造函数
    LevelManager(int sWidth, int sHeight, float gY);
    
    // 加载外部背景素材
    void LoadAssets();
    
    // 卸载外部背景素材
    void UnloadAssets();
    
    // 绘制无缝平铺的视差背景和地面
    void Draw(float worldScrollOffset);
};