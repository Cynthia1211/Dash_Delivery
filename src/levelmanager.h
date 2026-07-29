#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include "entities/GameObject.h"
#include "entities/Player.h"


// 1. 定义天气类型
enum class WeatherType { SUNNY, RAIN, WINDY, NIGHT };

// 2. 定义障碍物与道具的数据结构
struct LevelObject {
    float worldX;     // 世界绝对坐标 X
    float width;      // 宽度
    float height;     // 高度（如果是道具，可以代表它离地面的高度）
    int type;         // 0: 普通障碍方块, 1: 旱冰鞋, 2: 无人机, 3: 流氓, 4: 恶猫
    Color color;      // 临时测试颜色
};

// 3. 核心：关卡配置结构体
struct LevelConfig {
    int levelNumber;          // 关卡编号（1, 2, 3）
    float maxDistance;        // 关卡总长度（快递送达的终点线）
    float backScrollSpeed;    // 远景滚动系数（例如第一关 0.1，后面越来越快）
    float foreScrollSpeed;    // 近景滚动系数（例如第一关 0.5）
    WeatherType weather;      // 当前关卡的天气
    float countdownTimer;     // 关卡倒计时时间（秒）
    std::vector<std::shared_ptr<GameObject>> objects;
};


class LevelManager {
public:
    // 远景和近景纹理
    Texture2D backTexture;
    Texture2D foreTexture;
    Texture2D roadblockTexture;
    Texture2D skatesTexture;
    Texture2D catTexture;       // <-- 【新增】猫的贴图
    Texture2D gangsterTexture;  // <-- 【新增】流氓的贴图
    Texture2D couponTexture;    // <-- 【新增】优惠券的贴图

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

    // 【新增】当前正在运行的关卡数据
    LevelConfig currentLevel;

    // 构造函数
    LevelManager(int sWidth, int sHeight, float gY);
    
    // 加载外部背景素材
    void LoadAssets();
    
    // 卸载外部背景素材
    void UnloadAssets();
    
    // 用 switch-case 实现的核心函数
    void SetupLevel(int levelNumber);
    void Draw(float worldScrollOffset);
    bool CheckWin(float worldScrollOffset);
    void UpdateCountdown(float deltaTime); // 倒计时递减
    void AddCountdownTime(float seconds); // 增加倒计时时间（供Coupon使用）
    
    void DrawHUD(const Player& player, float worldScrollOffset);
};
