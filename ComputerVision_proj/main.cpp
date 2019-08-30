#include <iostream>
#include <opencv2/opencv.hpp>

//function prototypes
void resizeFrame(cv::Mat &);
std::vector<std::vector<cv::Point>> findLargestQuad(std::vector<std::vector<cv::Point>>&);

/*************** MAIN **********************/
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

		//find quadrilateral contours. Example: contours[0][0].x
		std::vector<std::vector<cv::Point>> squareCon = findLargestQuad(contours);
		
		//draw contours only if contours vector is not empty
		if (squareCon.size() != 0) {
			cv::Scalar color(0, 255, 0);
			cv::drawContours(frame, squareCon, 0, color, 2);
		}

		//display frame (w/ contours if contours were found)
		cv::imshow("frame", frame);
		
		//exit main loop when user presses quit
		if (cv::waitKey(1) == 'q') break;
	}//exit main loop

	return 0;
}/***************END MAIN**************/

// Resizes the input frame to 700x700
void resizeFrame(cv::Mat &oFrame) {
	//calculate scale factor
	double dx = 700.0 / oFrame.cols;
	double dy = 700.0 / oFrame.rows;

	//apply new dimensions
	cv::resize(oFrame, oFrame, cv::Size(), dx, dy);
}

//sorts contours by area in descending order.
//takes sorted contours and finds the largest quadrilateral
std::vector<std::vector<cv::Point>> findLargestQuad(std::vector<std::vector<cv::Point>>& conts) {
	std::vector<std::vector<cv::Point>> returnVect;

	//sort contours by area
	std::sort(conts.begin(), conts.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
		return cv::contourArea(c1, false) > cv::contourArea(c2, false);
	});

	//find quadrilateral
	for (std::vector<cv::Point> cont : conts) {
		std::vector<cv::Point> approx;
		cv::approxPolyDP(cont, approx, (.01 * cv::arcLength(cont, true)), true);

		if (approx.size() == 4) {
			returnVect.push_back(approx);
			return returnVect;
		}
	}

	//nothing was found if we reach this far
	return returnVect;
}