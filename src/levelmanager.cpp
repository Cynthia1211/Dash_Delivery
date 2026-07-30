#include "levelmanager.h"
#include <cmath>
#include "entities/Roadblock.h"
#include "entities/Skates.h"
#include "entities/Coupon.h"          // <-- 【新增】优惠券道具
#include "entities/DeliveryDrone.h"   // <-- 【新增】配送无人机道具
#include "entities/StrayCat.h"       // <-- 【新增】
#include "entities/StreetGangster.h" // <-- 【新增】
#include "entities/BlackTechBox.h"   // <-- 【新增】黑市技术盒

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
    droneTexture = LoadTexture("../assets/deliveryDrone.png");   // 【新增】无人机贴图
    catTexture = LoadTexture("../assets/cat.png");               // <-- 【新增】
    gangsterTexture = LoadTexture("../assets/gangster.png");
    couponTexture = LoadTexture("../assets/coupon.png");         // <-- 【新增】优惠券贴图
    blackBoxTexture = LoadTexture("../assets/blackBox.png");       // 黑市技术盒贴图

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
    UnloadTexture(droneTexture);     // 【新增】无人机贴图
    UnloadTexture(catTexture);       // <-- 【新增】
    UnloadTexture(gangsterTexture);
    UnloadTexture(couponTexture);    // <-- 【新增】优惠券贴图
    UnloadTexture(blackBoxTexture);  // 黑市技术盒贴图
}

// 【核心改动】爽快的 switch-case 结构
void LevelManager::SetupLevel(int levelNumber) {
    currentLevel.objects.clear(); // 切换关卡前，必须先把上一关的物体清空
    currentLevel.levelNumber = levelNumber;

    switch (levelNumber) {
        case 1: {
            // ------- 第一关配置 -------
            currentLevel.maxDistance = 2000.0f;     // 关卡总长度2000米
            currentLevel.countdownTimer = 90.0f;   // 第一关：90秒倒计时
            currentLevel.foodDecayRate = 1.0f/90.0f;   // 食物每秒掉落 1/90
            
            // 每600米处生成 roadblock (600m, 1200m) — 地面道具
            const float groundObjY = groundY - 50.0f;
            for (float x = 500.0f; x < 2000.0f; x += 600.0f) {
                currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY, roadblockTexture));
            }
            
            // 每800米处随机出现 skates, coupon, blackTechbox (800m, 1600m) — 空中道具
            const float airObjY = GameObject::GetVerticalMiddleY(30.0f, screenHeight); // 30是道具高度
            for (float x = 800.0f; x < 2000.0f; x += 700.0f) {
                int randomChoice = rand() % 3;
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<Skates>(x, airObjY, skatesTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<Coupon>(x, airObjY, couponTexture, *this, 15.0f));
                } else {
                    currentLevel.objects.push_back(std::make_shared<BlackTechBox>(x, airObjY, blackBoxTexture));
                }
            }
            break;
        }

        case 2: {
            // ------- 第二关配置 -------
            currentLevel.maxDistance = 5000.0f;     // 关卡总长度5000米
            currentLevel.countdownTimer = 40.0f;   // 第二关：40秒倒计时
            currentLevel.foodDecayRate = 1.0f/60.0f;   // 食物每秒掉落 1/60
            
            const float groundObjY2 = groundY - 50.0f;
            const float airObjY2 = GameObject::GetVerticalMiddleY(30.0f, screenHeight);
            
            // 每600米处随机出现 straycat, streetgangster, roadblock (地面道具)
            for (float x = 500.0f; x < 5000.0f; x += 600.0f) {
                int randomChoice = rand() % 3;
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<StrayCat>(x, groundObjY2, catTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<StreetGangster>(x, groundObjY2, gangsterTexture));
                } else {
                    currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY2, roadblockTexture));
                }
            }
            
            // 每800米处随机出现 skates, coupon, blackTechbox (空中道具)
            for (float x = 800.0f; x < 5000.0f; x += 700.0f) {
                int randomChoice = rand() % 3;
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<Skates>(x, airObjY2, skatesTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<Coupon>(x, airObjY2, couponTexture, *this, 15.0f));
                } else {
                    currentLevel.objects.push_back(std::make_shared<BlackTechBox>(x, airObjY2, blackBoxTexture));
                }
            }
            
            // 3300米处放置 deliveryDrone (空中道具)
            currentLevel.objects.push_back(std::make_shared<DeliveryDrone>(3300.0f, airObjY2, droneTexture));
            break;
            }

        case 3: {
            // ------- 第三关配置 -------
            currentLevel.maxDistance = 4000.0f;     // 关卡总长度4000米
            currentLevel.countdownTimer = 30.0f;   // 第三关：30秒倒计时
            currentLevel.foodDecayRate = 1.0f/40.0f;   // 食物每秒掉落 1/40
            
            const float groundObjY3 = groundY - 50.0f;
            const float airObjY3 = GameObject::GetVerticalMiddleY(30.0f, screenHeight);
            
            // 每500米处随机出现 straycat, streetgangster, roadblock (地面道具)
            for (float x = 400.0f; x < 4000.0f; x += 500.0f) {
                int randomChoice = rand() % 3;
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<StrayCat>(x, groundObjY3, catTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<StreetGangster>(x, groundObjY3, gangsterTexture));
                } else {
                    currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY3, roadblockTexture));
                }
            }
            
            // 每800米处随机出现 skates, coupon, blackTechbox (空中道具)
            for (float x = 800.0f; x < 4000.0f; x += 700.0f) {
                int randomChoice = rand() % 3;
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<Skates>(x, airObjY3, skatesTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<Coupon>(x, airObjY3, couponTexture, *this, 15.0f));
                } else {
                    currentLevel.objects.push_back(std::make_shared<BlackTechBox>(x, airObjY3, blackBoxTexture));
                }
            }
            
            // 2700米处放置 deliveryDrone (空中道具)
            currentLevel.objects.push_back(std::make_shared<DeliveryDrone>(2700.0f, airObjY3, droneTexture));
            break;
            }

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
    float backBgScroll = -(worldScrollOffset * backScrollSpeed);
    // 使用 fmodf 对浮点数取模，防止转成 int 丢失精度导致卡顿
    backBgScroll = fmodf(backBgScroll, backRenderWidth); 
    if (backBgScroll > 0) backBgScroll -= backRenderWidth;

    // 绘制远景：我们只需要在当前位置画一张，再紧接着在右边贴一张，就能实现完美无缝
    DrawTextureEx(backTexture, (Vector2){ backBgScroll, backY }, 0.0f, backScale, backTint);
    DrawTextureEx(backTexture, (Vector2){ backBgScroll + backRenderWidth, backY }, 0.0f, backScale, backTint);


    // ==========================================
    // 2. 近景滚动计算（使用图片实际渲染宽度 foreRenderWidth 取模）
    // ==========================================
    float foreBgScroll = -(worldScrollOffset * foreScrollSpeed);
    foreBgScroll = fmodf(foreBgScroll, foreRenderWidth);
    if (foreBgScroll > 0) foreBgScroll -= foreRenderWidth;

    // 绘制近景：同样是连续贴两张
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll, foreY }, 0.0f, foreScale, foreTint);
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll + foreRenderWidth, foreY }, 0.0f, foreScale, foreTint);

    DrawRectangle(0, groundY, screenWidth, screenHeight - groundY, GRAY);

    // 绘制当前关卡的所有物体
    for (const auto& obj : currentLevel.objects) {
        // 1. 如果有些道具被吃了（isAlive == false），就不渲染它
        if (!obj->isAlive) continue;

        // 2. 将所有的 . 换成 -> 访问智能指针
        float screenX = obj->worldX - worldScrollOffset;

        if (screenX + obj->width > 0 && screenX < screenWidth) {

            obj->Draw(worldScrollOffset, groundY, screenHeight);

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

// 食物完整度递减函数
void LevelManager::UpdateFoodDecay(float deltaTime, Player& player) {
    player.foodStatus -= currentLevel.foodDecayRate * deltaTime * 100.0f;
    if (player.foodStatus < 0.0f) {
        player.foodStatus = 0.0f;
    }
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
    
    // 检查黑市技术盒护盾
    if (player.shieldActive) {
        DrawRectangle(buffX, 48, 110, 24, BLUE);
        DrawText("SHIELD ACTIVE", buffX + 5, 52, 11, WHITE);
        buffX += 120;
    }
}
