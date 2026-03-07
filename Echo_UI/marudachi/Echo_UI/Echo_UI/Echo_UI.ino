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
    static constexpr uint16_t draw_time = 100;
    uint32_t last_time = 0;
    // コンストラクタ
    Monitor() :
      display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) // 関数の前に値を初期化(代入みたいな感じ)
    {}

    void initMonitor() {
      Wire.setSDA(4);
      Wire.setSCL(5);
      Wire.begin();
      Wire.setClock(400000);   // 400kHz

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
      // display.display();
    }

    void clearMonitor() {
      display.clearDisplay();
    }

    void drawMonitor() {
      display.display();
    }
};

class Echo {
  private:
    static constexpr uint8_t NUM_SENSORS = 8;
    static constexpr uint8_t pins[NUM_SENSORS] = {11, 12, 13, 14, 15, 8, 9, 10};  // 前　

  public:
    int16_t echoValues[NUM_SENSORS] = {0};

    int16_t getEchoValue(int pin) {
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

    void readEcho() {
      for (int i = 0; i < NUM_SENSORS; i++) {
        echoValues[i] = getEchoValue(pins[i]);
      }
    }
};

class Button {
  private:
    static constexpr uint8_t NUM_PINS = 4;  // 関数内ではなく外で書かれたstaticはこのクラスからできたオブジェクト間で共有される
    static constexpr uint8_t pins[NUM_PINS] = {2, 3, 6, 7};

  public:
    int8_t buttonStates[NUM_PINS] = {0};

    void initButton() {
      for (int i = 0; i < NUM_PINS; i++) {
        pinMode(pins[i], INPUT_PULLUP);
      }
    }

    void readButton() {  // ポインタ指定で引数の配列を編集する
      for (int i = 0; i < NUM_PINS; i++) {

        int button_state = digitalRead(pins[i]) == HIGH ? 0 : 1;  // ON = 0、OFF = 1に変換

        buttonStates[i] =
          (button_state == HIGH)  // ボタンが押されている時
          ? (buttonStates[i] == 0 ? 1 : 2)  // 前フレームでボタンが押されてなかったら1、押されてたら2
          : 0;
      }
    }

    // int8_t buttonState(int index) {
    //   return buttonStates[index];
    // }
};

class UICommand {
  private:
    uint8_t ack = 0;
    uint32_t last_time = 0;
    uint32_t last_send_time = 0;
    // static constexpr uint32_t SEND_CYCLE = 100; // ms
  static constexpr uint32_t FAIL_TIME = 1000;

  public:
    void sendCommand(uint8_t id, uint8_t this_ack, uint8_t content_id, int16_t content) {
      to_main.id = id;
      to_main.ack = this_ack;
      to_main.content_id = content_id;
      to_main.content = content;
    }
    void command(uint8_t id, uint8_t content_id, int16_t content) {
      // last_time = millis();

      ack++;  // オーバーフローしたら勝手に0になる
      sendCommand(id, ack, content_id, content);  // 一回送る

      // last_send_time = millis();

      // while (millis() - last_time < FAIL_TIME) {
      //   if (data_main.ack == ack) return;

      //   // if (millis() - last_send_time > SEND_CYCLE) {
      //   //   sendCommand(id, ack, content_id, content);
      //   //   last_send_time = millis();
      //   // }
      // }
      
      // Serial.println("No reply for command");
      // return;
    }
};

Monitor monitor;
Echo echo;
Button button;
UICommand ui_command;

void setup() {
  initUART();
  monitor.initMonitor();
  button.initButton();
}

void loop() {
  UART();

  // monitor.setNumber(date_main.test);
  
  button.readButton();
  echo.readEcho();

  if ((millis() - monitor.last_time) >= monitor.draw_time) {
    monitor.last_time = millis();
    
    monitor.clearMonitor();

    monitor.writeNumber(20, 10, data_main.content);
    // monitor.writeNumber(20, 10, 1000);

    // for (int i = 0; i < 4; i++) {
    //   monitor.writeNumber(10, 5 + i * 10, echo.echoValues[i]);
    // }

    // for (int i = 4; i < 8; i++) {
    //   monitor.writeNumber(30, 5 + (i - 4) * 10, echo.echoValues[i]);
    // }

    monitor.drawMonitor();
  }

  if (button.buttonStates[0] == 1) {
    ui_command.command(MAIN, START, 1);
  }
}

