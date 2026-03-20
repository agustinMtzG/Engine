#include "drawImage.h"

// IMAGERENDERPIPELINE

ImageRenderPipeline::ImageRenderPipeline(const Image& img, const Color* pix, float& scaleX, float& scaleY)
  :
  scaleX(scaleX),
  scaleY(scaleY),
  width(img.width),
  height(img.height)
{
  imgPixels.assign(pix, pix + (size_t)width * (size_t)height);
  cutImage();
  scaleImage();
  imageData(false);
}

void ImageRenderPipeline::cutImage(){
  // ARRAYS
  bool arr[4] = {false, false, false, false};
  int arr2[4] = {0, 0, 0, 0};
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
  width = arr2[3] - arr2[2];
  height = arr2[1] - arr2[0];
  basePixels = imgPixels;
  baseW = width;
  baseH = height;
}

void ImageRenderPipeline::scaleImage(){
  if (scaleX < 0.05f) scaleX = 0.05f;
  if (scaleY < 0.05f) scaleY = 0.05f;
  int scaledWidth  = std::max(1, (int)std::round(baseW * scaleX));
  int scaledHeight = std::max(1, (int)std::round(baseH * scaleY));
  if (scaledWidth == width && scaledHeight == height) return;
  std::vector<Color> scaled((size_t)scaledWidth * (size_t)scaledHeight);
  float percentageX = (float)baseW / (float)scaledWidth;
  float percentageY = (float)baseH / (float)scaledHeight;
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
          if (x < 0 || x >= baseW || y < 0 || y >= baseH) continue;
          float initialX = std::max(x0, (float)x);
          float initialY = std::max(y0, (float)y);
          float finalX   = std::min(x1, (float)(x + 1));
          float finalY   = std::min(y1, (float)(y + 1));
          float area = (finalX - initialX) * (finalY - initialY);
          if (area <= 0) continue;
          Color c = basePixels[(size_t)y * (size_t)baseW + (size_t)x];
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
  width = scaledWidth;
  height = scaledHeight;
}

void ImageRenderPipeline::imageData(bool b1){
  uint16_t n1 = 0;
  size_t index = 0;
  Real.clear();
  smallImage.clear();
  if(b1){
    for(uint16_t x = 0; x < width; x++){
      for(uint16_t y = 0; y < height; y++){
        index = (size_t)(y * width + x);
          if(imgPixels[index].a > 0){
          n1++;
          Color c = imgPixels[index];
          Pixel p;
          p.r = c.r; p.g = c.g; p.b = c.b; p.a = c.a;
          p.x = x; p.y = y;
          smallImage.push_back(p);
        }else{
          if(n1 > 0){
            Real.push_back(n1);
            n1 = 0;
          }
        }
      }
      if(n1 > 0){
        Real.push_back(n1);
        n1 = 0;
      }
    }
  }else{
    for(uint16_t y = 0; y < height; y++){
      for(uint16_t x = 0; x < width; x++){
        index = (size_t)(y * width + x);
          if(imgPixels[index].a > 0){
          n1++;
          Color c = imgPixels[index];
          Pixel p;
          p.r = c.r; p.g = c.g; p.b = c.b; p.a = c.a;
          p.x = x; p.y = y;
          smallImage.push_back(p);
        }else{
          if(n1 > 0){
            Real.push_back(n1);
            n1 = 0;
          }
        }
      }
      if(n1 > 0){
        Real.push_back(n1);
        n1 = 0;
      }
    }
  }
  pixels = smallImage;
}

void ImageRenderPipeline::showImage(Framebuffer& fb, int posX, int posY) const {
  for(int i = 0; i < pixels.size(); i++){
    Color c;
    c.r = pixels[i].r;
    c.g = pixels[i].g;
    c.b = pixels[i].b;
    c.a = pixels[i].a;
    fb.pix[(size_t)(posY + pixels[i].y) * fb.w + (posX + pixels[i].x)] = c;
  }
}

// ROTATINGIMAGEPIPELINE

RotatingImagePipeline::RotatingImagePipeline(const Image& img, const Color* pix, float& scaleX, float& scaleY)
  : scaleX(scaleX),
    scaleY(scaleY),
    width(img.width),
    height(img.height)
{
    imgPixels.assign(pix, pix + (size_t)width * (size_t)height);
    cutImage();
    scaleImage();
    rotate(0.0f);
}

void RotatingImagePipeline::cutImage(){
  // ARRAYS
  bool arr[4] = {false, false, false, false};
  int arr2[4] = {0, 0, 0, 0};
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
  width = arr2[3] - arr2[2];
  height = arr2[1] - arr2[0];
  basePixels = imgPixels;
  baseW = width;
  baseH = height;
}

void RotatingImagePipeline::scaleImage(){
  if (scaleX < 0.05f) scaleX = 0.05f;
  if (scaleY < 0.05f) scaleY = 0.05f;
  int scaledWidth  = std::max(1, (int)std::round(baseW * scaleX));
  int scaledHeight = std::max(1, (int)std::round(baseH * scaleY));
  if (scaledWidth == width && scaledHeight == height) return;
  std::vector<Color> scaled((size_t)scaledWidth * (size_t)scaledHeight);
  float percentageX = (float)baseW / (float)scaledWidth;
  float percentageY = (float)baseH / (float)scaledHeight;
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
          if (x < 0 || x >= baseW || y < 0 || y >= baseH) continue;
          float initialX = std::max(x0, (float)x);
          float initialY = std::max(y0, (float)y);
          float finalX   = std::min(x1, (float)(x + 1));
          float finalY   = std::min(y1, (float)(y + 1));
          float area = (finalX - initialX) * (finalY - initialY);
          if (area <= 0) continue;
          Color c = basePixels[(size_t)y * (size_t)baseW + (size_t)x];
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
  width = scaledWidth;
  height = scaledHeight;
  cx = (width - 1) * 0.5f;
  cy = (width - 1) * 0.5f;
  cx2 = (width - 1) * 0.5f;
  cy2 = (height - 1) * 0.5f;
  newSize = sqrt((0 - width) * (0 - width) + (0 - height) * (0 - height));
  newSize = (newSize % 2 == 0) ? newSize : newSize + 1;
  imgPixels2.clear();
  imgPixels2.resize((size_t)newSize * newSize);
}

void RotatingImagePipeline::whenToRotate(float degrees, int x, int y, int index){
  float angle = (degrees * Pi) / 180.0f;
  float sinA = sin(angle);
  float cosA = cos(angle);
  float centerX = (width - 1) * 0.5f;
  float centerY = (height - 1) * 0.5f;
  float dx = x - centerX;
  float dy = y - centerY;
  float srcX = cosA * dx + sinA * dy + centerX;
  float srcY = -sinA * dx + cosA * dy + centerY;
  int XX = round(srcX);
  int YY = round(srcY);
  size_t index2 = (size_t)YY * width + XX;
  int n1 = index;
  if(XX >= 0 && YY >= 0 && XX < width && YY < height){
    imgPixels2[n1] = imgPixels[index2];
  }else{
    imgPixels2[n1] = Color{0, 0, 0, 0};
  }
}

void RotatingImagePipeline::rotate(float degrees){
  int n1 = -1;
  for(int y = cy2 - (newSize / 2); y < cy2 + (newSize / 2); y++){
    for(int x = cx2 - (newSize / 2); x < cx2 + (newSize / 2); x++){
      n1++;
      whenToRotate(degrees, x, y, n1);
    }
  }
}

void RotatingImagePipeline::showImage(Framebuffer& fb, int posX, int posY) const {
  size_t totalBytes = newSize * sizeof(Color);
  for(int y = posY; y < newSize + posY; y++){
    if(y < 0) continue; // ERROR
    if(y >= fb.h) break;
    int yi = y - posY;
    size_t index = (size_t)y * fb.w + posX;
    size_t index2 = (size_t)yi * newSize;
    memcpy(&fb.pix[index], &imgPixels2[index2], totalBytes);
  }
}

/*
0. Optimizar cutImage, scaleImage y showImage de ambos structs
1. Como le vamos a hacer con renderQueue ahora hay dos tipos de drawImage
2. De los dos structs nuevos (cutImage, scaleImage) son las mismas como heredar
3. Separar los archivos
4. Buscar la mejor manera para showImage en ImageRenderPipeline
5. hacer structs para la carga de imagenes en main.cpp
6. Optimizar main.cpp
7. funcion de Luminosidad con respecto a un punto
8. funcion de transparencia
9. Estudiar y documentar
10. Comenzar videojuego
*/