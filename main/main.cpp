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
const char* inputfile = "../test/resources/Images/40mph/20220312_105508.mp4_frame509.jpg";


// Global Mats
cv::Mat originalImg;
cv::Mat outputImg;


int main() {

	originalImg = cv::imread(inputfile, cv::IMREAD_COLOR);

	// ensure image was valid
	if (!originalImg.data) {
		std::cout << "Missing data." << std::endl;
		return -1;
	}

	showDebugImage("Input Image", originalImg, cv::WINDOW_NORMAL);

	processFrame(originalImg, outputImg);

	// Clean up
	cv::destroyAllWindows();
	return 0;
}