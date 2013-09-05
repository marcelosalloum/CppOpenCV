/*
 * LibCpp.cpp


 *
 *  Created on: Aug 26, 2013
 *      Author: marcelosalloum
 */

#include "AntiShake.h"
#include "opencv2/highgui/highgui.hpp"
#include "cv.h"

using namespace cv;

int add(int a, int b) {
	return a + b;
}

void antishake(std::string path1, std::string path2) {
//	cv::Mat img_1 = cv::imread("/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/Debug/aux/foot0.jpeg");
//	cv::Mat img_2 = cv::imread("/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/Debug/aux/foot1.jpeg");
	cv::Mat img_1 = cv::imread(path1);
	cv::Mat img_2 = cv::imread(path2);
	AntiShake *aux = AntiShake::getInstance();

	// ----- VARIABLES:
	int loops = 1; // Numbers of times the algorithm will run again over the already tried-to-fix pictures
	double final_pic_size = 590.0; //The pic size that will be used for the algorithm
	double maxDetDiff = 0.15; // or 0.12?? -> The max value of abs(det-1), in other words, the maximum distance avoidable between the calculated Homography maytrix and the Identity
	int featurePoints = 100;
	int coreSize = 4;
	double absoluteRelation = 2;
	Mat H = aux->fixPictures(img_1, img_2, loops, final_pic_size, maxDetDiff, featurePoints, coreSize,
			absoluteRelation, MATCHES_QUAD_DEFAULT);

	aux->displayWindow(img_1, "antiSHake1", true);
	aux->displayWindow(img_2, "antiShake2", true);
	cout << "THE END!!" << endl;
}

