import sensor, time, math
from pyb import UART

# parameters
FOCAL_LENGTH_MM = 2.8
SENSOR_WIDTH_MM = 3.984 # for OV7725
TAG_SIZE_MM = 200 # irl size of AprilTag
BALL_DIAMETER = 40 # irl size of ball in mm
BOX_SIZE = 152 # irl size of container

# convert focal length to pixels
IMAGE_WIDTH = 160 # QQVGA
FOCAL_LENGTH_PIXELS = (FOCAL_LENGTH_MM / SENSOR_WIDTH_MM) * IMAGE_WIDTH

# color of ball
OBJECT_THRESHOLD = (8, 85, 30, -6, 21, 69)
BOX_THRESHOLD = (7, 21, -13, 3, -4, 16)

# initialize camera
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...

clock = time.clock()

# initialize uart (TX: P4, RX: P5)
uart = UART(3, 115200)

# helper functions
def calc_distance(width, size):
    pixel_width = width
    if pixel_width == 0:
        return 0
    distance_mm = (size * FOCAL_LENGTH_PIXELS) / pixel_width
    return distance_mm / 10.0 # convert to cm

def calc_angle(cx):
    center_x = sensor.width() / 2
    x_offset = cx - center_x
    angle_rad = math.atan2(x_offset, FOCAL_LENGTH_PIXELS)
    return math.degrees(angle_rad)

def detect_apriltags(img):
    tags = img.find_apriltags()

    for tag in tags:
        distance_cm = calc_distance(tag.w, TAG_SIZE_MM)
        angle_deg = calc_angle(tag.cx)
        msg = "TAG, {},{:.2f},{:.2f}\n".format(
            tag.id,
            angle_deg,
            distance_cm
        )
        packet = bytearray(2)
        packet[0] = int(distance_cm)
        packet[1] = int(angle_deg)
        uart.write(packet)
        print(msg)

        img.draw_rectangle(tag.rect, color=(255, 0, 0))
        img.draw_cross(tag.cx, tag.cy, color=(0, 255, 0))

        return tag
    return 0

def detect_blob(img, threshold, size):
    blobs = img.find_blobs([threshold], pixels_threshold=10, area_threshold=10)
    valid_blobs = []

    for b in blobs:
        aspect = b.w() / b.h()
        area = b.pixels()
        solidity = b.density()

        if(0.8 < aspect < 1.2) and (area < 5000) and (solidity > 0.35):
            blob_diameter = (b.w() + b.h()) / 2
            angle_deg = calc_angle(b.cx())
            distance_cm = calc_distance(blob_diameter, size)

            valid_blobs.append((b, angle_deg, distance_cm))

            img.draw_rectangle(b.rect())
            img.draw_cross(b.cx(), b.cy())

    if not valid_blobs:
        return 0

    closest = min(valid_blobs, key=lambda x: x[2])
    blob, angle_deg, distance_cm = closest

    msg = "OBJ, {:.2f}, {:.2f}".format(angle_deg, distance_cm)
    #msg = f"{int(angle_deg)}"

    packet = bytearray(2)
    packet[0] = int(angle_deg)
    packet[1] = int(distance_cm)
    uart.write(packet)

    print(msg)

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

# Main Loop
frame_count = 0
tag = 0
balls = 0
box = 0

while True:
    clock.tick()
    img = sensor.snapshot()
    frame_count += 1

    if tag == 0:
        tag = detect_apriltags(img)

    #if frame_count % 160 == 0:
    #    balls = detect_blob(img, OBJECT_THRESHOLD, BALL_DIAMETER)
        #box = detect_blob(img, BOX_THRESHOLD, BOX_SIZE)
        #items = balls + box
        #if balls > 0:
        #    print("Number of balls: ")
        #else:
        #    print("Turn right or move forward idk")

