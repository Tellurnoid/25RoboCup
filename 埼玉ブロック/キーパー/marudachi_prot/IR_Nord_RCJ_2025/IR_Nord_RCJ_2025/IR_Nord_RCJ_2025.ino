#include "IR.h"
#include "UART.h" // UART()でいろんなファイルの変数使うから最後にincludeする必要あり

void setup() {
  initIR();
  initUART();
}

void loop() {
  IR();
  UART();
}
