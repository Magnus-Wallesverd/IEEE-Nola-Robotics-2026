import sensor
import time
import math
from pyb import UART

# =========================================================
# Parameters
# =========================================================
FOCAL_LENGTH_MM = 2.8
SENSOR_WIDTH_MM = 3.984     # OV7725
TAG_SIZE_MM = 200           # real AprilTag size
BALL_DIAMETER_MM = 40       # real ball diameter
BOX_SIZE_MM = 152           # real box/container size

IMAGE_WIDTH = 160           # QQVGA width
FOCAL_LENGTH_PIXELS = (FOCAL_LENGTH_MM / SENSOR_WIDTH_MM) * IMAGE_WIDTH

OBJECT_THRESHOLD = (8, 85, 30, -6, 21, 69)
BOX_THRESHOLD = (7, 21, -13, 3, -4, 16)

# =========================================================
# Packet constants
# =========================================================
START_BYTE = 0xAA
PACKET_TAG = 0x01
PACKET_OBJ = 0x02

# =========================================================
# Camera setup
# =========================================================
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

clock = time.clock()

# UART3 on OpenMV: TX=P4, RX=P5
uart = UART(3, 115200)

# =========================================================
# Helper functions
# =========================================================
def clamp_u8(value):
    if value < 0:
        return 0
    if value > 255:
        return 255
    return value

def encode_signed_8bit(value):
    """
    Encodes signed integer range [-128, 127] into unsigned byte [0, 255].
    Decode on receiver with: decoded = byte - 128
    """
    if value < -128:
        value = -128
    elif value > 127:
        value = 127
    return value + 128

def calc_distance(width_pixels, real_size_mm):
    if width_pixels <= 0:
        return 0

    distance_mm = (real_size_mm * FOCAL_LENGTH_PIXELS) / width_pixels
    distance_cm = int(distance_mm / 10.0)

    return clamp_u8(distance_cm)

def calc_angle(cx):
    center_x = sensor.width() / 2
    x_offset = cx - center_x
    angle_rad = math.atan2(x_offset, FOCAL_LENGTH_PIXELS)
    angle_deg = int(math.degrees(angle_rad))
    return angle_deg

def send_tag_packet(tag_id, angle_deg, distance_cm):
    """
    Packet format:
    [0xAA][0x01][tag_id][angle_encoded][distance_cm]
    """
    tag_id_u8 = clamp_u8(int(tag_id))
    angle_u8 = encode_signed_8bit(int(angle_deg))
    dist_u8 = clamp_u8(int(distance_cm))

    packet = bytearray(5)
    packet[0] = START_BYTE
    packet[1] = PACKET_TAG
    packet[2] = tag_id_u8
    packet[3] = angle_u8
    packet[4] = dist_u8

    uart.write(packet)

def send_obj_packet(angle_deg, distance_cm):
    """
    Packet format:
    [0xAA][0x02][angle_encoded][distance_cm]
    """
    angle_u8 = encode_signed_8bit(int(angle_deg))
    dist_u8 = clamp_u8(int(distance_cm))

    packet = bytearray(4)
    packet[0] = START_BYTE
    packet[1] = PACKET_OBJ
    packet[2] = angle_u8
    packet[3] = dist_u8

    uart.write(packet)

# =========================================================
# Detection functions
# =========================================================
def detect_apriltags(img):
    tags = img.find_apriltags()

    for tag in tags:
        # OpenMV usually uses methods like tag.w(), tag.cx(), tag.id()
        try:
            tag_width = tag.w()
            tag_cx = tag.cx()
            tag_cy = tag.cy()
            tag_id = tag.id()
            tag_rect = tag.rect()
        except TypeError:
            # fallback if your firmware exposes fields instead
            tag_width = tag.w
            tag_cx = tag.cx
            tag_cy = tag.cy
            tag_id = tag.id
            tag_rect = tag.rect

        distance_cm = calc_distance(tag_width, TAG_SIZE_MM)
        angle_deg = calc_angle(tag_cx)

        send_tag_packet(tag_id, angle_deg, distance_cm)

        print("TAG:", tag_id, "angle=", angle_deg, "dist_cm=", distance_cm)

        img.draw_rectangle(tag_rect, color=(255, 0, 0))
        img.draw_cross(tag_cx, tag_cy, color=(0, 255, 0))

        return 1

    return 0

def detect_blob(img, threshold, real_size_mm):
    blobs = img.find_blobs([threshold], pixels_threshold=10, area_threshold=10)
    valid_blobs = []

    for b in blobs:
        w = b.w()
        h = b.h()

        if h == 0:
            continue

        aspect = w / h
        area = b.pixels()
        solidity = b.density()

        if (0.8 < aspect < 1.2) and (area < 5000) and (solidity > 0.35):
            blob_diameter = (w + h) / 2
            angle_deg = calc_angle(b.cx())
            distance_cm = calc_distance(blob_diameter, real_size_mm)

            valid_blobs.append((b, angle_deg, distance_cm))

            img.draw_rectangle(b.rect())
            img.draw_cross(b.cx(), b.cy())

    if not valid_blobs:
        return 0

    closest = min(valid_blobs, key=lambda x: x[2])
    blob, angle_deg, distance_cm = closest

    send_obj_packet(angle_deg, distance_cm)

    print("OBJ: angle=", angle_deg, "dist_cm=", distance_cm)

    if distance_cm <= 10:
        print("STOP, pick up")
    elif angle_deg > 7:
        print("Turn right")
    elif angle_deg < -7:
        print("Turn left")
    else:
        print("Move forward")

    img.draw_rectangle(blob.rect(), color=(0, 255, 0))
    img.draw_cross(blob.cx(), blob.cy(), color=(255, 0, 0))

    return len(valid_blobs)

# =========================================================
# Main loop
# =========================================================
frame_count = 0
tag_found = 0

while True:
    clock.tick()
    img = sensor.snapshot()
    frame_count += 1

    if tag_found == 0:
        tag_found = detect_apriltags(img)

    if frame_count % 240 == 0:
        balls = detect_blob(img, OBJECT_THRESHOLD, BALL_DIAMETER_MM)
        # box = detect_blob(img, BOX_THRESHOLD, BOX_SIZE_MM)

    # Optional debug:
    # print("FPS:", clock.fps())
