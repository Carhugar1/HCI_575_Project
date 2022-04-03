#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>


// --------------------------
// Structuring Element Utility Code
// Author: Branden Sammons
// --------------------------


// compile command:
// cl /EHsc structuringElementUtil.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib

// execute command:
// structuringElementUtil.exe


// Input files
const char* inputfile = "../temp/0.jpg";
const char* outputfile = "../temp/seUtilImage0.jpg";
const char* matfile = "../temp/seUtilMat0.txt";

// Mats
cv::Mat originalImg;
cv::Mat greyImg;
cv::Mat bwImg;
cv::Mat thinMat;
cv::Mat seMat;

// Function prototypes
void thin(cv::Mat inputMat, cv::Mat& destMat);
void reduceMat(cv::Mat inputMat, cv::Mat& destMat);
void writeMatToFile(const std::string& filename, cv::Mat inputMat);


int main() {
	
	originalImg = cv::imread(inputfile, cv::IMREAD_COLOR);

	// ensure image was valid
	if (!originalImg.data) {
		std::cout << "Missing data." << std::endl;
		return -1;
	}
	
	// greyscale the image
	cv::cvtColor(originalImg, greyImg, cv::COLOR_BGR2GRAY);

	// threshold the image (convert to binary)
	cv::threshold(greyImg, bwImg, 100, 1, cv::THRESH_BINARY);

	cv::namedWindow("BW Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("BW Image", (bwImg * 255));
	cv::waitKey(0);

	// Thin the image
	thin(bwImg, thinMat);

	// remove empty rows and columns
	reduceMat(thinMat, seMat);

	cv::namedWindow("Thinned Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Thinned Image", (seMat * 255));
	cv::waitKey(0);

	cv::imwrite(outputfile, (seMat * 255));
	writeMatToFile(matfile, seMat);
	
	// Clean up
	cv::destroyAllWindows();
	return 0;
}

// https://theailearner.com/tag/thinning-opencv/
// AND
// https://en.wikipedia.org/wiki/Morphological_skeleton
void thin(cv::Mat inputMat, cv::Mat& destMat) {

	cv::Mat img;
	inputMat.copyTo(img);

	// Structuring Element
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

	// Empty Output Img
	cv::Mat thin = cv::Mat::zeros(inputMat.size(), CV_8UC1);

	bool done = false;
	while (!done) {

		// Erode
		cv::Mat erode;
		cv::erode(img, erode, element);

		// Opening
		cv::Mat opening;
		cv::morphologyEx(erode, opening, cv::MORPH_OPEN, element);

		// Subtraction
		cv::Mat subset;
		subset = erode - opening;

		// Union
		cv::bitwise_or(subset, thin, thin);

		// Set variables for next iteration
		erode.copyTo(img);
		done = cv::countNonZero(img) == 0;
	}

	thin.copyTo(destMat);
	return;
}

// TODO Documentation
void reduceMat(cv::Mat inputMat, cv::Mat& destMat) {

	// get the horizontal and vertical projections
	cv::Mat horizontalProjection, verticalProjection;
	cv::reduce(inputMat, horizontalProjection, 0, cv::REDUCE_SUM, CV_32F);
	cv::reduce(inputMat, verticalProjection, 1, cv::REDUCE_SUM, CV_32F);

	// Get the start and end of the symbol in both axis
	int minX = 0, maxX = horizontalProjection.cols - 1;
	for (int i = 0; i < horizontalProjection.cols; i++) {
		if (horizontalProjection.at<uint>(0, i) > 0) {
			minX = i;
			break;
		}
	}
	for (int i = horizontalProjection.cols - 1; i >= 0; i--) {
		if (horizontalProjection.at<uint>(0, i) > 0) {
			maxX = i;
			break;
		}
	}

	int minY = 0, maxY = verticalProjection.rows - 1;
	for (int i = 0; i < verticalProjection.rows; i++) {
		if (verticalProjection.at<uint>(i, 0) > 0) {
			minY = i;
			break;
		}
	}
	for (int i = verticalProjection.rows - 1; i >= 0; i--) {
		if (verticalProjection.at<uint>(i, 0) > 0) {
			maxY = i;
			break;
		}
	}

	// copy in the reduced Mat
	cv::Range rRange(minY, maxY + 1);
	cv::Range cRange(minX, maxX + 1);
	
	inputMat(rRange, cRange).copyTo(destMat);
	return;
}

// TODO Documentation
void writeMatToFile(const std::string& filename, cv::Mat inputMat) {

	//uint8_t data[size] = {...};
	//cv::Mat Identifier = cv::Mat(rows, cols, CV_8UC1, data);

	// Create and Open file
	std::ofstream file(filename);

	int rows = inputMat.rows;
	int cols = inputMat.cols;
	int size = inputMat.rows * inputMat.cols;

	file << "uint8_t <Identifier>Data[" << size << "] = {" << '\n';

	// Write to file
	for (int r = 0; r < rows; r++) {

		file << '\t'; // insert tab at start of row

		for (int c = 0; c < cols; c++) {
			
			file << (int)(inputMat.at<uint8_t>(r, c));

			// not the last row and last column
			if (!(r == rows - 1 && c == cols - 1)) {
				file << ", ";
			}
		}

		file << '\n'; // insert newline at end of row
	}

	file << "};" << '\n';
	file << "cv::Mat <Identifier> = cv::Mat(" << inputMat.rows << ", " << inputMat.cols << ", CV_8UC1, <Identifier>Data);" << '\n';

	// Close the File
	file.close();
	return;
}