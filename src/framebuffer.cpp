#include "framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
    : w(width),
      h(height),
      pix(static_cast<size_t>(width) * height, Color{255, 0, 0, 225})
    {}

void Framebuffer::clear(Color c){
    std::fill(pix.begin(), pix.end(), c);
}

void Framebuffer::setPixel(int x, int y, Color c){
    if(x < 0 || y < 0 || x >= w || y >= h) return;
    pix[static_cast<size_t>(y) * w + x] = c;
}