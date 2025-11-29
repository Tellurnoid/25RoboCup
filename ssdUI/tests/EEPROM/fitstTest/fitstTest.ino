#include <EEPROM.h>
#include <stddef.h>

//型を定義
struct DataFormat {
  char* teamName;
  int LED_output;
  int yourValA;
  float yourValB;
};


void setup() {
  Serial.begin(115200);

  //EEPROMの全体のディスクサイズ(削除可)
  int RomSize = EEPROM.length();
  Serial.print("EEPROM's maxSize:");
  Serial.print(RomSize);
  Serial.println("byte");

  //型を使って変数を作成
  DataFormat myData = {"URAWA BEOLSAE (2025)",255,1,3.14};

  EEPROM.put(0, myData);  // 先頭(アドレス0)から書き込む
  Serial.println("myData Written!");

  //読み出し方
  DataFormat readData;
  EEPROM.get(0, readData);
  Serial.println(readData.teamName); 
  Serial.println(readData.LED_output);
  Serial.println(readData.yourValA);  
  Serial.println(readData.yourValB);  


  delay(1000);

  //個別に書き込み
  myData.LED_output = 100;
  EEPROM.put(offsetof(DataFormat,LED_output),myData.LED_output);

  Serial.println("LED_output re-written. Reloadin..");

  //読み出し方
  //DataFormat readData;
  EEPROM.get(0, readData);
  Serial.println(readData.teamName); 
  Serial.println(readData.LED_output);
  Serial.println(readData.yourValA);  
  Serial.println(readData.yourValB);  
}

void loop() {
}
