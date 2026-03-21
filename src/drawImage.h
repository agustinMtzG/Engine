#pragma once
#include "framebuffer.h"
#include <cmath>
#include <iostream>
#include <cstdint>
using namespace std;

struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
  int x;
  int y;
};

struct RotatingImagePipeline {
  int cx = 0, cy = 0, cx2 = 0, cy2 = 0;
  int newSize = 0;
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
  bool b1 = true;
  bool b2 = true;
  uint32_t indexReal = 0, indexFake = 0;
  //
  int baseW = 0, baseH = 0;
  RotatingImagePipeline(const Image& img, const Color* pix, float& scaleX, float& scaleY);
  void cutImage();
  void scaleImage();
  void showImage(Framebuffer& fb, int posX, int posY);
  void whenToRotate(float degrees, int x, int y);
  void rotate(float degrees);
};

struct ImageRenderPipeline {
  vector<Pixel> pixels;
  int width = 0, height = 0;
  float& scaleX;
  float& scaleY;
  vector<Color> imgPixels;
  vector<Color> imgPixels2;
  vector<Color> basePixels;
  int baseW = 0, baseH = 0;
  vector<Pixel> smallImage;
  vector<uint16_t> Real;
  ImageRenderPipeline(const Image& img, const Color* pix, float& scaleX, float& scaleY);
  void cutImage();
  void scaleImage();
  void imageData(bool b1);
  void showImage(Framebuffer& fb, int posX, int posY) const;
};