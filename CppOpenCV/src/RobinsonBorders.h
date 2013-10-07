/*
 * RobinsonBorders.h
 *
 *  Created on: Oct 2, 2013
 *      Author: marcelosalloum
 */

#ifndef ROBINSONBORDERS_H_
#define ROBINSONBORDERS_H_

//#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class RobinsonBorders {
public:
	void dilation(Mat &src, Mat &out, string windowName, bool shouldShow);
	void erosion(Mat &src, Mat &out, string windowName, bool shouldShow);
	void filter2DAndShow(Mat &src, Mat &out, Mat &kernel, string windowName);
	Mat robinsonDetectBorder(Mat src);
	RobinsonBorders();
	virtual ~RobinsonBorders();
private:
	void readme();
};

#endif /* ROBINSONBORDERS_H_ */
