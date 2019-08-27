#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char * argv[]) {
	//open camera
	cv::VideoCapture camera(0);

	//check if camera was opened
	if (!camera.isOpened()) {
		return -1;
	}

	//main loop
	for (;;) {
		cv::Mat frame;
		cv::Mat filteredFrame;
		cv::Mat edges;
		std::vector<std::vector<cv::Point>> contours;

		//get new frame from camera
		camera >> frame;

		//resize frame

		//apply bilateral filter
		cv::bilateralFilter(frame, filteredFrame, 9, 75, 75);

		//grayscale
		cv::cvtColor(filteredFrame, filteredFrame, cv::COLOR_BGR2GRAY);

		//perform canny edge detection
		cv::Canny(filteredFrame, edges, 100, 200);

		//find contours
		cv::findContours(edges, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

		//find quadrilateral contours

		//draw contours only if contours vector is not empty

		//display frame (w/ contours if contours were found)

		//display
		
		//exit when user presses quit
		if (cv::waitKey(1) == 'q') break;
	}//exit main loop

	return 0;
}