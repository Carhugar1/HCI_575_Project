#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "detection.h"

// --------------------------
// Sign Detection Code
//		HCI 575 Project
//		Team 5
//		Spring 2022
// --------------------------


const bool DEBUG_MODE = true;

// compile command:
// cl /EHsc detection.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib


// Input file
const char* inputfile = "../test/resources/Images/40mph/20220312_105508.mp4_frame509.jpg";

// Constants
// TODO make these porpotional to the image size
const int MIN_CONTOUR_AREA = 1000;
const int MAX_CONTOUR_AREA = 100000;

// Globals for the processFrame function
cv::Mat grayFrame;
cv::Mat blurFrame;
cv::Mat bwFrame;


// fuction prototypes
void showDebugImage(std::string windowName, cv::Mat inputMat, bool debugMode);
void printDebugImage(cv::Mat inputMat, bool debugMode);


int main() {

	// Mats
	cv::Mat originalImg;
	cv::Mat outputImg;

	originalImg = cv::imread(inputfile, cv::IMREAD_COLOR);

	// ensure image was valid
	if (!originalImg.data) {
		std::cout << "Missing data." << std::endl;
		return -1;
	}

	showDebugImage("Input Image", originalImg, DEBUG_MODE);

	processFrame(originalImg, outputImg, 0);

	// Clean up
	cv::destroyAllWindows();
	return 0;
}


/**
 *	Helper Method to show a window with the given windowName and inputMat when debugMode is enabled.
 */
void showDebugImage(std::string windowName, cv::Mat inputMat, bool debugMode) {
	
	if (debugMode) {
		cv::namedWindow(windowName, cv::WINDOW_NORMAL);
		cv::imshow(windowName, inputMat);
		cv::waitKey(0);
	}
}

/**
 *	Helper Method to print a frame from the given inputMat when debug mode is enabled.
 */
void printDebugImage(cv::Mat inputMat, bool debugMode) {

	if (debugMode) {
		cv::imwrite("debugPrintImage.jpg", inputMat);
	}
}


/**
 *	Process Frame
 * 
 *	TODO add more info, change the signature as needed.
 *	TODO clone information to header file
 */
void processFrame(cv::Mat inputMat, cv::Mat& destMat, int frameNum) {

	inputMat.copyTo(destMat);
	
	// Convert to GrayScale
	cv::cvtColor(inputMat, grayFrame, cv::COLOR_BGR2GRAY);

	// Blur Image to smooth edges
	cv::medianBlur(grayFrame, blurFrame, 3);
	
	showDebugImage("Blurred Image", blurFrame, DEBUG_MODE);
	//printDebugImage(blurFrame, DEBUG_MODE);

	// Threshold
	cv::threshold(blurFrame, bwFrame, 50, 255, cv::THRESH_BINARY_INV);

	showDebugImage("BW Image 1", bwFrame, DEBUG_MODE);

	// Clean up with Morphological Transform
	//cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	//cv::morphologyEx(bwFrame, bwFrame, cv::MORPH_CLOSE, se, cv::Point(-1, -1), 1);

	//showDebugImage("BW Image 2", bwFrame, DEBUG_MODE);

	// Find edges 


	// Find all the Contors
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(bwFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// Draw the bounding Rectangles on the Image
	std::vector<cv::Point> contour;
	std::vector<cv::Point> approxPoly;
	for (int i = 0; i < contours.size(); i++) {

		contour = contours.at(i);

		// filter based on size
		int area = cv::contourArea(contour);
		if (area > MIN_CONTOUR_AREA && area < MAX_CONTOUR_AREA) {

			// approximate the shape
			double perimeter = cv::arcLength(contour, true);
			cv::approxPolyDP(contour, approxPoly, 0.1 * perimeter, true);

			// Rectangles have 4 points
			if (approxPoly.size() == 4) {
				cv::drawContours(destMat, contours, i, cv::Scalar(0, 255, 0), 3);
			}
			else {
				cv::drawContours(destMat, contours, i, cv::Scalar(255, 0, 0), 3);
			}
		}
	}

	showDebugImage("Output Image", destMat, DEBUG_MODE);

	return;
}