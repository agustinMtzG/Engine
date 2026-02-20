#include "drawImage.h"

DrawImage::DrawImage(const Image& img, const Color* pixels, Framebuffer& fb, int& posX, int& posY, int& rev, bool& bMargin, float& scaleX, float& scaleY, float& degrees)
  : degrees(degrees),
    scaleX(scaleX),
    scaleY(scaleY),
    bMargin(bMargin),
    n1(rev),
    posX(posX),
    posY(posY),
    fb(fb),
    width(img.width),
    height(img.height)
{
  imgPixels.assign(pixels, pixels + (size_t)width * (size_t)height);
  cutImage();
  scaleImage();
  buildMatrix();
}

void DrawImage::cutImage(){
  // ARRAYS
  bool arr[4] = {false, false, false, false};
  int arr2[4] = {0, 0, 0, 0};
  vector<Color> imgPixels2;
  // FOR ARRIBA A ABAJO
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      if(imgPixels[(size_t)y * width + x].a != 0){
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
      if(imgPixels[(size_t)y * width + x].a != 0){
        arr[1] = true;
        arr2[1] = y;
        break;
      }
    }
    if(arr[1]) break;
    // FOR IZQUIERDA A DERECHA
    for(int x = 0; x < width; x++){
      for(int y = 0; y < height; y++){
        if(imgPixels[(size_t)y * width + x].a != 0){
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
        if(imgPixels[(size_t)y * width + x].a != 0){
          arr[3] = true;
          arr2[3] = x;
          break;
        }
      }
      if(arr[3]) break;
    }
  }
  
  for(int y = arr2[0]; y < arr2[1]; y++){
    for(int x = arr2[2]; x < arr2[3]; x++){
      imgPixels2.push_back(imgPixels[(size_t)y * width + x]);
    }
  }
  imgPixels = move(imgPixels2);
  newWidth = arr2[3] - arr2[2];
  newHeight = arr2[1] - arr2[0];
}

void DrawImage::scaleImage(){
  int oldWidth = newWidth;
  int oldHeight = newHeight;
  int scaledWidth = max(1, (int)round(oldWidth * scaleX));
  int scaledHeight = max(1, (int)round(oldHeight * scaleY));
  vector<Color> scaled(scaledWidth * scaledHeight);
  float percentageX = (float)oldWidth / scaledWidth;
  float percentageY = (float)oldHeight / scaledHeight;
  for(int newY = 0; newY < scaledHeight; newY++){
    for(int newX = 0; newX < scaledWidth; newX++){
      float x0 = newX * percentageX;
      float y0 = newY * percentageY;
      float x1 = x0 + percentageX;
      float y1 = y0 + percentageY;
      int Xx0 = (int)floor(x0);
      int Yy0 = (int)floor(y0);
      int Xx1 = (int)ceil(x1);
      int Yy1 = (int)ceil(y1);
      float r = 0, g = 0, b = 0, a = 0;
      float totalArea = 0;
      for(int y = Yy0; y < Yy1; y++){
        for(int x = Xx0; x < Xx1; x++){
          if(x < 0 || x >= oldWidth || y < 0 || y >= oldHeight) continue;
          float initialX = max(x0, (float)x);
          float initialY = max(y0, (float)y);
          float finalX = min(x1, (float)(x + 1));
          float finalY = min(y1, (float)(y + 1));
          float area = (finalX - initialX) * (finalY - initialY);
          if(area <= 0) continue;
          Color c = imgPixels[(size_t)y * (size_t)oldWidth + (size_t)x]; // newImage
          r += c.r * area;
          g += c.g * area;
          b += c.b * area;
          a += c.a * area;
          totalArea += area;
        }
      }
      Color out{0, 0, 0, 0};
      if(totalArea > 0){
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
  // VECTORES
  std::vector<int> vectorNumbers;
  std::vector<int> vectorRealPixels;
  std::vector<int> vectorFakePixels;
  // BOLEANOS
  bool bool2 = true;
  bool bool3 = true;
  // ENTEROS
  int n1 = 0;
  int n2 = 0;
  // FOR ANIDADO
  for(int y = 0; y < newHeight; y++){
    for(int x = 0; x < newWidth; x++){
      if(imgPixels[(size_t)y * newWidth + x].a != 0){
        smallImage.push_back(imgPixels[(size_t)y * newWidth + x]);
        if(n2 > 0){
          vectorFakePixels.push_back(n2);
          n2 = 0;
        }
        if(bool2){
          bool2 = false;
          bool3 = true;
          vectorNumbers.push_back(1);
        }
        n1++;
      }else{
        if(n1 > 0){
          vectorRealPixels.push_back(n1);
          n1 = 0;
        }
        if(bool3){
          bool3 = false;
          bool2 = true;
          vectorNumbers.push_back(0);
        }
        n2++;
      }
    }
    if(n1 > 0){
      vectorRealPixels.push_back(n1);
      n1 = 0;
    }
    if(n2 > 0){
      vectorFakePixels.push_back(n2);
      n2 = 0;
    }
    if(vectorRealPixels[0] == newWidth){
      vectorFakePixels.push_back(0);
    }
    if(vectorFakePixels[0] == newWidth){
      vectorRealPixels.push_back(0);
    }
    bool2 = true;
    bool3 = true;
    // WHERE ARE REAL PIXELS
    numbers.push_back(vectorNumbers);
    vectorNumbers.clear();
    vectorNumbers.shrink_to_fit();
    // Real Pixels
    realPixels.push_back(vectorRealPixels);
    vectorRealPixels.clear();
    vectorRealPixels.shrink_to_fit();
    // Fake Pixels
    fakePixels.push_back(vectorFakePixels);
    vectorFakePixels.clear();
    vectorFakePixels.shrink_to_fit();
  }
}

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

void DrawImage::showImage(){
  int indexNumbers = 0;
  int indexFakePixels = 0;
  int indexSmallImage = 0;
  int indexRealPixels = 0;
  for(int y = posY; y < newHeight + posY; y++){
    for(int x = posX; x < newWidth + posX; x++){
      int iy = y - posY;
      if(numbers[iy][indexNumbers] == 1){ // CAMBIOS DE 1 A 0
        int count = realPixels[iy][indexRealPixels];
        memcpy(&fb.pix[(size_t)y * fb.w + x], &smallImage[(size_t)indexSmallImage], (size_t)count * sizeof(Color));
        indexSmallImage += count;
        x += count - 1;
        indexNumbers++;
        indexRealPixels++;
      }else{ // CAMBIOS DE 1 A 0
        x += fakePixels[iy][indexFakePixels] - 1;
        indexNumbers++;
        indexFakePixels++;
      }
    }
    indexNumbers = 0;
    indexFakePixels = 0;
    indexRealPixels = 0;
  }
}

/*
  CREAR MI PROPIA FUNCION MEMCPY
  ANIMACION CONSTANTE DE CRECIMIENTO DE IMAGEN
  ESTABLECER LIMITES DE DIBUJO FUERA DEL TAMAÑO DEL FRAMEBUFFER

  SOLUCIONAR ROTATE IMAGE

  OPTIMIZAR

*/