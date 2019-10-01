#include <iostream>
#include <opencv2/opencv.hpp>
#include "Tess/InformationExtractor.h"
#include "BudgetDocument.h"


//function prototypes
void resizeFrame(cv::Mat &);
std::vector<std::vector<cv::Point>> findLargestQuad(std::vector<std::vector<cv::Point>>&);
std::vector<cv::Point> sortBy_Xcoordinate(std::vector<std::vector<cv::Point>>&);
void sortCoordinates(std::vector<cv::Point>&);

/*************** MAIN **********************/
int main(int argc, char * argv[]) {
	///////////testtttttttt
	BudgetDocument bd;
	//open camera
	cv::VideoCapture camera(0);

	//check if camera was opened
	if (!camera.isOpened()) {
		return -1;
	}

	//holds selected quadrilateral contour
	std::vector<std::vector<cv::Point>> finalContour;
	//holds last frame
	cv::Mat frameCopy;
	//tesseract
	InformationExtractor infoExtractor;
	infoExtractor.Init("Tess/tessdata", "eng");

	/*main loop*/
	//this whole loop may potentially be replaced by IOS VISION
	while (true) {
		cv::Mat frame;
		cv::Mat filteredFrame;
		cv::Mat edges;
		std::vector<std::vector<cv::Point>> contours;

		//get new frame from camera
		camera >> frame;
		
		//resize frame
		resizeFrame(frame);
		
		//apply bilateral filter
		cv::bilateralFilter(frame, filteredFrame, 5, 75, 75);
		
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
		if (cv::waitKey(1) == 'q') {
			if (squareCon.size() != 0) { finalContour = squareCon; }
			//copy frame
			frame.copyTo(frameCopy);
			break;
		}
	}/*exit main loop*/
	
	/*only performs projection and OCR if contours were found*/
	if (finalContour.size() != 0) {
		//sort corners of bounding box
		std::vector<cv::Point> sortedPoints = sortBy_Xcoordinate(finalContour);
		sortCoordinates(sortedPoints);

		//create source points
		std::vector<cv::Point2f> src_pts = {sortedPoints[0], sortedPoints[1], sortedPoints[2], sortedPoints[3]};

		//create destination points
		std::vector<cv::Point2f> dst_pts = {cv::Point2f(0.0,0.0), cv::Point2f(0.0,700.0), cv::Point2f(700.0,700.0), cv::Point2f(700.0,0.0) };

		//find homography matrix
		cv::Mat hom = cv::findHomography(src_pts, dst_pts);

		//perform warpPerspective
		cv::Mat finalImage;
		cv::warpPerspective(frameCopy, finalImage, hom, cv::Size( cvRound(dst_pts[3].x), cvRound(dst_pts[3].y) ) );

		//process final image for better ocr recognition
		cv::cvtColor(finalImage, finalImage, cv::COLOR_BGR2GRAY);
		cv::threshold(finalImage, finalImage, 150, 255, cv::THRESH_BINARY);

		//display final image
		cv::imshow("final image", finalImage);
		cv::waitKey(0);


		//process image with tesseract
		infoExtractor.ExtractTotal((uchar*)finalImage.data, finalImage.cols, finalImage.rows, 1, finalImage.cols);

		//pause
		std::cin.get();
	}
	
	//release memory
	camera.release();
	cv::destroyAllWindows();

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
	int i = 0;
	while (i < 5 && i < conts.size()){
		std::vector<cv::Point> approx;
		cv::approxPolyDP(conts[i], approx, (.01 * cv::arcLength(conts[i], true)), true);

		if (approx.size() == 4) {
			returnVect.push_back(approx);
			return returnVect;
		}

		i++;
	}

	//nothing was found if we reach this far
	return returnVect;
}

//sorts an arraOfarray holding Points by x-coordinates
//returns an array of Points sorted by x-coordinates
std::vector<cv::Point> sortBy_Xcoordinate(std::vector<std::vector<cv::Point>>& contArrayOfArray) {
	std::vector<cv::Point> contArray = { cv::Point(0,0), cv::Point(0,0), cv::Point(0,0), cv::Point(0,0) };

	for (int i = 0; i < 4; i++) {
		cv::Point point = contArrayOfArray[0][i]; 
		contArray[i] = point;
	}

	//sort by x-coordinate
	std::sort(contArray.begin(), contArray.end(), [](const cv::Point& p1, const cv::Point& p2) {
		return p1.x < p2.x;
	});

	return contArray;
}

//sort points in following order: top-left, bottom-left, bottom-right, top-right
//parameter must be sorted by x-coordinate value
void sortCoordinates(std::vector<cv::Point>& x_sortedCont) {
	int y1 = x_sortedCont[0].y;
	int y2 = x_sortedCont[1].y;

	if (y2 < y1) {
		//swap
		cv::Point temp(x_sortedCont[0].x, x_sortedCont[0].y);	//copy first point
		x_sortedCont[0] = x_sortedCont[1];
		x_sortedCont[1] = temp;
	}

	int y3 = x_sortedCont[2].y;
	int y4 = x_sortedCont[3].y;

	if (y4 > y3) {
		cv::Point temp(x_sortedCont[2].x, x_sortedCont[2].y);	//copy third point
		x_sortedCont[2] = x_sortedCont[3];
		x_sortedCont[3] = temp;
	}
}