//FreeSansBoldOblique9pt7b
/*
書き込み関数↓
EEPROM.put(構造体の先頭から何番目か,構造体内の書き込みたい変数);

構造体の先頭から何番目かは、offsetof(DataFormat,メンバ名)で求まる
・DataFormatは構造体の型なのでみんな共通
・メンバ名はmyData.aのaにあたる部分




値編集関数の作り方
①メインファイルprot1.inoにある構造体に変数名を追加
struct DataFormat {
  char* ROM_teamName;
  int ROM_LED_output;
  ここにいつも通り定義;
};

②このファイルapps.ino内にある未使用のapp関数に書く
 app1をお手本にしてください
 ●setYourVal_int(min,max);と、setYourVal_float(min,max);は値調整UIの関数です。(systems.inoにある)
   引数に最小値と最大値を入れます
   そのまま使うとUIとして動き、引数として使うと、ユーザーが打ち込んだ値をintまたはfloatを返します
    
 ●以下2行は画面表示用なのでそのまま
    display.fillRect(0,64-8,128,8,0);
    printString_char("saved!",70,64-8,1);
　
 ●myData. とはprot1.inoの上の方で定義している構造体の型で作った変数です。追加した変数をint a;とするとmyData.aで呼び出せます
*/

//以下各アプリ

  void app1(){//LEDの明るさ調整
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
    if(digitalRead(BackPin)==LOW){
      draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else if(IsPress_OK()){
    myData.ROM_LED_output = setYourVal_int(0,255);//値調整UI関数で取得した値をEEPROM用の仮保存変数に適用
    EEPROM.put(offsetof(DataFormat,ROM_LED_output),myData.ROM_LED_output);//EEPROMに書き込み
    display.fillRect(0,64-8,128,8,0);

    //intの場合↓
    display.setFont(NULL);
    display.setTextSize(1);
    int writtenIntData = myData.ROM_LED_output;
    printString_int("saved as",writtenIntData,52,64-8);
    //floatの場合↓
    //float writtenFloatData = myData.ROM_LED_output;
    //printString_float("saved as",writtenFloatData,58,64-8,1);
    }
    else{
       setYourVal_int(0,255);//値調整UIを回し続ける
   }    
}



  void app2(){
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
    if(digitalRead(BackPin)==LOW){
      draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else{

    }
  }

void app3(){
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
  if(digitalRead(BackPin)==LOW){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{
    

  }
}

void app4(){
  DataFormat readData;
  EEPROM.get(0, readData);
  if(digitalRead(BackPin)==LOW){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{

  }
}

void app5(){
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
  if(digitalRead(BackPin)==LOW){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{
    

  }
}
/*
文字サイズと使用するピクセル
TextSize 1 → 6×8
TextSize 2 → 12×16
TextSize 3 → 18×24
TextSize 4 → 24×32
*/