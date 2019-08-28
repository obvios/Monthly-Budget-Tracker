#include <iostream>
#include <opencv2/opencv.hpp>

//function prototypes
void resizeFrame(cv::Mat &);
std::vector<std::vector<cv::Point>> findLargestQuad(std::vector<std::vector<cv::Point>>);

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
		resizeFrame(frame);

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
		cv::imshow("edges resized", edges);
		
		//exit when user presses quit
		if (cv::waitKey(1) == 'q') break;
	}//exit main loop

	return 0;
}

void resizeFrame(cv::Mat &oFrame) {
	//calculate scale factor
	double dx = 700.0 / oFrame.cols;
	double dy = 700.0 / oFrame.rows;

	//apply new dimensions
	cv::resize(oFrame, oFrame, cv::Size(), dx, dy);
}