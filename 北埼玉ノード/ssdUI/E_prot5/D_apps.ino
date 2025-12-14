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
    sp0.UISendHead = 1;
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
    if(digitalRead(BackPin)==LOW){
     cursor=readData.ROM_cursor;
      tone(SPKPin,550,50);
      delay(80);
      tone(SPKPin,110,100);
      draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else if(IsPress_OK()){
    myData.ROM_LED_output = setYourVal_int(0,255);//値調整UI関数で取得した値をEEPROM用の仮保存変数に適用
    EEPROM.put(offsetof(DataFormat,ROM_LED_output),myData.ROM_LED_output);//EEPROMに書き込み
      decidedSound();
    display.fillRect(0,64-8,128,8,0);

    //intの場合↓
    display.setFont(NULL);
    display.setTextSize(1);
    int writtenIntData = myData.ROM_LED_output;
    printString("saved as",writtenIntData,52,64-8);
    //floatの場合↓
    //float writtenFloatData = myData.ROM_LED_output;
    //printString
    //printString("saved as",writtenFloatData,58,64-8,1);
    if(IsPress_OK){
    while(IsPress_OK());
    }
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
      myData.ROM_BoolVal1 = BoolVals[0];
      myData.ROM_BoolVal2 = BoolVals[1];
      myData.ROM_BoolVal3 = BoolVals[2];
      EEPROM.put(offsetof(DataFormat,ROM_BoolVal1),myData.ROM_BoolVal1);//EEPROMに書き込み
      EEPROM.put(offsetof(DataFormat,ROM_BoolVal2),myData.ROM_BoolVal2);//EEPROMに書き込み
      EEPROM.put(offsetof(DataFormat,ROM_BoolVal3),myData.ROM_BoolVal3);//EEPROMに書き込み
      cursor=readData.ROM_cursor;
      tone(SPKPin,550,50);
      delay(80);
      tone(SPKPin,110,100);
      draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else if(IsPress_OK()){
    //myData.ROM_LED_output = setYourVal_int(0,255);//値調整UI関数で取得した値をEEPROM用の仮保存変数に適用
    //EEPROM.put(offsetof(DataFormat,ROM_LED_output),myData.ROM_LED_output);//EEPROMに書き込み
  //  display.fillRect(0,64-8,128,8,0);
  //  display.setFont(NULL);
   // display.setTextSize(1);
   // bool writtenIntData = myData.ROM_LED_output;
   // printString("saved as",writtenIntData,52,64-8);
   BoolVals[cursor_settingBool]=!BoolVals[cursor_settingBool]; 
      decidedSound();

     display.fillRect(128-6*5,8,128-6*5,64-8,0);
     for(int i=0; i<boolCursorMax; i++){
      //printString_char(BoolValsNames[i],15,8+i*8);
       if(BoolVals[i]==0){
        printString_char("TRUE",128-6*5,8+i*8);
       }
       else{
        printString_char("FALSE",128-6*5,8+i*8);
       }
      }
    if(IsPress_OK){
    while(IsPress_OK());
    }
    }
    else{
       setYourVal_Bool();//値調整UIを回し続ける
   } 
  }

void app3(){
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
  if(digitalRead(BackPin)==LOW){
    cursor=readData.ROM_cursor;
      tone(SPKPin,550,50);
      delay(80);
      tone(SPKPin,110,100);
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{
    

  }
}

void preview_SetYourVal_BigInt(){
  //display.clearDisplay();
  //display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  //display.print(app4Name);
  display.fillRect(35,8,53,64-16,0);
      //数字と三角のポインター
      //display.setTextSize(1);
      display.setCursor(36,35);//左端x=37、右端x=37+30=67
      display.setFont(&FreeSans9pt7b);
      for(int i=0; i<5; i++){
        display.drawTriangle(36+10*i-2+FontDistX,35-5-FontDistY, 36+10*i+FontDistX,35-5-5-FontDistY, 36+10*i+2+FontDistX,35-5-FontDistY ,1);
        display.print(costom1_nums[i]);
        display.drawTriangle(36+10*i-2+FontDistX,35+11+7-FontDistY, 36+10*i+FontDistX,35+11+5+7-FontDistY, 36+10*i+2+FontDistX,35+11+7-FontDistY ,1);
      }
      display.fillTriangle(36+10*(5-cursor_BigInt)-2+FontDistX,35-5-FontDistY, 36+10*(5-cursor_BigInt)+FontDistX,35-5-5-FontDistY, 36+10*(5-cursor_BigInt)+2+FontDistX,35-5-FontDistY ,1);
      display.fillTriangle(36+10*(5-cursor_BigInt)-2+FontDistX,35+11+7-FontDistY, 36+10*(5-cursor_BigInt)+FontDistX,35+11+5+7-FontDistY, 36+10*(5-cursor_BigInt)+2+FontDistX,35+11+7-FontDistY ,1);
      display.drawRoundRect(34+10*(5-cursor_BigInt)-2+FontDistX-1,34-5-5-FontDistY-1, 12,38,2,1);
      display.display();
}

void app_costoms(int costoms){//costom1
  DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
  if(IsPress_OK()){
    if(cursor_BigInt>4){
      sum_bigInt = costom1_nums[0]*10^5 +
                   costom1_nums[1]*10^4 +
                   costom1_nums[2]*10^3 +
                   costom1_nums[3]*10^2 +
                   costom1_nums[4]*10^1 +
                   costom1_nums[5];
      cursor_BigInt=5;
      if(costoms==1){
      readData.ROM_costom1=sum_bigInt;
      EEPROM.put(offsetof(DataFormat,ROM_costom1),myData.ROM_costom1);//EEPROMに書き込み
      }
      else if(costoms==2){
      readData.ROM_costom2=sum_bigInt;
      EEPROM.put(offsetof(DataFormat,ROM_costom2),myData.ROM_costom2);//EEPROMに書き込み
      }
      decidedSound();
    
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
      //ここに送信、EEPROM書き込みなど保存関数
    }
    else{
      cursor_BigInt++;
      preview_SetYourVal_BigInt();
    }
    if(IsPress_OK()){while(IsPress_OK());}
   }
   else if(IsPress_Back()){
    cursor_BigInt--;
    if(cursor_BigInt<1){
      /*
      sum_bigInt = costom1_nums[0]*10^5 +
                   costom1_nums[1]*10^4 +
                   costom1_nums[2]*10^3 +
                   costom1_nums[3]*10^2 +
                   costom1_nums[4]*10^1 +
                   costom1_nums[5];     
      //戻るはキャンセルとみなす
      */
      tone(SPKPin,550,50);
      delay(80);
      tone(SPKPin,110,100);
      

    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
    } 
    else{
      preview_SetYourVal_BigInt();
    }
    if(IsPress_Back()){while(IsPress_Back());}
    
  }
  else if(IsPress_UP()){
    costom1_nums[5-cursor_BigInt] ++;
    if(costom1_nums[5-cursor_BigInt]>9){
      costom1_nums[5-cursor_BigInt]=0;
    }
    if(IsPress_UP()){while(IsPress_UP());}
    preview_SetYourVal_BigInt();
  }
  else if(IsPress_Down()){
    costom1_nums[5-cursor_BigInt] --;
    if(costom1_nums[5-cursor_BigInt]<0){
      costom1_nums[5-cursor_BigInt]=9;
    }
    if(IsPress_Down()){while(IsPress_Down());}
    preview_SetYourVal_BigInt();
  }
  
}


void app4(){//costom1
  
}
void app5(){}

/*
文字サイズと使用するピクセル
TextSize 1 → 6×8
TextSize 2 → 12×16
TextSize 3 → 18×24
TextSize 4 → 24×32
*/