//Bluetoothデバッグ用
#include "BluetoothSerial.h"
#define USE_NAME  // Comment this to use MAC address instead of a slaveName
BluetoothSerial SerialBT;
#ifdef USE_NAME
String slaveName = "ESP32-BT-Slave";  // Change this to reflect the real name of your slave BT device
#else
String MACadd = "AA:BB:CC:11:22:33";                        // This only for printing
uint8_t address[6] = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33};  // Change this to reflect real MAC address of your slave BT device
#endif
String BTName = "ESP32-BT-Master";



//ヤタガラスの設定(136行まで)-----------------------
#define INA1 33
#define INA2 32
#define INB1 26
#define INB2 25
#define INC1 27
#define INC2 14
#define IND1 13
#define IND2 4

void straight(){
  ledcWrite(INA1,motorSpeed);
  ledcWrite(INA2,0);
  ledcWrite(INB1,motorSpeed);
  ledcWrite(INB2,0);
  ledcWrite(INC1,motorSpeed);
  ledcWrite(INC2,0);
  ledcWrite(IND1,motorSpeed);
  ledcWrite(IND2,0);
}
void back(){
  ledcWrite(INA1, 0);
  ledcWrite(INA2,motorSpeed);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed);
}
void slideRight(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,motorSpeed);
  ledcWrite(INB1,motorSpeed);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed);
  ledcWrite(IND1,motorSpeed);
  ledcWrite(IND2,0);
}
void slideLeft(){
  ledcWrite(INA1, motorSpeed);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed);
  ledcWrite(INC1,motorSpeed);
  ledcWrite(INC2, 0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed);
}
void turnRight(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,motorSpeed);
  ledcWrite(INB1,motorSpeed);
  ledcWrite(INB2,0);
  ledcWrite(INC1,motorSpeed);
  ledcWrite(INC2, 0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed);
}
void turnLeft(){
  ledcWrite(INA1,motorSpeed);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed);
  ledcWrite(IND1,motorSpeed);
  ledcWrite(IND2,0);
}

void slideRightStraight(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,0);
  ledcWrite(INB1,motorSpeed+150);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,0);
  ledcWrite(IND1,motorSpeed+150);
  ledcWrite(IND2,0);
}
void slideLeftBack(){
  ledcWrite(INA1, 0);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed+150);
  ledcWrite(INC1,0);
  ledcWrite(INC2,0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed+150);
}
void slideLeftStraight(){
  ledcWrite(INA1,motorSpeed+150);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,0);
  ledcWrite(INC1,motorSpeed+150);
  ledcWrite(INC2,0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,0);
}
void slideRightBack(){
  ledcWrite(INA1, 0);
  ledcWrite(INA2,motorSpeed+150);
  ledcWrite(INB1,0);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed+150);
  ledcWrite(IND1,0);
  ledcWrite(IND2,0);
}
void stop(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,0);
}
//-----------------------------------------------------


const int freq = 550;        //pwm 周波数 Hz
const int resolution = 8;    //pwm 8ビット解像度（0〜motorSpeed）
HardwareSerial &mySerial = Serial2; // UART2を使用
int motorSpeed= 120;//pwmの最大値(フルパワー:255)

void setup() {
  Serial.begin(115200); 
  SerialBT.begin(BTName, true);
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  //モーター
  ledcAttach(INA1, freq, resolution);   
  ledcAttach(INA2, freq, resolution);
  ledcAttach(INB1, freq, resolution);   
  ledcAttach(INB2, freq, resolution);
  ledcAttach(INC1, freq, resolution);   
  ledcAttach(INC2, freq, resolution);
  ledcAttach(IND1, freq, resolution);   
  ledcAttach(IND2, freq, resolution);
  stop();
}


int Power;//距離からpwmの出力値をおおよそ決めるための変数
int guess = 0;//見失ったときの判断基準(0:前を見る 1:最後に見たのが左 2:最後に見たのが右)

void loop() {
  // データが6バイト以上あればブロブ情報を読む(x座標,y座標,面積の順で計6バイト受信)
  if (mySerial.available() >= 2) {
    // まず2バイト読み込む
    uint8_t buf[6];
    mySerial.readBytes(buf, 2);
    int first = (buf[0] << 8) | buf[1];

      if (first == 0xFFFF) {// ブロブなし(青色未検出の信号)
        Power=90; //回転するときは遅め
        if(guess==0){
          stop();
        }
        else if(guess==1)
        {
          turnLeft();
          delay(200);
          guess=0;
        }
        else{//guess==2
          turnRight();
          delay(500);
          guess=0;
        }
        Serial.println("No blob detected");
      } 
      else 
      {
        // ブロブあり(青検出)、残り4バイトも読む
        mySerial.readBytes(buf + 2, 4);
        int cx     = first;                 //x座標
        int cy     = (buf[2] << 8) | buf[3];//y座標
        int pixels = (buf[4] << 8) | buf[5];//面積    (面積の求め方(ピクセル数を数えるor四角形で囲った面積)はunitV側のコードで設定)
        int paraCx = 160-cx;                //x座標の、中央からの距離(右に行くと負、左に行くと正)
        int paraMotor1,paraMotor2,paraMotor3,paraMotor4;//Powerの値とcxを考慮した各モーターの出力値

        if(pixels > 8000){
          Power = 0;//近すぎたら出力0
         }
        else
        {
          Power = motorSpeed - (pixels-100)*motorSpeed/9900;//離れるほど速く 
        }

        if(abs(paraCx)<25){//正面に青
          straight();
          guess=0;
        }
        else if(paraCx < 0){//右側に青
          guess=2;
          //中央に近いほど前進、右にあるほどスライド
          paraMotor1 = Power+(Power*paraCx/160);
          paraMotor2 = -1*Power*paraCx/160;
          ledcWrite(INA1,paraMotor1);
          ledcWrite(INA2,paraMotor2);
          ledcWrite(INB1,Power);
          ledcWrite(INB2,0);
          ledcWrite(INC1,paraMotor1);
          ledcWrite(INC2,paraMotor2);
          ledcWrite(IND1,Power);
          ledcWrite(IND2,0);
        }
        else if(paraCx > 0){//左側に青
          guess=1;
          //中央に近いほど前進、左にあるほどスライド
          paraMotor3 = -1*Power*paraCx/160;
          paraMotor4 = Power+(Power*paraCx/160);
          ledcWrite(INA1,Power);
          ledcWrite(INA2,0);
          ledcWrite(INB1,paraMotor3);
          ledcWrite(INB2,paraMotor4);
          ledcWrite(INC1,Power);
          ledcWrite(INC2,0);
          ledcWrite(IND1,paraMotor3);
          ledcWrite(IND2,paraMotor4);      
        }

        Serial.print("cx: "); Serial.print(cx);
        Serial.print(", cy: "); Serial.print(cy);
        Serial.print(", area: "); Serial.print(pixels);
        Serial.print(", Power: "); Serial.print(Power);
        Serial.print(", guess: "); Serial.print(guess);
        Serial.print(", paraCx:");Serial.print(paraCx);
      }
  }
}
