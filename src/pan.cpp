// main.cpp (regresado a la versión “simple”)
// 4 funciones principales: clampU8, LerpColor, AlphaBlend, BlitScaledBilinear
// Bucle while minimal: clear -> blit -> UpdateTexture -> DrawTexturePro
/*
#include "framebuffer.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

// Ventana
static constexpr int WIN_W = 1900;
static constexpr int WIN_H = 950;

// Framebuffer interno
static constexpr int FB_W = 2500;
static constexpr int FB_H = 1250;

// ---------- Helpers ----------
static inline unsigned char clampU8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (unsigned char)v;
}

static inline Color LerpColor(Color a, Color b, float t) {
    int r  = (int)(a.r + (b.r - a.r) * t);
    int g  = (int)(a.g + (b.g - a.g) * t);
    int b2 = (int)(a.b + (b.b - a.b) * t);
    int a2 = (int)(a.a + (b.a - a.a) * t);
    return Color{ clampU8(r), clampU8(g), clampU8(b2), clampU8(a2) };
}

// Alpha blend: src sobre dst
static inline Color AlphaBlend(Color dst, Color src) {
    if (src.a == 255) return src;
    if (src.a == 0)   return dst;

    float s = src.a / 255.0f;               // alpha de src 0..1
    int r = (int)(dst.r * (1.0f - s) + src.r * s);
    int g = (int)(dst.g * (1.0f - s) + src.g * s);
    int b = (int)(dst.b * (1.0f - s) + src.b * s);
    int a = (int)(dst.a * (1.0f - s) + src.a);

    return Color{ clampU8(r), clampU8(g), clampU8(b), clampU8(a) };
}

// Escalado BILINEAR del sprite
static void BlitScaledBilinear(Framebuffer& fb,
                               const Color* src, int sw, int sh,
                               int dstX, int dstY, int dstW, int dstH)
{
    if (!src || dstW <= 0 || dstH <= 0 || sw <= 0 || sh <= 0) return;

    // Clip contra framebuffer
    int x0 = std::max(0, dstX);
    int y0 = std::max(0, dstY);
    int x1 = std::min(fb.w, dstX + dstW);
    int y1 = std::min(fb.h, dstY + dstH);
    if (x0 >= x1 || y0 >= y1) return;

    // Map destino->origen usando (dstW-1) y (sh-1) como tu estilo original
    for (int y = y0; y < y1; ++y) {
        int dy = y - dstY;

        float v  = (dstH == 1) ? 0.0f : (float)dy / (float)(dstH - 1);
        float sy = v * (sh - 1);

        int yA = (int)sy;
        int yB = (yA + 1 < sh) ? (yA + 1) : yA;
        float ty = sy - yA;

        for (int x = x0; x < x1; ++x) {
            int dx = x - dstX;

            float u  = (dstW == 1) ? 0.0f : (float)dx / (float)(dstW - 1);
            float sx = u * (sw - 1);

            int xA = (int)sx;
            int xB = (xA + 1 < sw) ? (xA + 1) : xA;
            float tx = sx - xA;

            // 4 muestras
            Color c00 = src[yA * sw + xA];
            Color c10 = src[yA * sw + xB];
            Color c01 = src[yB * sw + xA];
            Color c11 = src[yB * sw + xB];

            // bilinear
            Color top  = LerpColor(c00, c10, tx);
            Color bot  = LerpColor(c01, c11, tx);
            Color samp = LerpColor(top, bot, ty);

            if (samp.a == 0) continue;

            Color& dst = fb.pix[(size_t)y * fb.w + x];
            dst = AlphaBlend(dst, samp);
        }
    }
}

int main() {
    InitWindow(WIN_W, WIN_H, "Cheese - Framebuffer Simple");
    SetTargetFPS(60);

    Framebuffer fb(FB_W, FB_H);

    // Cargar PNG
    Image dragonImg = LoadImage("assets/dragon.png");
    if (dragonImg.data == nullptr) {
        TraceLog(LOG_ERROR, "No se pudo cargar assets/dragon.png");
        CloseWindow();
        return 1;
    }

    // Asegurar RGBA8 para leer como Color
    ImageFormat(&dragonImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    int sw = dragonImg.width;
    int sh = dragonImg.height;
    Color* pixels = (Color*)dragonImg.data;

    // Imagen que apunta al framebuffer
    Image fbImg{};
    fbImg.data = fb.pix.data();
    fbImg.width = FB_W;
    fbImg.height = FB_H;
    fbImg.mipmaps = 1;
    fbImg.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    Texture2D tex = LoadTextureFromImage(fbImg);

    // ---- parámetros para dibujar ----
    int dstW = 600;
    int dstH = 494;
    int x = FB_W / 2 - dstW / 2;
    int y = FB_H / 2 - dstH / 2;

    while (!WindowShouldClose()) {
        x -= 6;
        if(x < 0 - dstW / 2){
            x = FB_W - dstW / 2;
        }
        fb.clear(Color{16, 16, 24, 255});

        // dibujar dragón escalado
        BlitScaledBilinear(fb, pixels, sw, sh, x, y, dstW, dstH);

        UpdateTexture(tex, fb.pix.data());

        BeginDrawing();
        ClearBackground(BLACK);

        // Mostrar el framebuffer en la ventana (fit + center, sin deformar)
        float sx = (float)WIN_W / (float)FB_W;
        float sy = (float)WIN_H / (float)FB_H;
        float scale = (sx < sy) ? sx : sy;

        int outW = (int)(FB_W * scale);
        int outH = (int)(FB_H * scale);
        int offX = (WIN_W - outW) / 2;
        int offY = (WIN_H - outH) / 2;

        Rectangle srcRect{ 0, 0, (float)FB_W, (float)FB_H };
        Rectangle dstRect{ (float)offX, (float)offY, (float)outW, (float)outH };

        DrawTexturePro(tex, srcRect, dstRect, Vector2{0,0}, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadTexture(tex);
    UnloadImage(dragonImg);
    CloseWindow();
    return 0;
}
*/
/*
g++ src\main.cpp src\framebuffer.cpp -I src -o app.exe -lraylib -lopengl32 -lgdi32 -lwinmm
*/

// src/main.cpp
// src/main.cpp
#include "framebuffer.h"
#include "raylib.h"
#include <algorithm>   // std::min
#include <cmath>
#include <iostream>

static constexpr int windowWidth  = 1400;
static constexpr int windowHeight = 700;

int main() {
    InitWindow(windowWidth, windowHeight, "Framebuffer PNG");
    SetTargetFPS(60);
    Framebuffer fb(windowWidth, windowHeight);
    Image hidranteImg = LoadImage("assets/hidrante.png");
    if (hidranteImg.data == nullptr) {
        TraceLog(LOG_ERROR, "No se pudo cargar assets/hidrante.png");
        CloseWindow();
        return 1;
    }
    Color* hidrantePixels = LoadImageColors(hidranteImg);
    if (!hidrantePixels) {
        TraceLog(LOG_ERROR, "No se pudieron extraer los pixeles del PNG");
        UnloadImage(hidranteImg);
        CloseWindow();
        return 1;
    }
    Image fbImg{};
    fbImg.data = fb.pix.data();
    fbImg.width = fb.w;
    fbImg.height = fb.h;
    fbImg.mipmaps = 1;
    fbImg.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    Texture2D fbTex = LoadTextureFromImage(fbImg);
    while (!WindowShouldClose()) {
        fb.clear(BLACK);
        for (int y = 0; y < hidranteImg.height; y++) {
            for (int x = 0; x < hidranteImg.width; x++) {
                if(hidrantePixels[y * hidranteImg.width + x].a != 0){
                    fb.pix[y * hidranteImg.width + x] = hidrantePixels[y * hidranteImg.width + x];
                }
            }
        }
        UpdateTexture(fbTex, fb.pix.data());
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(fbTex, 0, 0, WHITE);
        EndDrawing();
    }
    UnloadTexture(fbTex);
    UnloadImageColors(hidrantePixels);
    UnloadImage(hidranteImg);
    CloseWindow();
    return 0;
}
