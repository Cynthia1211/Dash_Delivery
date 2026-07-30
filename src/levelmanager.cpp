#include "levelmanager.h"
#include <cmath>
#include <random>
#include "entities/Roadblock.h"
#include "entities/Skates.h"
#include "entities/Coupon.h"          // Coupon item
#include "entities/DeliveryDrone.h"   // Delivery drone item
#include "entities/StrayCat.h"        // Stray cat enemy
#include "entities/StreetGangster.h"  // Street gangster enemy
#include "entities/BlackTechBox.h"    // Black tech box power-up

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
    droneTexture = LoadTexture("../assets/deliveryDrone.png");   // Delivery drone texture
    catTexture = LoadTexture("../assets/cat.png");               // Stray cat texture
    gangsterTexture = LoadTexture("../assets/gangster.png");
    couponTexture = LoadTexture("../assets/coupon.png");         // Coupon texture
    blackBoxTexture = LoadTexture("../assets/blackBox.png");       // Black tech box texture

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
    UnloadTexture(droneTexture);     // Unload drone texture
    UnloadTexture(catTexture);       // Unload cat texture
    UnloadTexture(gangsterTexture);
    UnloadTexture(couponTexture);    // Unload coupon texture
    UnloadTexture(blackBoxTexture);  // Unload black box texture
}

// Core level setup function: configures level based on level number
// Before loading new level, clear all objects from previous level
void LevelManager::SetupLevel(int levelNumber) {
    currentLevel.objects.clear(); // Clear objects before switching levels
    currentLevel.levelNumber = levelNumber;

    switch (levelNumber) {
        case 1: {
            // ------- Level 1 Configuration -------
            currentLevel.maxDistance = 2000.0f;     // Total level distance: 2000m
            currentLevel.countdownTimer = 90.0f;   // Level 1: 90-second countdown
            currentLevel.initialTimer = 90.0f;     // Save initial time
            currentLevel.foodDecayRate = 1.0f/100.0f;   // Food decays 1/90 per second
            
            // Spawn roadblocks every 600m (at 600m, 1200m) - ground objects
            const float groundObjY = groundY - 50.0f;
            for (float x = 500.0f; x < 2000.0f; x += 600.0f) {
                currentLevel.objects.push_back(std::make_shared<Roadblock>(x, groundObjY, roadblockTexture));
            }
            
            // Randomly spawn skates, coupon, blackTechBox every 800m (at 800m, 1600m) - airborne objects
            const float airObjY = GameObject::GetVerticalMiddleY(30.0f, screenHeight); // 30 is the item height for airborne objects
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
            currentLevel.maxDistance = 5000.0f;     // Total level distance: 5000m
            currentLevel.countdownTimer = 40.0f;   // Level 2: 40-second countdown
            currentLevel.initialTimer = 40.0f;     // Save initial time
            currentLevel.foodDecayRate = 1.0f/100.0f;   // Food decays 1/60 per second
            
            std::uniform_int_distribution<int> groundObjDist(0, 2);
            std::uniform_int_distribution<int> airObjDist(0, 2);
            
            const float groundObjY2 = groundY - 50.0f;
            const float airObjY2 = GameObject::GetVerticalMiddleY(30.0f, screenHeight);
            
            // Randomly spawn straycat, streetgangster, roadblock every 600m - ground objects
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
            
            // Randomly spawn skates, coupon, blackTechBox every 800m - airborne objects
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
            
            // Place delivery drone at 3300m - airborne power-up
            currentLevel.objects.push_back(std::make_shared<DeliveryDrone>(3300.0f, airObjY2, droneTexture));
            break;
            }

        case 3: {
            // ------- Level 3 Configuration -------
            currentLevel.maxDistance = 4000.0f;     // Total level distance: 4000m
            currentLevel.countdownTimer = 30.0f;   // Level 3: 30-second countdown
            currentLevel.initialTimer = 30.0f;     // Save initial time
            currentLevel.foodDecayRate = 1.0f/70.0f;   // Food decays 1/40 per second
            
            const float groundObjY3 = groundY - 50.0f;
            const float airObjY3 = GameObject::GetVerticalMiddleY(30.0f, screenHeight);
            
            // Randomly spawn straycat, streetgangster, roadblock every 500m - ground objects
            std::uniform_int_distribution<int> groundObjDist3(0, 2);
            std::uniform_int_distribution<int> airObjDist3(0, 2);
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
            
            // Randomly spawn skates, coupon, blackTechBox every 800m - airborne objects
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
            
            // Place delivery drone at 2700m - airborne power-up
            currentLevel.objects.push_back(std::make_shared<DeliveryDrone>(2700.0f, airObjY3, droneTexture));
            break;
            }

        // Future: Add level 4 configuration here if needed
        // case 4:
        //     ...
        //     break;

        default:
            break; // Unknown level number
    }
}

void LevelManager::Draw(float worldScrollOffset) {
    // ==========================================
    // 1. Draw Background: Calculate far background scrolling (using actual rendered width with modulo)
    // ==========================================
    float backBgScroll = -(worldScrollOffset * backScrollSpeed);
    // Use fmodf for floating-point modulo to prevent precision loss causing stuttering
    backBgScroll = fmodf(backBgScroll, backRenderWidth); 
    if (backBgScroll > 0) backBgScroll -= backRenderWidth;

    // Draw far background: draw one image at current position and another immediately to the right for seamless looping
    DrawTextureEx(backTexture, (Vector2){ backBgScroll, backY }, 0.0f, backScale, backTint);
    DrawTextureEx(backTexture, (Vector2){ backBgScroll + backRenderWidth, backY }, 0.0f, backScale, backTint);


    // ==========================================
    // 2. Draw Foreground: Calculate near background scrolling (using actual rendered width with modulo)
    // ==========================================
    float foreBgScroll = -(worldScrollOffset * foreScrollSpeed);
    foreBgScroll = fmodf(foreBgScroll, foreRenderWidth);
    if (foreBgScroll > 0) foreBgScroll -= foreRenderWidth;

    // Draw near background: draw two consecutive images for seamless looping
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll, foreY }, 0.0f, foreScale, foreTint);
    DrawTextureEx(foreTexture, (Vector2){ foreBgScroll + foreRenderWidth, foreY }, 0.0f, foreScale, foreTint);

    DrawRectangle(0, groundY, screenWidth, screenHeight - groundY, GRAY);

    // Draw all objects in the current level
    for (const auto& obj : currentLevel.objects) {
        // 1. Skip rendering if item has been collected (isAlive == false)
        if (!obj->isAlive) continue;

        // 2. Use -> to access smart pointer members
        float screenX = obj->worldX - worldScrollOffset;

        if (screenX + obj->width > 0 && screenX < screenWidth) {

            obj->Draw(worldScrollOffset, groundY, screenHeight);

        }
    }

    // Draw finish line
    float finishScreenX = currentLevel.maxDistance - worldScrollOffset;
    if (finishScreenX > 0 && finishScreenX < screenWidth) {
        DrawRectangle(finishScreenX, 0, 20, groundY, GREEN);
        DrawText("FINISH", finishScreenX - 15, 20, 12, GREEN);
    }
}

bool LevelManager::CheckWin(float worldScrollOffset) {
    return worldScrollOffset >= currentLevel.maxDistance;
}

// Update countdown timer: decreases the level's remaining time
void LevelManager::UpdateCountdown(float deltaTime) {
    currentLevel.countdownTimer -= deltaTime;
    if (currentLevel.countdownTimer < 0.0f) {
        currentLevel.countdownTimer = 0.0f;
    }
}

// Interface for Coupon: add extra time to countdown
void LevelManager::AddCountdownTime(float seconds) {
    currentLevel.countdownTimer += seconds;
}

// Food decay function: decreases player's food status over time
void LevelManager::UpdateFoodDecay(float deltaTime, Player& player) {
    player.foodStatus -= currentLevel.foodDecayRate * deltaTime * 100.0f;
    if (player.foodStatus < 0.0f) {
        player.foodStatus = 0.0f;
    }
}

// Score calculation function
// Algorithm: remaining time (seconds) * 10 * food completeness * current level number
float LevelManager::CalculateScore(float remainingTime, float foodCompleteness) {
    return remainingTime * 10.0f * foodCompleteness * (float)currentLevel.levelNumber;
}

// HUD (Heads-Up Display) drawing function: renders game info at top of screen
void LevelManager::DrawHUD(const Player& player, float worldScrollOffset) {
    // 1. Draw top semi-transparent gray background bar (width 800, height 80)
    float hudHeight = 80.0f;
    DrawRectangle(0, 0, screenWidth, hudHeight, (Color){ 40, 40, 40, 180 }); 
    DrawLine(0, hudHeight, screenWidth, hudHeight, (Color){ 200, 200, 200, 100 }); // Elegant divider line

    // --- First row content (y=20) ---
    // 2. Display current level number
    DrawText(TextFormat("STAGE %d", currentLevel.levelNumber), 20, 20, 20, WHITE);

    // 3. Calculate and display progress percentage
    float playerAbsoluteX = worldScrollOffset + player.pos.x;
    float progressPercent = (playerAbsoluteX / currentLevel.maxDistance) * 100.0f;
    if (progressPercent > 100.0f) progressPercent = 100.0f; // Cap at 100%
    
    DrawText(TextFormat("PROGRESS: %.1f%%", progressPercent), 150, 22, 16, LIGHTGRAY);

    // 4. Display countdown timer (positioned to the right of progress bar)
    int countdownX = 380; // Position for countdown display
    DrawText("TIME:", countdownX, 20, 16, WHITE);
    
    // Change color based on remaining time
    Color timeColor = LIME;
    if (currentLevel.countdownTimer <= 10.0f) {
        timeColor = RED; // Turn red in last 10 seconds
    } else if (currentLevel.countdownTimer <= 30.0f) {
        timeColor = GOLD; // Turn gold in last 30 seconds
    }
    DrawText(TextFormat("%ds", (int)currentLevel.countdownTimer), countdownX + 55, 20, 18, timeColor);
    
    // --- Second row content (y=48) ---
    // 5. Display total score
    DrawText(TextFormat("SCORE: %.0f", totalScore), 20, 48, 16, GOLD);
    
    // 6. Display Food Status bar (positioned below first row)
    DrawText("FOOD:", 250, 48, 16, WHITE);
    DrawRectangle(300, 50, 150, 16, MAROON); // Dark red health bar background
    
    float barWidth = (player.foodStatus / 100.0f) * 150.0f;
    if (barWidth < 0) barWidth = 0;
    DrawRectangle(300, 50, (int)barWidth, 16, RED); // Bright red current health
    
    DrawText(TextFormat("%.0f%%", player.foodStatus), 350, 52, 12, WHITE);
    
    // 7. Display active power-up Buffs (positioned to the right of food bar)
    int buffX = 480; // Starting X coordinate for Buff indicators
    
    // Check stray cat debuff (speed reduction) timer
    if (player.catDebuffTimer > 0) {
        DrawRectangle(buffX, 48, 100, 24, DARKPURPLE);
        DrawText(TextFormat("SPEED- %.1fs", player.catDebuffTimer), buffX + 5, 52, 10, WHITE);
        buffX += 110;
    }
    
    // Check skates power-up timer
    if (player.skatesTimer > 0) {
        DrawRectangle(buffX, 48, 100, 24, MAROON);
        DrawText(TextFormat("SPEED+ %.1fs", player.skatesTimer), buffX + 5, 52, 10, WHITE);
        buffX += 110;
    }
    
    // Check black tech box shield status
    if (player.shieldActive) {
        DrawRectangle(buffX, 48, 80, 24, BLUE);
        DrawText("SHIELD", buffX + 5, 52, 11, WHITE);
        buffX += 90;
    }
    
    // Check drone power-up timer
    if (player.droneTimer > 0) {
        DrawRectangle(buffX, 48, 80, 24, GOLD);
        DrawText("FLY", buffX + 5, 52, 11, BLACK);
        buffX += 90;
    }
}
