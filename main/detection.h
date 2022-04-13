#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

/* Struct Definitions */
struct Shape_Info_s {
	int shape;
	std::vector<cv::Point> contour;
};

struct Sign_Info_s {
	int shape;
	int sign;
	std::vector<cv::Point> contour;
	int intValue;
	std::string stringValue;
};

/* Type Definitions */

/**
 *	Shape Info : 
 *		shape (as an int)
 *		contour: Vector of points that make up that shape
 */
typedef struct Shape_Info_s ShapeInfo;

/**
 *	Sign Info :
 *		shape (as an int)
 *		sign (as an int)
 *		contour: Vector of points that make up that shape
 *		intValue: Value of the Sign as an Integer
 *		stringValue: Value of the Sign as a String
 */
typedef struct Sign_Info_s SignInfo;

/* Shape Constants */

// Undefined Shape
const int SHAPE_UNDEFINED = 0;
// Triangle
const int SHAPE_TRIANGLE = 3;
// Rectangle
const int SHAPE_RECTANGLE = 4;
// Octagon
const int SHAPE_OCTAGON = 8;

/* Sign Constants */

// Undefined Sign
const int SIGN_UNDEFINED = 0;
// Speed Limit
const int SIGN_SPEEDLIMIT = 1;


/* Function Prototypes */

// TODO process image input (call into processFrame once)
// void processInput(cv::Mat inputMat)

// TODO process video/camera input (call into processFrame multiple times)
// void processInput(cv::VideoCapture inputCap)

/**
 *	Process Frame
 *
 *	TODO add more info, change the signature as needed.
 *	TODO clone information to header file
 */
void processFrame(cv::Mat inputMat, cv::Mat& destMat, int frameNum = 0);

/**
 *	DetectShapes
 *
 *	Using the openCV function 'findContours' on passed in Black and White Image; the method will return the
 *	  shape (as an int) as well as the vector of points that make up that shape as a pair
 *
 *	@param	inputMat	input matrix with Black and White Image
 *	@param	shapeVect	output shapeInfo vector
 */
void detectShapes(cv::Mat inputMat, std::vector<ShapeInfo>* shapeVect);

/**
 *	Detect Speed Limit
 *
 *	TODO Documentation
 *
 *	@param	inputMat	input matrix with Black and White Image of Sign
 *  @param	signInfo	output sign information object
 */
void detectSpeedLimit(cv::Mat inputMat, SignInfo* signInfo);

/**
 *	Get Shape SubMatrix
 *
 *	@param	inputMat	input matrix
 *	@param	subMat		sub matrix output
 *	@param	shapeInfo	shape information
 */
void getShapeSubMatrix(cv::Mat inputMat, cv::Mat& subMat, ShapeInfo* shapeInfo);

/**
 *	Converts the given SignInfo Object to a ShapeInfo Object
 * 
 *	@param	obj			SignInfo Object to get ShapeInfo from
 *	@param	shapeInfo	ShapeInfo Object output
 */
void getShapeInfo(SignInfo* obj, ShapeInfo* shapeInfo);