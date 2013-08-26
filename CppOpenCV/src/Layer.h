/*
 * Layer.h
 *
 *  Created on: Jun 24, 2013
 *      Author: marcelosalloum
 */

#ifndef LAYER_H_
#define LAYER_H_

#include <cv.h>
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

class Layer {
public:
	void trackFocusedAreas(cv::Mat &img_1, cv::Mat &img_2);
	cv::Mat getBinaryOnFocus(cv::Mat &img_1, int gain);
	Layer();
	virtual ~Layer();
};

#endif /* LAYER_H_ */
