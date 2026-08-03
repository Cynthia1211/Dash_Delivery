#include "levelmanager.h"
#include <cmath>
#include <random>
#include "entities/Roadblock.h"
#include "entities/Skates.h"
#include "entities/Coupon.h"
#include "entities/DeliveryDrone.h"
#include "entities/StrayCat.h"
#include "entities/StreetGangster.h" 
#include "entities/BlackTechBox.h" 

LevelManager::LevelManager(int sWidth, int sHeight, float gY) {
    screenWidth = sWidth;
    screenHeight = sHeight;
    groundY = gY;

    // Initialize score system
    currentLevelScore = 0.0f;
    totalScore = 0.0f;
    levelsCompleted = 0;

    // Initialize filter colors for parallax backgrounds
    backTint = { 200, 200, 200, 100 };
    foreTint = { 255, 255, 255, 255 };
    
    // Initialize C++11 random number generator
    std::random_device rd;
    rng = std::mt19937(rd());
}

void LevelManager::LoadAssets() {
    // Load all texture assets for game objects
    backTexture = LoadTexture("../assets/background.png");
    foreTexture = LoadTexture("../assets/foreground.png");
    roadblockTexture = LoadTexture("../assets/roadblock.png");
    skatesTexture = LoadTexture("../assets/skates.png");
    droneTexture = LoadTexture("../assets/deliveryDrone.png");
    catTexture = LoadTexture("../assets/cat.png");
    gangsterTexture = LoadTexture("../assets/gangster.png");
    couponTexture = LoadTexture("../assets/coupon.png"); 
    blackBoxTexture = LoadTexture("../assets/blackBox.png"); 

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
    // Unload all texture assets
    UnloadTexture(backTexture);
    UnloadTexture(foreTexture);
    UnloadTexture(roadblockTexture);
    UnloadTexture(skatesTexture);
    UnloadTexture(droneTexture); 
    UnloadTexture(catTexture); 
    UnloadTexture(gangsterTexture);
    UnloadTexture(couponTexture); 
    UnloadTexture(blackBoxTexture); 
}

// Core level setup function: configures level based on level number
// Before loading new level, clear all objects from previous level
void LevelManager::SetupLevel(int levelNumber) {
    currentLevel.objects.clear();
    currentLevel.levelNumber = levelNumber;

    switch (levelNumber) {
        case 1: {
            // ------- Level 1 Configuration -------
            // Distance: 2000; Time: 90s; Food decays: 1/100 per second
            // Enemy: Roadblocks at 500m, 1100m, 1900m
            // Power-up: Randomly put skates, coupon, blackTechBox at 800m, 1500m
            currentLevel.maxDistance = 2000.0f;
            currentLevel.countdownTimer = 90.0f;
            currentLevel.foodDecayRate = 1.0f/100.0f;
            
            const float groundObjY = groundY - 50.0f;
            for (float x = 500.0f; x < 2000.0f; x += 600.0f) {
                currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY, roadblockTexture));
            }
            
            const float airObjY = GameObject::GetVerticalMiddleY(30.0f, screenHeight);
            std::uniform_int_distribution<int> airObjDist(0, 2);
            for (float x = 800.0f; x < 2000.0f; x += 700.0f) {
                int randomChoice = airObjDist(rng);
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<Skates>(x, airObjY, skatesTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<Coupon>(x, airObjY, couponTexture, *this, 10.0f));
                } else {
                    currentLevel.objects.push_back(std::make_shared<BlackTechBox>(x, airObjY, blackBoxTexture));
                }
            }
            break;
        }

        case 2: {
            // ------- Level 2 Configuration -------
            // Distance: 5000; Time: 40s; Food decays: 1/100 per second
            // Enemy: Randomly put straycat, streetgangster, roadblock every 600m start from 500m
            // Power-up: Randomly put skates, coupon, blackTechBox every 700m start from 800m
            //           delivery drone at 3300m - airborne power-up
            currentLevel.maxDistance = 5000.0f; 
            currentLevel.countdownTimer = 40.0f;
            currentLevel.foodDecayRate = 1.0f/100.0f;
            
            const float groundObjY2 = groundY - 50.0f;
            std::uniform_int_distribution<int> groundObjDist(0, 2);
            for (float x = 500.0f; x < 5000.0f; x += 600.0f) {
                int randomChoice = groundObjDist(rng);
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<StrayCat>(x, groundObjY2, catTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<StreetGangster>(x, groundObjY2, gangsterTexture));
                } else {
                    currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY2, roadblockTexture));
                }
            }
            
            const float airObjY2 = GameObject::GetVerticalMiddleY(30.0f, screenHeight);            
            std::uniform_int_distribution<int> airObjDist(0, 2);
            for (float x = 800.0f; x < 5000.0f; x += 700.0f) {
                int randomChoice = airObjDist(rng);
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<Skates>(x, airObjY2, skatesTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<Coupon>(x, airObjY2, couponTexture, *this, 10.0f));
                } else {
                    currentLevel.objects.push_back(std::make_shared<BlackTechBox>(x, airObjY2, blackBoxTexture));
                }
            }
            
            currentLevel.objects.push_back(std::make_shared<DeliveryDrone>(3300.0f, airObjY2, droneTexture));
            break;
            }

        case 3: {
            // ------- Level 3 Configuration -------
            // Distance: 4000; Time: 30s; Food decays: 1/70 per second
            // Enemy: Randomly put straycat, streetgangster, roadblock every 500m start from 400m
            // Power-up: Randomly put skates, coupon, blackTechBox every 700m start from 800m
            //           delivery drone at 2700m - airborne power-up
            currentLevel.maxDistance = 4000.0f;
            currentLevel.countdownTimer = 30.0f;
            currentLevel.foodDecayRate = 1.0f/70.0f;
            
            std::uniform_int_distribution<int> groundObjDist3(0, 2);
            const float groundObjY3 = groundY - 50.0f;
            for (float x = 400.0f; x < 4000.0f; x += 500.0f) {
                int randomChoice = groundObjDist3(rng);
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<StrayCat>(x, groundObjY3, catTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<StreetGangster>(x, groundObjY3, gangsterTexture));
                } else {
                    currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY3, roadblockTexture));
                }
            }
            
            const float airObjY3 = GameObject::GetVerticalMiddleY(30.0f, screenHeight);
            std::uniform_int_distribution<int> airObjDist3(0, 2);
            for (float x = 800.0f; x < 4000.0f; x += 700.0f) {
                int randomChoice = airObjDist3(rng);
                if (randomChoice == 0) {
                    currentLevel.objects.push_back(std::make_shared<Skates>(x, airObjY3, skatesTexture));
                } else if (randomChoice == 1) {
                    currentLevel.objects.push_back(std::make_shared<Coupon>(x, airObjY3, couponTexture, *this, 10.0f));
                } else {
                    currentLevel.objects.push_back(std::make_shared<BlackTechBox>(x, airObjY3, blackBoxTexture));
                }
            }
            
            currentLevel.objects.push_back(std::make_shared<DeliveryDrone>(2700.0f, airObjY3, droneTexture));
            break;
            }

        default:
            break; 
    }
}

void LevelManager::Draw(float worldScrollOffset) {

    // Background: Calculate far background scrolling
    float backBgScroll = -(worldScrollOffset * backScrollSpeed);
    backBgScroll = fmodf(backBgScroll, backRenderWidth); 
    if (backBgScroll > 0) backBgScroll -= backRenderWidth;

    // For seamless looping
    DrawTextureEx(backTexture, (Vector2){ backBgScroll, backY }, 0.0f, backScale, backTint);
    DrawTextureEx(backTexture, (Vector2){ backBgScroll + backRenderWidth, backY }, 0.0f, backScale, backTint);


    // Foreground: Calculate near background scrolling
    float foreBgScroll = -(worldScrollOffset * foreScrollSpeed);
    foreBgScroll = fmodf(foreBgScroll, foreRenderWidth);
    if (foreBgScroll > 0) foreBgScroll -= foreRenderWidth;

    // For seamless looping
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll, foreY }, 0.0f, foreScale, foreTint);
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll + foreRenderWidth, foreY }, 0.0f, foreScale, foreTint);

    // Street road
    DrawRectangle(0, groundY, screenWidth, screenHeight - groundY, GRAY);

    // Draw objects in the current level
    for (const auto& obj : currentLevel.objects) {

        // Skip rendering if item has been collected
        if (!obj->isAlive) continue;

        // Calculate screen X relative position
        float screenX = obj->worldX - worldScrollOffset;

        // Render objects within visible screen bounds
        if (screenX + obj->width > 0 && screenX < screenWidth) {

            obj->Draw(worldScrollOffset, groundY, screenHeight);

        }
    }

}

// Check the player completed
bool LevelManager::CheckWin(float worldScrollOffset) {
    return worldScrollOffset >= currentLevel.maxDistance;
}

// Update countdown timer
void LevelManager::UpdateCountdown(float deltaTime) {
    currentLevel.countdownTimer -= deltaTime;
    if (currentLevel.countdownTimer < 0.0f) {
        currentLevel.countdownTimer = 0.0f;
    }
}

// Add extra time to countdown
void LevelManager::AddCountdownTime(float seconds) {
    currentLevel.countdownTimer += seconds;
}

// Decreases player's food status over time
void LevelManager::UpdateFoodDecay(float deltaTime, Player& player) {
    player.foodStatus -= currentLevel.foodDecayRate * deltaTime * 100.0f;
    if (player.foodStatus < 0.0f) {
        player.foodStatus = 0.0f;
    }
}

// Score calculation: remaining time (seconds) * 10 * food completeness * current level number
float LevelManager::CalculateScore(float remainingTime, float foodCompleteness) {
    return remainingTime * 10.0f * foodCompleteness * (float)currentLevel.levelNumber;
}

// Status bar displays game info at top of screen
void LevelManager::DrawHUD(const Player& player, float worldScrollOffset) {
    
    float hudHeight = 80.0f;
    DrawRectangle(0, 0, screenWidth, hudHeight, (Color){ 40, 40, 40, 180 }); 
    DrawLine(0, hudHeight, screenWidth, hudHeight, (Color){ 200, 200, 200, 100 }); 

    // Level number
    DrawText(TextFormat("STAGE %d", currentLevel.levelNumber), 20, 20, 20, WHITE);

    // Level percentage
    float playerAbsoluteX = worldScrollOffset + player.pos.x;
    float progressPercent = (playerAbsoluteX / currentLevel.maxDistance) * 100.0f;
    if (progressPercent > 100.0f) progressPercent = 100.0f; 
    DrawText(TextFormat("PROGRESS: %.1f%%", progressPercent), 150, 22, 16, LIGHTGRAY);

    // Timer
    int countdownX = 380; 
    DrawText("TIME:", countdownX, 20, 16, WHITE);
    DrawText(TextFormat("%ds", (int)currentLevel.countdownTimer), countdownX + 55, 20, 18, WHITE);
    
    // Score
    DrawText(TextFormat("SCORE: %.0f", totalScore), 20, 48, 16, GOLD);
    
    // Food Status
    DrawText("FOOD:", 250, 48, 16, WHITE);
    DrawRectangle(300, 50, 150, 16, MAROON); 
    float barWidth = (player.foodStatus / 100.0f) * 150.0f;
    if (barWidth < 0) barWidth = 0;
    DrawRectangle(300, 50, (int)barWidth, 16, RED); 
    DrawText(TextFormat("%.0f%%", player.foodStatus), 350, 52, 12, WHITE);
    
    // Active Buffs
    int buffX = 480; 
    if (player.catDebuffTimer > 0) {
        DrawRectangle(buffX, 48, 100, 24, DARKPURPLE);
        DrawText(TextFormat("SPEED- %.1fs", player.catDebuffTimer), buffX + 5, 52, 10, WHITE);
        buffX += 110;
    }

    if (player.skatesTimer > 0) {
        DrawRectangle(buffX, 48, 100, 24, MAROON);
        DrawText(TextFormat("SPEED+ %.1fs", player.skatesTimer), buffX + 5, 52, 10, WHITE);
        buffX += 110;
    }
    
    if (player.shieldActive) {
        DrawRectangle(buffX, 48, 80, 24, BLUE);
        DrawText("SHIELD", buffX + 5, 52, 11, WHITE);
        buffX += 90;
    }
    
    if (player.droneTimer > 0) {
        DrawRectangle(buffX, 48, 80, 24, GOLD);
        DrawText("FLY", buffX + 5, 52, 11, BLACK);
        buffX += 90;
    }
}
