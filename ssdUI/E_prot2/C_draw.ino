//以下画面に文字、変数を表示する関数----------------------------------------------------------------------------------
template <typename T>
void printString(const char* ValName, T yourVal, int printX, int printY) {
oled.setCursor(printX, printY);
oled.print(ValName);
oled.print(":");
oled.print(yourVal);
}

void printString_char(char* yourVal, int printIntX, int printIntY) {
  oled.setCursor(printIntX, printIntY);
  oled.print(yourVal);
}

void fillCircle(int xc, int yc, int r) {
  for (int y = -r; y <= r; y++) {
    int xSpan = sqrt(r * r - y * y);  // 円の公式
    for (int x = -xSpan; x <= xSpan; x++) {
      oled.setPixel(xc + x, yc + y);
    }
  }
}

void fillTriangle(int x0, int y0,
                  int x1, int y1,
                  int x2, int y2) {
  // Y座標で頂点をソート
  if (y0 > y1) { swap(y0, y1); swap(x0, x1); }
  if (y1 > y2) { swap(y1, y2); swap(x1, x2); }
  if (y0 > y1) { swap(y0, y1); swap(x0, x1); }

  auto drawLine = [&](int y, int xa, int xb) {
    if (xa > xb) swap(xa, xb);
    for (int x = xa; x <= xb; x++) {
      oled.setPixel(x, y);
    }
  };

  // 上半分
  for (int y = y0; y <= y1; y++) {
    float t0 = (y1 == y0) ? 0 : (float)(y - y0) / (y1 - y0);
    float t1 = (y2 == y0) ? 0 : (float)(y - y0) / (y2 - y0);

    int xa = x0 + t0 * (x1 - x0);
    int xb = x0 + t1 * (x2 - x0);

    drawLine(y, xa, xb);
  }

  // 下半分
  for (int y = y1; y <= y2; y++) {
    float t0 = (y2 == y1) ? 0 : (float)(y - y1) / (y2 - y1);
    float t1 = (y2 == y0) ? 0 : (float)(y - y0) / (y2 - y0);
    int xa = x1 + t0 * (x2 - x1);
    int xb = x0 + t1 * (x2 - x0);

    drawLine(y, xa, xb);
  }

}
void drawLine(int x0, int y0, int x1, int y1) {
  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx + dy;

  while (true) {
    oled.setPixel(x0, y0);

    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void drawLine(Tiny4kOLED &oled, int x0, int y0, int x1, int y1) {
  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx + dy;

  while (true) {
    oled.setPixel(x0, y0);

    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void drawTriangle(Tiny4kOLED &oled, 
                  uint8_t x0,  y0, 
                  uint8_t x1, int uint8_t y1, 
                  uint8_t x2, uint8_t y2) {
  drawLine(oled, x0, y0, x1, y1);
  drawLine(oled, x1, y1, x2, y2);
  drawLine(oled, x2, y2, x0, y0);
}



void startUpShow(){
  oled.clear();
}

void draw_homemenu(){}

void draw_setIntVal(){}

void draw_setFloatVal(){}