#include <Wire.h>
#include <math.h>

// MPU6050 レジスタ定義
#define MPU6050_ADDR         0x68
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1A
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_WHO_AM_I     0x75

// グローバル共有角度変数（volatileにして排他制御）
volatile float angleX = 0.0f;
volatile float angleY = 0.0f;
volatile float angleZ = 0.0f;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // 排他制御用

TaskHandle_t mpuTaskHandle = NULL;

// MPU6050へ書き込み
void writeMPU6050(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// MPU6050から1バイト読み込み
uint8_t readMPU6050(uint8_t reg) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, (uint8_t)1, (bool)true);
  return Wire.read();
}

// MPU6050接続確認
bool checkMPU6050() {
  return (readMPU6050(MPU6050_WHO_AM_I) == 0x68);
}

float dpsX, dpsY, dpsZ;

// Core0で動くタスク：MPU6050読み取り＆角度計算
void mpuTask(void* parameter) {

  // キャリブレーション用オフセット変数
  double offsetX = 0.0, offsetY = 0.0, offsetZ = 0.0;
  double gyro_angle_x = 0.0, gyro_angle_y = 0.0, gyro_angle_z = 0.0;
  float acc_angle_x = 0.0f, acc_angle_y = 0.0f;

  unsigned long preMicros = micros();

  // キャリブレーション（3000回読み込み）
  Serial.println("MPU6050 Calibration Start");
  for (int i = 0; i < 3000; i++) {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B); // 加速度センサデータ開始レジスタ
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, (uint8_t)14, (bool)true);

    // 生データ読み込み
    int16_t raw_acc_x = Wire.read() << 8 | Wire.read();
    int16_t raw_acc_y = Wire.read() << 8 | Wire.read();
    int16_t raw_acc_z = Wire.read() << 8 | Wire.read();
    Wire.read(); Wire.read(); // 温度スキップ
    int16_t raw_gyro_x = Wire.read() << 8 | Wire.read();
    int16_t raw_gyro_y = Wire.read() << 8 | Wire.read();
    int16_t raw_gyro_z = Wire.read() << 8 | Wire.read();

    // ジャイロ角速度をdpsに変換（±500dpsレンジで65.5 LSB/dps）
    dpsX = ((float)raw_gyro_x) / 16.4;
    dpsY = ((float)raw_gyro_y) / 16.4;
    dpsZ = ((float)raw_gyro_z) / 16.4;

    offsetX += dpsX;
    offsetY += dpsY;
    offsetZ += dpsZ;

    if (i % 1000 == 0) Serial.print(".");
    vTaskDelay(1 / portTICK_PERIOD_MS); // 1ms待つ


    if ((i / 100) % 2 == 0) { ////////////////////////////////////////////////////////////////////////
      digitalWrite(beep, HIGH);
    } 
    else {
      digitalWrite(beep, LOW);
    }

  }
  digitalWrite(beep, HIGH);///////////////////////////////////////////////////////////////////////////////////
  delay(500);
  digitalWrite(beep, LOW);  //////////////////////////////////////////////////////////////////////////////////

  offsetX /= 3000.0;
  offsetY /= 3000.0;
  offsetZ /= 3000.0;
  Serial.println(" Calibration done.");

  while (true) {
    // 14バイトのセンサデータをI2Cから読み込み
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, (uint8_t)14, (bool)true);

    int16_t raw_acc_x = Wire.read() << 8 | Wire.read();
    int16_t raw_acc_y = Wire.read() << 8 | Wire.read();
    int16_t raw_acc_z = Wire.read() << 8 | Wire.read();
    Wire.read(); Wire.read(); // 温度スキップ
    int16_t raw_gyro_x = Wire.read() << 8 | Wire.read();
    int16_t raw_gyro_y = Wire.read() << 8 | Wire.read();
    int16_t raw_gyro_z = Wire.read() << 8 | Wire.read();

    // 加速度センサの値をGに変換
    float acc_x = (float)raw_acc_x / 16384.0f;
    float acc_y = (float)raw_acc_y / 16384.0f;
    float acc_z = (float)raw_acc_z / 16384.0f;

    // 加速度から角度計算
    acc_angle_y = atan2(acc_x, acc_z + fabs(acc_y)) * 180.0f / PI;
    acc_angle_x = atan2(acc_y, acc_z + fabs(acc_x)) * -180.0f / PI;

    // ジャイロ角速度をdpsに変換
    dpsX = (float)raw_gyro_x / 16.4f;
    dpsY = (float)raw_gyro_y / 16.4f;
    dpsZ = (float)raw_gyro_z / 16.4f;

    // 時間差計算
    unsigned long now = micros();
    float dt = (now - preMicros) / 1000000.0f;
    preMicros = now;

    // ジャイロ角度積分
    gyro_angle_x += (dpsX - offsetX) * dt;
    gyro_angle_y += (dpsY - offsetY) * dt;
    gyro_angle_z += (dpsZ - offsetZ) * dt;

    // 相補フィルタで角度融合
    float angle_x = 0.996f * gyro_angle_x + 0.004f * acc_angle_x;
    float angle_y = 0.996f * gyro_angle_y + 0.004f * acc_angle_y;
    float angle_z = gyro_angle_z;

    gyro_angle_x = angle_x;
    gyro_angle_y = angle_y;
    gyro_angle_z = angle_z;

    // 排他制御してグローバル変数更新
    portENTER_CRITICAL(&mux);
    angleX = angle_x;
    angleY = angle_y;
    angleZ = angle_z;  // =======================================================
    portEXIT_CRITICAL(&mux);

    vTaskDelay(10 / portTICK_PERIOD_MS); // 約100Hzで更新

    //////////////////////////////////////
    // // --- XY移動加速度の計算 ------------------------
    // // acc_x, acc_y, acc_z はすでにある

    // // ラジアンに変換
    // float radX = angleX * PI / 180.0f;
    // float radY = angleY * PI / 180.0f;
    // float radZ = angleZ * PI / 180.0f;

    // // 1. 重力成分（G）を姿勢角から計算
    // float gx = -sin(radY);
    // float gy =  sin(radX);
    // float gz =  cos(radX) * cos(radY);

    // // 2. 実加速度（重力を引いた値）
    // float ax_real = acc_x - gx;
    // float ay_real = acc_y - gy;
    // float az_real = acc_z - gz;

    // // 3. XY平面の加速度にする（必要ならZ軸回転）
    // float ax_world = ax_real * cos(radZ) - ay_real * sin(radZ);
    // float ay_world = ax_real * sin(radZ) + ay_real * cos(radZ);
    // Serial.print(ax_world * 100);
    // Serial.print(" : ");
    // Serial.println(ay_world * 100);
    // // --- ax_world, ay_world が「地面座標のXY加速度」 ---
  }
}

void initIMU() {
  // Serial.begin(115200);
  Wire.begin(22, 23, 400000); // SDA, SCL, 400kHz

  // MPU6050初期化（電源管理レジスタのスリープ解除）
  writeMPU6050(MPU6050_GYRO_CONFIG, 0x18);   //±2000dps
  delay(100);
  writeMPU6050(MPU6050_PWR_MGMT_1, 0x00);  // スリープ解除
  delay(100);

  if (!checkMPU6050()) {
    Serial.println("MPU6050 not found!");
    while (1) delay(100);
  }

  // センサ設定
  writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);    // サンプルレート設定（1kHz）
  writeMPU6050(MPU6050_CONFIG, 0x00);        // DLPF無効
  writeMPU6050(MPU6050_GYRO_CONFIG, 0x18);   // ジャイロ±500dps
  writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00);  // 加速度±2g

  delay(1000);
  // Core0にMPU読み取り＋角度計算タスクを作成
  xTaskCreatePinnedToCore(
    mpuTask,           // タスク関数
    "MPU6050_Task",    // 名前
    4096,              // スタックサイズ（バイト）
    NULL,              // パラメータ
    1,                 // 優先度
    &mpuTaskHandle,    // タスクハンドル
    0                  // Core0に割り当て
  );
}

void IMU() {
  float localX, localY, localZ;
  portENTER_CRITICAL(&mux);
  localX = angleX;
  localY = angleY;
  localZ = angleZ;
  portEXIT_CRITICAL(&mux);

  // Serial.printf("AngleX: %.2f, AngleY: %.2f, AngleZ: %.2f\n", localX, localY, localZ);
  // delay(1);
}

float Kp = 4.5, Ki = 0.0, Kd = 0.52; //dは角速度をそのまま使用しているため、シビア P:4.5,  D:0.52
float prevError = 0;
unsigned long prevTime = 0;
float integral = 0;

float camera_Kp = 1.5, camera_Kd = 0; //dは角速度をそのまま使用しているため、シビア P:4.5,  D:0.52

float rotatePID(int mode, int target_angle) {
  float error;
  if (mode == 0) {
    error = -target_angle - angleZ;
    while(error < -180) error += 360;
    while(error > 180) error -= 360;
    //I
    integral += error;
    float scale = 1.0;
    if (abs(error) < 20)  scale = 0.5;

    float output = Kp * error + Ki * integral - Kd * dpsZ * scale;
    if (abs(output) < 10)  output = 0;
    return output;
  }
  else if (mode == 1) {
    error = target_angle;
    while(error < -180) error += 360;
    while(error > 180) error -= 360;
    //I
    integral += error;
    float scale = 1.0;
    if (abs(error) < 20)  scale = 0.5;

    float output = camera_Kp * error - camera_Kd * dpsZ * scale;
    if (abs(output) < 10)  output = 0;
    return -output;
  }
}