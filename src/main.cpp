#include "drawImage.h"
// FRAMEBUFFER SIZE
static constexpr int windowWidth = 1800;
static constexpr int windowHeight = 900;
// IMAGE VARIABLES
int posX = 200;
int posY = 200;
int velocity = 12;
int reverseImage = 0;
float scaledImageX = 0.5f;
float scaledImageY = 0.5f;
float degrees = 0.0f;
bool boxMargin = false;

int main(){
    // INICIALIZE WINDOW AND FRAMES PER SECOND
    InitWindow(windowWidth, windowHeight, "Cheese");
    SetTargetFPS(60);
    // STRUCT FRAMEBUFFER
    Framebuffer framebuffer(windowWidth, windowHeight);
    // LOAD IMAGE
    Image Img = LoadImage("assets/hidrante.png");
    if(Img.data == nullptr){
        TraceLog(LOG_ERROR, "ERROR AL CARGAR Img");
        CloseWindow();
        return 1;
    }
    // LOAD IMAGE PIXELS
    Color* imgPixels = LoadImageColors(Img);
    if(!imgPixels){
        TraceLog(LOG_ERROR, "ERROR AL CARGAR LOS PIXELES DE Img");
        UnloadImage(Img);
        CloseWindow();
        return 1;
    }

    Image framebufferImg{};
        framebufferImg.data = framebuffer.pix.data();
        framebufferImg.width = framebuffer.w;
        framebufferImg.height = framebuffer.h;
        framebufferImg.mipmaps = 1;
        framebufferImg.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    Texture2D tex = LoadTextureFromImage(framebufferImg);

    DrawImage image(Img, imgPixels, framebuffer, posX, posY, reverseImage, boxMargin, scaledImageX, scaledImageY, degrees);
    
    while(!WindowShouldClose()){
        if (degrees > 360.0f) degrees = 0.0f;
        if (IsKeyDown(KEY_M) || IsKeyDown(KEY_N)) {
            degrees += IsKeyDown(KEY_M) ? 1.0f : -1.0f;
            //image.rotate();
        }
        // IMAGE MOVEMENT
        if (IsKeyDown(KEY_D)) posX += velocity;
        if (IsKeyDown(KEY_A)) posX -= velocity;
        if (IsKeyDown(KEY_S)) posY += velocity;
        if (IsKeyDown(KEY_W)) posY -= velocity;
        // TOGGLE IMAGE MARGIN
        if (IsKeyPressed(KEY_SPACE)) boxMargin = !boxMargin;
        // IMAGE FLIPPING
        if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_L)) {
            reverseImage += IsKeyPressed(KEY_L) ? 1 : -1;
            reverseImage = (reverseImage + 4) % 4;
        }
        // DRAWING IMAGE IN SCREEN
        framebuffer.clear(Color{50, 50, 50, 255});
        image.showImage();
        UpdateTexture(tex, framebuffer.pix.data());
        BeginDrawing();
        ClearBackground(Color{50, 50, 50, 255});
        DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
    }
    // MEMORY CLEANUP
    UnloadTexture(tex);
    UnloadImageColors(imgPixels);
    UnloadImage(Img);
    CloseWindow();
    return 0;
}

/*

// COMPILAR PROYECTO
g++ src/main.cpp src/framebuffer.cpp src/drawImage.cpp -I src -o app.exe -lraylib -lopengl32 -lgdi32 -lwinmm

// FUNCIONES Y STRUCTS QUE EVENTUALMENTE TENDREMOS QUE CAMBIAR POR PROPIAS
TraceLog()

InitWindow()
CloseWindow()
SetTargetFPS()
struct Image
struct Color
struct Texture2D
LoadImage()
LoadImageColors()
LoadTextureFromImage()
IsKeyDown()
IsKeyPressed()
UpdateTexture()
BeginDrawing()
ClearBackground()
DrawTexture()
EndDrawing();
UnloadTexture()
UnloadImageColors()
UnloadImage()

*/