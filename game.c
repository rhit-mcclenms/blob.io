#include "game.h"
#include <stdlib.h>

static void SpawnBlob(Blob *blob, int screenWidth, int screenHeight, float minSize, float maxSize, bool isVirus) {
    blob->position = (Vector2){
        screenWidth + GetRandomValue(0, screenWidth / 2),
        GetRandomValue(0, screenHeight)
    };
    blob->radius = GetRandomValue(minSize, maxSize);
    blob->color = isVirus ? PURPLE : ColorFromHSV(GetRandomValue(0, 360), 0.75f, 1.0f);
    blob->active = true;
    blob->isVirus = isVirus;
}

void InitGame(Game *game, int screenWidth, int screenHeight) {
    game->player.position = (Vector2){screenWidth / 4, screenHeight / 2};
    game->player.radius = INITIAL_PLAYER_SIZE;
    game->player.color = RED;
    game->player.active = true;
    game->player.isVirus = false;

    game->score = 0;
    game->scrollOffset = 0;
    game->state = GAME_INTRO;  // start with the intro screen

    for (int i = 0; i < MAX_BLOBS; i++) {
        SpawnBlob(&game->blobs[i], screenWidth, screenHeight, 5, 30, false);
    }

    for (int i = 0; i < MAX_VIRUSES; i++) {
        game->viruses[i].active = false;
    }

    // background ui
    Image bgImage = GenImageChecked(screenWidth, screenHeight, 64, 64, LIGHTGRAY, WHITE);
    game->backgroundTexture = LoadTextureFromImage(bgImage);
    UnloadImage(bgImage);

    // sfx section
    game->eatSound = LoadSound("resources/eat.wav");
    game->gameOverSound = LoadSound("resources/gameover.wav");
    game->virusSound = LoadSound("resources/virus.wav");
}

void UpdateGame(Game *game, int screenWidth, int screenHeight) {
    if (game->state == GAME_INTRO) {
        if (IsKeyPressed(KEY_ENTER)) {
            game->state = GAME_ACTIVE;
        }
        return;
    }

    if (game->state != GAME_ACTIVE) return;

    // basic player movement
    Vector2 movement = {0};
    if (IsKeyDown(KEY_W)) movement.y -= 1;
    if (IsKeyDown(KEY_S)) movement.y += 1;
    if (IsKeyDown(KEY_A)) movement.x -= 1;
    if (IsKeyDown(KEY_D)) movement.x += 1;
    
    if (Vector2Length(movement) > 0) {
        movement = Vector2Normalize(movement);
        game->player.position = Vector2Add(game->player.position, Vector2Scale(movement, 2.0f));
    }

    // player position v2, corrected using clamps
    game->player.position.x = Clamp(game->player.position.x, game->player.radius, screenWidth - game->player.radius);
    game->player.position.y = Clamp(game->player.position.y, game->player.radius, screenHeight - game->player.radius);

    // adding to correct scrolling too fast/too slow
    game->scrollOffset += SCROLL_SPEED;
    if (game->scrollOffset >= screenWidth) {
        game->scrollOffset -= screenWidth;
    }

    // update blobs and check collisions
    for (int i = 0; i < MAX_BLOBS; i++) {
        if (game->blobs[i].active) {
            game->blobs[i].position.x -= SCROLL_SPEED;

            // changing collision from surface touch to center2center
            float distance = Vector2Distance(game->player.position, game->blobs[i].position);
            if (distance < game->player.radius) {
                if (game->player.radius > game->blobs[i].radius) {
                    // yummy time
                    game->player.radius += game->blobs[i].radius * 0.1f;
                    game->player.radius = Clamp(game->player.radius, INITIAL_PLAYER_SIZE, MAX_PLAYER_SIZE);
                    game->score += (int)game->blobs[i].radius;
                    game->blobs[i].active = false;
                    PlaySound(game->eatSound);

                    // wincon
                    if (game->score >= MAX_SCORE) {
                        game->state = GAME_WIN;
                    }
                } else {
                    // player is eaten
                    game->state = GAME_OVER;
                    PlaySound(game->gameOverSound);
                }
            }

            // adding despawn logic so my cpu fans stop space heating my room
            if (game->blobs[i].position.x + game->blobs[i].radius < 0) {
                game->blobs[i].active = false;
            }
        } else {
            // blob spawning logic
            float minSize = Clamp(game->player.radius * 0.5f, 5, 30);
            float maxSize = Clamp(game->player.radius * 1.5f, 10, 50);
            SpawnBlob(&game->blobs[i], screenWidth, screenHeight, minSize, maxSize, false);
        }
    }

    // virus spawn logic
    if (game->score >= VIRUS_SPAWN_THRESHOLD) {
        for (int i = 0; i < MAX_VIRUSES; i++) {
            if (game->viruses[i].active) {
                game->viruses[i].position.x -= SCROLL_SPEED;

                // collision check with player
                float distance = Vector2Distance(game->player.position, game->viruses[i].position);
                if (distance < game->player.radius) {
                    // player hitting virus
                    game->player.radius *= 0.7071f;  // reduce radius by approximately 50% (sqrt(0.5))
                    game->score /= 2;
                    game->viruses[i].active = false;
                    PlaySound(game->virusSound);
                }

                // despawn logic for viruses
                if (game->viruses[i].position.x + game->viruses[i].radius < 0) {
                    game->viruses[i].active = false;
                }
                // 1% virus spawnrate is unplayable, note to self: do not move above 0.5%
            } else if (GetRandomValue(0, 1000) < 2) {  // 0.2% chance to spawn a virus each frame
                SpawnBlob(&game->viruses[i], screenWidth, screenHeight, 15, 25, true);
            }
        }
    }
}

void DrawGame(const Game *game) {
    ClearBackground(RAYWHITE);

    // auto-scrolling background logic
    DrawTextureEx(game->backgroundTexture, (Vector2){-game->scrollOffset, 0}, 0.0f, 1.0f, WHITE);
    DrawTextureEx(game->backgroundTexture, (Vector2){-game->scrollOffset + GetScreenWidth(), 0}, 0.0f, 1.0f, WHITE);

    // draw blobs
    for (int i = 0; i < MAX_BLOBS; i++) {
        if (game->blobs[i].active) {
            DrawCircleV(game->blobs[i].position, game->blobs[i].radius, game->blobs[i].color);
        }
    }

    // draw viruses
    for (int i = 0; i < MAX_VIRUSES; i++) {
        if (game->viruses[i].active) {
            DrawCircleV(game->viruses[i].position, game->viruses[i].radius, game->viruses[i].color);
            // drawing V inside of virus because ik capstone partner tft buddy V will prob make a better icon than I could anyways
            DrawText("V", (int)game->viruses[i].position.x - 5, (int)game->viruses[i].position.y - 10, 20, WHITE);
        }
    }

    // draw player
    DrawCircleV(game->player.position, game->player.radius, game->player.color);

    // draw score
    DrawText(TextFormat("Score: %d", game->score), 10, 10, 20, BLACK);

    // draw intro, game over, or win screen
    if (game->state == GAME_INTRO) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        DrawText("Welcome to Blob.io! By Matt", GetScreenWidth()/2 - MeasureText("Welcome to Blob.io! By Matt", 30)/2, GetScreenHeight()/2 - 100, 30, RAYWHITE);
        DrawText("Controls: WASD to move", GetScreenWidth()/2 - MeasureText("Controls: WASD to move", 20)/2, GetScreenHeight()/2 - 20, 20, RAYWHITE);
        DrawText("Goal: Reach 5000 points to win", GetScreenWidth()/2 - MeasureText("Goal: Reach 5000 points to win", 20)/2, GetScreenHeight()/2 + 10, 20, RAYWHITE);
        DrawText("Avoid larger blobs", GetScreenWidth()/2 - MeasureText("Avoid larger blobs", 20)/2, GetScreenHeight()/2 + 40, 20, RAYWHITE);
        DrawText("Beware of viruses after 1000 points!", GetScreenWidth()/2 - MeasureText("Beware of viruses after 1000 points!", 20)/2, GetScreenHeight()/2 + 70, 20, RAYWHITE);
        DrawText("Press ENTER to start", GetScreenWidth()/2 - MeasureText("Press ENTER to start", 20)/2, GetScreenHeight()/2 + 120, 20, GREEN);
    } else if (game->state == GAME_OVER) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        DrawText("GAME OVER", GetScreenWidth()/2 - MeasureText("GAME OVER", 40)/2, GetScreenHeight()/2 - 40, 40, RED);
        DrawText("Press ENTER to restart", GetScreenWidth()/2 - MeasureText("Press ENTER to restart", 20)/2, GetScreenHeight()/2 + 20, 20, RAYWHITE);
    } else if (game->state == GAME_WIN) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        DrawText("YOU WIN!", GetScreenWidth()/2 - MeasureText("YOU WIN!", 40)/2, GetScreenHeight()/2 - 40, 40, GREEN);
        DrawText("Press ENTER to restart", GetScreenWidth()/2 - MeasureText("Press ENTER to restart", 20)/2, GetScreenHeight()/2 + 20, 20, RAYWHITE);
    }
}

void UnloadGame(Game *game) {
    UnloadTexture(game->backgroundTexture);
    UnloadSound(game->eatSound);
    UnloadSound(game->gameOverSound);
    UnloadSound(game->virusSound);
}