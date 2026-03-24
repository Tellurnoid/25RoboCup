#include <Arduino.h>
#include "Instance.h"
#include "Attack.h"
#include "Defense.h"


void setup() {
  uart.init();
  data.init();
  sound.init();
  attack.init();
  defense.init();
}

uint8_t ack = 0;
//ディフェンスがキーパーダッシュしているフラグ↓
//bool defense.keeper_dashing;
//keeper_dash
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
        if(defense.keeper_dashing != true){}
      }
      
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
  defense.update();
}