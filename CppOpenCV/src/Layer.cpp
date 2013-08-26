/*
 * Layer.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: marcelosalloum
 */

#include "Layer.h"
#include "AntiShake.h"
#include "Histogram1D.h"
using namespace std;
using namespace cv;

void Layer::trackFocusedAreas(cv::Mat &img_1, cv::Mat &img_2){
	Histogram1D *hist = new Histogram1D();

	img_1 = hist->stretch(img_1);
	img_2 = hist->stretch(img_2);

	Mat gauss1, gauss2;
	gauss1.create(img_1.rows, img_1.cols, img_1.type());
	gauss2.create(img_2.rows, img_2.cols, img_2.type());
	GaussianBlur(img_1, gauss1, Size(3, 3), 0, 0, BORDER_DEFAULT);
	GaussianBlur(img_2, gauss2, Size(3, 3), 0, 0, BORDER_DEFAULT);

	float gain = 2;
	Mat img_11 = gain * (img_1 - gauss1);
	Mat img_22 = gain * (img_2 - gauss2);

	//	Histogram1D *hist = new Histogram1D();

	//	img_11 = hist->stretch(img_11);
	//	img_22 = hist->stretch(img_22);
	float min1, max1, min2, max2;
	hist->findMaxMin(hist->getHistogram(img_11), 0, 255, min1, max1);
	hist->findMaxMin(hist->getHistogram(img_22), 0, 255, min2, max2);

	cout << "h1(min,Max) = " << min1 << "," << max1 << "    h2(min,Max) = "
			<< min2 << "," << max2 << endl;

	int x = 1;

//	while (x < 11) {
//		if (min1 < 150 || min2 < 150) {
//			gain = min((150.0 / min(min1, min2)), 3.0) * gain;
//			x++;
//		} else if (min1 > 1000 || min2 > 1000) {
//			gain = min((1000.0 / min(min1, min2)), 1 / 2.5) * gain;
//			x++;
//		} else {
//			x = 11;
//		}
//		img_11 = gain * (img_1 - gauss1);
//		img_22 = gain * (img_2 - gauss2);
//
//		hist->findMaxMin(hist->getHistogram(img_11), 0, 255, min1, max1);
//		hist->findMaxMin(hist->getHistogram(img_22), 0, 255, min2, max2);
//
//		cout << "h1(min,Max) = " << min1 << "," << max1 << "    h2(min,Max) = "
//				<< min2 << "," << max2 << endl;
//	}

	Mat hist_1 = hist->getHistogramImage(img_11);
	Mat hist_2 = hist->getHistogramImage(img_22);

	//Binary image
	cv::Mat binaryMat1(img_11.size(), img_11.type());
	cv::Mat binaryMat2(img_22.size(), img_22.type());
	//Apply thresholding
	cv::threshold(img_11, binaryMat1, 35, 255, cv::THRESH_BINARY);
	cv::threshold(img_22, binaryMat2, 35, 255, cv::THRESH_BINARY);

	AntiShake *aux = AntiShake::getInstance();

	aux->displayWindow(binaryMat1, "binaryMat1");
	aux->displayWindow(binaryMat2, "binaryMat2");

	aux->displayWindow(hist_1, "hist1");
	aux->displayWindow(hist_2, "hist2");
}

cv::Mat Layer::getBinaryOnFocus(cv::Mat &img, int gain){
	Histogram1D *hist = new Histogram1D();
	img = hist->stretch(img);

	Mat gauss1, gauss2;
	gauss1.create(img.rows, img.cols, img.type());
	GaussianBlur(img, gauss1, Size(3, 3), 0, 0, BORDER_DEFAULT);

//	float gain = 2;
	Mat imgHighFreq = gain * (img - gauss1);

	float min1, max1;
	hist->findMaxMin(hist->getHistogram(imgHighFreq), 0, 255, min1, max1);

	cout << "h1(min,Max) = " << min1 << "," << max1 << endl;

//	int x = 1;
//
//	while (x < 11) {
//		if (min1 < 150 || min2 < 150) {
//			gain = min((150.0 / min(min1, min2)), 3.0) * gain;
//			x++;
//		} else if (min1 > 1000 || min2 > 1000) {
//			gain = min((1000.0 / min(min1, min2)), 1 / 2.5) * gain;
//			x++;
//		} else {
//			x = 11;
//		}
//		img_11 = gain * (img_1 - gauss1);
//		img_22 = gain * (img_2 - gauss2);
//
//		hist->findMaxMin(hist->getHistogram(img_11), 0, 255, min1, max1);
//		hist->findMaxMin(hist->getHistogram(img_22), 0, 255, min2, max2);
//
//		cout << "h1(min,Max) = " << min1 << "," << max1 << "    h2(min,Max) = "
//				<< min2 << "," << max2 << endl;
//	}

	//Binary image
	cv::Mat binaryMat(imgHighFreq.size(), imgHighFreq.type());
	//Apply thresholding
	cv::threshold(imgHighFreq, binaryMat, 35, 255, cv::THRESH_BINARY);

//	AntiShake *aux = AntiShake::getInstance();
//	aux->displayWindow(binaryMat, "binaryMat");
	return binaryMat;
}

Layer::Layer() {
	// TODO Auto-generated constructor stub

}

Layer::~Layer() {
	// TODO Auto-generated destructor stub
}

