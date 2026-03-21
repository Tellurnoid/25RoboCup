#include "command.h"
#include "Instance.h" // commandが実体ないから、先にcommand.hをincludeした後に、instanceをinclude

void Command::makeCommand(uint8_t id, uint8_t this_ack, uint8_t content_id, int16_t content) {
  // if (id == MAIN) {
  //   uart.to_main.id = id;
  //   uart.to_main.ack = this_ack;
  //   uart.to_main.content_id = content_id;
  //   uart.to_main.content = content;
  // }

  // if (id == SUB) {
  //   uart.to_main.id = id;
  //   uart.to_main.ack = this_ack;
  //   uart.to_main.content_id = content_id;
  //   uart.to_main.content = content;
  // }
  uart.to_main.id = id;
  uart.to_main.ack = this_ack;
  uart.to_main.content_id = content_id;
  uart.to_main.content = content;
}

void Command::sendCommand(uint8_t id, uint8_t content_id, int16_t content) {
  ack++;  // オーバーフローしたら勝手に0になる
  makeCommand(id, ack, content_id, content);
}