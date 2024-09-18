#include "raylib.h"
#include "game.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Blob.io");
    InitAudioDevice();

    Game game;
    InitGame(&game, screenWidth, screenHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateGame(&game, screenWidth, screenHeight);

        // game restart logic
        if ((game.state == GAME_OVER || game.state == GAME_WIN) && IsKeyPressed(KEY_ENTER)) {
            InitGame(&game, screenWidth, screenHeight);
        }

        BeginDrawing();
        DrawGame(&game);
        EndDrawing();
    }

    UnloadGame(&game);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}