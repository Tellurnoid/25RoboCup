#include "button.h"
#include <Arduino.h>

    
    void Button::init() {
      for (int i = 0; i < NUM_PINS; i++) {
        pinMode(pins[i], INPUT_PULLUP);
      }
    }

    int8_t Button::read(int8_t pin) {  // ポインタ指定で引数の配列を編集する
        int button_state = (digitalRead(pin)==HIGH)  ? 0 : 1;  // ON = 0、OFF = 1に変換
        // ボタンが押されている時 // 前フレームでボタンが押されてなかったら1、押されてたら2
        states[pin] = (button_state == 1) ? (states[pin] == 0 ? 1 : 2) : (button_state == 2) ? 3 : 0;
       
        // states[pin]=button_state;//次のフレーム用に保存
        return states[pin]; 
    }

    //     int8_t Button::read(int8_t pin) {  // ポインタ指定で引数の配列を編集する
    //     int button_state = (digitalRead(pin)==HIGH)  ? 0 : 1;  // ON = 0、OFF = 1に変換
    //     // ボタンが押されている時 // 前フレームでボタンが押されてなかったら1、押されてたら2
    //     states[pin] = (button_state == 0) ? (states[pin] == 0 ? 1 : 2) : 0;
       
    //     // states[pin]=button_state;//次のフレーム用に保存
    //     return states[pin]; 
    // }

