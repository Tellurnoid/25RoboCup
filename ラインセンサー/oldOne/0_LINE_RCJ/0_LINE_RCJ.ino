#include <math.h>

struct Vector {
  float x;
  float y;
};

// 極座標 → ベクトル
Vector fromPolar(float deg, float mag) {
  Vector v;
  float rad = deg * M_PI / 180.0;
  v.x = cos(rad) * mag;
  v.y = sin(rad) * mag;
  return v;
}

// ベクトル加算
Vector add(Vector a, Vector b) {
  Vector r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  return r;
}

// 正規化（単位ベクトル化）
Vector normalize(Vector v) {
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / len, v.y / len };
}

Vector divide(Vector v, int num) {
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / num, v.y / num };
}

const int NUM_SENSORS = 16;
// const int sensorPins[NUM_SENSORS] = {4,7,6,8,9,11,12,15,14,17,16,18,19,2,3,5};
const int sensorPins[NUM_SENSORS] = {4,5,3,2,19,18,16,17,14,15,12,11,9,8,6,7};
const float sensorDeg[NUM_SENSORS] = {0,22.5,45,67.5,90,111.5,130,155.5,180,202.5,225,247.5,270,292.5,315,337.5};
int area_four[4] = {0,0,0,0};//前から右回り、反応あり=1

int sensorValue[NUM_SENSORS];
int sensorValue_law[NUM_SENSORS];//こっちは踏んだら１、注意


Vector line_v;

void setup() {
  initUART();
 Serial.begin(115200); 
 for (int i = 0; i < NUM_SENSORS; i++) {                     
    pinMode(sensorPins[i], INPUT);
  }
  pinMode(13, OUTPUT);
}

const int NUM_READ = 200;
int16_t line_angle;
int16_t line_dis;
int16_t line_num_of_sensors = 0;//現在未使用


const int num_draft = 600;
bool digitalRead_withDraft(int pin) {
  int highCount = 0;
  for (int i = 0; i < num_draft; i++) {
    if (digitalRead(pin) == 0) {
      highCount++;
    }
  }
  return (highCount > num_draft*0.4);
}

void forDebug(){
  analogWrite(10, 160);
  for(int i=0; i<NUM_SENSORS; i++){
    Serial.print(digitalRead_withDraft(sensorPins[i]));
    Serial.print(",");
  }
  Serial.println();
}

bool isOnT(){

  int COUNT_line_num_of_sensors = 0;
  for(int i=0;i<4;i++){area_four[i]=0;}
  for(int i = 0; i < NUM_SENSORS; i++){
    //反応した素子数カウント(senrorValue_lawの代入も)
    if(i==12 || i == 13){
      if(digitalRead_withDraft(sensorPins[i])==1){
        COUNT_line_num_of_sensors++;
        sensorValue_law[i]=1;
      }
      else{
        sensorValue_law[i]=0;
      }
    }
    else{
       if(digitalRead_withDraft(sensorPins[i])==0){
        COUNT_line_num_of_sensors++;
        sensorValue_law[i]=1;
      }
      else{
        sensorValue_law[i]=0;
      }
    }

    //方式2: 前後左右で分析
    if(sensorValue_law[i]==0){
      if(i<3 || i>14){//機体正面でラインを踏んでる
        area_four[0]=1;
      }
      else if(i<8){//機体右側でライン踏んでる
        area_four[1]=1;
      }
      else if(i>8){//機体左側でライン踏んでる
        area_four[3]=1;
      }
      else{//機体背後でライン踏んでる
        area_four[2]=1;
      }
    }
  }
    line_num_of_sensors = COUNT_line_num_of_sensors;


  if(area_four[0]==1 && area_four[1]==1 && area_four[2]==0 && area_four[3]==1){
    return 1; 
  }
  else{
    return 0;
  }
    // if(area_four[0]==1 && 
    //    (
    //     (sensorValue_law[7]==1 && sensorValue_law[9]==1) || 
    //     (sensorValue_law[6]==1 && sensorValue_law[10]==1) || 
    //     (sensorValue_law[5]==1 && sensorValue_law[11]==1) || 
    //     (sensorValue_law[4]==1 && sensorValue_law[12]==1) || 
    //     (sensorValue_law[3]==1 && sensorValue_law[13]==1) ||
    //     (sensorValue_law[2]==1 && sensorValue_law[14]==1) 
    //    )
    //   )
    //       {return true;}
    //   else{return false;}
return false;
}

void loop() {
  UART();
  
  if(line_angle!=400){digitalWrite(13, HIGH);}else{digitalWrite(13, LOW);}
  analogWrite(10, 160);

  for (int i = 0; i < NUM_SENSORS; i++) {
    int line_total = 0;
    for (int j = 0; j < NUM_READ; j++) {
      if (i == 12 || i == 13) { // 値が逆転しちゃうやつだけ修正
        line_total += (1 - digitalRead(sensorPins[i]));
      }
      else {
        line_total += digitalRead(sensorPins[i]);
      }
    }
    int line = 1;
    if (line_total != NUM_READ) {
      line = 0;
    }
    sensorValue[i] = line;
  }

  Vector line_v = {0, 0};

  float start = -1; // -1 == none
  float end = -1;
  int num_v = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (sensorValue[i] == 0) {
      if (start == -1) {
        start = sensorDeg[i];
      }
      end = sensorDeg[i];
    }
    else {
      if (i != 0 && end == sensorDeg[i-1]) {  // 角度
        Vector v = fromPolar((start + end) / 2, 1);
        line_v = add(line_v, v);
        num_v++;
        start = -1;
        end = -1;
      }
    }
  }
  if (start != -1) {
    Vector v = fromPolar((start + end) / 2, 1);
    line_v = add(line_v, v);
    num_v++;
  }

  line_v = divide(line_v, num_v);
    //T字認識したら500--------------------------------------------------
    //if(area_four[0]==1 && area_four[1]==1 && area_four[2]==0 && area_four[3]==1){
    if(isOnT()){line_angle=500;}
  else{
      if (num_v == 0) {
       line_angle = 400;
      }
     else {
       line_angle = atan2(line_v.y, line_v.x) * 180.0 / M_PI;
       if (line_angle < -180) line_angle += 360;
       if (line_angle >= 180) line_angle -= 360;
     }
  }
  line_dis = sqrt(line_v.x * line_v.x + line_v.y * line_v.y) * 100;

  // if (line_angle < 0) line_angle += 360.0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensorValue_law[i]);
    //Serial.print(digitalRead(sensorValue[i]));
    Serial.print(",");
  }  


  // Serial.print("total:");
  // Serial.print(line_num_of_sensors);
  // Serial.print(", [AREA]f:");
  // Serial.print(area_four[0]);Serial.print(",r:");
  // Serial.print(area_four[1]);Serial.print(",b:");
  // Serial.print(area_four[2]);Serial.print(",l:");
  // Serial.print(area_four[3]);Serial.print(",");
  // Serial.print(", angle:");
  // Serial.print(line_angle);
  // Serial.print(", dis: ");
  // Serial.println(line_dis);
  
 //forDebug();
}
