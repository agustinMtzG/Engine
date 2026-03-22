#include "engine.h"

// FRAMEBUFFER

Framebuffer::Framebuffer(size_t width, size_t height, Color c)
    : w(width),
      h(height),
      pix(width * height, c)
    {}

void Framebuffer::clear(Color c){
    std::fill(pix.begin(), pix.end(), c);
}

void Framebuffer::setPixel(size_t x, size_t y, Color c){
    if(x < 0 || y < 0 || x >= w || y >= h) return;
    pix[y * w + x] = c;
}

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
  imageData();
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
  imgPixels2.clear();
}

void ImageRenderPipeline::imageData(){
  b1 = true;
  b2 = true;
  Real.clear();
  Fake.clear();
  Group.clear();
  howManyGroups.clear();
  imgPixels2.clear();
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      size_t index = (size_t)y * width + x;
      if(imgPixels[index].a != 0){
        imgPixels2.push_back(imgPixels[index]);
        indexReal++;
        b2 = true;
        if(b1){
          b1 = false;
          Group.push_back(1);
          n1++;
          if(indexFake > 0){
            Fake.push_back(indexFake);
            indexFake = 0;
          }
        }
      }else{
        indexFake++;
        b1 = true;
        if(b2){
          b2 = false;
          Group.push_back(0);
          n1++;
          if(indexReal > 0){
            Real.push_back(indexReal);
            indexReal = 0;
          }
        }
      }
    }
    b1 = true;
    b2 = true;
    if(n1 > 0){
      howManyGroups.push_back(n1);
      n1 = 0;
    }
    if(indexReal > 0){
      Real.push_back(indexReal);
      indexReal = 0;
    }
    if(indexFake > 0){
      Fake.push_back(indexFake);
      indexFake = 0;
    }
  }
}

void ImageRenderPipeline::showImage(Framebuffer& fb, int posX, int posY) {
  int indexGroup = 0;
  int indexPixels = 0;
  if(posY >= fb.h || posY <= (height * -1) || posX >= fb.w || posX <= (width * -1)) return;
  for(int y = posY; y < height + posY; y++){
    if(y >= fb.h) break;
    if(y < 0){
      for(int i = 0; i < (y * -1); i++){
        for(int j = 0; j < howManyGroups[i]; j++){
          if(Group[indexGroup] == 0){
            indexFake++;
          }else{
            indexPixels += Real[indexReal];
            indexReal++;
          }
          indexGroup++;
        }
      }
      y = 0;
    }
    for(int x = posX; x < width + posX; x++){
      if(Group[indexGroup] == 0){
        x += Fake[indexFake] - 1;
        indexFake++;
        indexGroup++;
      }else{
        int v1 = x > 0 && (x + Real[indexReal]) >= fb.w ? (x + Real[indexReal]) - fb.w : 0;
        int v2 = 0;
        if(x < 0){
          v2 = x * -1;
          v1 = v2;
          x = 0;
        }
        int count = Real[indexReal];
        if(count - v1 > 0) memcpy(&fb.pix[(size_t)y * fb.w + x], &imgPixels2[(size_t)indexPixels + v2], (size_t)(count - v1) * sizeof(Color));
        x += count - 1 - v2;
        indexPixels += count;
        indexReal++;
        indexGroup++;
      }
    }
  }
  indexReal = 0;
  indexFake = 0;
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
}

void RotatingImagePipeline::whenToRotate(float degrees, int x, int y){
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
  if(XX >= 0 && YY >= 0 && XX < width && YY < height && imgPixels[index2].a > 0){
    imgPixels2.push_back(imgPixels[index2]);
    b1 = true;
    indexReal++;
    if(b2){
      b2 = false;
      Group.push_back(1);
      if(indexFake > 0){
        n1++;
        Fake.push_back(indexFake);
        indexFake = 0;
      }
    }
  }else{
    b2 = true;
    indexFake++;
    if(b1){
      b1 = false;
      Group.push_back(0);
      if(indexReal > 0){
        n1++;
        Real.push_back(indexReal);
        indexReal = 0;
      }
    }
  }
}

void RotatingImagePipeline::rotate(float degrees){
  Real.clear();
  Fake.clear();
  Group.clear();
  howManyGroups.clear();
  imgPixels2.clear();
  for(int y = cy2 - (newSize / 2); y < cy2 + (newSize / 2); y++){
    for(int x = cx2 - (newSize / 2); x < cx2 + (newSize / 2); x++){
      whenToRotate(degrees, x, y);
    }
    b1 = true;
    b2 = true;
    if(indexReal > 0){
      n1++;
      Real.push_back(indexReal);
      indexReal = 0;
    }
    if(indexFake > 0){
      n1++;
      Fake.push_back(indexFake);
      indexFake = 0;
    }
    howManyGroups.push_back(n1);
    n1 = 0;
  }
}

void RotatingImagePipeline::showImage(Framebuffer& fb, int posX, int posY){
  int indexGroup = 0;
  int indexPixels = 0;
  if(posY >= fb.h || posY <= (newSize * -1) || posX >= fb.w || posX <= (newSize * -1)) return;
  for(int y = posY; y < newSize + posY; y++){
    if(y >= fb.h) break;
    if(y < 0){
      for(int i = 0; i < (y * -1); i++){
        for(int j = 0; j < howManyGroups[i]; j++){
          if(Group[indexGroup] == 0){
            indexFake++;
          }else{
            indexPixels += Real[indexReal];
            indexReal++;
          }
          indexGroup++;
        }
      }
      y = 0;
    }
    for(int x = posX; x < newSize + posX; x++){
      if(Group[indexGroup] == 0){
        x += Fake[indexFake] - 1;
        indexFake++;
        indexGroup++;
      }else{
        int v1 = x > 0 && (x + Real[indexReal]) >= fb.w ? (x + Real[indexReal]) - fb.w : 0;
        int v2 = 0;
        if(x < 0){
          v2 = x * -1;
          v1 = v2;
          x = 0;
        }
        int count = Real[indexReal];
        if(count - v1 > 0) memcpy(&fb.pix[(size_t)y * fb.w + x], &imgPixels2[(size_t)indexPixels + v2], (size_t)(count - v1) * sizeof(Color));
        x += count - 1 - v2;
        indexPixels += count;
        indexReal++;
        indexGroup++;
      }
    }
  }
  indexReal = 0;
  indexFake = 0;
}

/*
-1. IMAGEDATA Y SHOWIMAGE HACER UNA SOLA FUNCION Y LLAMARLA EN LOS STRUCTS
0. Optimizar cutImage, scaleImage, showImage de ambos structs
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

1.- RENDERQUEUE dos tipos de datos
2.- Mejorar main.cpp
3.- Luminosidad
4.- Transparencia
5.- Comenzar videojuego
*/