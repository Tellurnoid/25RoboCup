#ifndef UART_H
#define UART_H



// ========= パケット用データ =========
struct ReadPacketData0 {
    int ball_angle;
    int goal_angle;
};
extern ReadPacketData0 rp0;

struct SendPacketData0 {
    int ball_angle;
    int goal_angle;
    int UISendHead;   // 3桁 1桁目:対象MCU 2桁目:種類
    float UISendData;
};
extern SendPacketData0 sp0;

// ========= ユーティリティ =========
uint8_t calcChecksum(uint8_t* data, uint8_t len);

// ========= パケット読み取り =========
template <typename readT>
bool readPacket(int num_serial, readT &d) {

    HardwareSerial *seri;
    if (num_serial == 0) seri = &Serial;

    uint32_t start = millis();
    uint8_t header;

    // --- ヘッダー検索（タイムアウト 30ms） ---
    while (true) {
        if (seri->available()) {
            header = seri->read();
            if (header == 0xAA) break;  // 見つかった
        }
        if (millis() - start > 30) return false;
    }

    // --- 長さ取得 ---
    while (!seri->available()) {
        if (millis() - start > 30) return false;
    }
    uint8_t len = seri->read();
    if (len != sizeof(readT)) return false;

    // --- データ + チェックサムが来るまで待つ ---
    while (seri->available() < len + 1) {
        if (millis() - start > 30) return false;
    }

    // --- 読み込み ---
    uint8_t buf[64];
    for (int i = 0; i < len; i++) buf[i] = seri->read();
    uint8_t checksum = seri->read();

    // --- チェックサム検証 ---
    uint8_t sum = calcChecksum(buf, len);
    if (checksum != sum) return false;

    memcpy(&d, buf, len);
    return true;
}

// ========= パケット送信 =========
template <typename sendT>
void sendPacket(int num_serial, sendT &d) {

    HardwareSerial *seri;
    if (num_serial == 0) seri = &Serial;

    uint8_t* p = (uint8_t*)&d;
    uint8_t len = sizeof(sendT);
    uint8_t cs = calcChecksum(p, len);

    seri->write(0xAA);
    seri->write(len);
    seri->write(p, len);
    seri->write(cs);
}

// ========= UART 初期化 =========
void initUART();

// ========= UART 読み取り =========
void UART();

#endif
