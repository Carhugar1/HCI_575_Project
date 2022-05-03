#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "detection.h"
#include "utils/debugUtil.h"

// --------------------------
// Sign Detection Code (Main Method)
//		HCI 575 Project
//		Team 5
//		Spring 2022
// --------------------------


// compile command:
// cl /EHsc /Fe:main main.cpp detection.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib utils/debugUtil.obj

// execute command:
// main.exe



// Input file
const char* inputfile = "../test/resources/Videos/20220312_105508.mp4";

// Output file
const char* outputfile = "../main/out.mp4";


// Video Capture
cv::VideoCapture inputCap;
cv::VideoWriter outputCap;

// Global Mats
cv::Mat originalImg;
cv::Mat outputImg;


int main() {

	inputCap = cv::VideoCapture(inputfile);

	// Check validity of target file
	if (!inputCap.isOpened()) {
		std::cout << "Input video not found." << std::endl;
		return -1;
	}

	outputCap = cv::VideoWriter(outputfile,
		cv::VideoWriter::fourcc('a', 'v', 'c', '1'),
		inputCap.get(cv::CAP_PROP_FPS),
		cv::Size(inputCap.get(cv::CAP_PROP_FRAME_WIDTH),
			inputCap.get(cv::CAP_PROP_FRAME_HEIGHT))
	);

	// Again, check validity of target output file
	if (!outputCap.isOpened()) {
		std::cout << "Could not create output file." << std::endl;
		return -1;
	}

	cv::Mat inputFrame, outputFrame;
	int frameNum = 0;
	while (inputCap.read(inputFrame)) {

		processFrame(inputFrame, outputFrame, frameNum);

		frameNum++;
		outputCap.write(outputFrame);
	}

	// free the capture objects from memory
	inputCap.release();
	outputCap.release();

	// Clean up
	cv::destroyAllWindows();
	return 0;
}