from picamera2.encoders import H264Encoder
from picamera2.outputs import CircularOutput2, PyavOutput
from picamera2 import Picamera2
import time


picam2 = Picamera2()

video_config = picam2.create_video_configuration(controls={"FrameDurationLimits": (33333, 33333)})
picam2.configure(video_config)

encoder = H264Encoder(bitrate=10000000)
circular = CircularOutput2(buffer_duration_ms=5000)

picam2.start_recording(encoder, circular)
print("Recording Started...")
picam2.title_fields = ["ExposureTime", "AnalougeGain"]

time.sleep(5) # 5 second video

circular.open_output(PyavOutput("start.mp4"))
time.sleep(5)
circular.close_output()

circular.open_output(PyavOutput("end.mp4"))
picam2.stop_recording()
print("...Recording Finished")



