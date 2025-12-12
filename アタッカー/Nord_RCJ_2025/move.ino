struct Motor {
  char name;       // モーター名（'A'〜'D'）
  int in1_pin;     // IN1ピン
  int in2_pin;     // IN2ピン
  int ch1;         // PWMチャンネル1
  int ch2;         // PWMチャンネル2
};

// モーターA〜Dの定義（ピン番号とチャンネルを確実に割り当て）
Motor motors[] = {
  {'A', 32, 33, 8, 9},
  {'B', 13, 4, 14, 15},
  {'C', 27, 14, 12, 13},
  {'D', 25, 26, 10, 11},
};

const int motor_quanty = 4;
//motors
//A:B
//D:C
//cables

//front
//D:A
//C:B

const int LEDC_TIMER_BIT = 8;   // 8bit = 0〜255
const int LEDC_BASE_FREQ = 500; // PWM周波数
const int VALUE_MAX = 255;      // PWM最大値

void initMotor() {//3.3.2
  for (int i = 0; i < motor_quanty; i++) {
    pinMode(motors[i].in1_pin, OUTPUT);
    pinMode(motors[i].in2_pin, OUTPUT);

    ledcSetup(motors[i].ch1, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcSetup(motors[i].ch2, LEDC_BASE_FREQ, LEDC_TIMER_BIT);

    ledcAttachPin(motors[i].in1_pin, motors[i].ch1);
    ledcAttachPin(motors[i].in2_pin, motors[i].ch2);
  }
}
/*
void initMotor() {//3.3.3用？(失敗)
  for (int i = 0; i < motor_quanty; i++) {

    // PWMピンをアタッチ（freq と resolution を直接指定）
    ledcAttach(motors[i].in1_pin, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcAttach(motors[i].in2_pin, LEDC_BASE_FREQ, LEDC_TIMER_BIT);

    // 初期 duty = 0
    ledcWrite(motors[i].in1_pin, 0);
    ledcWrite(motors[i].in2_pin, 0);
  }
}
*/

void motor(char motorName, int pwm) {
  pwm = constrain(pwm, -VALUE_MAX, VALUE_MAX);  //最小値,最大値の範囲に収めてくれる

  for (int i = 0; i < motor_quanty; i++) {
    if (motors[i].name == motorName) {
      Motor m = motors[i];

      if (pwm > 0) {
        ledcWrite(m.ch1, pwm);
        ledcWrite(m.ch2, 0);
      } else if (pwm < 0) {
        ledcWrite(m.ch1, 0);
        ledcWrite(m.ch2, -pwm);
      } else {
        ledcWrite(m.ch1, 0);
        ledcWrite(m.ch2, 0);
      }
      return;  // 一致したら終了
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------

int omega_max = 180;
// int speedMax = 210;
int pwm_max = 255;

// void vector(int deg, int speed, int omega){
  
void moveVector(Vector v, int omega){ 
  if (omega > omega_max)  omega = omega_max;
  if (omega < -omega_max)  omega = -omega_max;

  // if (speed > speedMax)  speed = speedMax;
  int speed = sqrt(v.x * v.x + v.y * v.y);
  if (speed < 0)  speed = 0;

  //right : + / left : -
  // float deg = atan2(v.y, v.x) * 180.0 / M_PI;
  // deg += 45;
  // float rad = (90 - deg) * (M_PI / 180); //正面が0度でdegを入力しているため、radにπ/2足して右側を0度に直して計算し、さらに正面が0のやつは右側が正なので、マイナスをradにかけて修正している

  float deg = atan2(v.y, v.x);
  float rad = M_PI / 2 - deg - M_PI / 4;

  int A = sin(rad) * speed + omega; //speedは全方位においてもスピードを統一したかったらspeed < pwm_max (255)だけど、前後左右などで最高速度を出したいときは、pwmMax * √2(255の時、=360)にする
  int B = cos(rad) * speed + omega;
  int C = -sin(rad) * speed + omega;
  int D = -cos(rad) * speed + omega;

  float max_val = fmax(fmax(fabs(A), fabs(B)), fmax(fabs(C), fabs(D)));//max(整数のみ)、fmax(小数いける)は引数2つまでしか扱えない

  if (max_val > pwm_max){
    float scale = pwm_max / max_val;
    A *= scale;
    B *= scale;
    C *= scale;
    D *= scale;
  }

  motor('A', A);
  motor('B', B);
  motor('C', C);
  motor('D', D);  
}