#pragma once
#include "raylib.h"
#include "framebuffer.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

struct DrawImage {
  Framebuffer& fb;
  bool& bMargin;
  int& posX;
  int& posY;
  int posY1 = 0, posY2 = 0, posX1 = 0, posX2 = 0;
  int width = 0, height = 0, newWidth = 0, newHeight = 0;
  int firstLineX = 0, firstLineY = 0;
  int& n1;
  float& scaleX;
  float& scaleY;
  float& degrees;
  vector<Color> newImage2;
  vector<Color> imgPixels;
  vector<Color> newImage;

  // CUTIMAGE CHANGES
  vector<vector<int>> numbers;
  vector<vector<int>> realPixels;
  vector<vector<int>> fakePixels;
  vector<Color> smallImage;

  DrawImage(const Image& img, const Color* pixels, Framebuffer& fb, int& posX, int& posY, int& rev, bool& bMargin, float& scaleX, float& scaleY, float& degrees);
  void cutImage();
  void scaleImage();
  void buildMatrix();
  void showImage();
  void rotate();
};