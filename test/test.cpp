#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "../main/detection.h"
#include "../main/utils/debugUtil.h"

/** Testing Headers **/
#include "testResult.h"

/** Test Case Headers **/
#include "testDetectSpeedLimitFunction.h"

// --------------------------
// Test Runner
//		HCI 575 Project
//		Team 5
//		Spring 2022
// --------------------------


const bool suppressPassing = true;


// compile command:
// cl /EHsc /Fe:test test*.cpp ../main/detection.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib ../main/utils/debugUtil.obj

// execute command:
// test.exe


// globals
VecTestResult testResults;

// private function prototypes
void printTestResults(VecTestResult* testResults, const std::string& testFilename);


int main() {

	// Detect Speed Limit Function Testing
	testResults.clear();
	runDetectSpeedLimitFunctionTests(&testResults);
	printTestResults(&testResults, "runDetectSpeedLimitFunctionTests");
	
	// Clean up any windows
	cv::destroyAllWindows();
	return 0;
}

/**
 *	Print Test Results : 
 *		Prints out the given Test Results
 */
void printTestResults(VecTestResult* testResults, const std::string& testFilename) {
	
	int passedNum = 0, failedNum = 0;

	// Print Filename
	std::cout << std::endl;
	std::cout << testFilename << ": " << std::endl;

	// Print Test Results
	for (int i = 0; i < testResults->size(); i++) {
		TestResult test = testResults->at(i);

		if (test.result) {
			passedNum++;
			if (!suppressPassing) {
				std::cout << "\t" << test.testName << ": Passed" << std::endl;
			}
		}
		else {
			failedNum++;
			std::cout << "\t" << test.testName << ": Failed\n\t\t" << test.errorMessage << std::endl;
		}
	}

	std::cout << std::endl;
	std::cout << "Total Ran: " << testResults->size() << ", ";
	std::cout << "Total Passing: " << passedNum << ", ";
	std::cout << "Total Failed: " << failedNum << std::endl;
}