#pragma once
#include "raylib.h"
#include "framebuffer.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

struct DrawImage {
  Framebuffer& fb;
  bool& bMargin;
  int& posX;
  int& posY;
  int posY1 = 0, posY2 = 0, posX1 = 0, posX2 = 0;
  int width = 0, height = 0, newWidth = 0, newHeight = 0;
  int cx = 0, cy = 0;
  int& n1;
  float& scaleX;
  float& scaleY;
  float& degrees;
  std::vector<Color> newImage2;
  std::vector<Color> imgPixels;
  std::vector<int> Xx;
  std::vector<int> Xx2;
  std::vector<Color> newImage;
  DrawImage(const Image& img, const Color* pixels, Framebuffer& fb, int& posX, int& posY, int& rev, bool& bMargin, float& scaleX, float& scaleY, float& degrees);
  void cutImage();
  void showImage();
  void scaleImage();
  void rotate();
};