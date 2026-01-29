#include <math.h>

struct Vector {
  float x;
  float y;
};

// 極座標 → ベクトル
Vector fromPolar(float deg, float mag) {
  Vector v;
  float rad = deg * M_PI / 180.0;
  v.x = cos(rad) * mag;
  v.y = sin(rad) * mag;
  return v;
}

// ベクトル加算
Vector add(Vector a, Vector b) {
  Vector r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  return r;
}

// 正規化（単位ベクトル化）
Vector normalize(Vector v) {
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / len, v.y / len };
}

Vector divide(Vector v, int num) {
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / num, v.y / num };
}

const int NUM_SENSORS = 16;
// const int sensorPins[NUM_SENSORS] = {4,7,6,8,9,11,12,15,14,17,16,18,19,2,3,5};
const int sensorPins[NUM_SENSORS] = {4,5,3,2,19,18,16,17,14,15,12,11,9,8,6,7};
const float sensorDeg[NUM_SENSORS] = {0,22.5,45,67.5,90,111.5,130,155.5,180,202.5,225,247.5,270,292.5,315,337.5};

int sensorValue[NUM_SENSORS];

Vector line_v;

void setup() {
  initUART();
  for (int i = 0; i < NUM_SENSORS; i++) {                     
    pinMode(sensorPins[i], INPUT);
  }
}

const int NUM_READ = 200;
int16_t line_angle;
int16_t line_dis;

void loop() {
  UART();
  analogWrite(10, 180);

  for (int i = 0; i < NUM_SENSORS; i++) {
    int line_total = 0;
    for (int j = 0; j < NUM_READ; j++) {
      if (i == 12 || i == 13) { // 値が逆転しちゃうやつだけ修正
        line_total += (1 - digitalRead(sensorPins[i]));
      }
      else {
        line_total += digitalRead(sensorPins[i]);
      }
    }
    int line = 1;
    if (line_total != NUM_READ) {
      line = 0;
    }
    sensorValue[i] = line;
    // Serial.print(sensorValue[i]);
  }
  // Serial.println(" ");
  Vector line_v = {0, 0};

  float start = -1; // -1 == none
  float end = -1;
  int num_v = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (sensorValue[i] == 0) {
      if (start == -1) {
        start = sensorDeg[i];
      }
      end = sensorDeg[i];
    }
    else {
      if (i != 0 && end == sensorDeg[i-1]) {  // 角度
        Vector v = fromPolar((start + end) / 2, 1);
        line_v = add(line_v, v);
        num_v++;
        start = -1;
        end = -1;
      }
    }
  }
  if (start != -1) {
    Vector v = fromPolar((start + end) / 2, 1);
    line_v = add(line_v, v);
    num_v++;
  }

  line_v = divide(line_v, num_v);
  if (num_v == 0) {
    line_angle = 400;
  }
  else {
    line_angle = atan2(line_v.y, line_v.x) * 180.0 / M_PI;
    if (line_angle < -180) line_angle += 360;
    if (line_angle >= 180) line_angle -= 360;
  }
  line_dis = sqrt(line_v.x * line_v.x + line_v.y * line_v.y) * 100;

  // if (line_angle < 0) line_angle += 360.0;
  // Serial.print(" ");
  // Serial.println(line_angle);
}
