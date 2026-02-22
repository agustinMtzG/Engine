#include "renderQueue.h"

void Renderer::beginFrame() {
    renderQueue.clear();
}

void Renderer::submit(const DrawImage& image, int x, int y, int layer) {
    RenderCommand cmd;
    cmd.image = &image;
    cmd.x = x;
    cmd.y = y;
    cmd.layer = layer;
    renderQueue.push_back(cmd);
}

void Renderer::flush(Framebuffer& fb) {
  std::sort(renderQueue.begin(), renderQueue.end(),
    [](const RenderCommand& a, const RenderCommand& b) {
      return a.layer < b.layer;
  });
  for(const RenderCommand& cmd : renderQueue){
    if (!cmd.image) continue;
    cmd.image->showImage(fb, cmd.x, cmd.y);
  }
}