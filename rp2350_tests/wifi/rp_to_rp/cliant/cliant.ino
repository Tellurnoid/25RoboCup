#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "PICO_AP";
const char* password = "12345678";

IPAddress serverIP(192,168,4,1);
const uint16_t port = 5000;

WiFiUDP udp;

struct __attribute__((packed)) DataPacket {
  int32_t value1;
  int32_t value2;
  uint8_t flag1;
  uint8_t flag2;
};

void setup() {
  Serial.begin(115200);
  while(!Serial);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");

  udp.begin(4000);  // ローカルポート（任意）
}

void loop() {

  DataPacket data;
  data.value1 = millis();
  data.value2 = 1234;
  data.flag1 = 1;
  data.flag2 = 0;

  udp.beginPacket(serverIP, port);
  udp.write((uint8_t*)&data, sizeof(data));
  udp.endPacket();

  delay(50);

  int packetSize = udp.parsePacket();

  if (packetSize == sizeof(DataPacket)) {

    DataPacket reply;
    udp.read((uint8_t*)&reply, sizeof(reply));

    Serial.println("Reply:");
    Serial.println(reply.value1);
    Serial.println(reply.value2);
    Serial.println(reply.flag1);
    Serial.println(reply.flag2);
  }

  delay(1000);
}