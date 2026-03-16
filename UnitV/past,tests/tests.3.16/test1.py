import sensor
import time

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

sensor.skip_frames(time=2000)

print("before")

sensor.set_auto_gain(False)

print("after")

while True:
    img = sensor.snapshot()
