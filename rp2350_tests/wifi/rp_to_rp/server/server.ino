#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "PICO_AP";
const char* password = "12345678";

WiFiUDP udp;
const uint16_t port = 5000;

struct __attribute__((packed)) DataPacket {
  int32_t value1;
  int32_t value2;
  uint8_t flag1;
  uint8_t flag2;
};

void setup() {
  Serial.begin(115200);
  while(!Serial);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  udp.begin(port);
}

void loop() {

  int packetSize = udp.parsePacket();

  if (packetSize == sizeof(DataPacket)) {

    DataPacket received;
    udp.read((uint8_t*)&received, sizeof(received));

    Serial.println("Received:");
    Serial.println(received.value1);
    Serial.println(received.value2);
    Serial.println(received.flag1);
    Serial.println(received.flag2);

    // 返信生成
    DataPacket reply;
    reply.value1 = received.value1 + 1;
    reply.value2 = received.value2 + 2;
    reply.flag1 = !received.flag1;
    reply.flag2 = !received.flag2;

    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write((uint8_t*)&reply, sizeof(reply));
    udp.endPacket();
  }
}