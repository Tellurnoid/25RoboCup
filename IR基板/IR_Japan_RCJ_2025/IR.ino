// struct Vector {
//   float x;
//   float y;
// };
#include <Arduino.h>

Vector fromPolar(float deg, float speed) {
  Vector v;
  float rad = deg * M_PI / 180.0;
  v.x = cos(rad) * speed;
  v.y = sin(rad) * speed;
  return v;
}

Vector add(Vector a, Vector b) {
  Vector result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

const int NUM_SENSORS = 12;
const int sensorPins[NUM_SENSORS] = {34, 35, 32, 33, 25, 26, 27, 14, 13, 4, 2, 15};//const int sensorPins[NUM_SENSORS] = {35, 32, 33, 25, 26, 27, 14, 2, 15};
const int sensorDeg[NUM_SENSORS] = {30, 60, 90, 120, 150, 180, 210, 300, 330};//const int sensorDeg[NUM_SENSORS] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330};
const int DEG_DIFF = 270;
const int MAX_VALUE = 4095;
const int NUM_AVE = 40;
const int NUM_SERCH_SENSORS = 4;

int sensorValues[NUM_SENSORS];
long sensorTotals[NUM_SENSORS];

int ball_angle;
int ball_distance;
Vector vec_ball;

void initIR(){
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

// ---------------- 安全版 ----------------
void readSensorsAve() {
  ball_distance = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorTotals[i] = 0;
  }

  for (int i = 0; i < NUM_AVE; i++) {
    for (int j = 0; j < NUM_SENSORS; j++) {
      int val = analogRead(sensorPins[j]);
      // ADC安全化
      if (val < 0) val = 0;
      if (val > MAX_VALUE) val = MAX_VALUE;

      sensorTotals[j] += val;
      delayMicroseconds(10);
    }
    delayMicroseconds(50);
  }

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = MAX_VALUE - sensorTotals[i] / NUM_AVE;
    if (sensorValues[i] < 0) sensorValues[i] = 0;
    if (sensorValues[i] > MAX_VALUE) sensorValues[i] = MAX_VALUE;
    if (sensorValues[i] != 0) ball_distance++;
  }
}


int getMaxIndex() {
  int max_index = 0;
  int max_val = sensorValues[0];
  for (int i = 1; i < NUM_SENSORS; i++) {
    if (sensorValues[i] > max_val) {
      max_val = sensorValues[i];
      max_index = i;
    }
  }
  return max_index;
}

float getPeakAngle() {
  int k = getMaxIndex();
  int kL = (k - 1 + NUM_SENSORS) % NUM_SENSORS;
  int kR = (k + 1) % NUM_SENSORS;

  float SL = max(sensorValues[kL], 1);  // log(0)対策
  float S0 = max(sensorValues[k], 1);
  float SR = max(sensorValues[kR], 1);

  float d = 0.5 * (log(SL) - log(SR)) / (log(SL) - 2*log(S0) + log(SR));
  if (!isfinite(d)) d = 0;  // 異常値対策

  float angle = sensorDeg[k] + d * 30.0 + DEG_DIFF;

  if (angle < -180) angle += 360;
  if (angle >= 180) angle -= 360;

  return angle;
}

// struct BallResult {
//   float angle;
//   float distance;
// };

BallResult getBallAngleAndDistance() {

  int k = getMaxIndex();
  int kL = (k - 1 + NUM_SENSORS) % NUM_SENSORS;
  int kR = (k + 1) % NUM_SENSORS;

  float SL = sensorValues[kL];
  float S0 = sensorValues[k];
  float SR = sensorValues[kR];

  if (SL < 1.0f) SL = 1.0f;
  if (S0 < 1.0f) S0 = 1.0f;
  if (SR < 1.0f) SR = 1.0f;

  // ==== 角度（θ） ====
  float d = 0.5f * (log(SL) - log(SR)) /
            (log(SL) - 2.0f * log(S0) + log(SR));

  if (!isfinite(d)) d = 0;

  float angle = sensorDeg[k] + d * 30.0f + DEG_DIFF;

  if (angle < -180) angle += 360;
  if (angle >= 180) angle -= 360;

  // ==== 距離①：強度ベース ====
  float dist1 = 1.0f / sqrt(S0);

  // ==== 距離②：ガウス幅ベース ==== 
  float sigma2 = -(30.0f * 30.0f) / (2.0f * (log(SL) - log(S0)));

  if (!isfinite(sigma2) || sigma2 < 1.0f) sigma2 = 1.0f;
  float dist2 = sqrt(sigma2);

  // ==== 融合 ====
  // float distance = 0.7f * dist1 + 0.3f * dist2;
  float distance = dist2;

  return {angle, distance};
}


void get_ball_angle(){
  vec_ball = {0, 0};
  int max_index = getMaxIndex();

  for (int i = max_index - NUM_SERCH_SENSORS/2; i <= max_index + NUM_SERCH_SENSORS/2; i++){
    int index = (i + NUM_SENSORS) % NUM_SENSORS;

    Vector vec_sensor = fromPolar(sensorDeg[index], sensorValues[index]);
    vec_ball = add(vec_ball, vec_sensor);
  }

  ball_angle = getPeakAngle();

  if (sensorValues[max_index] == 0) ball_angle = 400;  // ボール未検出
}

int ball_max;

void IR(){
  readSensorsAve();
  get_ball_angle();
  int x = sensorValues[(getMaxIndex() - 1 + NUM_SENSORS) % NUM_SENSORS];
  if (x < sensorValues[(getMaxIndex() + 1 + NUM_SENSORS) % NUM_SENSORS]) x = sensorValues[(getMaxIndex() + 1 + NUM_SENSORS) % NUM_SENSORS];
  ball_max = x; //indexが負の数にならないようにする
   //Serial.println(ball_angle);


///デバッグ
  // for (int i=0;i<NUM_SENSORS; i++){
  //   Serial.print(", ");Serial.print(sensorPins[i]);Serial.print(":");Serial.print(analogRead(sensorPins[i]));
  // }
  // Serial.println();
///////

//Serial.print(", ");Serial.print("8");Serial.print(":");Serial.print(analogRead(sensorPins[13]));
//Serial.print(", ");Serial.print("9");Serial.print(":");Serial.println(analogRead(sensorPins[4]));
//Serial.print(analogRead(4));Serial.print(",");Serial.println(analogRead(2));//4ピンのみしんでる
}
