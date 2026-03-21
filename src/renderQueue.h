#pragma once
#include "drawImage.h"

struct RenderCommand {
    RotatingImagePipeline* image;  // RotatingImagePipeline
    int x;
    int y;
    int layer;
};

class Renderer {
public:
    void beginFrame();
    void submit(RotatingImagePipeline& image, int x, int y, int layer); // RotatingImagePipeline
    void flush(Framebuffer& fb);
private:
  std::vector<RenderCommand> renderQueue;
};