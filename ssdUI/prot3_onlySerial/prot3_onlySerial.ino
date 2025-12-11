#include <EEPROM.h>
#include <stddef.h>
#include <Tiny4kOLED.h>
//oled

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

// ========= パケット用データ =========
struct ReadPacketData0 {int16_t receive_data; };
ReadPacketData0 rp0;


/*
ヘッダー対応表
1 LEDの明るさ(0~255)
2 costom1 (int )5ケタ
3 costom2 (int )5ケタ
*/

struct SendPacketData0 {
  int ball_angle;
  int goal_angle;
  int UISendHead;//3ケタで表現(1桁目：対象マイコン 2桁目:送信する変数の種類)
  float UISendData;
  };
SendPacketData0 sp0;

// ========= ユーティリティ =========
uint8_t calcChecksum(uint8_t* data, uint8_t len) {
  uint8_t sum = 0;
  for (int i = 0; i < len; i++) sum ^= data[i];
  return sum;
}

// ========= パケット読み取り =========
template <typename readT>
bool readPacket(int num_serial, readT &d) {

    HardwareSerial *seri;
    if (num_serial == 0) seri = &Serial;
    uint32_t start = millis();
    uint8_t header;

    // --- ヘッダー検索（タイムアウト 30ms） ---
    while (true) {
      if (seri->available()) {
        header = seri->read();
        if (header == 0xAA) break;  // 見つかった
      }
      if (millis() - start > 30) return false; // タイムアウト
    }

    // --- 長さ取得 ---
    while (!seri->available()) {
      if (millis() - start > 30) return false;
    }
    uint8_t len = seri->read();
    if (len != sizeof(readT)) return false;

    // --- データ + チェックサムが来るまで待つ ---
    while (seri->available() < len + 1) {
      if (millis() - start > 30) return false;
    }

    // --- 読み込み ---
    uint8_t buf[64];
    for (int i = 0; i < len; i++) buf[i] = seri->read();
    uint8_t checksum = seri->read();

    // --- チェックサム検証 ---
    uint8_t sum = calcChecksum(buf, len);
    if (checksum != sum) return false;

    // --- struct にコピー ---
    memcpy(&d, buf, len);

    return true;
}

// ========= パケット送信 =========
template <typename sendT>
void sendPacket(int num_serial, sendT &d) {

    HardwareSerial *seri;
    if (num_serial == 0) seri = &Serial;

    uint8_t* p = (uint8_t*)&d;
    uint8_t len = sizeof(sendT);
    uint8_t cs = calcChecksum(p, len);

    seri->write(0xAA);
    seri->write(len);
    seri->write(p, len);
    seri->write(cs);
}

// ========= UART 初期化 =========
void initUART() {
  Serial.begin(115200);
}

// ========= UART 読み取り =========
void UART() {
  readPacket(0, rp0);
  //ball_angle = rp1.ball_angle;
  //ball_distance = rp1.ball_distance;

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------



#define screenWidth 128
#define screenHeight 64

#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6


//型を定義 EEPROMに書き込む変数を増やす場合はここに定義UpPin
struct DataFormat {
  int ROM_cursor;
  char* ROM_teamName;
  int ROM_LED_output;
  bool ROM_BoolVal1;
  bool ROM_BoolVal2;
  bool ROM_BoolVal3;
  long ROM_costom1;  //1 costom1 (int )
  long ROM_costom2;  //5 costom2  intは16bit（-32768〜32767）
};
DataFormat writeData;

//ボタン
bool IsPress_UP(){
  return digitalRead(UpPin) == LOW;
}
bool IsPress_Down(){
  return digitalRead(DownPin) == LOW;
}
bool IsPress_OK(){
  return digitalRead(OKPin) == LOW;
}
bool IsPress_Back(){
  return digitalRead(BackPin) == LOW;
}
volatile uint8_t app;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
uint8_t cursor;
uint8_t howManyApps;


uint8_t width = 128;
uint8_t height = 64;



void setup(){
  initUART();
  //Serial.begin(115200);
  //oled.begin(width, height, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);  
  oled.setFont(FONT8X16);
  oled.on();
  randomSeed(analogRead(0));
 // startUpShow();
  pinMode(UpPin, INPUT_PULLUP);
  pinMode(DownPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor = readData.ROM_cursor;
  oled.clear();
  oled.setCursor(8,1);
  oled.print("HELLO");
  if (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down())) {
    while (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down()));
  }

}

void loop()
{
readPacket(0,rp0);
//oled.clear();
oled.setCursor(0,64-8);
oled.print("     ");
oled.setCursor(0,64-8);
int16_t readData = rp0.receive_data;
oled.print(readData);
//delay(1);
}
