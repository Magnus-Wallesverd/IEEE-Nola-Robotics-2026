from picamera2 import Picamera2
import time
import cv2
from apriltag import apriltag
# import detect_tag # cpp file for detecting tags

# Create Picamera2 instance
picam2 = Picamera2()

# create configuration for image capture 
video_config = picam2.create_video_configuration(main={"format": "RGB888", "size": (854,480)}, buffer_count=1)
picam2.configure(video_config)

# start the camera
picam2.start()
time.sleep(1) # camera might need to warm up

# start_time = time.time()
# capture and save JPEG. record until break condition
# while True:
frame = picam2.capture_file("img.jpg") # take a photo
   #  detect_tag.process_frame(frame) # send frame over for processing

   # if time.time() - start_time > 5: # if time since start > 5
       #  break
picam2.close()

greyimg = cv2.imread('img.jpg',cv2.IMREAD_GRAYSCALE)
#options = apriltag.Detectoroptions(families='tag36h11')
detector = apriltag("tag36h11")

result = detector.detect(greyimg)


print(type(greyimg)) # grey img is numpy nd array
print(type(detector)) 
print(type(result)) # tuple, need to grab from this
