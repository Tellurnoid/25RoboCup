#include "button.h"
#include <Arduino.h>

    
    void Button::init() {
      for (int i = 0; i < NUM_PINS; i++) {
        pinMode(pins[i], INPUT_PULLUP);
      }
    }

    void Button::read() {  // ポインタ指定で引数の配列を編集する
      for (int i = 0; i < NUM_PINS; i++) {

        int button_state = digitalRead(pins[i]) == HIGH ? 0 : 1;  // ON = 0、OFF = 1に変換

        buttonStates[i] =
          (button_state == HIGH)  // ボタンが押されている時
          ? (buttonStates[i] == 0 ? 1 : 2)  // 前フレームでボタンが押されてなかったら1、押されてたら2
          : 0;
      }
    }
