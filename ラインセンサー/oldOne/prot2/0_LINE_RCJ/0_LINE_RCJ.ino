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
int area_four[4] = {0,0,0,0};//前から右回り、反応あり=1

int sensorValue[NUM_SENSORS];
int sensorValue_law[NUM_SENSORS];//こっちは踏んだら１、注意


Vector line_v;

void setup() {
  //initUART();
 Serial.begin(115200); 
 for (int i = 0; i < NUM_SENSORS; i++) {                     
    pinMode(sensorPins[i], INPUT);
  }
  pinMode(13, OUTPUT);
}

int16_t line_angle;
int16_t line_dis;
int16_t line_num_of_sensors = 0;//反応素子数


const int num_draft = 600;
bool digitalRead_withDraft(int pin) {
  int highCount = 0;
  for (int i = 0; i < num_draft; i++) {
    if (digitalRead(pin) == 0) {
      highCount++;
    }
  }
  return (highCount > num_draft*0.4);
}

void forDebug(){
  analogWrite(10, 160);
  for(int i=0; i<NUM_SENSORS; i++){
    Serial.print(digitalRead_withDraft(sensorPins[i]));
    Serial.print(",");
  }
  Serial.println();
}

void loop(){
  //UART();
}