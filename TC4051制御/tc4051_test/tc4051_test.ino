const int S1 = 1;
const int S2 = 2;
const int S3 = 3;
const int EN_A = 10;
const int EN_B = 5;
const int EN_C = 4;
const int EN_D = 9;
const int analogPin = A0;
const int enPins[4] = {EN_A, EN_B, EN_C, EN_D};//右から時計回り

int sensor[4][6]{//素子とMUXのチャンネルはばらばらにつながっているので補正
  // {3, 0, 1, 2, 6, 4},
  // {6, 7, 3, 0, 1, 2},
  // {5, 7, 3, 0, 1, 2},
  // {3, 0, 1, 2, 6, 4}
  {4,6,2,1,0,3},
  {2,1,0,3,7,5},
  {2,1,4,3,7,5},//６は✗ 4,5 325486
  {4,6,2,1,0,3}
};

int sensorValue[4][8];// 読み取り結果を格納、[マルチプレクサ選択][チャンネル]
//4123865


void initMUX(){
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(enPins[i], OUTPUT);
    digitalWrite(enPins[i], HIGH); // すべて無効
  }
}

// チャンネル選択（0〜7）
void selectChannel(int ch) {
  digitalWrite(S1, ch & 0x01);
  digitalWrite(S2, (ch >> 1) & 0x01);
  digitalWrite(S3, (ch >> 2) & 0x01);
}

void readMUX(){
  for (int mux = 0; mux < 4; mux++) {
    for (int i = 0; i < 4; i++) { // 全MUX無効
      digitalWrite(enPins[i], HIGH);
    }
    digitalWrite(enPins[mux], LOW);// 対象MUXのみ有効
    delayMicroseconds(5); // 切り替え安定待ち

    for (int ch = 0; ch < 6; ch++) {//読み込む
      selectChannel(sensor[mux][ch]);
      //selectChannel(ch);
      delayMicroseconds(5); // チャンネル安定待ち
      sensorValue[mux][ch] = analogRead(analogPin);
    }  
  }
}

void printAllMUX(){
  for (int mux = 0; mux < 4; mux++) {
      for (int ch = 0; ch < 6; ch++) {
        Serial.print(",");
        Serial.print(sensorValue[mux][ch]/100);
      }
    Serial.print("..");
   }
  Serial.println();
}

void setup() {
  initMUX();
  Serial.begin(250000);
}

void loop() {
  readMUX();
  printAllMUX();
}
 