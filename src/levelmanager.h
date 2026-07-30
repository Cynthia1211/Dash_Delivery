#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include <random>
#include "entities/GameObject.h"
#include "entities/Player.h"


// Define item structure
struct LevelObject {
    float worldX;
    float width;
    float height;
    int type;
};

// Define level configuration structure
struct LevelConfig {
    int levelNumber;
    float maxDistance;
    float countdownTimer;
    float foodDecayRate;
    std::vector<std::shared_ptr<GameObject>> objects;
};


class LevelManager {
public:

    // Scores
    float currentLevelScore;
    float totalScore;
    int levelsCompleted;

    // Random number generator
    std::mt19937 rng;

    // Far and near background textures
    Texture2D backTexture;
    Texture2D foreTexture;
    Texture2D roadblockTexture;
    Texture2D skatesTexture;
    Texture2D catTexture; 
    Texture2D gangsterTexture; 
    Texture2D couponTexture; 
    Texture2D blackBoxTexture; 
    Texture2D droneTexture;

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

    float backScrollSpeed;
    float foreScrollSpeed;

    // Currently running level data
    LevelConfig currentLevel;

    // Initializes screen metrics, score trackers, and random number generator
    LevelManager(int sWidth, int sHeight, float gY);
    
    // Set scroll speeds
    void SetParallaxScrollSpeed(float backSpeed, float foreSpeed)
    {
        backScrollSpeed = backSpeed;
        foreScrollSpeed = foreSpeed;
    }
    
    // Loads texture files
    void LoadAssets();
    
    // Releases GPU texture memory upon game shutdown
    void UnloadAssets();
    
    // Initializes level distance, decay rates, and put obstacles/items
    void SetupLevel(int levelNumber);

    // Main render function for parallax backgrounds, street, active entities
    void Draw(float worldScrollOffset);

    // Returns true if the player's world position reaches or exceeds maxDistance
    bool CheckWin(float worldScrollOffset);

    // Decreases the current level countdown timer by deltaTime
    void UpdateCountdown(float deltaTime);
    
    // Increases countdown timer (coupon)
    void AddCountdownTime(float seconds); 
    
    // Decreases player's food status based on level food decay rate
    void UpdateFoodDecay(float deltaTime, Player& player); 
    
    // Renders status bar
    void DrawHUD(const Player& player, float worldScrollOffset);
    
    // Score calculation function
    float CalculateScore(float remainingTime, float foodCompleteness);
    
    // Get random integer (0 to max-1)
    int GetRandomInt(int max) {
        std::uniform_int_distribution<int> dist(0, max - 1);
        return dist(rng);
    }
};
