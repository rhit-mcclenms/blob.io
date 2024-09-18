# Welcome to Blog.io!
**Made by Matt | CSSE497 Capstone Project**

This is a reimagined version of the famous Agar.io, built under a forked Raylib library. My future plans are to add OpenXR VR support. For now, enjoy this 2D recreation!


Raylib Features:
   - Graphics: Utilizes Raylib's drawing functions (DrawCircleV, DrawText, DrawRectangle) for rendering blobs, text, and UI elements.
   - Input: Uses IsKeyDown and IsKeyPressed for responsive player controls.
   - Audio: Implements sound effects with LoadSound and PlaySound for eating, game over, and virus collision.
   - Textures: Creates a scrolling background with LoadTextureFromImage and DrawTextureEx.
   - Math: Leverages Raylib's Vector2 operations for smooth player movement and collision detection.

Game Logic:
   - Implements a simple game loop (InitGame, UpdateGame, DrawGame) facilitated by Raylib's window and drawing functions.
   - Uses Raylib's random number generator for blob and virus spawning.
