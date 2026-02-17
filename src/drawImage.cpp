#include "drawImage.h"
// Hola
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
}

void DrawImage::cutImage() {
    int minX = width, maxX = 0;
    int minY = height, maxY = 0;
    bool foundPixel = false;
    // 2. Escaneo en un solo paso (Más eficiente y limpio)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (imgPixels[(size_t)y * width + x].a != 0) {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
                foundPixel = true;
            }
        }
    }

    // Caso de seguridad: Si la imagen es totalmente transparente
    if (!foundPixel) {
        newWidth = 1; newHeight = 1;
        newImage.assign(1, Color{0,0,0,0});
        return;
    }

    // 3. Cálculo del "Bounding Box" circular para rotación segura
    float centreX = (minX + maxX) * 0.5f;
    float centreY = (minY + maxY) * 0.5f;

    // Distancia del centro a la esquina más lejana del recorte
    float diffX = (float)maxX - centreX;
    float diffY = (float)maxY - centreY;
    int radius = (int)std::ceil(std::sqrt(diffX * diffX + diffY * diffY));

    // Definir dimensiones de la nueva imagen cuadrada (necesaria para rotar sin cortes)
    newWidth = radius * 2;
    newHeight = radius * 2;
    newImage.assign((size_t)newWidth * newHeight, Color{0, 0, 0, 0});

    // 4. Transferencia de píxeles con validación de límites (Anti-Crash)
    int startX = (int)(centreX - radius);
    int startY = (int)(centreY - radius);

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int oldX = startX + x;
            int oldY = startY + y;

            // Solo copiamos si estamos dentro de los límites de la imagen original
            if (oldX >= 0 && oldX < width && oldY >= 0 && oldY < height) {
                newImage[(size_t)y * newWidth + x] = imgPixels[(size_t)oldY * width + oldX];
            }
        }
    }
}

void DrawImage::scaleImage(){
  int oldWidth = newWidth;
  int oldHeight = newHeight;
  int scaledWidth = std::max(1, (int)std::round(oldWidth * scaleX));
  int scaledHeight = std::max(1, (int)std::round(oldHeight * scaleY));
  std::vector<Color> scaled(scaledWidth * scaledHeight);
  float percentageX = (float)oldWidth / scaledWidth;
  float percentageY = (float)oldHeight / scaledHeight;
  for(int newY = 0; newY < scaledHeight; newY++){
    for(int newX = 0; newX < scaledWidth; newX++){
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
      for(int y = Yy0; y < Yy1; y++){
        for(int x = Xx0; x < Xx1; x++){
          if(x < 0 || x >= oldWidth || y < 0 || y >= oldHeight) continue;
          float initialX = std::max(x0, (float)x);
          float initialY = std::max(y0, (float)y);
          float finalX = std::min(x1, (float)(x + 1));
          float finalY = std::min(y1, (float)(y + 1));
          float area = (finalX - initialX) * (finalY - initialY);
          if(area <= 0) continue;
          Color c = newImage[(size_t)y * (size_t)oldWidth + (size_t)x];
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
  newImage = std::move(scaled);
  newImage2 = newImage;
  newWidth = scaledWidth;
  newHeight = scaledHeight;
}

void DrawImage::rotate(){
  float rad = degrees * 3.14159265f / 180.0f;
  float cosA = std::cos(rad);
  float sinA = std::sin(rad);
  int srcW = newWidth;
  int srcH = newHeight;
  float cx = (srcW - 1) * 0.5f;
  float cy = (srcH - 1) * 0.5f;
  std::vector<Color> rotated(srcW * srcH, Color{0,0,0,0});
  for(int y = 0; y < srcH; y++){
    for(int x = 0; x < srcW; x++){
      float dx = x - cx;
      float dy = y - cy;
      float srcX = cosA * dx + sinA * dy + cx;
      float srcY = -sinA * dx + cosA * dy + cy;
      int ix = (int)std::floor(srcX);
      int iy = (int)std::floor(srcY);
      if(ix >= 0 && ix < srcW && iy >= 0 && iy < srcH){
        rotated[(size_t)y * (size_t)srcW + (size_t)x] = newImage2[(size_t)iy * (size_t)srcW + (size_t)ix];
      }
    }
  }
  newImage = std::move(rotated);
}

void DrawImage::showImage(){
  for(int y = posY; y < newHeight + posY; y++){
    for(int x = posX; x < newWidth + posX; x++){
      if(x < 0 || x >= fb.w || y < 0 || y >= fb.h) continue;
      cy = (n1 == 2 || n1 == 3) ? (newHeight - 1) - (y - posY) : y - posY;
      cx = (n1 == 1 || n1 == 3) ? (newWidth - 1) - (x - posX) : x - posX;
      if(newImage[(size_t)cy * (size_t)newWidth + (size_t)cx].a != 0){
        fb.pix[(size_t)y * (size_t)fb.w + (size_t)x] = newImage[(size_t)cy * (size_t)newWidth + (size_t)cx];
      }
      if(bMargin && (cy == 0 || cx == 0 || cy == newHeight - 1 || cx == newWidth - 1)){
        fb.pix[(size_t)y * (size_t)fb.w + (size_t)x] = Color{255, 255, 255, 255};
      }
    }
  }
}