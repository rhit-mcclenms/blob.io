#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "raymath.h"

#define MAX_BLOBS 50
#define MAX_VIRUSES 5
#define INITIAL_PLAYER_SIZE 10
#define MAX_PLAYER_SIZE 100
#define SCROLL_SPEED 2
#define MAX_SCORE 5000
#define VIRUS_SPAWN_THRESHOLD 1000

typedef enum {
    GAME_INTRO,
    GAME_ACTIVE,
    GAME_OVER,
    GAME_WIN
} GameState;

typedef struct {
    Vector2 position;
    float radius;
    Color color;
    bool active;
    bool isVirus;
} Blob;

typedef struct {
    Blob player;
    Blob blobs[MAX_BLOBS];
    Blob viruses[MAX_VIRUSES];
    int score;
    float scrollOffset;
    Texture2D backgroundTexture;
    Sound eatSound;
    Sound gameOverSound;
    Sound virusSound;
    GameState state;
} Game;

void InitGame(Game *game, int screenWidth, int screenHeight);
void UpdateGame(Game *game, int screenWidth, int screenHeight);
void DrawGame(const Game *game);
void UnloadGame(Game *game);

#endif