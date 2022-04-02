#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "detection.h"

#define DEBUG true
#include "utils/debugUtil.h"

// --------------------------
// Sign Detection Code
//		HCI 575 Project
//		Team 5
//		Spring 2022
// --------------------------


// compile command:
// cl /EHsc detection.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib utils/*.obj


// Input file
const char* inputfile = "../test/resources/Images/40mph/20220312_105508.mp4_frame509.jpg";

// Constants
const float MIN_CONTOUR_AREA_PERCENTAGE = 0.001;
const float MAX_CONTOUR_AREA_PERCENTAGE = 0.05;

// Globals for the processFrame function
int min_contour_area;
int max_contour_area;
cv::Mat grayFrame;
cv::Mat blurFrame;
cv::Mat bwFrame;

// private function prototypes
void detectShapes(cv::Mat inputMat, std::vector<ShapeInfo>* shapeVect);


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

	showDebugImage("Input Image", originalImg);

	processFrame(originalImg, outputImg);

	// Clean up
	cv::destroyAllWindows();
	return 0;
}


/**
 *	Process Frame
 * 
 *	TODO add more info, change the signature as needed.
 *	TODO clone information to header file
 */
void processFrame(cv::Mat inputMat, cv::Mat& destMat, int frameNum) {

	inputMat.copyTo(destMat);

	// Frame 0; setup some globals
	if (frameNum == 0) {
		int pixelSize = inputMat.rows * inputMat.cols;
		
		min_contour_area = pixelSize * MIN_CONTOUR_AREA_PERCENTAGE;
		max_contour_area = pixelSize * MAX_CONTOUR_AREA_PERCENTAGE;

		printDebugLine("min_contour_area = " + std::to_string(min_contour_area));
		printDebugLine("max_contour_area = " + std::to_string(max_contour_area));
	}
	
	// Convert to GrayScale
	cv::cvtColor(inputMat, grayFrame, cv::COLOR_BGR2GRAY);

	// Blur Image to smooth edges
	cv::medianBlur(grayFrame, blurFrame, 3);
	
	showDebugImage("Blurred Image", blurFrame);
	//printDebugImage(blurFrame, DEBUG_MODE);

	// Threshold
	cv::threshold(blurFrame, bwFrame, 50, 255, cv::THRESH_BINARY_INV);

	showDebugImage("BW Image 1", bwFrame);

	std::vector<ShapeInfo> shapeVect;
	detectShapes(inputMat, &shapeVect);

	for (int i = 0; i < shapeVect.size(); i++) {
		ShapeInfo sInfo = shapeVect.at(i);

		if (sInfo.first == SHAPE_RECTANGLE) {
			cv::polylines(destMat, sInfo.second, true, cv::Scalar(0, 255, 0), 1);
		}
	}

	showDebugImage("Output Image", destMat);

	return;
}

/**
 *	DetectShapes
 *
 *	TODO add more info, change the signature as needed.
 *	TODO clone information to header file
 */
void detectShapes(cv::Mat inputMat, std::vector<ShapeInfo>* shapeVect) {

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
		if (area > min_contour_area && area < max_contour_area) {

			// approximate the shape
			double perimeter = cv::arcLength(contour, true);
			cv::approxPolyDP(contour, approxPoly, 0.1 * perimeter, true);

			int shape = SHAPE_UNDEFINED;
			if (approxPoly.size() == 3) {
				shape = SHAPE_TRIANGLE;
			}
			else if (approxPoly.size() == 4) {
				shape = SHAPE_RECTANGLE;
			}
			else if (approxPoly.size() == 8) {
				shape = SHAPE_OCTAGON;
			}

			ShapeInfo shapeInfo = ShapeInfo(shape, approxPoly);
			shapeVect->push_back(shapeInfo);
		}
	}

}