#include <WiFi.h>
#include <time.h>

const char* ssid = "ssid";
const char* password = "pass";

const long tzOffset = 9 * 3600;

const char* ntp1 = "ntp.nict.jp";
const char* ntp2 = "pool.ntp.org";

void setup() {
  Serial.begin(115200);
  delay(2000);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Pico W 用 NTP開始
  configTime(10000, tzOffset, ntp1, ntp2);
}

void loop() {

  time_t now = time(NULL);      // UNIX time取得
  struct tm *t = localtime(&now);

  if (t->tm_year > (2020 - 1900)) {  // 同期完了判定
    Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                  t->tm_year + 1900,
                  t->tm_mon + 1,
                  t->tm_mday,
                  t->tm_hour,
                  t->tm_min,
                  t->tm_sec);
  } else {
    Serial.println("Waiting NTP...");
  }

  delay(1000);
}