#include "raylib.h"
#include "entities/Player.h"
#include "levelmanager.h"

// Background music resource
Music backgroundMusic;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const float GRAVITY = 1500.0f;     
const float JUMP_FORCE = -600.0f;  
const float MOVE_SPEED = 300.0f;  
const float GROUND_Y = 350.0f;

// Global parallax scroll speeds (shared across all levels)
const float BACK_SCROLL_SPEED = 0.1f;
const float FORE_SCROLL_SPEED = 0.5f;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dash Delivery - Modular Architecture");
    SetTargetFPS(60);

    // Initialize audio device
    InitAudioDevice();

    // Load and loop background music (LoadMusicStream defaults to looping)
    backgroundMusic = LoadMusicStream("../assets/backgroundSond.mp3");
    PlayMusicStream(backgroundMusic);

    // Instantiate player module
    Player player;
    player.texture = LoadTexture("../assets/player.png");

    // Track current stage number
    int currentStage = 1;

    // Define game states: 0 = playing, 1 = level transition, 2 = completed, 3 = failed
    int gameState = 0; 

    // Timer for transition screen display (e.g., show level complete for 3 seconds)
    float stageTransitionTimer = 0.0f;

    LevelManager level(SCREEN_WIDTH, SCREEN_HEIGHT, GROUND_Y);
    level.SetParallaxScrollSpeed(BACK_SCROLL_SPEED, FORE_SCROLL_SPEED); // Set global parallax scroll speeds
    level.LoadAssets();
    level.SetupLevel(currentStage);

    const float CAMERA_TRIGGER_X = SCREEN_WIDTH / 3.0f; 
    float worldScrollOffset = 0.0f; 

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Update background music stream
        UpdateMusicStream(backgroundMusic);

        // =================================================================
        // [Core Logic Control] Route game behavior based on gameState
        // =================================================================
        if (gameState == 0) {
            // ----- State 0: Normal gameplay -----

            float deltaTime = GetFrameTime();

            // 1. Update player timers first
            player.UpdateTimers(deltaTime);
            level.UpdateCountdown(deltaTime); // Update level countdown timer
            level.UpdateFoodDecay(deltaTime, player); // Update food decay

            // 2. Process player input and update physics
            player.HandleInput(MOVE_SPEED, JUMP_FORCE);
            player.UpdatePhysics(deltaTime, GRAVITY, GROUND_Y);

            // =================================================================
            // 3. Screen Locking and Scrolling Control
            // =================================================================
            float nextPlayerX = player.pos.x + player.velocity.x * deltaTime;

            // 1. Calculate max scroll limit (finish line at screen right edge)
            float maxScrollLimit = level.currentLevel.maxDistance - SCREEN_WIDTH;

            // 2. Only scroll when map hasn't reached end AND player passes 1/3 trigger line
            if (player.velocity.x > 0 && nextPlayerX >= CAMERA_TRIGGER_X && worldScrollOffset < maxScrollLimit) {
                
                // Lock player to 1/3 of screen
                player.pos.x = CAMERA_TRIGGER_X;
                
                // Scroll the world
                worldScrollOffset += player.velocity.x * deltaTime;
                
                // Clamp scroll offset to max limit
                if (worldScrollOffset > maxScrollLimit) {
                    worldScrollOffset = maxScrollLimit;
                }
            } else {
                // Player moves freely on screen when world isn't scrolling
                player.pos.x = nextPlayerX;
            }

            // 3. Clamp player to screen edges
            if (player.pos.x - player.radius < 0) player.pos.x = player.radius;
            if (player.pos.x + player.radius > SCREEN_WIDTH) player.pos.x = SCREEN_WIDTH - player.radius;

            // 4. Collision detection (AABB using world coordinates)
            float pWorldX = worldScrollOffset + player.pos.x;
            for (auto& obj : level.currentLevel.objects) {
                if (!obj->isAlive) continue;

                // X-axis check: player right edge > object left edge AND player left edge < object right edge
                bool overlapX = pWorldX + player.radius > obj->worldX &&
                                pWorldX - player.radius < obj->worldX + obj->width;
                
                // Y-axis check: use object's worldY for collision (supports airborne items)
                // Use player.radius instead of spriteHeight to avoid overly large collision boxes
                bool overlapY = player.pos.y + player.radius > obj->worldY &&
                                player.pos.y - player.radius < obj->worldY + obj->height;

                if (overlapX && overlapY) {
                    obj->OnCollision(player);
                }
            }

            // 5. Check if food status reached zero → mission failed
            if (player.foodStatus <= 0.0f) {
                player.foodStatus = 0.0f;
                gameState = 3;
            }

            // 5b. Check if countdown reached zero → mission failed
            if (level.currentLevel.countdownTimer <= 0.0f) {
                gameState = 3;
            }

            // 6. Check if player reached finish line (player's feet must touch the finish line)
            float playerWorldX = pWorldX;
            if (playerWorldX + player.radius >= level.currentLevel.maxDistance) {
                player.shieldActive = false;  // Reset shield upon level completion
                
                // Calculate current level score: remaining time * 10 * food completeness * level number
                float foodCompleteness = player.foodStatus / 100.0f; // 0-1 range (food completeness ratio)
                level.currentLevelScore = level.CalculateScore(level.currentLevel.countdownTimer, foodCompleteness);
                level.totalScore += level.currentLevelScore;
                level.levelsCompleted++;
                
                if (currentStage < 3) {
                    // Move to level transition state
                    gameState = 1; 
                    stageTransitionTimer = 3.0f; // Show hint for 3 seconds
                } else {
                    // Completed all 3 levels → game finished
                    gameState = 2;
                }
            }
        } 
        else if (gameState == 1) {
            // ----- State 1: Level transition (auto countdown switch) -----
            stageTransitionTimer -= deltaTime;
            
            if (stageTransitionTimer <= 0.0f) {
            // 3-second countdown ended, auto switch to next level
            currentStage++;
            worldScrollOffset = 0.0f;        // Reset world scroll distance

            player.pos.x = 100.0f;
            player.pos.y = GROUND_Y - player.radius;
            player.velocity = { 0.0f, 0.0f }; // Reset velocity
            player.isGrounded = true;
            player.foodStatus = 100.0f;       // Reset food status
            player.shieldActive = false;      // Reset shield
            
            level.SetupLevel(currentStage);   // Load new level data (speed, obstacles, countdown)
                gameState = 0;                    // Switch back to normal gameplay state
            }
        }

        // =================================================================
        // 6. Rendering Section
        // =================================================================
        BeginDrawing();
        ClearBackground(SKYBLUE); 

        // Render background, ground, and player regardless of game state
        level.Draw(worldScrollOffset);
        player.Draw();

        // Display different UI text overlays based on game state
        if (gameState == 0) {
            // Draw in-game HUD (score, timer, food status)
            level.DrawHUD(player, worldScrollOffset);
        } 
        else if (gameState == 1) {
            // Level complete: show semi-transparent overlay waiting for next level
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
            DrawText(TextFormat("STAGE %d CLEAR!", currentStage), 260, 140, 40, GREEN);
            DrawText(TextFormat("Stage Score: %.0f", level.currentLevelScore), 280, 190, 24, GOLD);
            DrawText(TextFormat("Total Score: %.0f", level.totalScore), 280, 230, 24, GOLD);
            DrawText(TextFormat("Next Stage starts in %d seconds...", (int)stageTransitionTimer), 240, 280, 20, WHITE);
        } 
        else if (gameState == 2) {
            // Final completion - show total score across all levels
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
            DrawText("CONGRATULATIONS!", 180, 120, 40, GOLD);
            DrawText("You Delivered All Orders On Time!", 200, 170, 20, WHITE);
            DrawText(TextFormat("FINAL SCORE: %.0f", level.totalScore), 250, 230, 32, GOLD);
            DrawText(TextFormat("Levels Completed: %d/3", level.levelsCompleted), 290, 280, 20, WHITE);
        }
        else if (gameState == 3) {
            // Mission failed
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
            DrawText("MISSION FAILED", 210, 140, 40, RED);
            DrawText("Food status dropped to 0%!", 240, 190, 20, WHITE);
            DrawText(TextFormat("Final Score: %.0f", level.totalScore), 280, 230, 24, GOLD);
            DrawText("Press R to Restart", 290, 270, 20, LIGHTGRAY);

            if (IsKeyPressed(KEY_R)) {
                // Restart game: reset all state variables
                currentStage = 1;
                worldScrollOffset = 0.0f;
                player.pos = { 100.0f, GROUND_Y - player.radius };
                player.velocity = { 0.0f, 0.0f };
                player.isGrounded = true;
                player.foodStatus = 100.0f;
                player.skatesTimer = 0.0f;
                player.droneTimer = 0.0f;
                player.catDebuffTimer = 0.0f;
                player.shieldActive = false;      // Reset shield
                level.totalScore = 0.0f;          // Reset total score
                level.levelsCompleted = 0;        // Reset levels completed
                level.currentLevelScore = 0.0f;   // Reset current level score
                level.SetupLevel(currentStage);
                gameState = 0;
            }
        }

        DrawLine(CAMERA_TRIGGER_X, 0, CAMERA_TRIGGER_X, GROUND_Y, LIGHTGRAY);

        EndDrawing();
    }
    
        // Stop and unload background music
        StopMusicStream(backgroundMusic);
        UnloadMusicStream(backgroundMusic);
    
        // Close audio device
        CloseAudioDevice();

        // Clean up resources
        UnloadTexture(player.texture);
    level.UnloadAssets();
    CloseWindow();

    return 0;
}