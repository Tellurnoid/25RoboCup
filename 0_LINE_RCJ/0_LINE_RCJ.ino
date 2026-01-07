#include <Arduino.h>
#include <math.h>

const int S1 = 1;
const int S2 = 2;
const int S3 = 3;
const int EN_A = 10;
const int EN_B = 5;
const int EN_C = 4;
const int EN_D = 9;
const int analogPin = A0;
const int enPins[4] = {EN_A, EN_B, EN_C, EN_D};//右から時計回り

int sensor[4][6]{//素子とMUXのチャンネルはばらばらにつながっているので補正
  // {3, 0, 1, 2, 6, 4},
  // {6, 7, 3, 0, 1, 2},
  // {5, 7, 3, 0, 1, 2},
  // {3, 0, 1, 2, 6, 4}
  {4,6,2,1,0,3},
  {2,1,0,3,7,5},
  {2,1,4,3,7,5},//６は✗ 4,5 325486
  {4,6,2,1,0,3}
};
//マルチプレクサ
///////////////////////////////////////////////////////////////////////////


void initMUX(){
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(enPins[i], OUTPUT);
    digitalWrite(enPins[i], HIGH); // すべて無効
  }
}

// チャンネル選択（0〜7）
void selectChannel(int ch) {
  digitalWrite(S1, ch & 0x01);
  digitalWrite(S2, (ch >> 1) & 0x01);
  digitalWrite(S3, (ch >> 2) & 0x01);
}

void readMUX(){
  for (int mux = 0; mux < 4; mux++) {
    for (int i = 0; i < 4; i++) { // 全MUX無効
      digitalWrite(enPins[i], HIGH);
    }
    digitalWrite(enPins[mux], LOW);// 対象MUXのみ有効
    delayMicroseconds(5); // 切り替え安定待ち

    for (int ch = 0; ch < 6; ch++) {//読み込む
      selectChannel(sensor[mux][ch]);
      //selectChannel(ch);
      delayMicroseconds(5); // チャンネル安定待ち
      sensorValue[mux][ch] = analogRead(analogPin);
    }  
  }
}

void printAllMUX(){
  for (int mux = 0; mux < 4; mux++) {
      for (int ch = 0; ch < 6; ch++) {
        Serial.print(",");
        Serial.print(sensorValue[mux][ch]);
      }
    Serial.print("..");
   }
  Serial.println();
}


///////////////////////////////////////////////////////////////////////////

int sensorValue[4][8];// 読み取り結果を格納、[マルチプレクサ選択][チャンネル]
//4123865


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

const int NUM_SENSORS = 24;
// const int sensorPins[NUM_SENSORS] = {4,7,6,8,9,11,12,15,14,17,16,18,19,2,3,5};
const int sensorPins[NUM_SENSORS] = {4,5,3,2,19,18,16,17,14,15,12,11,9,8,6,7};
const int sensorDeg[NUM_SENSORS] = {0,15,30,45,60,75,90,105,120,135,150,165};

int sensorValue[NUM_SENSORS];

Vector line_v;

void setup() {
  initUART();
  initMUX();
}

const int NUM_READ = 200;
int16_t line_angle;
int16_t line_dis;

void loop() {
  UART();
  

  for (int i = 0; i < NUM_SENSORS; i++) {
    int line_total = 0;
    // for (int j = 0; j < NUM_READ; j++) {
    //   if (i == 12 || i == 13) { // 値が逆転しちゃうやつだけ修正
    //     line_total += (1 - digitalRead(sensorPins[i]));
    //   }
    //   else {
    //     line_total += digitalRead(sensorPins[i]);
    //   }
    // }
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
