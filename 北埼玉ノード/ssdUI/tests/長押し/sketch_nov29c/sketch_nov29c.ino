const int buttonPin = A3;   // ボタン接続ピン
const unsigned long longPressTime = 1000; // 長押しとみなす時間（ミリ秒）
int counter = 0;
bool lastButtonState = LOW;
unsigned long buttonPressStart = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // ボタンをプルアップで使用
  Serial.begin(115200);
}

void loop() {
  bool buttonState = digitalRead(buttonPin) == LOW; // 押されているとき true

  if (buttonState && !lastButtonState) {
    // ボタンが押された瞬間
    buttonPressStart = millis();
    counter++; // 単押しとして1加算
    Serial.println(counter);
  }

  if (buttonState) {
    // ボタンが押され続けている場合
    unsigned long pressDuration = millis() - buttonPressStart;
    if (pressDuration >= longPressTime) {
      // 長押し判定：押している間毎ループ加算
      counter++;
      Serial.println(counter);
      delay(30);
    }
  }

  lastButtonState = buttonState;
}
