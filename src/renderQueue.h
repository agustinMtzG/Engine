#pragma once
#include "drawImage.h"

struct RenderCommand {
    const DrawImage* image;
    int x;
    int y;
    int layer;
};

class Renderer {
public:
    void beginFrame();
    void submit(const DrawImage& image, int x, int y, int layer);
    void flush(Framebuffer& fb);
private:
  std::vector<RenderCommand> renderQueue;
};