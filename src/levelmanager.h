#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include <random>
#include "entities/GameObject.h"
#include "entities/Player.h"


// 2. Define obstacle and item data structure
struct LevelObject {
    float worldX;     // World absolute X coordinate
    float width;      // Width
    float height;     // Height (for items, represents height above ground)
    int type;         // 0: Normal obstacle, 1: Skates, 2: Drone, 3: Gangster, 4: Stray cat
    Color color;      // Temporary test color
};

// 3. Core: Level configuration structure
struct LevelConfig {
    int levelNumber;          // Level number (1, 2, 3)
    float maxDistance;        // Total level distance (finish line for delivery)
    float countdownTimer;     // Level countdown timer (seconds)
    float foodDecayRate;      // Food status decay rate per second (e.g., 1/90, 1/60, 1/40)
    float initialTimer;       // Initial level countdown time (for calculating remaining time ratio)
    std::vector<std::shared_ptr<GameObject>> objects;
};


class LevelManager {
public:
    // Score system
    float currentLevelScore;    // Current level score
    float totalScore;           // Total score (sum of all completed level scores)
    int levelsCompleted;        // Number of completed levels

    // C++11 random number generator
    std::mt19937 rng;

    // Far and near background textures
    Texture2D backTexture;
    Texture2D foreTexture;
    Texture2D roadblockTexture;
    Texture2D skatesTexture;
    Texture2D catTexture;       // Stray cat texture
    Texture2D gangsterTexture;  // Street gangster texture
    Texture2D couponTexture;    // Coupon item texture
    Texture2D blackBoxTexture;  // Black tech box texture
    Texture2D droneTexture;     // Delivery drone item texture

    // Screen and rendering dimension parameters
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

    // Parallax scroll speeds (global fixed values)
    float backScrollSpeed;
    float foreScrollSpeed;

    // Currently running level data
    LevelConfig currentLevel;

    // Constructor
    LevelManager(int sWidth, int sHeight, float gY);
    
    // Set global parallax scroll speeds
    void SetParallaxScrollSpeed(float backSpeed, float foreSpeed)
    {
        backScrollSpeed = backSpeed;
        foreScrollSpeed = foreSpeed;
    }
    
    // Load external background assets
    void LoadAssets();
    
    // Unload external background assets
    void UnloadAssets();
    
    // Core functions implemented with switch-case
    void SetupLevel(int levelNumber);
    void Draw(float worldScrollOffset);
    bool CheckWin(float worldScrollOffset);
    void UpdateCountdown(float deltaTime); // Decrease countdown timer
    void AddCountdownTime(float seconds); // Add time to countdown (used by Coupon)
    void UpdateFoodDecay(float deltaTime, Player& player); // Decrease food status
    
    void DrawHUD(const Player& player, float worldScrollOffset);
    
    // Score calculation function
    float CalculateScore(float remainingTime, float foodCompleteness);
    
    // Get random integer (0 to max-1)
    int GetRandomInt(int max) {
        std::uniform_int_distribution<int> dist(0, max - 1);
        return dist(rng);
    }
};
