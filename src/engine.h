#pragma once
#include <cmath>
#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include "raylib.h"
using namespace std;

struct Framebuffer {
    size_t w, h;
    std::vector<Color> pix;
    Framebuffer(size_t width, size_t height, Color c);
    void clear(Color c);
    void setPixel(size_t x, size_t y, Color c);
};

struct RotatingImagePipeline {
  int cx = 0, cy = 0, cx2 = 0, cy2 = 0;
  int newSize = 0;
  int baseW = 0, baseH = 0;
  float Pi = 3.14159265358979323846f;
  int width = 0, height = 0;
  float& scaleX;
  float& scaleY;
  vector<Color> imgPixels;
  vector<Color> imgPixels2;
  vector<Color> basePixels;
  //
  vector<uint32_t> Real;
  vector<uint32_t> Fake;
  vector<uint16_t> Group;
  vector<uint16_t> howManyGroups;
  bool b1 = true;
  bool b2 = true;
  uint32_t indexReal = 0, indexFake = 0;
  uint16_t n1 = 0;
  //
  RotatingImagePipeline(const Image& img, const Color* pix, float& scaleX, float& scaleY);
  void cutImage();
  void scaleImage();
  void showImage(Framebuffer& fb, int posX, int posY);
  void whenToRotate(float degrees, int x, int y);
  void rotate(float degrees);
};

struct ImageRenderPipeline {
  int width = 0, height = 0;
  float& scaleX;
  float& scaleY;
  vector<Color> imgPixels;
  vector<Color> imgPixels2;
  vector<Color> basePixels;
  int baseW = 0, baseH = 0;
  //
  vector<uint32_t> Real;
  vector<uint32_t> Fake;
  vector<uint16_t> Group;
  vector<uint16_t> howManyGroups;
  bool b1 = true;
  bool b2 = true;
  uint32_t indexReal = 0, indexFake = 0;
  uint16_t n1 = 0;
  //
  ImageRenderPipeline(const Image& img, const Color* pix, float& scaleX, float& scaleY);
  void cutImage();
  void scaleImage();
  void imageData();
  void showImage(Framebuffer& fb, int posX, int posY);
};