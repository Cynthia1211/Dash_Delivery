#include "raylib.h"
#include "entities/Player.h"
#include "levelmanager.h"

// Background music
Music backgroundMusic;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const float GRAVITY = 1500.0f;     
const float JUMP_FORCE = -600.0f;  
const float MOVE_SPEED = 300.0f;  
const float GROUND_Y = 350.0f;

// Global parallax scroll speeds
const float BACK_SCROLL_SPEED = 0.1f;
const float FORE_SCROLL_SPEED = 0.5f;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dash Delivery - Group3");
    SetTargetFPS(60);

    // Initialize audio device
    InitAudioDevice();

    // Load and loop background music
    backgroundMusic = LoadMusicStream("../assets/backgroundSond.mp3");
    PlayMusicStream(backgroundMusic);

    // Instantiate player module
    Player player;
    player.texture = LoadTexture("../assets/player.png");

    // Track current stage number
    int currentStage = 1;

    // Define game states: 0 = playing, 1 = level transition, 2 = completed, 3 = failed
    int gameState = 0; 

    // Timer for transition screen display
    float stageTransitionTimer = 0.0f;

    LevelManager level(SCREEN_WIDTH, SCREEN_HEIGHT, GROUND_Y);
    level.SetParallaxScrollSpeed(BACK_SCROLL_SPEED, FORE_SCROLL_SPEED); 
    level.LoadAssets();
    level.SetupLevel(currentStage);

    const float CAMERA_TRIGGER_X = SCREEN_WIDTH / 3.0f; 
    float worldScrollOffset = 0.0f; 

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Update background music stream
        UpdateMusicStream(backgroundMusic);

        // Main gameplay state
        if (gameState == 0) {


            float deltaTime = GetFrameTime();

            // Update player setup and food status
            player.UpdateTimers(deltaTime);
            level.UpdateCountdown(deltaTime); 
            level.UpdateFoodDecay(deltaTime, player);

            player.HandleInput(MOVE_SPEED, JUMP_FORCE);
            player.UpdatePhysics(deltaTime, GRAVITY, GROUND_Y);

            // Player's potential next X position in the current frame
            float nextPlayerX = player.pos.x + player.velocity.x * deltaTime;

            // Maximum scroll distance limit for current level
            float maxScrollLimit = level.currentLevel.maxDistance - SCREEN_WIDTH;

            // Only scroll when map hasn't reached end AND player passes 1/3 
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

            // Clamp player to screen edges
            if (player.pos.x - player.radius < 0) player.pos.x = player.radius;
            if (player.pos.x + player.radius > SCREEN_WIDTH) player.pos.x = SCREEN_WIDTH - player.radius;

            // Collision detection (AABB using world coordinates)
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

            // If food status reached zero -> mission failed
            if (player.foodStatus <= 0.0f) {
                player.foodStatus = 0.0f;
                gameState = 3;
            }

            // If countdown reached zero -> mission failed
            if (level.currentLevel.countdownTimer <= 0.0f) {
                gameState = 3;
            }

            // If player reached the end
            float playerWorldX = pWorldX;
            if (playerWorldX + player.radius >= level.currentLevel.maxDistance) {
                player.shieldActive = false;  
                
                // Calculate current level score
                float foodCompleteness = player.foodStatus / 100.0f; 
                level.currentLevelScore = level.CalculateScore(level.currentLevel.countdownTimer, foodCompleteness);
                level.totalScore += level.currentLevelScore;
                level.levelsCompleted++;
                
                if (currentStage < 3) {

                    // Wait for 3s to start next level
                    gameState = 1; 
                    stageTransitionTimer = 3.0f; 
                } else {

                    // Completed all 3 levels -> game finished
                    gameState = 2;
                }
            }
        } 
        // Stage transition state
        else if (gameState == 1) {

            // Countdown transition timer
            stageTransitionTimer -= deltaTime;
            
            if (stageTransitionTimer <= 0.0f) {

            // Reset player state for next level
            currentStage++;
            worldScrollOffset = 0.0f;
            player.pos.x = 100.0f;
            player.pos.y = GROUND_Y - player.radius;
            player.velocity = { 0.0f, 0.0f }; 
            player.isGrounded = true;
            player.foodStatus = 100.0f;       
            player.shieldActive = false; 
            player.skatesTimer = 0.0f;
            player.droneTimer = 0.0f;
            player.catDebuffTimer = 0.0f;
            
            // Setup next level configuration and resume playing state
            level.SetupLevel(currentStage);  
                gameState = 0;  
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE); 

        // Render background, ground and player
        level.Draw(worldScrollOffset);
        player.Draw();

        // Display different UI text overlays based on game state
        if (gameState == 0) {

            // Draw status bar
            level.DrawHUD(player, worldScrollOffset);
        } 
        // Render stage clear overlay
        else if (gameState == 1) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
            DrawText(TextFormat("STAGE %d CLEAR!", currentStage), 260, 140, 40, GREEN);
            DrawText(TextFormat("Stage Score: %.0f", level.currentLevelScore), 280, 190, 24, GOLD);
            DrawText(TextFormat("Total Score: %.0f", level.totalScore), 280, 230, 24, GOLD);
            DrawText(TextFormat("Next Stage starts in %d seconds...", (int)stageTransitionTimer), 240, 280, 20, WHITE);
        } 
        // Render game completion overlay
        else if (gameState == 2) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
            DrawText("CONGRATULATIONS!", 180, 120, 40, GOLD);
            DrawText("You Delivered All Orders On Time!", 200, 170, 20, WHITE);
            DrawText(TextFormat("FINAL SCORE: %.0f", level.totalScore), 250, 230, 32, GOLD);
            DrawText(TextFormat("Levels Completed: %d/3", level.levelsCompleted), 290, 280, 20, WHITE);
        }
        // Render game over overlay
        else if (gameState == 3) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
            DrawText("MISSION FAILED", 210, 140, 40, RED);
            DrawText("Food status dropped to 0%!", 240, 190, 20, WHITE);
            DrawText(TextFormat("Final Score: %.0f", level.totalScore), 280, 230, 24, GOLD);
            DrawText("Press R to Restart", 290, 270, 20, LIGHTGRAY);

            if (IsKeyPressed(KEY_R)) {
                // Restart game
                currentStage = 1;
                worldScrollOffset = 0.0f;
                player.pos = { 100.0f, GROUND_Y - player.radius };
                player.velocity = { 0.0f, 0.0f };
                player.isGrounded = true;
                player.foodStatus = 100.0f;
                player.skatesTimer = 0.0f;
                player.droneTimer = 0.0f;
                player.catDebuffTimer = 0.0f;
                player.shieldActive = false; 
                level.totalScore = 0.0f;  
                level.levelsCompleted = 0;
                level.currentLevelScore = 0.0f; 
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