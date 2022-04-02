#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

/**
 *	Helper Method to show a window with the given windowName and inputMat when DEBUG_MODE is true.
 */
void showDebugImage(std::string windowName, cv::Mat inputMat);

/**
 *	Helper Method to print the image from the given inputMat when DEBUG_MODE equals true.
 */
void printDebugImage(cv::Mat inputMat);

/**
 *	Helper Method to print the given debugLine to the console when DEBUG_MODE equals true.
 */
void printDebugLine(std::string debugLine);