#pragma once
#include "raylib.h"
#include "framebuffer.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdint>
using namespace std;

struct DrawImage {
  bool& bMargin;
  int posY1 = 0, posY2 = 0, posX1 = 0, posX2 = 0;
  int width = 0, height = 0, newWidth = 0, newHeight = 0;
  int oldWidth = 0, oldHeight = 0;
  int firstLineX = 0, firstLineY = 0;
  int& n1;
  float& scaleX;
  float& scaleY;
  float& degrees;
  vector<Color> newImage2;
  vector<Color> imgPixels;
  vector<Color> newImage;
  vector<Color> basePixels;
  int baseW = 0, baseH = 0;

  vector<Color> smallImage;
  vector<uint16_t> Real;
  vector<uint16_t> Fake;
  vector<bool> Group;

  DrawImage(const Image& img, const Color* pixels, int& rev, bool& bMargin, float& scaleX, float& scaleY, float& degrees);
  void cutImage();
  void scaleImage();
  void buildMatrix();
  void showImage(Framebuffer& fb, int posX, int posY) const;
  void rotate();
};