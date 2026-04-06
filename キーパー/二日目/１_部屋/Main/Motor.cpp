#include "Motor.h"

void Kicker::init() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(SENSOR, INPUT);

  pinMode(3, INPUT_PULLUP);
}

void Kicker::update() {
  kicker_charge = analogRead(SENSOR) < CHARGE_BORDER; // センサーが閾値より小さくなったら、チャージ完了
  data.dp.kicker_charge = kicker_charge;

  if (!kicker_charge) {
    charge();
    kicking = false;
  }
  if (kicker_charge)  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  if (kicker_charge && digitalRead(3) == 0)  {
    kick();
  }

  if (kicking) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
}

void Kicker::charge() {
  if (kicker_charge) {
    return;  // チャージ完了してたら終了
  }
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void Kicker::kick() {
  if (!kicker_charge) return; // チャージ中だったら
  kicking = true;
}


void Sensor::init() {
  pinMode(BALL_ZONE, INPUT);
}

void Sensor::update() {
  ball_catch = analogRead(BALL_ZONE) < BALL_CATCH_BORDER;
  data.dp.ball_catch = ball_catch;
  Serial.println(analogRead(BALL_ZONE));
}