#include "Instance.h"
#include "Attack.h"

int worldAngle(int angle) { // angleを絶対軸に修正
  angle = angle + data.dp.robot_angle;
  return angle;
}

void Line::update() {
  line_angle = data.dp.line_angle;
  line_distance = data.dp.line_distance;

  on_line = line_angle != 400;  // lineが400だったらfalse
  on_t_line = line_angle == 500;  // lineが500だったらtrue
  if (getOverLine())  over_line = !over_line; // ライン中央を跨いだら、over_lineを前とは違う値を代入
  in_line = on_line ? false : (over_line ? true : false); // ラインがない、かつover_lineだったらin_line

  field_line_angle = getFieldLineAngle();
  field_line_distance = over_line? -line_distance: line_distance;

  last_line_angle = line_angle;
  last_on_line_angle = on_line ? (on_t_line ? last_on_line_angle: line_angle) : last_on_line_angle; // lineに乗ってかつT字じゃなければ代入
}

int Line::getFieldLineAngle() {
  if (!on_line)  return over_line? last_on_line_angle: 400;
  if (on_t_line) return over_line? last_on_line_angle: wrap180(last_on_line_angle + 180); // Tのときは前回の値を返す
  
  if (over_line)  return line_angle;
  else            return wrap180(line_angle + 180);
}

bool Line::getOverLine() {
  if (!on_line)   return false;
  if (last_line_angle == 400) return false; // 前の値が400だと,last_on_lineが前回の別のライン乗ってるときなので、無視
  if (on_t_line)  return false; // T字も

  if (abs(angleDiff(line_angle, last_on_line_angle)) > BORDER_OVER_LINE)  return true;
  return false;
}

Vector Line::lineV() {
  if (field_line_angle == 400)  return {0, 0};
  if (in_line)  return makeV(reverseAngle(field_line_angle), 1000);
  return makeV(reverseAngle(field_line_angle), AVOID_LINE_SPEED);
}

Vector Line::cancelV(Vector v) {
  if (field_line_angle == 400)  return {0, 0};
  float len_v = lenV(v, reverseAngle(field_line_angle + 180));
  return makeV(reverseAngle(field_line_angle), len_v);
}



void Ball::update() {
  ball_angle = data.dp.ball_angle;
  ball_distance = data.dp.ball_distance;

  ball_found = ball_angle != 400;

  ball_angle = convertAngle(ball_angle);  // 180~-180に直す
}

float Ball::getBallD() {
  uint32_t now = millis();
  uint32_t dt_ms = now - last_time;

  if (dt_ms > BALL_D_INTERVAL) {
    float dt = dt_ms / 1000.0f;
    ball_d = (convertAngle(ball_angle) - last_ball_angle) / dt;

    last_time = now;
    last_ball_angle = convertAngle(ball_angle);
  }
  ball_d = ball_d * 0.4 + last_ball_d * 0.6;
  last_ball_d = ball_d;
  return ball_d;
}

Vector Ball::ballV(int SPEED) {

  float ball_d = getBallD();
  if (ball_d < 80) ball_d = 0.0f;

  if (!ball_found)  return {0, 0};

  int front_angle = borderDis[ball_distance];
  int zero_range = 15;

  if (abs(ball_angle) < front_angle) {
    float vx = ball_angle * 11.0;
    vx = constrain(vx, -SPEED, SPEED);
    if (abs(ball_angle) < zero_range) vx = 0;

    float vy = ((front_angle - abs(ball_angle)) / (float)front_angle) * SPEED;

    return {vx, vy};
  }
  else {
    // int move_angle = ball_angle + (abs(ball_angle) > 0 ? 50 : -50);
    int move_angle = ball_angle + copysign(50, ball_angle);
    int move_speed = SPEED;
    return makeV(reverseAngle(move_angle), move_speed);
  }
}


void Field::update() {
  for(int i = 0; i < NUM_ECHOS; i++){
    echoValues[i] = data.dp.echoValues[i];
  }
  robot_angle = data.dp.robot_angle;

  move_x = data.dp.move_x;
  move_y = data.dp.move_y;

  movement_x = (move_x - last_move_x) * cos(robot_angle * M_PI / 180);
  movement_y = (move_y - last_move_y) * cos(robot_angle * M_PI / 180);

  calculateCrimp();
  calculatePosition();

  last_move_x = move_x;
  last_move_y = move_y;
}

int Field::getEchoForwardIndex() {
  float angle = wrap360(-robot_angle) + 22.5;  // 22.5ずらす,minus注意
  int a = angle / 45; // 0 ~ 8
  a = a % NUM_ECHOS;  // 0 ~ 7にする
  return a;
}

void Field::calculateCrimp() {
  int echoDiff[NUM_ECHOS];
  for (int i = 0; i < NUM_ECHOS; i++) {
    echoDiff[i] = echoValues[i] - lastEchoValues[i];

    if (echoDiff[i] < -CRIMP_BORDER) {
      crimpFlags[i] = true;
    }

    if (echoDiff[i] > CRIMP_BORDER) {
      // crimp_flag = false;
    }
    lastEchoValues[i] = echoValues[i];

    if (crimpFlags[i])  crimp[i] = echoValues[i];
    else crimp[i] = -2;
  }
}

void Field::resetCrimpFlags() {
  for (int i = 0; i < NUM_ECHOS; i++) {
    crimpFlags[i] = false;
  }
}

void Field::calculatePosition() {
  // int forward_index = getEchoForwardIndex();
  // int F_index = forward_index;
  // int R_index = (forward_index + 2) % NUM_ECHOS;
  // int B_index = (forward_index + 4) % NUM_ECHOS;
  // int L_index = (forward_index + 6) % NUM_ECHOS;

  // float wall_deg = fmod((wrap360(robot_angle) + 22.5f), 45.0f) - 22.5f;
  // float wall_rad = wall_deg * M_PI / 180.0f;

  // int dis_F = echoValues[F_index] * cos(wall_rad);  // ロボット分の10cmを追加
  // int dis_R = echoValues[R_index] * cos(wall_rad);
  // int dis_B = echoValues[B_index] * cos(wall_rad);
  // int dis_L = echoValues[L_index] * cos(wall_rad);

  // if (dis_R == -1 || dis_L == -1) {
  //   position_lost.x = true;
  // }
  // else if (dis_R + dis_L > FIELD_X - FIELD_TOLERANCE) {
  //   position_lost.x = false;
  //   if (dis_R > dis_L)  field_position.x = FIELD_X / 2 - dis_R;
  //   else                field_position.x = -FIELD_X / 2 + dis_L;

  // }
  // // else if (crimp[R_index] == -2 || crimp[L_index] == -2) {
  // //   position_lost.x = false;

  // //   if (crimp[R_index] == -2 && crimp[L_index] == -2) {
  // //     position_lost.x = true;
  // //   }
  // //   else if (crimp[R_index] == -2) {
  // //     field_position.x = FIELD_X / 2 - dis_R;
  // //   }
  // //   else if (crimp[L_index] == -2) {
  // //     field_position.x = -FIELD_X / 2 + dis_L;
  // //   }
  // //   else  position_lost.x = true;

  // // }
  // else {
  //   field_position.x += movement_x;
  //   field_position.y += movement_y;
  //   // position_lost.x = true;
  // }


}

Field::Position Field::getFieldPosition() {
  return {field_position.x, field_position.y};
}

Field::PositionLost Field::getPositionLost() {
  return {position_lost.x, position_lost.y};
}

Vector Field::brakeV() {
  int forward_index = getEchoForwardIndex();
  int F_index = forward_index;
  int R_index = (forward_index + 2) % NUM_ECHOS;
  int B_index = (forward_index + 4) % NUM_ECHOS;
  int L_index = (forward_index + 6) % NUM_ECHOS;

  float wall_deg = fmod((wrap360(robot_angle) + 22.5f), 45.0f) - 22.5f;
  float wall_rad = wall_deg * M_PI / 180.0f;

  int dis_F = echoValues[F_index] * cos(wall_rad);  // ロボット分の10cmを追加
  int dis_R = echoValues[R_index] * cos(wall_rad);
  int dis_B = echoValues[B_index] * cos(wall_rad);
  int dis_L = echoValues[L_index] * cos(wall_rad);

  world_velocity_x = data.dp.velocity_v.x * cos(robot_angle * M_PI / 180.0f);
  world_velocity_y = data.dp.velocity_v.y * cos(robot_angle * M_PI / 180.0f);

  Serial.println(world_velocity_x);
  float vx, vy;

  float Kd = 1.2f;
  int wall_dis = 350;
  state.sp.near_wall = false;

  if (dis_R < wall_dis && world_velocity_x > 50) {
    vx = -(Kd * world_velocity_x);
    state.sp.near_wall = true;
  }
  if (dis_L < wall_dis && world_velocity_x < -50) {
    vx = Kd * -world_velocity_x;
    state.sp.near_wall = true;
  }
  if (dis_F < wall_dis && world_velocity_y > 50) {
    vx = -(Kd * world_velocity_y);
    state.sp.near_wall = true;
  }
  if (dis_B < wall_dis && world_velocity_y < -50) {
    vx = Kd * -world_velocity_y;
    state.sp.near_wall = true;
  }

  return {vx, vy};

  // world_velocity_x = (field_position.x - last_field_position.x) * cos(robot_angle * M_PI / 180.0f);
  // world_velocity_y =  * cos(robot_angle * M_PI / 180.0f);

  // last_field_position.x = field_position.x;
  // last_field_position.y = field_position.y;

  // last_field_position

  // if (field_position.x > 300 && world_velocity_x > 0) {
  //   vx = makeV(worldAngle(180), world_velocity_x * 1.5f);
  //   state.sp.near_wall = true;
  // }
  // else if (field_position.x < -300 && world_velocity_x < 0) {
  //   vx = makeV(worldAngle(180), world_velocity_x * 1.5f);
  //   state.sp.near_wall = true;
  // }
  // else  state.sp.near_wall = false;

  // if (position_lost.x)  state.sp.near_wall = false;

  // Serial.print(field_position.x);
  // Serial.print(":");
  // Serial.print(data.dp.velocity_v.x);
  // Serial.print(":");
  // Serial.println(state.sp.near_wall);

  // return addV(vx, vy);
}


void Attack::init() {
}

void Attack::setBehavior(Behavior b) {
  behavior = b;
}

void Attack::update() {
  line.update();
  ball.update();
  field.update();

  switch (behavior) {
    case BALL_CHASE:
      ballChase();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case BALL_CHASE_WITH_KICKER:
      // ballChaseWithKicker();
      
    case SHOOT:
      shoot();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case KEEPER_DASH:
      keeperDash();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case KEEPER_SHOOT:
      keeperShoot();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;


    case STOP:
      stop();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case OFF:
      off();
      break;
  }
}

void Attack::startKeeperDash() {
  setBehavior(KEEPER_DASH);
  defense.setBehavior(Defense::OFF);  
}

void Attack::ballChase() {

  if (data.dp.kicker_charge && data.dp.ball_catch) {
    setBehavior(SHOOT);
    return;
  }
  
  if (ball_found_time > 0)  ball_found_time = ball_found_time < 0 ? 0 : ball_found_time - 1;
  Vector ball_v;

  // if (field.getPositionLost().x)  {
  //   ball_v = ball.ballV(NOMAL_SPEED);
  // }
  // else {
  //   ball_v = ball.ballV(ECHO_SPEED);
  //   ball_v = addV(ball_v, field.brakeV());
  // }

  // ball_v = ball.ballV(ECHO_SPEED);
  // ball_v = addV(ball_v, field.brakeV());
  
  ball_v = ball.ballV(NOMAL_SPEED);

  ball_v = addV(ball_v, line.cancelV(ball_v));

  Vector line_v = line.lineV();

  attack_v = assembleV(line_v, ball_v, MAX_SPEED);

  int goal_angle = attack_goal == 0 ? data.dp.blue_angle : data.dp.yellow_angle;
  int goal_distance = data.dp.is_attack_to_blue == 1 ? data.dp.blue_distance : data.dp.yellow_distance;

  // attack_rotate = gyro.controlAngle(0);
  if (goal_angle == 400)        attack_rotate = gyro.controlAngle(0);
  // else if (goal_distance < 360) {
  //   attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
  // }
  else                          attack_rotate = gyro.controlAngle(0);
}

void Attack::shoot() {
  if (!data.dp.kicker_charge || !data.dp.ball_catch) {
    setBehavior(BALL_CHASE);
    return;
  }

  if (ball_found_time == 0) ball_found_time = millis();

  Vector v = makeV(90, NOMAL_SPEED);

  v = addV(v, line.cancelV(v));
  Vector line_v = line.lineV(); 

  attack_v = assembleV(line_v, v, MAX_SPEED);
  attack_rotate = gyro.controlAngle(0);

  // int goal_angle = attack_goal == 0 ? data.dp.blue_angle : data.dp.yellow_angle;
  // int goal_distance = attack_goal == 0 ? data.dp.blue_distance : data.dp.yellow_distance;
  int goal_angle = data.dp.is_attack_to_blue == 1 ? data.dp.blue_angle : data.dp.yellow_angle;
  int goal_distance = data.dp.is_attack_to_blue == 1 ? data.dp.blue_distance : data.dp.yellow_distance;



  if (goal_distance < 260) {
    if (goal_angle == 400) {
      attack_rotate = gyro.controlAngle(0);
      kicker.kick();
    }
    else {
      attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
      if (abs(goal_angle) < goal_tolerance) {
        kicker.kick();
      }
    }
  }
  else {
    if (goal_angle == 400) {
      attack_rotate = gyro.controlAngle(0);
      kicker.kick();
    }
    else {
      if (millis() - ball_found_time > 500) {
        attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
        if (abs(goal_angle) < goal_tolerance) {
          kicker.kick();
        }
      }
      else {
        attack_rotate = gyro.controlAngle(0);
      }
    }
  }

  // if (goal_angle == 400) {
  //   attack_rotate = gyro.controlAngle(0);
  //   kicker.kick();
  // }
  // else {
  //   if (abs(goal_angle) < goal_tolerance && millis() - ball_found_time > 500) {
  //     attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
  //     kicker.kick();
  //   }
  //   else {
  //     attack_rotate = gyro.controlAngle(0);
  //   }
  // }
}

void Attack::keeperDash() {
  // if (start_keeper_dash_time == 0) {
  //   start_keeper_dash_time = millis();
  //   first_line = true;
  // }

  // if (millis() - start_keeper_dash_time > keeper_dash_time_limit) {
  //   start_keeper_dash_time = 0;

  //   defense.finishKeeperDash();
  // }

  // Vector ball_v;
  // Vector line_v;

  // ball_v = ball.ballV(500);
  // line_v = line.lineV();

  // if (first_line) {
  //   if (line_v.x == 0 && line_v.y == 0) first_line = false;
  //   line_v = {0, 0};
  // }
  // else {
  //   ball_v = addV(ball_v, line.cancelV(ball_v));
  // }

  // attack_v = assembleV(line_v, ball_v, MAX_SPEED);

  // attack_rotate = gyro.controlAngle(0);
  // // int goal_angle = data.dp.blue_angle;
  // // if (goal_angle == 400)  attack_rotate = gyro.controlAngle(0);
  // // else                    attack_rotate = gyro.controlAngle(worldAngle(goal_angle));

  // if (data.dp.kicker_charge && data.dp.ball_catch) {
  //   kicker.kick();
  //   static unsigned long kick_time;
  //   if (kick_time == 0) kick_time = millis();

  //   if (millis() - kick_time > 500) {
  //     kick_time = 0;
  //     start_keeper_dash_time = 0;
  //     defense.finishKeeperDash();
  //   }
  // }


  if (data.dp.kicker_charge && data.dp.ball_catch) {
    setBehavior(KEEPER_SHOOT);
    return;
  }

  if (start_keeper_dash_time == 0) {
    start_keeper_dash_time = millis();
    first_line = true;
  }

  if (millis() - start_keeper_dash_time > keeper_dash_time_limit) {
    start_keeper_dash_time = 0;
    defense.finishKeeperDash();
    return;
  }

  Vector ball_v;
  Vector line_v;

  ball_v = ball.ballV(NOMAL_SPEED);
  line_v = line.lineV();

  if (first_line) {
    if (line_v.x == 0 && line_v.y == 0) first_line = false;
    line_v = {0, 0};
  }
  else {
    ball_v = addV(ball_v, line.cancelV(ball_v));
  }

  if (ball_v.y < 0) { // 移動ベクトルが後ろ成分が出ればに行ったら終了
    start_keeper_dash_time = 0;
    defense.finishKeeperDash();
    return;
  }

  attack_v = assembleV(line_v, ball_v, MAX_SPEED);

  attack_rotate = gyro.controlAngle(0);
  
}

void Attack::keeperShoot() {
  if (!data.dp.kicker_charge || !data.dp.ball_catch) {
    start_keeper_dash_time = 0;
    defense.finishKeeperDash();
    return;
  }

  if (millis() - start_keeper_dash_time > keeper_dash_time_limit) {
    start_keeper_dash_time = 0;
    defense.finishKeeperDash();
    return;
  }

  Vector v = makeV(90, NOMAL_SPEED);

  v = addV(v, line.cancelV(v));
  Vector line_v = line.lineV(); 

  if (v.y < 0) { // 移動ベクトルが後ろ成分が出ればに行ったら終了
    start_keeper_dash_time = 0;
    defense.finishKeeperDash();
    return;
  }

  attack_v = assembleV(line_v, v, MAX_SPEED);

  // int goal_angle = attack_goal == 0 ? data.dp.blue_angle : data.dp.yellow_angle;
  // int goal_distance = attack_goal == 0 ? data.dp.blue_distance : data.dp.yellow_distance;
  int goal_angle = data.dp.is_attack_to_blue == 1 ? data.dp.blue_angle : data.dp.yellow_angle;
  int goal_distance = data.dp.is_attack_to_blue == 1 ? data.dp.blue_distance : data.dp.yellow_distance;


  if (goal_angle == 400) {
    attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
      if (abs(goal_angle) < goal_tolerance) {
        kicker.kick();
      }
  }
  else {
    attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
    if (abs(goal_angle) < goal_tolerance) {
      kicker.kick();
    }
  }

  // if (goal_angle == 400) {
  //   attack_rotate = gyro.controlAngle(0);
  //   kicker.kick();
  // }
  // else {
  //   attack_rotate = gyro.controlAngle(worldAngle(goal_angle));
  //   if (abs(goal_angle) < goal_tolerance) {
  //     kicker.kick();
  //   }
  // }
}

void Attack::stop() {
  attack_v = {0, 0};
  attack_rotate = gyro.controlAngle(0);
}

void Attack::off() {
  // attack_v = {0, 0};
  // attack_rotate = 0;
}