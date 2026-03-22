import sensor, image, time, struct, math
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

# 1フレーム取得
img = sensor.snapshot()


# その後に設定
sensor.set_auto_gain(False, gain_db=16)
sensor.set_auto_whitebal(False,rgb_gain_db=(73.0, 62.0, 134.0))#R G B , rgb_gain_db=(10.0, 10.0, 20.0)
sensor.set_auto_exposure(False, exposure_us=400)
#値の目安がわからないとき：
#すべてオートにして出力↓
#print("gain:", sensor.get_gain_db())
#print("exp:", sensor.get_exposure_us())
#print("wb:", sensor.get_rgb_gain_db())


# 上下反転（左右反転は使わない）
#sensor.set_vflip(1)
#sensor.set_hmirror(1)  # ← 色ズレ防止でOFF

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
    cs = len(data)
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
    uart1.write(bytes([0x55]))      # header2
    uart1.write(bytes([length]))    # len
    uart1.write(data)               # payload
    uart1.write(bytes([cs]))        # checksum

# ===============================
# LAB色空間で青色検出
# ===============================
blue_threshold = [(40, 100, -22, 3, 47, 85)]

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
        pixels_threshold=20,
        area_threshold=20,
        merge=True
    )

    if blobs:
        # 一番大きいblobを使う（安定）
        b = max(blobs, key=lambda x: x.pixels())

        cx = int(cameraWidth / 2 - b.cx())
        #cx = b.cx()
        cy = int(cameraHeight / 2 - b.cy())

        x, y, w, h = b.rect()
        area = w * h

        # 描画
        img.draw_rectangle(b.rect())
        img.draw_cross(b.cx(), b.cy())

        # Arduino struct SendPacketData0 { int cx; int cy; int area; };

        HFOV = 60.0

        rad = math.atan2(cy, cx)
        deg = rad * 180 / math.pi
        angle = int(-deg)
        #print(angle)
        # ×100 → 小数対応（0.01deg単位）

        camera_dis = int(math.sqrt(cy*cy + cx*cx))
        #print(camera_dis)


        #dis = int(3500.0 / (105.0 - camera_dis));
        den = (105.0 - camera_dis)
        if abs(den) < 1:
            den = 1
        dis = 3500.0 / den
        dis = int(max(min(dis, 32767), -32768))


        #print(dis)

        data = struct.pack('<hh', angle, dis) #hの数が変数の数?
        send_packet(data)

    else:
        # 検出なし → cx = 400 を送る
        data = struct.pack('<hh', 400, -1)
        send_packet(data)

    #time.sleep_ms(30)
