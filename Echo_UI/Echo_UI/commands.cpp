#include "commands.h"
#include <Arduino.h>



    void Command::sendCommand(uint8_t id, uint8_t this_ack, uint8_t content_id, int16_t content) {
      to_main.id = id;
      to_main.ack = this_ack;
      to_main.content_id = content_id;
      to_main.content = content;
    }
    void Command::command(uint8_t id, uint8_t content_id, int16_t content) {
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
