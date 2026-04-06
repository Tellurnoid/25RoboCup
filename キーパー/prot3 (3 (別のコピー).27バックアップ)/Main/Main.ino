#include "Instance.h"
#include "Attack.h"

void setup() {
  uart.init();
  data.init();
  sound.init();
  attack.init();
  defense.init();
  kicker.init();
}

uint8_t ack = 0;

void loop() {
  uart.update();
  data.update();

  if (data.dp.id == MAIN && data.dp.ack != ack) {
    int content = data.dp.content;
    switch (data.dp.content_id) {
    case START:
      if (content == 0){
          data.dp.main_v = {0,0};
          data.dp.main_rotate = 0;
          attack.setBehavior(Attack::OFF);
          defense.setBehavior(Defense::OFF);  
      } 
      if (content == 1) {
        attack.setBehavior(Attack::BALL_CHASE);
        defense.setBehavior(Defense::OFF);  
      }
      if (content == 2) {
        attack.setBehavior(Attack::OFF);
        defense.setBehavior(Defense::back_to_goal_withCAM);
        Serial.println("CONTENT 2");
      }
      break;

    case RESET_GYRO:
      if (content == 1) {
        gyro.resetGyro();
      }
      break;
      
    case KICK:
      if (content == 1) {
        kicker.kick();
      }
      break;
    }

    ack = data.dp.ack;
  }
  sound.update();
  attack.update();
  defense.update();

  kicker.update();

  // Serial.print(data.dp.kicker_charge);
  // Serial.print(":");
  // Serial.println(data.dp.ball_catch);
}