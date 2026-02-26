
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "UART.h"

class Monitor {

private:
  static constexpr uint8_t SCREEN_WIDTH  = 128;
  static constexpr uint8_t SCREEN_HEIGHT = 64;  // 座標は0~63
  static constexpr int8_t OLED_RESET    = -1;
  static constexpr uint8_t SCREEN_ADDRESS = 0x3C;

  Adafruit_SSD1306 display;
  // int number;

public:
  // コンストラクタ
  Monitor() :
    display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) // 関数の前に値を初期化(代入みたいな感じ)
  {}

  void initMonitor() {
    Wire.setSDA(4);
    Wire.setSCL(5);
    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
  }

  void writeNumber(int x, int y, int number) {
    display.setCursor(x, y);
    display.print(number);
    display.display();
  }

  void updateMonitor() {
    display.clearDisplay();
  }
};

class Echo {
private:
  static constexpr uint8_t NUM_SENSORS = 8;
  static constexpr uint8_t pins[NUM_SENSORS] = {11, 12, 13, 14, 15, 8, 9, 10};  // 前　

public:
  int16_t readEcho(int pin) {
    // --- 出力モードでトリガ発射 ---
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delayMicroseconds(2);
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);

    // --- 入力モードに切り替えて Echo 待ち ---
    pinMode(pin, INPUT);

    // ✅ タイムアウト付き pulseIn（最大10ms ≒ 170cm）
    long t = pulseIn(pin, HIGH, 10000);

    // ✅ 反射なしは -1 を返す
    if (t == 0) return -1;

    // ✅ 距離へ変換（mm）
    return (int16_t)(t / 5.8);
  }
  // int16_t echoValue() {
  //   for (int i = 0; i < NUM_SENSORS; i++) {
      
  //   }
  // }
};

class Button {
  private:
    static constexpr uint8_t NUM_PINS = 4;  // 関数内ではなく外で書かれたstaticはこのクラスからできたオブジェクト間で共有される
    static constexpr uint8_t pins[NUM_PINS] = {2, 3, 6, 7};
    int8_t buttonStates[NUM_PINS] = {0};  // このオブジェクトがある間はずっと値は保持される

  public:
    void initButton() {
      for (int i = 0; i < NUM_PINS; i++) {
        pinMode(pins[i], INPUT_PULLUP);
      }
    }

    void readButton() {  // ポインタ指定で引数の配列を編集する
      for (int i = 0; i < NUM_PINS; i++) {

        int button_state = digitalRead(pins[i]) == HIGH ? 0 : 1;  // ON = 0、OFF = 1に変換

        buttonStates[i] =
          (button_state == HIGH)
          ? (buttonStates[i] == 0 ? 1 : 2)
          : 0;
      }
    }

    int8_t buttonState(int index) {
      return buttonStates[index];
    }
};

Monitor monitor;
Echo echo;
Button button;

void setup() {
  initUART();
  monitor.initMonitor();
  button.initButton();
}

void loop() {
  UART();

  // monitor.setNumber(date_main.test);   // ← ここはUART.h側の問題
  
  button.readButton();
  monitor.updateMonitor();
  for (int i = 0; i < 4; i++) {
    monitor.writeNumber(20, 10 + 10 * i, button.buttonState(i));
    Serial.print(button.buttonState(i));
  }
  // Serial.print(digitalRead(2));
  // Serial.print(digitalRead(3));
  // Serial.print(digitalRead(6));
  // Serial.print(digitalRead(7));
  Serial.println();
  // delay();
}

