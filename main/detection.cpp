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

// Constants
const float MIN_CONTOUR_AREA_PERCENTAGE = 0.001;
const float MAX_CONTOUR_AREA_PERCENTAGE = 0.05;
const int IDEAL_SIGN_SIZE_PX = 250;
const int SIGN_OUTSIDE_OFFSET_PX = 50;
const int SIGN_MIDDLE_OFFSET_PX = 50;
const int SIGN_DETECTION_ORDER[] = { 1, 3, 0, 2, 4, 5, 6, 7, 8 };

// Text Constants
const int FONT_FACE = cv::FONT_HERSHEY_PLAIN;
const double FONT_SCALE = 1.5;
const int FONT_THICKNESS = 2;
const int TEXT_OFFSET = 5;

// Globals for the processFrame function
int min_contour_area;
int max_contour_area;
cv::Mat grayFrame;
cv::Mat blurFrame;
cv::Mat bwFrame;
cv::Mat subMat;

// private function prototypes
void cloneShapeInfo(ShapeInfo* shapeInfo, SignInfo* obj);


/**
 *	Process Frame
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

	// Identify the Speed Limits in the submatrixes
	SignInfo signInfo;
	for (int i = 0; i < shapeVect.size(); i++) {
		ShapeInfo shapeInfo = shapeVect.at(i);

		cloneShapeInfo(&shapeInfo, &signInfo);
		signInfo.sign = SIGN_UNDEFINED;

		if (shapeInfo.shape == SHAPE_RECTANGLE) {

			getShapeSubMatrix(bwFrame, subMat, &shapeInfo);
			detectSpeedLimit(subMat, &signInfo);

			if (signInfo.sign == SIGN_SPEEDLIMIT) {
				cv::String text = "Speed Limit: " + signInfo.stringValue;

				printDebugLine(text);

				cv::Rect rect = cv::boundingRect(signInfo.contour);

				// Add the Text
				int baseline = 0;
				cv::Size textSize = cv::getTextSize(text, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &baseline);
				cv::Point textPoint(rect.x, rect.y - baseline);
				cv::putText(destMat, text, textPoint, FONT_FACE, FONT_SCALE, cv::Scalar(0, 255, 0), FONT_THICKNESS);

				// Draw the Identified Rectangle
				cv::rectangle(destMat, rect, cv::Scalar(0, 255, 0), 1);
			}
			else {
				// Draw the Identified Rectangle
				cv::polylines(destMat, signInfo.contour, true, cv::Scalar(0, 255, 0), 1);
			}
		}
	}

	showDebugImage("Output Image", destMat, cv::WINDOW_NORMAL);

	return;
}

/**
 *	DetectShapes
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
			ShapeInfo shapeInfo;
			shapeInfo.shape = shape;
			shapeInfo.contour = approxPoly;

			shapeVect->push_back(shapeInfo);
		}
	}

	return;
}

/**
 *	Detect Speed Limit
 */
void detectSpeedLimit(cv::Mat inputMat, SignInfo* signInfo) {

	// Resize the Image so we have a known size to work with
	cv::Mat resizedInput;

	// Calculate resize factors (cast to double to force floating point division)
	double wFactor = static_cast<double>(IDEAL_SIGN_SIZE_PX) / inputMat.cols; // width factor
	double hFactor = static_cast<double>(IDEAL_SIGN_SIZE_PX) / inputMat.rows; // height factor

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
	cv::Mat rotatedSE;
	cv::Mat horizontalProjection, left, right;
	int cols = 0;
	uint8_t tensDigitPlaceValue = 0; 
	uint8_t onesDigitPlaceValue = 0;
	int effectiveNum = 0;
	// Speed Limit Detection via Morphological Operations
	// TODO handle 9 as well (handles 0 - 8 at the moment)
	for (int i = 0; i < 9; i++) {
		
		effectiveNum = SIGN_DETECTION_ORDER[i];

		cv::Mat numMat;
		cv::morphologyEx(resizedInput, numMat, cv::MORPH_ERODE, SE_NUM_ARR[effectiveNum]);

		// If the Result has a non 0 in it we have a match
		if (cv::countNonZero(numMat) > 0) {
			bool found = false;
			
			cv::reduce(numMat, horizontalProjection, 0, cv::REDUCE_SUM, CV_32F);
			cols = horizontalProjection.cols;
			left = horizontalProjection(cv::Range::all(), cv::Range(SIGN_OUTSIDE_OFFSET_PX, (cols / 2 - SIGN_MIDDLE_OFFSET_PX)));
			right = horizontalProjection(cv::Range::all(), cv::Range((cols / 2 + SIGN_MIDDLE_OFFSET_PX), cols - SIGN_OUTSIDE_OFFSET_PX));

			if (cv::countNonZero(left) > 0) {
				tensDigitPlaceValue = effectiveNum;
				found = true;
				//printDebugLine("left count for number (" + std::to_string(effectiveNum) + ") = " + std::to_string(cv::countNonZero(left)));
			}
			if (cv::countNonZero(right) > 0) {
				onesDigitPlaceValue = effectiveNum;
				found = true;
				//printDebugLine("right count for number (" + std::to_string(effectiveNum) + ") = " + std::to_string(cv::countNonZero(right)));
			}

			// Add it to the Output Mat?
			if (found) {
				cv::rotate(SE_NUM_ARR[effectiveNum], rotatedSE, cv::ROTATE_180);
				cv::morphologyEx(numMat, numMat, cv::MORPH_DILATE, rotatedSE);
				speed += numMat;
				//showDebugImage("Number Mat", numMat, cv::WINDOW_AUTOSIZE);
			}
		}
	}

	if (tensDigitPlaceValue != 0 || onesDigitPlaceValue != 0) {
		signInfo->sign = SIGN_SPEEDLIMIT;
		signInfo->intValue = (tensDigitPlaceValue * 10) + onesDigitPlaceValue;
		signInfo->stringValue = std::to_string(signInfo->intValue);
	}

	showDebugImage("Speed Limit Number Image", speed, cv::WINDOW_AUTOSIZE);

	return;
}

/**
 *	Get Shape SubMatrix
 */
void getShapeSubMatrix(cv::Mat inputMat, cv::Mat& subMat, ShapeInfo* shapeInfo) {

	int minC = inputMat.cols, maxC = 0, minR = inputMat.rows, maxR = 0;
	for (int j = 0; j < shapeInfo->contour.size(); j++) {

		cv::Point p = shapeInfo->contour.at(j); // (x, y) --> (column, row)

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
 *	Converts the given SignInfo Object to a ShapeInfo Object
 */
void getShapeInfo(SignInfo* obj, ShapeInfo* shapeInfo) {
	
	shapeInfo->shape = obj->shape;
	shapeInfo->contour = obj->contour;
}

/**
 *	Copies the given ShapeInfo Object into a SignInfo Object
 */
void cloneShapeInfo(ShapeInfo* shapeInfo, SignInfo* obj) {

	obj->shape = shapeInfo->shape;
	obj->contour = shapeInfo->contour;
}