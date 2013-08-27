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
	Mat H = aux->fixPictures(img_1, img_2, 1);

	aux->displayWindow(img_1, "antiSHake1", true);
	aux->displayWindow(img_2, "antiShake2", true);
	cout << "THE END!!" << endl;
}

