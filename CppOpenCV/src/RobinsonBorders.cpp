/*
 * RobinsonBorders.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: marcelosalloum
 */

#include "RobinsonBorders.h"

void RobinsonBorders::erosion(Mat &src, Mat &out, string windowName, bool shouldShow) {
	int erosion_size = 1;
	Mat erosionKernel = getStructuringElement(MORPH_CROSS,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	erode(src, out, erosionKernel);
	if (shouldShow) {
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		imshow(windowName, out);
	}
}

void RobinsonBorders::dilation(Mat &src, Mat &out, string windowName, bool shouldShow) {
	int dilation_size = 1;
	Mat dilationKernel = getStructuringElement(MORPH_CROSS,
			Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size));
	dilate(src, out, dilationKernel);
	if (shouldShow) {
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		imshow(windowName, out);
	}
}

void RobinsonBorders::filter2DAndShow(Mat &src, Mat &out, Mat &kernel, string windowName) {
	filter2D(src, out, src.depth(), kernel);
//	erosion(out, out, "erosion", false);
//	erosion(out, out, "erosion", false);
//	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
//	imshow(windowName, out);
}

Mat RobinsonBorders::robinsonDetectBorder(Mat src) {
	Mat out, total, colorOut, workImage;
	cvtColor(src,src,CV_BGR2GRAY);

	total = Mat::zeros(src.size(), src.type());
	out = Mat::zeros(src.size(), src.type());

	// print source


	Mat vertical = (Mat_<char>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	filter2DAndShow(src, out, vertical, "vertical");
	total += out;

	Mat verticalNeg = -1 * (Mat_<char>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	filter2DAndShow(src, out, verticalNeg, "verticalNeg");
	total += out;

	Mat horizontal = (Mat_<char>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
	filter2DAndShow(src, out, horizontal, "horizontal");
	total += out;

	Mat horizontalNeg = -1 * (Mat_<char>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
	filter2DAndShow(src, out, horizontalNeg, "horizontalNeg");
	total += out;

	Mat northwest = (Mat_<char>(3, 3) << 2, 1, 0, 1, 0, -1, 0, -1, -2);
	filter2DAndShow(src, out, northwest, "northwest");
	total += out;

	Mat northwestNeg = -1 * (Mat_<char>(3, 3) << 2, 1, 0, 1, 0, -1, 0, -1, -2);
	filter2DAndShow(src, out, northwestNeg, "northwestNeg");
	total += out;

	Mat northeast = (Mat_<char>(3, 3) << 0, -1, -2, 1, 0, -1, 2, 1, 0);
	filter2DAndShow(src, out, northeast, "northeast");
	total += out;

	Mat northeastNeg = -1 * (Mat_<char>(3, 3) << 0, -1, -2, 1, 0, -1, 2, 1, 0);
	filter2DAndShow(src, out, northeastNeg, "northeastNeg");
	total += out;

//	windowName = "total";
//	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
//	imshow(windowName, total);

	return total;
//	colorOut = Mat(src.size(), CV_32F);
//	Mat3b src = imread("/pic.png", 0);
//
//	for (Mat3b::iterator it = src.begin(); it != src.end(); it++) {
//	    if (*it == Vec3b(255, 255, 255)) {
//	        *it = Vec3b(0, 0, 0);
//	    }
//	}
//	sourceName = "colorido";
//	namedWindow(sourceName, CV_WINDOW_AUTOSIZE);
//	imshow(sourceName, colorOut);

//
//	   for(int row =0; row < colorOut.rows; row++){
//			for(int col = 0; col < colorOut.cols; col++){
//				colorOut.at<cv::Vec3b>(row,col)[0] = 0;  //turn the pixel value @ (k,i) to yellow (0,255,255)
//				colorOut.at<cv::Vec3b>(row,col)[1] = 255;
//				colorOut.at<cv::Vec3b>(row,col)[2] = 255;
//			}
//
//		}
//		windowName = "sdjanfdn";
//		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
//		imshow(windowName, colorOut);
//		total += colorOut;
//	out = vertical + horizontal + noroeste + nordeste;
}

string readme() {
	string readme = " Usage: ./RobinsonBorder <img1>";
	std::cout << readme << std::endl;
	return readme;
}

RobinsonBorders::RobinsonBorders() {
	// TODO Auto-generated constructor stub

}

RobinsonBorders::~RobinsonBorders() {
	// TODO Auto-generated destructor stub
}
