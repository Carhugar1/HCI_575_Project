#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "debugUtil.h"

// --------------------------
// Debugging Utility Code
// Author: Branden Sammons
// --------------------------

const bool DEBUG_MODE = true;

// compile command:
// cl /EHsc /c debugUtil.cpp /I %OpenCV_Path%\build\include

/**
 *	Helper Method to show a window with the given windowName and inputMat when DEBUG_MODE equals true.
 */
void showDebugImage(std::string windowName, cv::Mat inputMat) {
	if (DEBUG_MODE) {
		cv::namedWindow(windowName, cv::WINDOW_NORMAL);
		cv::imshow(windowName, inputMat);
		cv::waitKey(0);
	}
}

/**
 *	Helper Method to print the image from the given inputMat when DEBUG_MODE equals true.
 */
void printDebugImage(cv::Mat inputMat) {
	if (DEBUG_MODE) {
		cv::imwrite("debugPrintImage.jpg", inputMat);
	}
}

/**
 *	Helper Method to print the given debugLine to the console when DEBUG_MODE equals true.
 */
void printDebugLine(std::string debugLine) {
	if (DEBUG_MODE) {
		std::cout << debugLine << std::endl;
	}
}