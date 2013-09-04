/**
 * @file SURF_Homography
 * @brief SURF detector + descriptor + FLANN Matcher + FindHomography
 * @author A. Huaman
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "AntiShake.h"
#include "Histogram1D.h"
#include "Layer.h"
#include "dirent.h"
#include <iostream>

using namespace std;
using namespace cv;

cv::Mat cropBeta(cv::Mat &img);
void readme();
int fetchSharpAreas(int argc, char** argv);
//int oldmain(int argc, char** argv);

/**
 * @function main
 * @brief Main function
 */
int main(int argc, char** argv) {

	cv::Mat img_1 = cv::imread(argv[1]);
	cv::Mat img_2 = cv::imread(argv[2]);

	AntiShake *aux = AntiShake::getInstance();
	// ----- VARIABLES:
	int loops = 1; // Numbers of times the algorithm will run again over the already tried-to-fix pictures
	double final_pic_size = 590.0; //The pic size that will be used for the algorithm
	double maxDetDiff = 0.15; // or 0.12?? -> The max value of abs(det-1), in other words, the maximum distance avoidable between the calculated Homography maytrix and the Identity
	int featurePoints = 60;
	int coreSize = 4;
	double absoluteRelation = 0.9;
	Mat H = aux->fixPictures(img_1, img_2, loops, final_pic_size, maxDetDiff, featurePoints, coreSize, absoluteRelation);

	aux->displayWindow(img_1, "antiSHake1", true);
	aux->displayWindow(img_2, "antiShake2", true);

//	aux->reduceDifferences(img_1, img_2, img_1, img_2, 7, 7);

	//  ==== storing data ====
//	std::stringstream ss;
//	ss << H;
	//	return [NSString stringWithCString:ss.str().c_str() encoding:NSASCIIStringEncoding];

//	cout << "++++++" << ss.str().c_str() << endl;

	waitKey(0);
	return 0;
}

cv::Mat cropBeta(cv::Mat &img) {
	// Convert RGB Mat to GRAY
	cv::Mat gray;
	cv::cvtColor(img, gray, CV_BGR2GRAY);

	// Store the set of points in the image before assembling the bounding box
	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = gray.begin<uchar>();
	cv::Mat_<uchar>::iterator end = gray.end<uchar>();
	for (; it != end; ++it) {
		if (*it)
			points.push_back(it.pos());
	}

	// Compute minimal bounding box
	Rect box = cv::boundingRect(cv::Mat(points));

	// Draw bounding box in the original image (debug purposes)
	cv::Point2f vertices[4];
	vertices[0] = Point2f(box.x, box.y);
	vertices[1] = Point2f(box.x + box.width, box.y);
	vertices[2] = Point2f(box.x, box.y + box.height);
	vertices[3] = Point2f(box.x + box.width, box.y + box.height);

	cv::Mat markedImage;
	img.copyTo(markedImage);
	cv::rectangle(markedImage, vertices[0], vertices[3], cv::Scalar(0, 255, 0));

	cv::Mat finalImage;
	img(box).copyTo(finalImage);

	//	cv::imshow("markedImage", markedImage);
	//    cv::imwrite("box.png", img);
	cv::imshow("firstCrop", finalImage);
	return finalImage;
}

/* This method reads all files of a given directory and shows its a binary file with its highFrequencies (marks
 * white pixels where there is sharp focus and left the rest in black).
 * At the end, it also shows the sum of all sharp areas in a binary file*/
int fetchSharpAreas(int argc, char** argv) {
	//First of all, lets check if the number of arguments is correct
	if (argc != 2) {
		readme();
		return -1;
	}

	// Step 1: find all JPG files in that directory
	vector<std::string> fileNames; // this will contain all the filePaths for the JPG files in that path (argv[1])
	DIR *dir;		//Directory
	struct dirent *ent;
	if ((dir = opendir(argv[1])) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string fileName(ent->d_name);
			std::string fileType(".JPG");

			// Checks if the file is a JPG file:
			if (fileName.find(fileType) != std::string::npos) {
				fileNames.push_back(fileName);
				cout << "filePath: " << fileName << endl;
			}
		}
		closedir(dir);
	} else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}

	// STEP 2: get the image vector [vector(std::string filePaths, int matrixTypeCV_8U)]
	cv::Mat totalFocus;
	AntiShake *aux = AntiShake::getInstance();
	std::vector<cv::Mat> fileImages;
	for (unsigned int index = 0; index < fileNames.size(); index++) {
		Mat img = imread(argv[1] + fileNames[index], CV_8U);

		// Re-scale image
		Mat workImage;
		double scale = 1.0 / (MAX(img.rows,img.cols) / 590.0);
		workImage.create(scale * img.rows, scale * img.cols, img.type());
		cv::resize(img, workImage, workImage.size());

		// Gets binary: white points on the best focused parts of the image
		Layer *layer = new Layer();
		Mat binary = layer->getBinaryOnFocus(workImage, 2);

		// Initializes totalFocus Mat
		if (totalFocus.empty()) {
			binary.copyTo(totalFocus);
		} else {
			totalFocus = totalFocus + binary;
		}
		aux->displayWindow(binary, "binary_" + fileNames[index]);
	}
	if (!totalFocus.empty()) {
		waitKey(0);
		aux->displayWindow(totalFocus, "totalFocus");
	}

	waitKey(0);

	return 0;
}

/**
 * @function readme
 */
void readme() {
	std::cout << " Usage: ./SURF_Homography <img1> <img2>" << std::endl;
}
