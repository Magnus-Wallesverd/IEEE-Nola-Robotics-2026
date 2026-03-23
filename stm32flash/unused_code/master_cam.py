# master camera

# UART3  P4 (TX) / P5 (RX) <->  STM32 115200 baud
# UART1  P0 (RX) <- slave rear camera, 115200 baud
# Shared GND between master, slave, and STM32.

# receive packet from STM:
#    [0]  0xAA     header
#    [1]  fn_id    function ID
#    [2]  arg1     arg: 0 = none, 1 = front, 2 = back, or lo byte
#    [3]  arg2     arg: high byte (for math functions)
#    [3]  0x55     footer

# return packet to STM:
#    [0]  0xAA     header
#    [1]  value low
#    [2]  value high
#    [3]  0x55     footer

# function dispatch table:
#    FN_SEE_TAG = 0x00   return: 0 = no see, 1 = front sees, 2 = back sees, 3 = both sees
#    FN_ID      = 0x01   return: tag ID
#    FN_DIST    = 0x02   return: tag distance from cam
#    FN_ANG     = 0x03   return: tag angle from cam
#    FN_YAW     = 0x04   return: tag yaw from cam
#    FN_SQRT    = 0x05   return: sqrt(arg)
#    FN_ATAN    = 0x06   return: atan(arg)
#    FN_PROJ_X  = 0x07   return: dist * sin(ang)
#    FN_PROJ_Y  = 0x08   return: dist * cos(ang)
#    FN_START   = 0x09   return: 1 if led detected, 0 otherwise

#  Slave packet:
#    [0]  0xBB        header
#    [1]  tag_id
#    [2]  distance
#    [3]  bearing deg
#    [4]  yaw
#    [5]  flags

import sensor, time, machine, math
from pyb import UART

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

clock = time.clock()
led1 = machine.LED("LED_BLUE")
led2 = machine.LED("LED_RED")
led3 = machine.LED("LED_GREEN")

uart_stm  = UART(3, 115200)   # P4 TX / P5 RX -> STM32
uart_slave = UART(1, 115200)  # P0 RX <- slave camera

# UART / protocol constants
STM_HEADER   = 0xAA
STM_FOOTER   = 0x55
SLAVE_HEADER = 0xBB

FN_SEE_TAG  = 0x00
FN_ID       = 0x01
FN_DIST     = 0x02
FN_ANG      = 0x03
FN_YAW      = 0x04
FN_SQRT     = 0x05
FN_ATAN     = 0x06
FN_PROJ_X   = 0x07
FN_PROJ_Y   = 0x08
FN_START    = 0x09

# camera paramters
TAG_SIZE_IN = 80.0 / 25.4   # 3.1496 in

FX = (2.8/3.984) * 160 * 1.36
FY = (2.8/2.952) * 120 * 1.36
#FX = (2.8 / 3.984) * 656 * 0.51
#FY = (2.8 / 2.952) * 488 * 0.51

# Set to -1 if the front camera is mounted mirrored/upside-down
CAM_FRONT_FLIP = 1

Tc = 0.0159
Ts = 1.0 / 13.3
y_prev = {}

# starting values
front_buf = {}
rear_buf = {}
front_id = None
rear_id = None

start_detected = 0

# helpers
def _unpack_int16(lo, hi):
    val = (hi << 8) | lo
    if val >= 32768:
        val -= 65536
    return val

# APRILTAG READINGS =============================================
def proc_front(img):
    global front_buf, front_id
    front_buf.clear()
    front_id = None

    for tag in img.find_apriltags(fx=FX, fy=FY, cx=img.width() / 2, cy=img.height() / 2):
        led1.on()
        img.draw_rectangle(tag.rect, color=(255, 0, 0))
        img.draw_cross(tag.cx, tag.cy, color=(0, 255, 0))

        front_id = tag.id

        # Depth via pinhole formula
        tag_w_px = max(tag.w, tag.h)
        if tag_w_px <= 0:
            continue
        raw_depth_in = TAG_SIZE_IN * FX / tag_w_px

        # IIR smoother
        z  = abs(raw_depth_in)
        y1 = y_prev.get(front_id, z)
        y  = (Ts * z + Tc * y1) / (Tc + Ts)
        y_prev[front_id] = y

        # Bearing from pixel column
        x_trans     = CAM_FRONT_FLIP * tag.x_translation
        z_trans     = abs(tag.z_translation)
        bearing_deg = math.degrees(math.atan2(x_trans, z_trans))

        yaw = int(math.degrees(tag.y_rotation))
        if yaw >= 180:
            yaw = yaw - 360

        deg = int(bearing_deg)
        dist = int(y*2.54)

        print("id=%d dist=%f bearing_deg=%f yaw=%f"  % (front_id, dist, deg, yaw))
        front_buf[front_id] = (dist,deg,yaw)

    if not front_buf:
        led1.off()


# SLAVE UART READER ============================================
def proc_rear():
    global rear_buf, rear_id, start_detected
    rear_buf.clear()
    rear_id = None

    while uart_slave.any() >= 6:
        # Sync to header byte, discard anything before 0xBB
        b = uart_slave.read(1)
        if b is None or b[0] != SLAVE_HEADER:
            continue
        # Need 5 more bytes
        if uart_slave.any() < 5:
            break
        rest = uart_slave.read(5)
        if rest is None or len(rest) < 5:
            break

        tag_id = rest[0]
        dist = rest[1]
        bearing = rest[2]
        yaw = rest[3]
        flags = rest[4]

        if flags & 0x01:
            start_detected = 1

        if tag_id != 0xFF:
            bearing_deg = bearing if bearing < 128 else bearing - 256
            yaw_deg = yaw if yaw < 128 else yaw - 256

            rear_id = tag_id
            rear_buf[tag_id] = (dist, bearing_deg, yaw_deg)

            print("REAR  id=%d dist=%d bearing=%d yaw=%d" % (tag_id, dist, bearing_deg, yaw_deg))

# receive requests from STM
def receive_request():
    """ scans for instruction from STM """
    while uart_stm.any() >= 5:
        b = uart_stm.read(1)
        if b is None:
            return None, None, None
        if b[0] != STM_HEADER:
            continue    # discard misaligned bytes
        if uart_stm.any() < 4:
            return None, None, None
        rest = uart_stm.read(4)
        if rest is None or len(rest) < 4:
            return None, None, None
        if rest[3] != STM_FOOTER:
            continue
        return rest[0], rest[1], rest[2] #function id, arg1, arg2
    return None, None, None

#  UART HELPERS  (STM32)
def send_pkt(value):
    v = int(value) & 0xFF

    pkt    = bytearray(4)
    pkt[0] = STM_HEADER
    pkt[1] = v & 0xFF
    pkt[2] = (v >> 8) & 0xFF
    pkt[3] = STM_FOOTER
    uart_stm.write(pkt)
    print("TX function return: {}".format(value))

def get_tag(cam):
    """Return (dist, bearing, yaw) for cam=1/2, or None."""
    if cam == 1 and front_id is not None and front_id in front_buf:
        return front_buf[front_id]
    if cam == 2 and rear_id is not None and rear_id in rear_buf:
        return rear_buf[rear_id]
    return None

def cmd_see_tag():
    has_front = len(front_buf) > 0
    has_rear = len(rear_buf) > 0
    if has_front and has_rear:
        send_pkt(3)
    elif has_rear:
        send_pkt(2)
    elif has_front:
        send_pkt(1)
    else:
        send_pkt(0)

def cmd_id(cam):
    if cam == 1 and front_id is not None:
        send_pkt(front_id)
    elif cam == 2 and rear_id is not None:
        send_pkt(rear_id)
    else:
        send_pkt(0xFF) # no tag

def cmd_dist(cam):
    t = get_tag(cam)
    send_pkt(t[0] if t else 0)

def cmd_ang(cam):
    t = get_tag(cam)
    send_pkt(t[1] & 0xFF if t else 0)

def cmd_yaw(cam):
    t = get_tag(cam)
    send_pkt(t[2] & 0xFF if t else 0)

def cmd_sqrt(lo,hi):
    value = (hi << 8) | lo
    send_pkt(int(math.sqrt(abs(value))))

def cmd_atan(lo,hi):
    value = _unpack_int16(lo,hi)
    send_pkt(int(math.degrees(math.atan(value))) & 0xFFFF)

def cmd_proj_x(cam):
    t = get_tag(cam)
    val = int(t[0] * math.sin(math.radians(t[1]))) if t else 0
    send_pkt(val & 0xFFFF)

def cmd_proj_y(cam):
    t = get_tag(cam)
    val = int(t[0] * math.cos(math.radians(t[1]))) if t else 0
    send_pkt(val & 0xFFFF)

def cmd_start():
    send_pkt(1 if start_detected else 0)

# process request from STM
def process_request(function, arg1, arg2):
    if function is None:
        return
    if function == FN_SEE_TAG:
        cmd_see_tag()
    elif function == FN_ID:
        cmd_id(arg1)
    elif function == FN_DIST:
        cmd_dist(arg1)
    elif function == FN_ANG:
        cmd_ang(arg1)
    elif function == FN_YAW:
        cmd_yaw(arg1)
    elif function == FN_SQRT:
        cmd_sqrt(arg1,arg2)
    elif function == FN_ATAN:
        cmd_atan(arg1,arg2)
    elif function == FN_PROJ_X:
        cmd_proj_x(arg1)
    elif function == FN_PROJ_Y:
        cmd_proj_y(arg1)
    elif function == FN_START:
        cmd_start()
    else:
        return

#  MAIN LOOP
while True:
    clock.tick()
    Ts = 1.0/ max(clock.fps(), 1.0)
    img = sensor.snapshot()

    proc_front(img)
    proc_rear()

    fn,arg1,arg2 = receive_request()
    process_request(fn, arg1, arg2)
