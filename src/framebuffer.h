#pragma once
#include <vector>
#include <algorithm>
#include "raylib.h"

struct Framebuffer {
    int w, h;
    std::vector<Color> pix;
    Framebuffer(int width, int height);
    void clear(Color c);
    void setPixel(int x, int y, Color c);
};