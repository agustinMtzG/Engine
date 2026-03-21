#include "renderQueue.h"
// FRAMEBUFFER SIZE
static constexpr int windowWidth = 1800;
static constexpr int windowHeight = 900;
// IMAGE VARIABLES
int posX = 200;
int posY = 200;
int velocity = 3;
int reverseImage = 0;
float scaledImageX = 0.2f;
float scaledImageY = 0.2f;
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
    Image Img2 = LoadImage("assets/layer3.png");
    Image Img3 = LoadImage("assets/dragon.png");
    // LOAD IMAGE PIXELS
    Color* imgPixels = LoadImageColors(Img);
    Color* imgPixels2 = LoadImageColors(Img2);
    Color* imgPixels3 = LoadImageColors(Img3);
    if(Img.data == nullptr){
        TraceLog(LOG_ERROR, "ERROR AL CARGAR IMG");
        CloseWindow();
        return 1;
    }
    if(Img2.data == nullptr){
        TraceLog(LOG_ERROR, "ERROR AL CARGAR IMG2");
        CloseWindow();
        return 1;
    }
    if(Img3.data == nullptr){
        TraceLog(LOG_ERROR, "ERROR AL CARGAR IMG3");
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

    RotatingImagePipeline image(Img, imgPixels, scaledImageX, scaledImageY);
    RotatingImagePipeline image2(Img2, imgPixels2, scaledImageX, scaledImageY);
    RotatingImagePipeline image3(Img3, imgPixels3, scaledImageX, scaledImageY);

    // STRUCT RENDERER
    Renderer renderer;
    //renderer.renderQueue.reserve(20000);
    
    while(!WindowShouldClose()){
        if (degrees > 360.0f) degrees = 0.0f;
        if (IsKeyDown(KEY_M) || IsKeyDown(KEY_N)) {
            degrees += IsKeyDown(KEY_M) ? 4.0f : -4.0f;
            image.rotate(degrees);
        }
        // IMAGE MOVEMENT
        if (IsKeyDown(KEY_D)) posX += velocity;
        if (IsKeyDown(KEY_A)) posX -= velocity;
        if (IsKeyDown(KEY_S)) posY += velocity;
        if (IsKeyDown(KEY_W)) posY -= velocity;
        // IMAGE SIZE
        if(IsKeyDown(KEY_O)){
            scaledImageX += 0.01f;
            scaledImageY += 0.01f;
            image.scaleImage();
            //image.imageData(false);
            image.rotate(degrees);
        } else if (IsKeyDown(KEY_I)){
            scaledImageX -= 0.01f;
            scaledImageY -= 0.01f;
            image.scaleImage();
            //image.imageData(false);
            image.rotate(degrees);
        }
        // TOGGLE IMAGE MARGIN
        if (IsKeyPressed(KEY_SPACE)) boxMargin = !boxMargin;
        // IMAGE FLIPPING
        if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_L)) {
            reverseImage += IsKeyPressed(KEY_L) ? 1 : -1;
            reverseImage = (reverseImage + 4) % 4;
        }
        // DRAWING IMAGE IN SCREEN
        framebuffer.clear(Color{50, 50, 50, 255});
        renderer.beginFrame();
        renderer.submit(image, posX, posY, 2);
        renderer.submit(image2, 300, 300, 0);
        renderer.submit(image3, 400, 200, 1);
        renderer.flush(framebuffer);
        // RAYLIB DRAW FUNCTIONS
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