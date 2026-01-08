#include <Arduino.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A5, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(A1, INPUT);
  pinMode(A0, INPUT);
  pinMode(A3, INPUT);
  pinMode(A2, INPUT);
  pinMode(A4, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  analogWrite(10, 160);
  Serial.print("A5:");Serial.print(digitalRead(A5));//OK
  Serial.print(", 2:");Serial.print(digitalRead(2));
  Serial.print(", 3:");Serial.print(digitalRead(3));
  Serial.print(", 5:");Serial.print(digitalRead(5));
  Serial.print(", 4:");Serial.print(digitalRead(4));
  Serial.print(", 7:");Serial.print(digitalRead(7));  
  Serial.print(", 6:");Serial.print(digitalRead(6));//OK
  Serial.print(", 8:");Serial.print(!digitalRead(8));
  
  Serial.print(", 9:");Serial.print(!digitalRead(9));
  Serial.print(", 11:");Serial.print(digitalRead(11));//OK
  Serial.print(", 12:");Serial.print(digitalRead(12));
  Serial.print(", A1:");Serial.print(digitalRead(A1));//OK
  Serial.print(", A0:");Serial.print(digitalRead(A0));
  Serial.print(", A3:");Serial.print(digitalRead(A3));//OK
  Serial.print(", A2:");Serial.print(digitalRead(A2));
  Serial.print(", A4:");Serial.println(digitalRead(A4));
  /*
  Serial.print(", A5:");Serial.print(digitalRead(A5));//OK
  Serial.print(", 6:");Serial.print(digitalRead(6));//OK
  Serial.print(", 11:");Serial.println(digitalRead(11));//OK
  */
}
