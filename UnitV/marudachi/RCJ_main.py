import sensor, image, time, struct
from fpioa_manager import fm
from machine import UART

# ===============================
# カメラ初期化
# ===============================
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
#sensor.set_framesize(sensor.QQVGA)  # 160x120

sensor.skip_frames(time=2000)
clock = time.clock()

# 上下反転（左右反転は使わない）
sensor.set_vflip(0)
# sensor.set_hmirror(1)  # ← 色ズレ防止でOFF

# ===============================
# UART 初期化
# ===============================
fm.register(34, fm.fpioa.UART1_TX)
fm.register(35, fm.fpioa.UART1_RX)
uart1 = UART(UART.UART1, 115200, timeout=10, read_buf_len=256)

# ===============================
# XOR チェックサム（Arduino互換）
# ===============================
def calc_checksum(data):
    cs = 0
    for b in data:
        cs ^= b
    return cs & 0xFF

# ===============================
# sendPacket（Arduino互換）
# ===============================
def send_packet(data):
    length = len(data)
    if length > 64:
        return  # Arduino側 buf[64] 対策

    cs = calc_checksum(data)

    uart1.write(bytes([0xAA]))      # header
    uart1.write(bytes([length]))    # len
    uart1.write(data)               # payload
    uart1.write(bytes([cs]))        # checksum

# ===============================
# LAB色空間で青色検出
# ===============================
blue_threshold = [(48, 93, -23, -1, 10, 54)]
#blue_threshold =[(24, 64, -10, 6, 13, 56)]#yellow

cameraWidth  = 320 #QVGA
cameraHeight = 240

#cameraWidth  = 160  # QQVGA
#cameraHeight = 120


# ===============================
# メインループ
# ===============================
while True:
    clock.tick()
    img = sensor.snapshot()

    blobs = img.find_blobs(
        blue_threshold,
        pixels_threshold=100,
        area_threshold=100,
        merge=True
    )

    if blobs:
        # 一番大きいblobを使う（安定）
        b = max(blobs, key=lambda x: x.pixels())

        cx = int(cameraWidth / 2 - b.cx())
        #cx = b.cx()
        cy = b.cy()

        x, y, w, h = b.rect()
        area = w * h

        # 描画
        img.draw_rectangle(b.rect())
        img.draw_cross(cx, cy)

        # Arduino struct SendPacketData0 { int cx; int cy; int area; };

        HFOV = 60.0

        angle = int((cx / (cameraWidth / 2)) * (HFOV / 2) * 100)
        # ×100 → 小数対応（0.01deg単位）

        data = struct.pack('<hhh', cx, cy, area)
        send_packet(data)

    else:
        # 検出なし → cx = 400 を送る
        data = struct.pack('<hhh', 400, 0, 0)
        send_packet(data)

    #time.sleep_ms(30)
