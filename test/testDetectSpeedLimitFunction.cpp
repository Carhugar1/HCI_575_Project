#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "../main/detection.h"
#include "../main/utils/debugUtil.h"

#include "testResult.h"

// --------------------------
// Test detectSpeedLimit Function
// Author: Branden Sammons
// --------------------------


// Input file
std::string resourcePath("./resources/Images/speed_limit_signs/individual_signs/");

// Globals
cv::Mat originalImg;
cv::Mat bwImg;
SignInfo signInfo;

// Test function prototypes
void testKnownSpeedLimit(VecTestResult* testResults, const std::string& testCaseName, std::string filename, int expectedValue);

// private function prototypes
void loadFromFile(cv::Mat& destMat, std::string filePath);


/**
 *	Runs the Tests for the detectSpeedLimit function
 */
void runDetectSpeedLimitFunctionTests(VecTestResult* testResults) {

	// Test 15mph
	testKnownSpeedLimit(testResults, "15mph Test", "15.jpg", 15);

	testKnownSpeedLimit(testResults, "20mph Test", "20.jpg", 20);

	testKnownSpeedLimit(testResults, "25mph Test", "25.jpg", 25);

	testKnownSpeedLimit(testResults, "30mph Test", "30.jpg", 30);

	testKnownSpeedLimit(testResults, "35mph Test", "35.jpg", 35);

	testKnownSpeedLimit(testResults, "40mph Test", "40.jpg", 40);

	testKnownSpeedLimit(testResults, "45mph Test", "45.jpg", 45);

	testKnownSpeedLimit(testResults, "50mph Test", "50.jpg", 50);

	testKnownSpeedLimit(testResults, "55mph Test", "55.jpg", 55);

	testKnownSpeedLimit(testResults, "60mph Test", "60.jpg", 60);

	testKnownSpeedLimit(testResults, "65mph Test", "65.jpg", 65);

	testKnownSpeedLimit(testResults, "70mph Test", "70.jpg", 70);

	testKnownSpeedLimit(testResults, "75mph Test", "75.jpg", 75);

	testKnownSpeedLimit(testResults, "80mph Test", "80.jpg", 80);

	testKnownSpeedLimit(testResults, "85mph Test", "85.jpg", 85);

}

/*** TESTS ***/
void testKnownSpeedLimit(VecTestResult* testResults, const std::string& testCaseName, std::string filename, int expectedValue) {

	TestResult test;
	test.testName = testCaseName;

	// reset signInfo
	signInfo.sign = SIGN_UNDEFINED;
	signInfo.intValue = 0;
	signInfo.stringValue = "";

	// Setup
	loadFromFile(originalImg, resourcePath + filename);
	cv::threshold(originalImg, bwImg, 50, 255, cv::THRESH_BINARY);

	// Run the Test
	detectSpeedLimit(bwImg, &signInfo);

	// Verify Results
	if (signInfo.sign != SIGN_SPEEDLIMIT) {
		test.result = false;
		test.errorMessage = "Not detected as a Speed Limit Sign";
		testResults->push_back(test);
		return;
	}

	if (signInfo.intValue != expectedValue) {
		test.result = false;
		test.errorMessage = "Expected Speed: " + std::to_string(expectedValue) + ", Actual: " + std::to_string(signInfo.intValue);
		testResults->push_back(test);
		return;
	}

	test.result = true;
	testResults->push_back(test);
	return;
}

/*** Private Helper Functions ***/
// Might make sence to move this to a TestUtil Class
void loadFromFile(cv::Mat& destMat, std::string filePath) {

	destMat = cv::imread(filePath, cv::IMREAD_GRAYSCALE);

	// ensure image was valid
	if (!destMat.data) {
		std::cout << "Missing data." << std::endl;
	}

	return;
}