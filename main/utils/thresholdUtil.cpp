#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>


// --------------------------
// Threshold Utility Code
// Copied from the chessboard_example.cpp code given in class
// --------------------------


// compile command:
// cl /EHsc thresholdUtil.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib


// Input files
const char* input = "../debugPrintImage.jpg";

// Mats
cv::Mat originalImg, greyImg, threshImg;
cv::Mat blackSquares, whiteSquares;
cv::Mat finishedImg;

// Thresholds

/**
 * range: 0-255
 */
int thresholdValue = 0;

/**
 * 0: binary
 * 1: binary-inverted
 * 2: truncate
 * 3: to-zero
 * 4: to-zero-inverted
 */ 
int thresholdType = 0;

int const MAX_TYPE = 4;
int const MAX_BINARY = 255;


// Window Names
const char* colorWindow = "Color Image";
const char* greyWindow = "Grey Image";
const char* threshWindow = "Threshold Image";
const char* whiteSquareWindow = "White Squares";
const char* blackSquareWindow = "Black Squares";

// Function Prototypes
void Threshold_Demo(int, void*);
void Colorize_Demo(int, void*);

int main() {
	/**
	 * Type: 
	 * CV_LOAD_IMAGE_UNCHANGED: include transparency (alpha) channel
	 * CV_LOAD_IMAGE_GRAYSCALE: load as single-channel
	 * CV_LOAD_IMAGE_COLOR: load as BGR
	 */
	originalImg = cv::imread(input, cv::IMREAD_COLOR);
	
	// ensure image was valid
	if(!originalImg.data) {
		std::cout << "Missing data." << std::endl;
		return -1;
	}
	
	// display the image
	cv::namedWindow(colorWindow, cv::WINDOW_AUTOSIZE);
	cv::imshow(colorWindow, originalImg);
	
	// wait for user to press any key
	cv::waitKey(0);
	
	// greyscale the image
	cv::cvtColor(originalImg, greyImg, cv::COLOR_BGR2GRAY);
	
	// display the grey image
	cv::namedWindow(greyWindow, cv::WINDOW_AUTOSIZE);
	cv::imshow(greyWindow, greyImg);
	
	// wait for user to press any key
	cv::waitKey(0);
	
	// close the windows
	//cvDestroyWindow(colorWindow);
	cv::destroyWindow(greyWindow);
	
	cv::namedWindow(threshWindow, cv::WINDOW_NORMAL);
	
	// create type trackbar
	cv::createTrackbar("Type ", threshWindow, &thresholdType,
					MAX_TYPE, Threshold_Demo);
					
	// create value trackbar
	cv::createTrackbar("Value ", threshWindow, &thresholdValue,
					MAX_BINARY, Threshold_Demo);
					
	Threshold_Demo(0, 0);
	
	cv::waitKey(0);
	
	cv::destroyWindow(threshWindow);

	if(thresholdType == 1) {
		blackSquares = threshImg.clone();
		cv::bitwise_not(blackSquares, whiteSquares);
	}
	else if(thresholdType == 0) {
		whiteSquares = threshImg.clone();
		cv::bitwise_not(whiteSquares, blackSquares);
	}
	else {
		cv::destroyWindow(colorWindow);
		return 0;
	}
	
	
	cv::namedWindow(whiteSquareWindow, cv::WINDOW_AUTOSIZE);
	cv::imshow(whiteSquareWindow, blackSquares);
	cv::waitKey(0);
	
	cv::destroyWindow(whiteSquareWindow);
	
	cv::namedWindow(blackSquareWindow, cv::WINDOW_AUTOSIZE);
	cv::imshow(blackSquareWindow, whiteSquares);
	cv::waitKey(0);
	
	cv::destroyWindow(blackSquareWindow);
	cv::destroyWindow(colorWindow);
	return 1;
}

void Threshold_Demo(int, void*) {
	
	// (src, dst, threshold value, max, type of threshold)
	cv::threshold(greyImg, threshImg, thresholdValue,
				MAX_BINARY, thresholdType);

	cv::imshow(threshWindow, threshImg);
}