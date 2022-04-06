#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "detection.h"
#include "numberSE.h"
#include "utils/debugUtil.h"

// --------------------------
// Sign Detection Code
//		HCI 575 Project
//		Team 5
//		Spring 2022
// --------------------------


// compile command:
// cl /EHsc detection.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib utils/debugUtil.obj

// execute command:
// detection.exe


// Input file
const char* inputfile = "../test/resources/Images/40mph/20220312_105508.mp4_frame509.jpg";

// Constants
const float MIN_CONTOUR_AREA_PERCENTAGE = 0.001;
const float MAX_CONTOUR_AREA_PERCENTAGE = 0.05;
const int IDEAL_SIGN_SIZE_PX = 250;

// Globals for the processFrame function
int min_contour_area;
int max_contour_area;
cv::Mat grayFrame;
cv::Mat blurFrame;
cv::Mat bwFrame;
cv::Mat subMat;

// private function prototypes
void detectShapes(cv::Mat inputMat, std::vector<ShapeInfo>* shapeVect);
void getShapeSubMatrix(cv::Mat inputMat, cv::Mat& subMat, ShapeInfo* shapeInfo);
void detectSpeedLimit(cv::Mat inputMat, cv::Mat& output);


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

	showDebugImage("Input Image", originalImg, cv::WINDOW_NORMAL);

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
	
	showDebugImage("Blurred Image", blurFrame, cv::WINDOW_NORMAL);
	//printDebugImage(blurFrame);

	// Threshold
	cv::threshold(blurFrame, bwFrame, 50, 255, cv::THRESH_BINARY_INV);

	showDebugImage("BW Image 1", bwFrame, cv::WINDOW_NORMAL);

	std::vector<ShapeInfo> shapeVect;
	detectShapes(bwFrame, &shapeVect);

	/*// Draw the Identified Shapes
	for (int i = 0; i < shapeVect.size(); i++) {
		ShapeInfo sInfo = shapeVect.at(i);

		if (sInfo.first == SHAPE_RECTANGLE) {
			cv::polylines(destMat, sInfo.second, true, cv::Scalar(0, 255, 0), 1);
		}
	}*/

	// Identify the Speed Limits in the submatrixes
	for (int i = 0; i < shapeVect.size(); i++) {
		ShapeInfo sInfo = shapeVect.at(i);

		if (sInfo.first == SHAPE_RECTANGLE) {

			getShapeSubMatrix(bwFrame, subMat, &sInfo);
			detectSpeedLimit(subMat, destMat);
		}
	}

	showDebugImage("Output Image", destMat, cv::WINDOW_NORMAL);

	return;
}

/**
 *	DetectShapes 
 *
 *	Using the openCV function 'findContours' on passed in Black and White Image; the method will return the
 *	  shape (as an int) as well as the vector of points that make up that shape as a pair
 * 
 *	@param	inputMat	input matrix with Black and White Image
 *	@param	shapeVect	output shapeInfo vector
 */
void detectShapes(cv::Mat inputMat, std::vector<ShapeInfo>* shapeVect) {

	// Find all the Contors
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(inputMat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// Identify the Shapes
	std::vector<cv::Point> contour;
	std::vector<cv::Point> approxPoly;
	for (int i = 0; i < contours.size(); i++) {

		contour = contours.at(i);

		// Filter based on size
		int area = cv::contourArea(contour);
		if (area > min_contour_area && area < max_contour_area) {

			// Approximate the shape
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

			// Build the ShapeInfo outcome
			ShapeInfo shapeInfo = ShapeInfo(shape, approxPoly);
			shapeVect->push_back(shapeInfo);
		}
	}

	return;
}

/**
 *	Get Shape SubMatrix
 *
 *	@param	inputMat	input matrix
 *	@param	subMat		sub matrix output
 *	@param	shapeInfo	shape information
 */
void getShapeSubMatrix(cv::Mat inputMat, cv::Mat& subMat, ShapeInfo* shapeInfo) {

	int minC = inputMat.cols, maxC = 0, minR = inputMat.rows, maxR = 0;
	for (int j = 0; j < shapeInfo->second.size(); j++) {

		cv::Point p = shapeInfo->second.at(j); // (x, y) --> (column, row)

		if (p.x < minC) { minC = p.x; }
		if (p.x > maxC) { maxC = p.x; }
		if (p.y < minR) { minR = p.y; }
		if (p.y > maxR) { maxR = p.y; }
	}

	cv::Range rRange(minR, maxR);
	cv::Range cRange(minC, maxC);
	inputMat(rRange, cRange).copyTo(subMat);

	return;
}

/**
 *	Detect Speed Limit 
 * 
 *	TODO Documentation
 * 
 *	@param	inputMat	input matrix with Black and White Image of Sign
 */
void detectSpeedLimit(cv::Mat inputMat, cv::Mat& output) {

	// Resize the Image so we have a known size to work with
	cv::Mat resizedInput;

	double wFactor = IDEAL_SIGN_SIZE_PX / inputMat.cols; // width factor
	double hFactor = IDEAL_SIGN_SIZE_PX / inputMat.rows; // height factor

	if (wFactor > hFactor) {
		hFactor = wFactor;
	}
	else {
		wFactor = hFactor;
	}

	cv::resize(inputMat, resizedInput, cv::Size(), wFactor, hFactor);

	// Clean up the Image
	cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(resizedInput, resizedInput, cv::MORPH_CLOSE, se, cv::Point(-1, -1), 2);

	showDebugImage("Resized Image", resizedInput, cv::WINDOW_AUTOSIZE);

	cv::Mat speed = cv::Mat::zeros(resizedInput.size(), resizedInput.type());
	// Speed Limit Detection via Morphological Operations
	// TODO loop

	// Zero
	cv::Mat numZero;
	cv::morphologyEx(resizedInput, numZero, cv::MORPH_OPEN, SE_NUM_ZERO);

	showDebugImage("Zero Image", numZero, cv::WINDOW_AUTOSIZE);

	speed += numZero;

	// Four
	cv::Mat numFour;
	cv::morphologyEx(resizedInput, numFour, cv::MORPH_OPEN, SE_NUM_FOUR);

	showDebugImage("Four Image", numFour, cv::WINDOW_AUTOSIZE);

	speed += numFour;

	showDebugImage("Speed Limit Number Image", speed, cv::WINDOW_AUTOSIZE);

	return;
}