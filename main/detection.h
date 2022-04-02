#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>


/* Type Definitions */

/**
 *	Shape Info : 
 *		First Element is the Shape (as an int)
 *		Second Element is the vector of points that make up that shape
 */
typedef std::pair<int, std::vector<cv::Point>> ShapeInfo;


/* Shape Constants */

// Undefined Shape
const int SHAPE_UNDEFINED = 0;
// Triangle
const int SHAPE_TRIANGLE = 3;
// Rectangle
const int SHAPE_RECTANGLE = 4;
// Octagon
const int SHAPE_OCTAGON = 8;


/* Function Prototypes */

/**
 *	Process Frame
 *
 *	TODO add more info, change the signature as needed.
 *	TODO clone information to header file
 */
void processFrame(cv::Mat inputMat, cv::Mat& destMat, int frameNum = 0);