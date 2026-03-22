#include "engine.h"
// FRAMEBUFFER SIZE
static constexpr int windowWidth = 1800;
static constexpr int windowHeight = 900;
// IMAGE VARIABLES
int posX = 200;
int posY = 200;
int velocity = 8;
int reverseImage = 0;
float scaledImageX = 0.2f;
float scaledImageY = 0.2f;
float degrees = 0.0f;
bool boxMargin = false;
Color c{0, 0, 0, 0};

int main(){
    // INICIALIZE WINDOW AND FRAMES PER SECOND
    InitWindow(windowWidth, windowHeight, "Cheese");
    SetTargetFPS(60);
    // STRUCT FRAMEBUFFER
    Framebuffer framebuffer(static_cast<size_t>(windowWidth), static_cast<size_t>(windowHeight), c);
    // LOAD IMAGE
    Image Img = LoadImage("assets/layer3.png");
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
    ImageRenderPipeline image2(Img2, imgPixels2, scaledImageX, scaledImageY);
    ImageRenderPipeline image3(Img3, imgPixels3, scaledImageX, scaledImageY);
    
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
            //image.imageData();
            image.rotate(degrees);
        } else if (IsKeyDown(KEY_I)){
            scaledImageX -= 0.01f;
            scaledImageY -= 0.01f;
            image.scaleImage();
            //image.imageData();
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
        framebuffer.clear(c);
        image.showImage(framebuffer, posX, posY);
        image2.showImage(framebuffer, 300, 500);
        image3.showImage(framebuffer, 500, 500);
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