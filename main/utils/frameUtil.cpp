#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

// --------------------------
// Author: Branden Sammons
// --------------------------


// compile command:
// cl /EHsc frameUtil.cpp /I %OpenCV_Path%\build\include /link /LIBPATH:%OpenCV_Path%\build\x64\vc15\lib opencv_world455.lib

// execute command:
// frameUtil.exe


// Input
const std::string inputVideoLocation = "../../test/resources/Videos/";
const std::string inputVideo = "20220312_105508.mp4";

// Output
const std::string outputImageLocation = "../../test/resources/Images/";

// VideoCapture
cv::VideoCapture inputCap;

// WindowName
const cv::String windowName = "Frame";

// Constants
const char SAVE_FRAME_KEY = 's';
const char QUIT_KEY = 'q';

// some shared globals
cv::Mat inputFrame;
int frameNum;

// Function Prototypes
void onChange(int, void*);


int main() {

	inputCap = cv::VideoCapture(inputVideoLocation + inputVideo);

	// check validity of target file
	if (!inputCap.isOpened()) {
		std::cout << "Input video not found." << std::endl;
		return -1;
	}

	int keyPressed;

	cv::namedWindow(windowName, cv::WINDOW_NORMAL);
	cv::String outputImg;

	std::cout << std::endl;
	std::cout << "Press the '" << SAVE_FRAME_KEY << "' key to save the current Frame" << std::endl;
	std::cout << "Press the '" << QUIT_KEY << "' key to quit" << std::endl;

	// create frame trackbar
	cv::createTrackbar("Frame ", windowName, NULL, inputCap.get(cv::CAP_PROP_FRAME_COUNT), onChange);

	onChange(0, NULL);

	while (true) {
		
		keyPressed = cv::waitKey(0);

		if (keyPressed == SAVE_FRAME_KEY) {

			outputImg = inputVideo + "_frame" + std::to_string(frameNum) + ".jpg";
			cv::imwrite(outputImageLocation + outputImg, inputFrame);
		} 
		else if (keyPressed == QUIT_KEY) {
			break;
		}
	}

	// free the capture objects from memory
	inputCap.release();

	// Clean up
	cv::destroyAllWindows();
	return 0;
}

void onChange(int num, void*) {

	frameNum = num;

	// set the next frame to the one selected by the track bar
	inputCap.set(cv::CAP_PROP_POS_FRAMES, frameNum);
	inputCap.read(inputFrame);

	cv::imshow(windowName, inputFrame);
}