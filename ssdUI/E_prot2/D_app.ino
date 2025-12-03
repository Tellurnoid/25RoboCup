
void preview_setIntVal(){}
int app_setIntVal(){}

void preview_setFloatVal(){}
int app_setFloatVal(){}

void app_costoms(uint8_t costoms){}


//boolsはつかうときに開ける
/*
const char* BoolValsNames[] = {
  "BoolVal1",
  "BoolVal2",
  "BoolVal3"
};
// 2. 名前配列の要素数をコンパイル時に取得
constexpr int boolCursorMax = sizeof(BoolValsNames) / sizeof(BoolValsNames[0]);
// 3. BoolVals 配列を boolCursorMax で定義
bool BoolVals[boolCursorMax] = {false}; // 全要素を false で初期化
int cursor_settingBool = 0;

void setYourVal_Bool_preview(){
      tone(SPKPin, 440, 20);
      display.fillRect(0,0,128,8,0);
      display.setCursor(20,0);
      display.setTextSize(1);
      display.print(cursor_settingBool);
      display.print(":");
      display.print(BoolValsNames[cursor_settingBool]);
      display.fillRect(10,8,3,64,0);
      display.fillRect(10,8+cursor_settingBool*8,3,8,1);
      display.display();
}

int setYourVal_Bool(){//StructTypeには共通でmyDataを入れる
    DataFormat myData;
    //上
    if(IsPress_Down()){
      cursor_settingBool=cursor_settingBool+1;
      if(cursor_settingBool>boolCursorMax){
        cursor_settingBool=0;
      }
      
      while(IsPress_Down());
      setYourVal_Bool_preview();
    }
    //下
    if(IsPress_UP()){
      cursor_settingBool=cursor_settingBool-1;
      if(cursor_settingBool<0){
        cursor_settingBool=boolCursorMax;
      }
      //while(IsPress_Down());
      
      while(IsPress_UP());
      setYourVal_Bool_preview();
    }
    
    return cursor_settingBool;
}


*/

