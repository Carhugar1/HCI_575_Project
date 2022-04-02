#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

typedef std::pair<int, std::vector<cv::Point>> ShapeInfo;

// Shape Constants
const int SHAPE_UNDEFINED = 0;
const int SHAPE_TRIANGLE = 3;
const int SHAPE_RECTANGLE = 4;
const int SHAPE_OCTAGON = 8;

// Function Prototypes

void processFrame(cv::Mat inputMat, cv::Mat& destMat, int frameNum = 0);