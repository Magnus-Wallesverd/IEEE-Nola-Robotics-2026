#include <pybind11/pybind11.h>
#include <opencv2/opencv.hpp>
extern "C" {
	#include <apriltag/apriltag.h>
	#include <apriltag/tag36h11.h>
	#include <apriltag/image_u8.h>
}


namespace py = pybind11;

