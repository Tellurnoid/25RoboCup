#include <Wire.h>
#include <MPU6050.h>
//cf) https://chatgpt.com/share/69a16724-a8e0-8002-8614-90b82d7c1a18
//デフォルト
#define SDA_PIN 4
#define SCL_PIN 5

MPU6050 mpu(0x68);

void setup() {
  Serial.begin(115200);
  //while (!Serial);//シリアルポートを開かれるまで待つ(デバッグ用。本番書きは使用禁止)  
  Wire.end();
  //GPIO初期化、I2C BUS 解放(GPIO安定化のため一応書くべき)
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  delay(20);
  //デフォルトはSDA=4,SCL=5、明示することが望ましい
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
  //Wire.setClock(100000);   //100kHz(安全)
  Wire.setClock(400000);   // 400kHz(仕様上はここまでOK)
  delay(200);
  mpu.initialize();//MPU初期化
  mpu.setDLPFMode(3);//DLPF（内部ローパス）の帯域を44Hzに設定、遅延を減らす(0にするとノイズが入るが最速)
  mpu.setRate(7);//サンプルレート約125Hz(I2C負荷が減るらしい)
}

void loop() {

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
//   if (mpu.testConnection()) {
//     Serial.print("MPU6050 connected__");
//   } else {
//     Serial.print("connection failed__");
//   }
  Serial.print("ACC: ");
  Serial.print(ax); Serial.print(", ");
  Serial.print(ay); Serial.print(", ");
  Serial.print(az);

  Serial.print(" | GYRO: ");
  Serial.print(gx); Serial.print(", ");
  Serial.print(gy); Serial.print(", ");
  Serial.println(gz);

  delay(10);
}