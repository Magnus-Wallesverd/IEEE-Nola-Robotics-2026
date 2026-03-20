#  slave.py: rear camera

#  Wiring:
#  UART1 P0 TX ->  master camera  UART1 RX (P1)
#  Shared GND with master.

#  Slave packet (5 bytes, sent once per visible tag per frame):
#    [0]  0xBB   header
#    [1]  tag_id uint8  (0-7)
#    [2]  dist   uint8  (cm, 0-255)
#    [3]  bearing int8 as uint8  (two's complement, + = right of centre)
#    [4]  yaw    int8 as uint8 (two's complement)
#  No tags visible → single 0x00 byte response.


import sensor, time, math, machine
from pyb import UART

# sensor
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)   # 160 × 120
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

led1 = machine.LED("LED_BLUE")
led2 = machine.LED("LED_RED")
led3 = machine.LED("LED_GREEN")

clock = time.clock()
uart = UART(1,115200)

SLAVE_HEADER = 0xBB

TAG_SIZE_IN = 80.0 / 25.4   # 3.1496 in

FX = (2.8/3.984) * 160 * 1.36
FY = (2.8/2.952) * 120 * 1.36

# Set to -1 if the rear camera is mounted mirrored/upside-down
CAM_REAR_FLIP = 1

Tc = 0.0159
Ts = 1.0 / 13.3
y_prev = {}

while True:
    clock.tick()
    img = sensor.snapshot()

    for tag in img.find_apriltags(fx=FX, fy=FY, cx=img.width() / 2, cy=img.height() / 2):
        img.draw_rectangle(tag.rect, color=(255, 0, 0))
        img.draw_cross(tag.cx, tag.cy, color=(0, 255, 0))

        # Depth via pinhole formula
        tag_w_px = max(tag.w, tag.h)
        if tag_w_px <= 0:
            continue
        raw_depth_in = TAG_SIZE_IN * FX / tag_w_px

        # IIR smoother
        z  = abs(raw_depth_in)
        y1 = y_prev.get(tag.id, z)
        y  = (Ts * z + Tc * y1) / (Tc + Ts)
        y_prev[tag.id] = y

        # Bearing from pixel column
        x_trans     = CAM_REAR_FLIP * tag.x_translation
        z_trans     = abs(tag.z_translation)
        bearing_deg = math.degrees(math.atan2(x_trans, z_trans))

        yaw = int(math.degrees(tag.y_rotation))
        if yaw >= 180:
            yaw = yaw - 360

        deg = int(bearing_deg)
        dist = int(y*2.54)

        print("id=%d dist=%f bearing_deg=%f yaw=%f"  % (tag.id, dist, deg, yaw))

        pkt = bytearray(5)
        pkt[0] = SLAVE_HEADER
        pkt[1] = tag.id & 0xFF
        pkt[2] = dist & 0xFF
        pkt[3] = deg & 0xFF
        pkt[4] = yaw & 0xFF

        uart.write(pkt)

    Ts = 1.0/clock.fps()
