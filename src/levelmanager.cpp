#include "levelmanager.h"
#include <cmath>
#include "entities/Roadblock.h"
#include "entities/Skates.h"
#include "entities/Coupon.h"          // <-- 【新增】优惠券道具
#include "entities/StrayCat.h"       // <-- 【新增】
#include "entities/StreetGangster.h" // <-- 【新增】

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
    roadblockTexture = LoadTexture("../assets/roadblock.png");
    skatesTexture = LoadTexture("../assets/skates.png");
    catTexture = LoadTexture("../assets/cat.png");               // <-- 【新增】
    gangsterTexture = LoadTexture("../assets/gangster.png");
    couponTexture = LoadTexture("../assets/coupon.png");         // <-- 【新增】优惠券贴图

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
    UnloadTexture(roadblockTexture);
    UnloadTexture(skatesTexture);
    UnloadTexture(catTexture);       // <-- 【新增】
    UnloadTexture(gangsterTexture);
    UnloadTexture(couponTexture);    // <-- 【新增】优惠券贴图
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
            currentLevel.countdownTimer = 30.0f;   // 第一关：90秒倒计时
            currentLevel.objects.push_back(std::make_shared<Roadblock>(600.0f, roadblockTexture));
            currentLevel.objects.push_back(std::make_shared<Skates>(1200.0f, skatesTexture)); 
            currentLevel.objects.push_back(std::make_shared<Coupon>(900.0f, couponTexture, *this, 15.0f)); // 优惠券：增加15秒

            break;

        case 2:
            // ------- 第二关配置 -------
            currentLevel.maxDistance = 4000.0f;
            currentLevel.backScrollSpeed = 0.15f;   // 速度稍微加快
            currentLevel.foreScrollSpeed = 0.7f;
            currentLevel.weather = WeatherType::NIGHT;
            currentLevel.countdownTimer = 25.0f;   // 第二关：75秒倒计时

            // 【新写法】摆放真实的敌人
            currentLevel.objects.push_back(std::make_shared<StreetGangster>(500.0f, gangsterTexture)); // 500米处有流氓
            currentLevel.objects.push_back(std::make_shared<StrayCat>(1500.0f, catTexture));          // 1500米处有恶猫
            currentLevel.objects.push_back(std::make_shared<Skates>(2500.0f, skatesTexture));          // 2500米处有旱冰鞋
            break;

        case 3:
            // ------- 第三关配置 -------
            currentLevel.maxDistance = 6000.0f;
            currentLevel.backScrollSpeed = 0.2f;    // 极速飙车
            currentLevel.foreScrollSpeed = 0.9f;
            currentLevel.weather = WeatherType::RAIN;
            currentLevel.countdownTimer = 30.0f;   // 第三关：60秒倒计时

            // 【新写法】用循环在第三关每隔600米交替生成流氓和恶猫
            for (float x = 800.0f; x < 5500.0f; x += 600.0f) {
                if ((int)x % 1200 == 0) {
                    currentLevel.objects.push_back(std::make_shared<StreetGangster>(x, gangsterTexture));
                } else {
                    currentLevel.objects.push_back(std::make_shared<StrayCat>(x, catTexture));
                }
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
    // ==========================================
    // 1. 远景滚动计算（使用图片实际渲染宽度 backRenderWidth 取模）
    // ==========================================
    float backBgScroll = -(worldScrollOffset * currentLevel.backScrollSpeed);
    // 使用 fmodf 对浮点数取模，防止转成 int 丢失精度导致卡顿
    backBgScroll = fmodf(backBgScroll, backRenderWidth); 
    if (backBgScroll > 0) backBgScroll -= backRenderWidth;

    // 绘制远景：我们只需要在当前位置画一张，再紧接着在右边贴一张，就能实现完美无缝
    DrawTextureEx(backTexture, (Vector2){ backBgScroll, backY }, 0.0f, backScale, backTint);
    DrawTextureEx(backTexture, (Vector2){ backBgScroll + backRenderWidth, backY }, 0.0f, backScale, backTint);


    // ==========================================
    // 2. 近景滚动计算（使用图片实际渲染宽度 foreRenderWidth 取模）
    // ==========================================
    float foreBgScroll = -(worldScrollOffset * currentLevel.foreScrollSpeed);
    foreBgScroll = fmodf(foreBgScroll, foreRenderWidth);
    if (foreBgScroll > 0) foreBgScroll -= foreRenderWidth;

    // 绘制近景：同样是连续贴两张
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll, foreY }, 0.0f, foreScale, foreTint);
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll + foreRenderWidth, foreY }, 0.0f, foreScale, foreTint);

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
        // 1. 如果有些道具被吃了（isAlive == false），就不渲染它
        if (!obj->isAlive) continue;

        // 2. 将所有的 . 换成 -> 访问智能指针
        float screenX = obj->worldX - worldScrollOffset;

        if (screenX + obj->width > 0 && screenX < screenWidth) {

            obj->Draw(worldScrollOffset, groundY);

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

void LevelManager::UpdateCountdown(float deltaTime) {
    currentLevel.countdownTimer -= deltaTime;
    if (currentLevel.countdownTimer < 0.0f) {
        currentLevel.countdownTimer = 0.0f;
    }
}

// 给Coupon使用的接口：增加倒计时时间
void LevelManager::AddCountdownTime(float seconds) {
    currentLevel.countdownTimer += seconds;
}

// ======= HUD 绘制函数 =======
void LevelManager::DrawHUD(const Player& player, float worldScrollOffset) {
    // 1. 绘制顶部半透明灰色背景条 (宽度 800, 高度 80 - 加高以容纳更多内容)
    float hudHeight = 80.0f;
    DrawRectangle(0, 0, screenWidth, hudHeight, (Color){ 40, 40, 40, 180 }); 
    DrawLine(0, hudHeight, screenWidth, hudHeight, (Color){ 200, 200, 200, 100 }); // 精致分割线

    // --- 第一行内容 (y=20) ---
    // 2. 显示当前关卡
    DrawText(TextFormat("STAGE %d", currentLevel.levelNumber), 20, 20, 20, WHITE);

    // 3. 计算并显示完成路程百分比 %
    float playerAbsoluteX = worldScrollOffset + player.pos.x;
    float progressPercent = (playerAbsoluteX / currentLevel.maxDistance) * 100.0f;
    if (progressPercent > 100.0f) progressPercent = 100.0f; // 封顶 100%
    
    DrawText(TextFormat("PROGRESS: %.1f%%", progressPercent), 150, 22, 16, LIGHTGRAY);

    // 4. 显示倒计时 (放在进度条右侧)
    int countdownX = 380; // 倒计时显示位置
    DrawText("TIME:", countdownX, 20, 16, WHITE);
    
    // 根据剩余时间改变颜色
    Color timeColor = LIME;
    if (currentLevel.countdownTimer <= 10.0f) {
        timeColor = RED; // 最后10秒变红
    } else if (currentLevel.countdownTimer <= 30.0f) {
        timeColor = GOLD; // 最后30秒变金色
    }
    DrawText(TextFormat("%.1fs", currentLevel.countdownTimer), countdownX + 55, 20, 18, timeColor);
    
    // --- 第二行内容 (y=48) ---
    // 5. 显示 Food Status 食物状态血条 (放在第一行下方)
    DrawText("FOOD:", 20, 48, 16, WHITE);
    DrawRectangle(70, 50, 150, 16, MAROON); // 血条暗红底槽
    
    float barWidth = (player.foodStatus / 100.0f) * 150.0f;
    if (barWidth < 0) barWidth = 0;
    DrawRectangle(70, 50, (int)barWidth, 16, RED); // 鲜红当前血量
    DrawText(TextFormat("%.0f%%", player.foodStatus), 120, 52, 12, WHITE);
    
    // 6. 显示生效中的道具 Buff (放在食物条右侧)
    int buffX = 280; // Buff 标志起始横坐标
    
    // 检查旱冰鞋道具计时器
    if (player.skatesTimer > 0) {
        DrawRectangle(buffX, 48, 90, 24, GOLD);
        DrawText(TextFormat("SKATES %.1fs", player.skatesTimer), buffX + 5, 52, 11, BLACK);
        buffX += 100;
    }
    
    // 检查无人机道具计时器
    if (player.droneTimer > 0) {
        DrawRectangle(buffX, 48, 90, 24, BLUE);
        DrawText(TextFormat("DRONE %.1fs", player.droneTimer), buffX + 5, 52, 11, WHITE);
        buffX += 100;
    }
    
    // 检查恶猫减速效果计时器
    if (player.catDebuffTimer > 0) {
        DrawRectangle(buffX, 48, 90, 24, DARKPURPLE);
        DrawText(TextFormat("DEBUFF %.1fs", player.catDebuffTimer), buffX + 5, 52, 11, WHITE);
        buffX += 100;
    }
}
