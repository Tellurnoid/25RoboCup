#include <Arduino.h>
#include <EEPROM.h>

//Configの部分を使用するstructの名前に置き換える///////////////////////
#define EEPROM_SIZE sizeof(Config) + 16
//データの位置。struct一つの場合は0固定
#define addr_EEPROM 0

struct Config {
    int data1;
    int data2;
    bool data3;
};
Config config;


void EEPROM_write_read_test(){
  // 読み込み (位置,struct)
  EEPROM.get(0,config);
  Serial.print("data1:");Serial.print(config.data1);
  Serial.print(", data2:");Serial.print(config.data2);
  Serial.print(", data3:");Serial.print(config.data3);
  Serial.println();

  //値を変更
  Serial.println("changing..");
  config.data1 = config.data1 + 1;
  config.data2 = config.data2 + 1;
  config.data3 = 1 - config.data3;

  // 書き込み
  EEPROM.put(0, config);
  //書込内容を適用
  EEPROM.commit(); 

  Serial.println("written!");
}


void setup() {
  Serial.begin(115200);
  // EEPROM初期化
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("EEPROM init failed");
   }


  EEPROM_write_read_test();


//   // 読み込み (位置,struct)
//   EEPROM.get(0,config);
//   // 書き込み
//   EEPROM.put(0, config);
//   //書込内容を適用
//   EEPROM.commit(); ad_test();

//使う関数↓
//   // 読み込み (位置,struct)
//   EEPROM.get(0,config);
//   // 書き込み
//   EEPROM.put(0, config);
//   //書込内容を適用
//   EEPROM.commit(); 
}

void loop() {
}