const int S1 = 2;  // Aピン (TC4051BPのピン11)
const int S2 = 3;  // Bピン (TC4051BPのピン10)
const int S3 = 4;  // Cピン (TC4051BPのピン9)
const int EN = 1;
const int analogPin = A0; // TC4051BPのCOMピン (ArduinoのA0に接続)

// 8つのポテンショメータの値を保存する配列
int potValues[8];

// マルチプレクサの制御信号を送るための関数
void selectChannel(int channel) {
  digitalWrite(S1, channel & 0x01);
  digitalWrite(S2, (channel >> 1) & 0x01);
  digitalWrite(S3, (channel >> 2) & 0x01);
}

void setup() {
  // 制御ピンを出力に設定
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);
  
  // シリアル通信の開始
  Serial.begin(115200);
}

void loop() {
  // 8つのチャンネルの値を順番に読み取る
  for (int i = 0; i < 8; i++) {
    selectChannel(i); // チャンネルを選択
    delay(5); // 安定するまで少し待つ
    potValues[i] = analogRead(analogPin); // アナログ値を読み取る
  }
  /*
  // 読み取った値をシリアルモニタに出力
  Serial.println("Potentiometer Values:");
  for (int i = 0; i < 8; i++) {
    Serial.print(" ,pin");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(potValues[i]);
  }
  */
  Serial.print(potValues[0]);Serial.print(",");
 Serial.println(potValues[1]);
  // 次の読み取りまで少し待つ
  //delay(10); // 1秒待機
}