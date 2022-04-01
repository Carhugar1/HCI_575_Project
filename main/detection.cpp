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


// Globals for the processFrame function
cv::Mat grayFrame;
cv::Mat edges;


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

	if (DEBUG_MODE) {
		cv::namedWindow("Input Image", cv::WINDOW_NORMAL);
		cv::imshow("Input Image", originalImg);
		cv::waitKey(0);
	}

	processFrame(originalImg, outputImg, 0);

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
	
	cv::cvtColor(inputMat, grayFrame, cv::COLOR_BGR2GRAY);

	// Edge Detection
	cv::Canny(grayFrame, edges, 200, 255);

	if (DEBUG_MODE) {
		cv::namedWindow("Edges Image", cv::WINDOW_NORMAL);
		cv::imshow("Edges Image", edges);
		cv::waitKey(0);
	}

	// Hough Lines
	std::vector<cv::Vec2f> lines; // will hold the results of the detection
	cv::HoughLines(edges, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection


	inputMat.copyTo(destMat);

	// Draw the lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;

		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));

		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		cv::line(destMat, pt1, pt2, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
	}

	if (DEBUG_MODE) {
		cv::namedWindow("Output Image", cv::WINDOW_NORMAL);
		cv::imshow("Output Image", destMat);
		cv::waitKey(0);
	}

	return;
}