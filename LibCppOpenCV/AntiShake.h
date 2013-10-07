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

#define MATCHES_MEAN_DIST 0
#define MATCHES_QUADRANTS 1
#define MATCHES_QUAD_PERIFERY 2
#define MATCHES_QUAD_CENTER 3
#define MATCHES_QUAD_DEFAULT 4

using namespace cv;
using namespace std;

class AntiShake {
public:
	bool shouldPrint;
	void meanMaxMin(Mat &image, double &max, double &minimal, double &mean);
	cv::Mat antiShake(Mat &img_1, Mat &img_2, int matches_type, int numberOfMatches, int ffd,
			double absoluteRelation);
	cv::Mat fixPictures(Mat &img_1, Mat &img_2, int loops, double final_pic_size, double masDetDiff,
			int featurePoints, int coreSize, double absoluteRelation, int matches_type = 0);
	static AntiShake *getInstance(bool shouldPrint = false, double maxDetDiff = 0.15); 				// Singleton Pattern
	static void displayWindow(Mat image, string fileName, bool mightSave);
	static void displayWindow(Mat image, string filename);
	static void readme(string info);
	Mat BorderDetector(Mat src, int type);
	void cvShowManyImages(char* title, int nArgs, ...);
	void reduceDifferences(Mat &img_1, Mat &img_2, Mat &workImage1, Mat &workImage2, int blur1,
			int blur2);
	virtual ~AntiShake();
	void compensateBrightness(Mat &src1, Mat &src2, Mat &output1, Mat &output2, double brightRate = 1.0);
private:
	cv::Mat eye3x3;
	double maxDetDiff;
	cv::Mat cropImageBlackFrame(cv::Mat &img);
	static AntiShake *instance;						// Singleton Pattern
	void applyHomography(Mat &homography, Mat &img_1, Mat &img_2);
	cv::Mat calcHomographyFeedbackController(Mat &img_1, Mat &img_2, int loops, double final_pic_size,
			int featurePoints, int coreSize, double absoluteRelation, int matches_type = 0);
	cv::Mat validateHomography(std::vector<Point2f> &pts1, std::vector<Point2f> &pts2,
			std::vector<uchar> &inliers, bool validate = true);
	// FILTER MATCHED POINTS:
	void getBestMatches(int method, int nthNumber, std::vector<DMatch> &matches, vector<Point2f> &pts1,
			vector<Point2f> &pts2, Mat descriptors_1, Mat descriptors_2, vector<KeyPoint> keypoints_1,
			vector<KeyPoint> keypoints_2, int img_y, int img_x, double absoluteRelation);
	void meanDistancesMatches(int nthNumber, std::vector<DMatch> &matches, vector<KeyPoint> keypoints_1,
			vector<KeyPoint> keypoints_2, double absoluteRelation);
	void quadrantMethod(int nthNumber, std::vector<DMatch> &matches, vector<KeyPoint> keypoints_1,
			vector<KeyPoint> keypoints_2, int Y, int X, int quad_type, double centerEdgeLimit);
	void filterElements(std::vector<DMatch> &matches, int nthNumber);
	// COMPENSATE DIFFERENCES BETWEEN THE PICTURES:
	void compensateBlurriness(Mat &src, Mat &output, int kernelSize);
	void sobelOperator(Mat &src, Mat &output, int scale, int delta);

protected:
//	AntiShake(bool shouldPrint = true, double maxDetDiff = 0.15);
	AntiShake(bool shouldPrint, double maxDetDiff); 								// Singleton Pattern
};

#endif /* ANTISHAKE_H_ */
