import sensor, image, time
from fpioa_manager import fm
from machine import UART

import struct
sensor.reset()
sensor.set_pixformat(sensor.RGB565) #16Bit color
sensor.set_framesize(sensor.QVGA)   #320x240
sensor.skip_frames(time = 2000)     #2秒間、カメラが安定するのを待つ。露出・ホワイトバランス調整のため
clock = time.clock()                #fps計測などに関わる

# K210のIO24=TX, IO25=RXにUART1を割り当て
fm.register(34, fm.fpioa.UART1_TX)
fm.register(35, fm.fpioa.UART1_RX)
# UART1を初期化
uart1 = UART(UART.UART1, 115200, timeout=1000, read_buf_len=4096)

# LAB色空間で青色を検出
#blue_threshold = [(33, 58, -128, 127, -128, -25)]
blue_threshold = [(15, 60, -40, 80, -100, -40)]
#(Lの最小,最大,  Aの最小,最大,  Bの最小,最大)  の順
#L:黒か白か
#A:緑か赤か
#B:青か黃か

#座標の端
cameraWidth  = 320
cameraHeight = 240

while True:
    #"clock.tick()：フレーム時間計測を開始
    #sensor.snapshot()：カメラ画像を1枚撮影して img に格納
    clock.tick()
    img = sensor.snapshot()
    #しきい値,ピクセル数の最小値,面積の最小値,隣り合った色をつなげる
    blobs = img.find_blobs(blue_threshold, pixels_threshold=100, area_threshold=100, merge=True)

    if blobs:
        for b in blobs:
            #中心の座標↓
            cx = b.cx()
            cy = b.cy()
            pixels = b.pixels()  # 面積（ピクセル数）
            Rect = b.rect()
            x, y, w, h = Rect  # それぞれ取り出す
            area = w * h       # 面積計算
            #マーキング(四角形,十字)
            img.draw_rectangle(b.rect())
            img.draw_cross(cx, cy)
            data_bytes = struct.pack('>HHH', cx, cy, area)
            uart1.write(data_bytes)
    else:
        uart1.write(struct.pack('>H', 0xFFFF))
        #time.sleep(1)



