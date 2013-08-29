/*
 * AntiShake.h
 *
 *  Created on: Oct 19, 2012
 *      Author: marcelo
 */

#ifndef ANTISHAKE_H_
#define ANTISHAKE_H_

#include <stdio.h>
#include <cv.h>
using namespace cv;
using namespace std;

class AntiShake {
public:
	void meanMaxMin(Mat &image, double &max, double &minimal, double &mean);
	cv::Mat antiShake(Mat &img_1, Mat &img_2, int matches_type, int numberOfMatches, int ffd);
	cv::Mat fixPictures(Mat &img_1, Mat &img_2, int loops, double final_pic_size, double masDetDiff);
	static AntiShake *getInstance(); 				// Singleton Pattern
	static void displayWindow(Mat image, string fileName, bool mightSave);
	static void displayWindow(Mat image, string filename);
	static void readme(string info);
	Mat BorderDetector(Mat src, int type);
	void cvShowManyImages(char* title, int nArgs, ...) ;
	void reduceDifferences(Mat &img_1, Mat &img_2, Mat &workImage1, Mat &workImage2, int blur1,
			int blur2);
	virtual ~AntiShake();
private:
	cv::Mat eye3x3;
	bool shouldPrint;
	double maxDetDiff;
	cv::Mat cropImageBlackFrame(cv::Mat &img);
	static AntiShake *instance;						// Singleton Pattern
	void applyHomography(Mat &homography, Mat &img_1, Mat &img_2);
	cv::Mat getHomographyFeedbackController(Mat &img_1, Mat &img_2, int loops, double final_pic_size);
	cv::Mat getHomography(std::vector<Point2f> &pts1, std::vector<Point2f> &pts2,
			std::vector<uchar> &inliers, bool validate);
	// FILTER MATCHED POINTS:
	void getBestMatches(int method, int nthNumber, std::vector<DMatch> &matches, vector<Point2f> &pts1,
			vector<Point2f> &pts2, Mat descriptors_1, Mat descriptors_2, vector<KeyPoint> keypoints_1,
			vector<KeyPoint> keypoints_2, int img_y, int img_x);
	void meanDistancesMatches(int nthNumber, std::vector<DMatch> &matches, vector<KeyPoint> keypoints_1,
			vector<KeyPoint> keypoints_2);
	void quadrantMethod(int nthNumber, std::vector<DMatch> &matches, vector<KeyPoint> keypoints_1,
			vector<KeyPoint> keypoints_2, int Y, int X, int quad_type, double centerEdgeLimit);
	void filterElements(std::vector<DMatch> &matches, int nthNumber);
	// COMPENSATE DIFFERENCES BETWEEN THE PICTURES:
	void compensateBrightness(Mat &src1, Mat &src2, Mat &output1, Mat &output2);
	void compensateBlurriness(Mat &src, Mat &output, int kernelSize);
	void sobelOperator(Mat &src, Mat &output, int scale, int delta);

protected:
	AntiShake(bool shouldPrint, double maxDetDiff); 								// Singleton Pattern
};

#endif /* ANTISHAKE_H_ */
