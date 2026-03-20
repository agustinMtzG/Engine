#include "drawImage.h"

DrawImage::DrawImage(const Image& img, const Color* pixels, int& rev, bool& bMargin, float& scaleX, float& scaleY)
  : scaleX(scaleX),
    scaleY(scaleY),
    bMargin(bMargin),
    n1(rev),
    width(img.width),
    height(img.height)
{
  imgPixels.assign(pixels, pixels + (size_t)width * (size_t)height);
  cutImage();
  scaleImage();
  buildMatrix();
  rotate(0.0f);
}

void DrawImage::cutImage(){
  // ARRAYS
  bool arr[4] = {false, false, false, false};
  int arr2[4] = {0, 0, 0, 0};
  vector<Color> imgPixels2;
  // FOR ARRIBA A ABAJO
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      if(imgPixels[(size_t)y * width + x].a > 0){
        arr[0] = true;
        arr2[0] = y;
        break;
      }
    }
    if(arr[0]) break;
  }
  // FOR ABAJO A ARRIBA
  for(int y = height - 1; y >= 0; y--){
    for(int x = 0; x < width; x++){
      if(imgPixels[(size_t)y * width + x].a > 0){
        arr[1] = true;
        arr2[1] = y + 1;
        break;
      }
    }
    if(arr[1]) break;
  }
  // FOR IZQUIERDA A DERECHA
  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){
      if(imgPixels[(size_t)y * width + x].a > 0){
        arr[2] = true;
        arr2[2] = x;
      break;
    }
  }
  if(arr[2]) break;
  }
  // FOR DERECHA A IZQUIERDA
  for(int x = width - 1; x >= 0; x--){
    for(int y = 0; y < height; y++){
      if(imgPixels[(size_t)y * width + x].a > 0){
        arr[3] = true;
        arr2[3] = x + 1;
        break;
      }
    }
    if(arr[3]) break;
  }
  
  for(int y = arr2[0]; y < arr2[1]; y++){
    for(int x = arr2[2]; x < arr2[3]; x++){
      imgPixels2.push_back(imgPixels[(size_t)y * width + x]);
    }
  }
  imgPixels = move(imgPixels2);
  newWidth = arr2[3] - arr2[2];
  newHeight = arr2[1] - arr2[0];
  basePixels = imgPixels;
  baseW = newWidth;
  baseH = newHeight;
}

void DrawImage::scaleImage() {
  if (scaleX < 0.05f) scaleX = 0.05f;
  if (scaleY < 0.05f) scaleY = 0.05f;
  int oldWidth = baseW;
  int oldHeight = baseH;
  int scaledWidth  = std::max(1, (int)std::round(baseW * scaleX));
  int scaledHeight = std::max(1, (int)std::round(baseH * scaleY));
  if (scaledWidth == newWidth && scaledHeight == newHeight) return;
  std::vector<Color> scaled((size_t)scaledWidth * (size_t)scaledHeight);
  float percentageX = (float)oldWidth / (float)scaledWidth;
  float percentageY = (float)oldHeight / (float)scaledHeight;
  for (int newY = 0; newY < scaledHeight; newY++) {
    for (int newX = 0; newX < scaledWidth; newX++) {
      float x0 = newX * percentageX;
      float y0 = newY * percentageY;
      float x1 = x0 + percentageX;
      float y1 = y0 + percentageY;
      int Xx0 = (int)std::floor(x0);
      int Yy0 = (int)std::floor(y0);
      int Xx1 = (int)std::ceil(x1);
      int Yy1 = (int)std::ceil(y1);
      float r = 0, g = 0, b = 0, a = 0;
      float totalArea = 0;
      for (int y = Yy0; y < Yy1; y++) {
        for (int x = Xx0; x < Xx1; x++) {
          if (x < 0 || x >= oldWidth || y < 0 || y >= oldHeight) continue;
          float initialX = std::max(x0, (float)x);
          float initialY = std::max(y0, (float)y);
          float finalX   = std::min(x1, (float)(x + 1));
          float finalY   = std::min(y1, (float)(y + 1));
          float area = (finalX - initialX) * (finalY - initialY);
          if (area <= 0) continue;
          Color c = basePixels[(size_t)y * (size_t)oldWidth + (size_t)x];
          r += c.r * area;
          g += c.g * area;
          b += c.b * area;
          a += c.a * area;
          totalArea += area;
        }
      }
      Color out{0, 0, 0, 0};
      if (totalArea > 0) {
        out.r = (unsigned char)(r / totalArea);
        out.g = (unsigned char)(g / totalArea);
        out.b = (unsigned char)(b / totalArea);
        out.a = (unsigned char)(a / totalArea);
      }
      scaled[(size_t)newY * (size_t)scaledWidth + (size_t)newX] = out;
    }
  }
  imgPixels = std::move(scaled);
  newWidth = scaledWidth;
  newHeight = scaledHeight;
  width = newWidth;
  height = newHeight;
}

void DrawImage::buildMatrix(){
  int n1 = 0;
  int n2 = 0;
  bool b1 = true;
  bool b2 = true;
  if(Real.size() != 0){
    Real.clear();
    Fake.clear();
    Group.clear();
    smallImage.clear();
  }
  for(int y = 0; y < newHeight; y++){
    for(int x = 0; x < newWidth; x++){
      if(imgPixels[(size_t)y * newWidth + x].a != 0){
        Color c = imgPixels[(size_t)y * newWidth + x];
        Pixel p;
        p.r = c.r;
        p.g = c.g;
        p.b = c.b;
        p.a = c.a;
        p.x = x;
        p.y = y;
        smallImage.push_back(p);
        // REAL PIXEL FOUND
        n1++;
        if(n2 > 0){
          Fake.push_back(n2);
          Group.push_back(false);
          n2 = 0;
        }
      }else{
        // FAKE PIXEL FOUND
        n2++;
        if(n1 > 0){
          Real.push_back(n1);
          Group.push_back(true);
          n1 = 0;
        }
      }
    }
    if(n1 > 0) Real.push_back(n1);
    if(n1 > 0) Group.push_back(true);
    if(n2 > 0) Fake.push_back(n2);
    if(n2 > 0) Group.push_back(false);
    n1 = 0;
    n2 = 0;
  }
}

void DrawImage::rotate(float degrees) {
  pixels.clear();
  pixels.reserve(smallImage.size());
  int x = 0;
  int y = 0;
  float centerX = (float)newWidth / 2.0f;
  float centerY = (float)newHeight / 2.0f;
  float cateto1 = 0.0f;
  float cateto2 = 0.0f;
  float hipotenusa = 0.0f;
  float rotation = degrees;
  float angle = (rotation * Pi) / 180.0f;
  for(int i = 0; i < smallImage.size(); i++){
    x = smallImage[i].x;
    y = smallImage[i].y;
      if(centerX >= x) cateto1 = centerX - x; else cateto1 = x - centerX;
      if(centerY >= y) cateto2 = centerY - y; else cateto2 = y - centerY;
      hipotenusa = sqrt(cateto1 * cateto1 + cateto2 * cateto2);
      float angle3 = 0.0f;
      float angle2 = 0.0f;
      float rotation2 = 0.0f;
      if(hipotenusa == 0){
        pixels.push_back(smallImage[i]);
        pixels[i].x = x;
        pixels[i].y = y;
        continue;
      }
      if(x >= centerX && y < centerY){
        rotation2 = ((asin(cateto1 / hipotenusa) * (180 / Pi)));
      }else if(x >= centerX && y >= centerY){
        rotation2 = 180 - ((asin(cateto1 / hipotenusa) * (180 / Pi)));
      }else if(x < centerX && y >= centerY){
        rotation2 = 180 + ((asin(cateto1 / hipotenusa) * (180 / Pi)));
      }else if(x < centerX && y < centerY){
        rotation2 = 360 - ((asin(cateto1 / hipotenusa) * (180 / Pi)));
      }
      angle2 = (rotation2 * Pi) / 180;
      angle3 = angle + angle2;
      float maxAngle = (360.0f * Pi) / 180;
      if(angle3 > maxAngle){
        angle3 = angle3 - maxAngle;
      }else if(angle3 < 0){
        angle3 = maxAngle + angle3;
      }
      pixels.push_back(smallImage[i]);
      pixels[i].x = round(centerX + sin(angle3) * hipotenusa);
      pixels[i].y = round(centerY - cos(angle3) * hipotenusa);
  }
}

void DrawImage::showImage(Framebuffer& fb, int posX, int posY) const {
  for(int i = 0; i < pixels.size(); i++){
    Color c;
    c.r = pixels[i].r;
    c.g = pixels[i].g;
    c.b = pixels[i].b;
    c.a = pixels[i].a;
    fb.pix[(size_t)(posY + pixels[i].y) * fb.w + (posX + pixels[i].x)] = c;
  }
}

/*
void DrawImage::showImage(Framebuffer& fb, int posX, int posY) const {
  for(int y = posY; y < newHeight + posY; y++){
    for(int x = posX; x < newWidth + posX; x++){
      int ix = x - posX;
      int iy = y - posY;
      if(bMargin && (ix == 0 || iy == 0 || ix == newWidth - 1 || iy == newHeight - 1)){
        fb.pix[(size_t)y * fb.w + x] = Color{255, 255, 255, 255};
      }else{
        fb.pix[(size_t)y * fb.w + x] = imgPixels[(size_t)iy * newWidth + ix];
      }
    }
  }
}

/*
void DrawImage::showImage(Framebuffer& fb, int posX, int posY) const {
  int x = posX;
  int y = posY;
  int indexReal = 0;
  int indexFake = 0;
  int indexSmallImage = 0;
  for(int i = 0; i < Group.size(); i++){
    if(Group[i]){
      int count = Real[indexReal];
      memcpy(&fb.pix[(size_t)y * fb.w + x], &smallImage[(size_t)indexSmallImage], (size_t)count * sizeof(Color));
      x += count;
      indexSmallImage += count;
      indexReal++;
      if(x >= newWidth + posX){
        x = posX;
        y++;
        continue;
      }
    }else{
      x += Fake[indexFake];
      indexFake++;
      if(x >= newWidth + posX){
        x = posX;
        y++;
        continue;
      }
    }
  }
}
*/


/*

  MEJORAR BUILDMATRIX
  ESTABLECER LIMITES DE DIBUJO FUERA DEL TAMAÑO DEL FRAMEBUFFER
  MIRROR/REVERSE IMAGE

  SOLUCIONAR ROTATE IMAGE

  VER FUNCIONES DE RAYLIB

  OPTIMIZAR

*/

/*
void DrawImage::rotate(){
  float rad = degrees * 3.14159265f / 180.0f;
  float cosA = cos(rad);
  float sinA = sin(rad);
  int srcW = newWidth;
  int srcH = newHeight;
  float cx = (srcW - 1) * 0.5f;
  float cy = (srcH - 1) * 0.5f;
  vector<Color> rotated(srcW * srcH, Color{0,0,0,0});
  for(int y = 0; y < srcH; y++){
    for(int x = 0; x < srcW; x++){
      float dx = x - cx;
      float dy = y - cy;
      float srcX = cosA * dx + sinA * dy + cx;
      float srcy = -sinA * dx + cosA * dy + cy;
      int ix = (int)floor(srcX);
      int iy = (int)floor(srcy);
      if(ix >= 0 && ix < srcW && iy >= 0 && iy < srcH){
        rotated[(size_t)y * (size_t)srcW + (size_t)x] = newImage2[(size_t)iy * (size_t)srcW + (size_t)ix];
      }
    }
  }
  newImage = move(rotated);
}
*/

/*
void DrawImage::whenToRotate(float degrees){
  int index = 0;
  int index2 = 0;
  int posX = 0;
  int posY = 0;
  float rX = 0;
  float rY = 0;
  bool b1 = true;
  for(int i = 0; i < smallImage.size(); i++){
    if(Real[index2] > index){
      //////////////////////////////////////////////////
      if(Real[index2] > 1 && b1){
        rotate(degrees, smallImage[i], pixels[i]);
        if(index == 0){
          posX = pixels[i].x;
          posY = pixels[i].y;
        }else{
          if(posX >= pixels[i].x){
            rX = posX - pixels[i].x;
          }else{
            rX = pixels[i].x - posX;
          }
          if(posY >= pixels[i].y){
            rY = posY - pixels[i].y;
          }else{
            rY = pixels[i].y - posY;
          }
          posX = pixels[i].x;
          posY = pixels[i].y;
          b1 = false;
        }
      }else if(Real[index2] == 1 && b1){
        //rotate(degrees, smallImage[i], pixels[i]);
      }
      /////////////////////////////////////
      if(!b1){
        if(index == 0){
          rotate(degrees, smallImage[i], pixels[i]);
          posX = pixels[i].x;
          posY = pixels[i].y;
        }else{
          pixels[i].x = posX + rX;
          pixels[i].y = posY + rY;
          posX = pixels[i].x;
          posY = pixels[i].y;
        }
      }
      index++;
      if(Real[index2] <= index){
        index = 0;
        index2++;
      }
    }
    //rotate(degrees, smallImage[i], pixels[i]);
  }
}
*/