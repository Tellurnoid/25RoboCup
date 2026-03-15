#include "Instance.h"
#include "Attack.h"

void setup() {
  uart.init();
  data.init();
  sound.init();
  attack.init();
}

uint8_t ack = 0;

void loop() {
  uart.update();
  data.update();

  if (data.dp.id == MAIN && data.dp.ack != ack) {
    int content = data.dp.content;
    switch (data.dp.content_id) {
    case START:
      if (content == 0) attack.setBehavior(Attack::OFF);
      if (content == 1) attack.setBehavior(Attack::BALL_CHASE);
      break;

    case RESET_GYRO:
      if (content == 1) {
        gyro.resetGyro();
        Serial.print("reset");
      }
    }
    ack = data.dp.ack;
  }
  sound.update();
  attack.update();
}