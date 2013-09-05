/*
 * AntiShake.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: marcelo
 *      This class was created to extract some information from features points
 */

#include <stdlib.h>
#include "AntiShake.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

// Singleton pattern:
AntiShake *AntiShake::instance;
AntiShake::AntiShake(bool shouldPrint = true, double maxDetDiff = 0.15) {
	eye3x3 = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	this->shouldPrint = shouldPrint;
	this->maxDetDiff = maxDetDiff;
}
AntiShake *AntiShake::getInstance() {
	if (!instance) {
		instance = new AntiShake();
	}
	return instance;
}

cv::Mat AntiShake::fixPictures(Mat &img_1, Mat &img_2, int loops, double final_pic_size,
		double maxDetDiff, int featurePoints, int coreSize, double absoluteRelation, int matches_type) {
	// Firstly we calculate the Homography matrix and refine it in the FeedbackController function:
	Mat H = calcHomographyFeedbackController(img_1, img_2, loops, final_pic_size, featurePoints, coreSize,
			absoluteRelation, matches_type);
	double det = determinant(H);
	cout << "STEP 11 final homography = " << endl << H << endl << " determinant = " << det << endl;

	//Secondly, lets transform the picture according to the calculated (resultant) smatrix.
	//TODO
	/*
	 * Create white Image with the same size as img_1 and img_2
	 * apply Homography on white one
	 * Count histogramas and pixels with value == 0 (black pixels)
	 * if value > 0, crop on both img_1 and img_2
	 *
	 * */
	if (det > 1.0) {
		applyHomography(H, img_1, img_2);
		cout << "STEP 12 fixed original pictures 1->2" << endl;
		return H;
	} else {
		Mat H2 = H.inv();
		applyHomography(H2, img_2, img_1);
		cout << "STEP 12 fixed original pictures 2->1 " << endl;
		return H2;
	}
}

// Applies the correction matrix to one image so it becomes closed to the other one
void AntiShake::applyHomography(Mat &homography, Mat &img_1, Mat &img_2) {
	Mat original, compensated;
	// STEP 10: Compare with identity and if not equal, applies correction
	if (determinant(homography) == 1.0) {
		cout << "==== applyHomography -> EQUALS IDENTITY " << endl;
	} else {
		cout << "==== applyHomography -> DIFFERENT from identity" << endl;
		img_2.copyTo(original);
		cv::warpPerspective(img_1,  // input image
				compensated,        // output image
				homography, cv::Size(img_1.cols, img_1.rows), INTER_CUBIC); // size of output image
		original.copyTo(img_1);
		compensated.copyTo(img_2);
		cout << endl << "==== STEP 10 complete: distortion fix applied" << endl << endl;
	}
}

/* It is a function that call getHoography multiple times and measure its accuracy.
 * If it runs the function more than the maxLoop value or if the accuracy measure
 * starts to increase, the loop stops*/
cv::Mat AntiShake::calcHomographyFeedbackController(Mat &img_1, Mat &img_2, int loops,
		double final_pic_size, int featurePoints, int coreSize, double absoluteRelation,
		int matches_type) {
	// STEP 0: RE-ESCALE, SO THE BIGGEST RESOLUTION IS 590x(something <= 590)
	Mat workImage1, workImage2;
	double scale = 1.0 / (MAX(img_1.rows,img_1.cols) / final_pic_size);
	workImage1.create(scale * img_1.rows, scale * img_1.cols, img_1.type());
	workImage2.create(scale * img_2.rows, scale * img_2.cols, img_2.type());
	cv::resize(img_1, workImage1, workImage1.size(), 0, 0, INTER_CUBIC);
	cv::resize(img_2, workImage2, workImage2.size(), 0, 0, INTER_CUBIC);
	cout << "=== STEP 0 complete: RE-ESCALE" << endl;

	// LETS NOW START TO ITERATE IN ORTHER TO get a Homography matrix and refine it
	Mat homography;
	vector<cv::Mat> Hs, eigenvalues;
	vector<double> dets;
	int loopCounter = 0;
	do {
		loopCounter++;
		try {
			//pixels:
			homography = antiShake(workImage1, workImage2, matches_type, featurePoints, coreSize,
					absoluteRelation); // exceptions could appear here... //STEPS 1 to 8 there.
			double det = determinant(homography);
			Mat eigen;
			cv::eigen(homography, eigen);
			cout << endl << "==== STEP 9: HOMOGRAPHY: " << endl << homography << endl << "determinant: "
					<< det << endl << "eigenvalues: " << eigen << endl << endl;

			// *** Checks if the determinant is small enough. If not, the transformation could be awful.
			if (det != 1) {
				Hs.push_back(homography);
				eigenvalues.push_back(eigen);
				dets.push_back(abs(det - 1));

				applyHomography(homography, workImage1, workImage2);

				//Checks if the error has decreased in the last iteration:
				int size = dets.size();
				if (size > 2) {
					if (dets[size - 1] > dets[size - 2]) {
						/*
						 * Explaining the above if comparision:
						 * 		- dets[size - 1] is the nth (last) calculated determinant
						 * 		- dets[size - 2] is the (n-1)th (penultimate) calculated determinant*
						 * 		- The biggest is the difference between the two pictures, the highest
						 * 		will be the det value. Thing is: once the algorithm is well applicated,
						 * 		if applyied again in the now fixed pictures, should generate a matrix similar
						 * 		to the Identity I (at least more similar to I than the first matrix).
						 * 		- dets receive the value ov abd(det -  1). It is used as the measuring parameter.
						 * 		If the value of the det(n) increases from one run to the next,
						 * 		means the algorithm may not be being well used (for any picture noise or miscalibration)
						 */
						cout << "==== POP BACK" << endl;
						//Remove last element of the vectors
						Hs.pop_back();
						eigenvalues.pop_back();
						dets.pop_back();
						break;
					}
				}
			} else {
				// if abs(det-1) is too high, give him some munchies along with the identity matrix:
				break;
			}
		} catch (Exception &e) {
			cout
					<< "ATTENTION ON THE ANTISHAKE.CPP CLASS: exception found in the function getHomographyFeedbackController. ERROR: "
					<< e.err << endl;
			homography = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
			cout << "IDENTITY 1" << endl;
		}
	} while (loopCounter < loops || homography.empty());

	// CALCULATES RESULTANT HOMOGRAPHY:
	Mat resultantH = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	for (unsigned int position = 0; position < Hs.size(); ++position) {
		resultantH = (Hs[position]) * resultantH;
	}
	if (abs(determinant(resultantH) - 1.0) < maxDetDiff) {
		return resultantH;
	} else {
		return Hs[0];
	}
}

void AntiShake::reduceDifferences(Mat &img_1, Mat &img_2, Mat &workImage1, Mat &workImage2, int blur1,
		int blur2) {

	img_1.copyTo(workImage1);
	img_2.copyTo(workImage2);
	cout << "=== STEP 1 complete: created workImage" << endl;

	// STEP 2: COMPENSATE BRIGHTNES
	compensateBrightness(workImage1, workImage2, workImage1, workImage2); //OK
	cout << "=== STEP 2 complete: compensateBrightness" << endl;

	// STEP 3: BLUR EVERYTHING TO NORMALIZE THE SOURCE IMAGES
	compensateBlurriness(workImage1, workImage1, 7);
	compensateBlurriness(workImage2, workImage2, 7);
	cout << "=== STEP 3 complete: compensate Blurriness" << endl;

	// STEP 4: SOBEL OPERATOR (BORDERS)
//	sobelOperator(workImage1, workImage1, 5, 1); // TODO
//	sobelOperator(workImage2, workImage2, 5, 1);
//	cout << "=== STEP 4 complete: sobelOperator" << endl;

	cv::cvtColor(workImage1, workImage1, CV_BGR2GRAY);
	cv::cvtColor(workImage2, workImage2, CV_BGR2GRAY);
	workImage1 = BorderDetector(workImage1, 3);
	workImage2 = BorderDetector(workImage2, 3);
	cout << "=== STEP 4 complete: sobelOperator" << endl;
}

// Detect keypoints and find
cv::Mat AntiShake::antiShake(Mat &img_1, Mat &img_2, int matches_type, int featurePoints, int corePx,
		double absoluteRelation) {

	Mat workImage1, workImage2;
	reduceDifferences(img_1, img_2, workImage1, workImage2, 7, 7); // STEPS 1 to 4 here

	// STEP 5: KeyPoint Detection:
	cv::FeatureDetector *detector = new cv::FastFeatureDetector(corePx, true);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	detector->detect(workImage1, keypoints_1);
	detector->detect(workImage2, keypoints_2);
	cout << "==== STEP 5 complete: keypoints detected, (keypoints1.size(), keypoints2.size()) = ("
			<< keypoints_1.size() << ", " << keypoints_2.size() << ")" << endl;
	delete (detector);

	// STEP 6: Calculate descriptors (feature vectors)
	cv::DescriptorExtractor *extractor = new cv::BriefDescriptorExtractor();
	Mat descriptors_1, descriptors_2;
	extractor->compute(workImage1, keypoints_1, descriptors_1);
	extractor->compute(workImage2, keypoints_2, descriptors_2);
	cout << "==== STEP 6 complete: extract descriptors" << endl;
	delete (extractor);

	// STEP 7: Get Matches
	vector<DMatch> good_matches;
	std::vector<Point2f> pts1, pts2;

	this->getBestMatches(matches_type, featurePoints, good_matches, pts1, pts2, descriptors_1,
			descriptors_2, keypoints_1, keypoints_2, workImage1.rows, workImage1.cols, absoluteRelation);
	Mat img_matches;
	drawMatches(workImage1, keypoints_1, workImage2, keypoints_2, good_matches, img_matches,
			Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	if (shouldPrint)
		displayWindow(img_matches, "MATCHES");
	cout << "==== STEP 7 complete: finished matching descriptors: " << featurePoints << endl;

	// STEP 8: Find Homography:
	vector<uchar> inliers(pts1.size(), 0);
	Mat homography = validateHomography(pts1, pts2, inliers, true);
	cout << "==== STEP 8 complete: finished calculating right homographY." << endl;

	return homography;
}

// Calculates homography and validates it if the flag is properly set
cv::Mat AntiShake::validateHomography(std::vector<Point2f> &pts1, std::vector<Point2f> &pts2,
		std::vector<uchar> &inliers, bool validate) {
	Mat HReference = findHomography(Mat(pts1), Mat(pts2), inliers, CV_RANSAC, 1);
	if (validate) {
		//Checks if some of the values are too out of normal. If so, sets matrix to Identity
		if (abs(HReference.at<double>(2, 0) > 0.0002) || abs(HReference.at<double>(2, 1) > 0.0002)
				|| abs(HReference.at<double>(0, 1) > 0.02) || abs(HReference.at<double>(1, 0)) > 0.02) {
			cout << "changed to IDENTITY (2) " << endl << HReference << endl;
			HReference = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
		} else if (abs(determinant(HReference) - 1.0) >= maxDetDiff) {
			cout << "changed to IDENTITY (3) " << endl << HReference << endl;
			HReference = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
		}
	}
	return HReference;
}

void AntiShake::cvShowManyImages(char* title, int nArgs, ...) {

	// img - Used for getting the arguments
	IplImage *img;

	// DispImage - the image in which input images are to be copied
	IplImage *DispImage;

	int size;
	int i;
	int m, n;
	int x, y;

	// w - Maximum number of images in a row
	// h - Maximum number of images in a column
	int w, h;

	// scale - How much we have to resize the image
	float scale;
	int max;

	// If the number of arguments is lesser than 0 or greater than 12
	// return without displaying
	if (nArgs <= 0) {
		printf("Number of arguments too small....\n");
		return;
	} else if (nArgs > 12) {
		printf("Number of arguments too large....\n");
		return;
	}
	// Determine the size of the image,
	// and the number of rows/cols
	// from number of arguments
	else if (nArgs == 1) {
		w = h = 1;
		size = 300;
	} else if (nArgs == 2) {
		w = 2;
		h = 1;
		size = 300;
	} else if (nArgs == 3 || nArgs == 4) {
		w = 2;
		h = 2;
		size = 300;
	} else if (nArgs == 5 || nArgs == 6) {
		w = 3;
		h = 2;
		size = 200;
	} else if (nArgs == 7 || nArgs == 8) {
		w = 4;
		h = 2;
		size = 200;
	} else {
		w = 4;
		h = 3;
		size = 150;
	}

	// Create a new 3 channel image
	DispImage = cvCreateImage(cvSize(100 + size * w, 60 + size * h), 8, 3);

	// Used to get the arguments passed
	va_list args;
	va_start(args, nArgs);

	// Loop for nArgs number of arguments
	for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {

		// Get the Pointer to the IplImage
		img = va_arg(args, IplImage*);

		// Check whether it is NULL or not
		// If it is NULL, release the image, and return
		if (img == 0) {
			printf("Invalid arguments");
			cvReleaseImage(&DispImage);
			return;
		}

		// Find the width and height of the image
		x = img->width;
		y = img->height;

		// Find whether height or width is greater in order to resize the image
		max = (x > y) ? x : y;

		// Find the scaling factor to resize the image
		scale = (float) ((float) max / size);

		// Used to Align the images
		if (i % w == 0 && m != 20) {
			m = 20;
			n += 20 + size;
		}

		// Set the image ROI to display the current image
		cvSetImageROI(DispImage, cvRect(m, n, (int) (x / scale), (int) (y / scale)));

		// Resize the input image and copy the it to the Single Big Image
		cvResize(img, DispImage);

		// Reset the ROI in order to display the next image
		cvResetImageROI(DispImage);
	}

	// Create a new window, and show the Single Big Image
	cvNamedWindow(title, 1);
	cvShowImage(title, DispImage);

	cvWaitKey();
	cvDestroyWindow(title);

	// End the number of arguments
	va_end(args);

	// Release the Image Memory
	cvReleaseImage(&DispImage);
}

//CALL DIFFERENT ALGORITHMS:
void AntiShake::getBestMatches(int matches_type, int nthNumber, std::vector<DMatch> &matches,
		vector<Point2f> &pts1, vector<Point2f> &pts2, Mat descriptors_1, Mat descriptors_2,
		vector<KeyPoint> keypoints_1, vector<KeyPoint> keypoints_2, int height, int width,
		double absoluteRelation) {

	// -- STEP A: Matching descriptor vectors using BruteForceMatcher
	BFMatcher matcher(NORM_L1, true);
	matcher.match(descriptors_1, descriptors_2, matches);
	vector<DMatch> centerPoints, periferyPoints;
// TODO
	switch (matches_type) {
	case MATCHES_MEAN_DIST:
		meanDistancesMatches(nthNumber, matches, keypoints_1, keypoints_2, absoluteRelation);
		break;
	case MATCHES_QUADRANTS:
		meanDistancesMatches(0, matches, keypoints_1, keypoints_2, absoluteRelation);
		quadrantMethod(nthNumber, matches, keypoints_1, keypoints_2, height / 2, width / 2,
				MATCHES_QUADRANTS, 0);
		break;
	case MATCHES_QUAD_PERIFERY:
		meanDistancesMatches(0, matches, keypoints_1, keypoints_2, absoluteRelation);
		quadrantMethod(nthNumber, matches, keypoints_1, keypoints_2, height / 2, width / 2,
				MATCHES_QUAD_PERIFERY, 0.7);
		break;
	case MATCHES_QUAD_CENTER:
		meanDistancesMatches(0, matches, keypoints_1, keypoints_2, absoluteRelation);
		quadrantMethod(nthNumber, matches, keypoints_1, keypoints_2, height / 2, width / 2,
				MATCHES_QUAD_CENTER, 0.4);
		break;
	case MATCHES_QUAD_DEFAULT:
		meanDistancesMatches(0, matches, keypoints_1, keypoints_2, absoluteRelation);
		centerPoints.insert(centerPoints.end(), matches.begin(), matches.end());
		periferyPoints.insert(periferyPoints.end(), matches.begin(), matches.end());
		quadrantMethod(nthNumber / 2, centerPoints, keypoints_1, keypoints_2, height / 2, width / 2,
				MATCHES_QUAD_CENTER, 0.4);
		quadrantMethod(nthNumber, periferyPoints, keypoints_1, keypoints_2, height / 2, width / 2,
				MATCHES_QUAD_PERIFERY, 0.4);
		matches = periferyPoints;
		matches.insert(matches.end(), centerPoints.begin(), centerPoints.end());
		break;
	default:
		break;
	}

	cout << "matches_type: " << matches_type << endl;

	// Lets now populate the pts1 and pts2 vector
	for (unsigned int index = 0; index < matches.size(); ++index) {
		pts1.push_back(keypoints_1[matches[index].queryIdx].pt);
		pts2.push_back(keypoints_2[matches[index].trainIdx].pt);
	}
}

void AntiShake::quadrantMethod(int nthNumber, std::vector<DMatch> &matches, vector<KeyPoint> keypoints_1,
		vector<KeyPoint> keypoints_2, int Yc, int Xc, int quad_type, double centerEdgeLimit) {
	/*
	 * quad2  |  quad1
	 * ____(Xc,Yc)_____
	 * quad3  |  quad4
	 * */
	cout << "Xc + Xc * centerEdgeLimit = " << Xc + Xc * centerEdgeLimit << endl;
	cout << "Yc - Yc * centerEdgeLimit = " << Yc - Yc * centerEdgeLimit << endl;
	std::vector<DMatch> quad1, quad2, quad3, quad4;
	//-- STEP 1: Lets separate each point accoding to its quadrant
	for (unsigned int i = 0; i < matches.size(); i++) {
		Point2f p1 = keypoints_1[matches[i].queryIdx].pt;
		Point2f p2 = keypoints_2[matches[i].trainIdx].pt;

		// Start Quadrant 1:
		if ((p1.x > Xc) && (p2.x > Xc) && (p1.y < Yc) && (p2.y < Yc)) {
			if (quad_type == MATCHES_QUADRANTS) {
				quad1.push_back(matches[i]);
			} else if ((p1.y < Yc - Yc * centerEdgeLimit) || (p1.x > Xc + Xc * centerEdgeLimit)) {
				if (quad_type == MATCHES_QUAD_PERIFERY) {
					quad1.push_back(matches[i]);
				}
			} else {
				if (quad_type == MATCHES_QUAD_CENTER) {
					quad1.push_back(matches[i]);
				}
			}
			// Start Quadrant 2:
		} else if ((p1.x < Xc) && (p2.x < Xc) && (p1.y < Yc) && (p2.y < Yc)) {
			if (quad_type == MATCHES_QUADRANTS) {
				quad2.push_back(matches[i]);
			} else if ((p1.y < Yc - Yc * centerEdgeLimit) || (p1.x < Xc - Xc * centerEdgeLimit)) {
				if (quad_type == MATCHES_QUAD_PERIFERY) {
					quad2.push_back(matches[i]);
				}
			} else {
				if (quad_type == MATCHES_QUAD_CENTER) {
					quad2.push_back(matches[i]);
				}
			}
			// Start Quadrant 3:
		} else if ((p1.x < Xc) && (p2.x < Xc) && (p1.y > Yc) && (p2.y > Yc)) {
			if (quad_type == MATCHES_QUADRANTS) {
				quad3.push_back(matches[i]);
			} else if ((p1.y > Yc + Yc * centerEdgeLimit) || (p1.x < Xc - Xc * centerEdgeLimit)) {
				if (quad_type == MATCHES_QUAD_PERIFERY) {
					quad3.push_back(matches[i]);
				}
			} else {
				if (quad_type == MATCHES_QUAD_CENTER) {
					quad3.push_back(matches[i]);
				}
			}
			// Start Quadrant 4:
		} else if ((p1.x > Xc) && (p2.x > Xc) && (p1.y > Yc) && (p2.y > Yc)) {
			if (MATCHES_QUADRANTS) {
				quad4.push_back(matches[i]);
			} else if ((p1.y > Yc + Yc * centerEdgeLimit) || (p1.x > Xc + Xc * centerEdgeLimit)) {
				if (quad_type == MATCHES_QUAD_PERIFERY) {
					quad4.push_back(matches[i]);
				}
			} else {
				if (quad_type == MATCHES_QUAD_CENTER) {
					quad4.push_back(matches[i]);
				}
			}
		}
	}

////	Choose points from each quadrant
//	int minSize = min(min(quad1.size(), quad2.size()), min(quad3.size(), quad4.size()));
	vector<vector<DMatch> > allQuads;
//	if (minSize == 0) {
//		if (!quad1.empty())
//			allQuads.push_back(quad1);
//		if (!quad2.empty())
//			allQuads.push_back(quad2);
//		if (!quad3.empty())
//			allQuads.push_back(quad3);
//		if (!quad4.empty())
//			allQuads.push_back(quad4);
//
//		if (allQuads.empty() || allQuads.size() < 3) {
//			cout << " ==== ERROR, less than 3 quadrants contain points" << endl;
//		} else {
//			cout << " ==== Number of good quadrants: " << allQuads.size() << " of 4" << endl;
//		}
//	} else if (minSize >= nthNumber) {
//		minSize = nthNumber; // increasing this number makes the algorithm go slower. So it shouldn't be very big
//	}
//
//	// if minSize wasn't null, the allQuads will be empty, so we populate it
	if (allQuads.empty()) {
		allQuads.push_back(quad1);
		allQuads.push_back(quad2);
		allQuads.push_back(quad3);
		allQuads.push_back(quad4);
	}

	vector<DMatch> allPoints;
	// Iterates over Quadrants: Getting the best matches for each vector and unifying them into the unique allPoints vector
	for (unsigned int index = 0; index < allQuads.size(); ++index) {
		vector<DMatch> quad = allQuads[index];
		int size = (int) (((int) quad.size()) * 0.2);
		if (size > 0) {
			filterElements(quad, size);			// Gets the best matches
			allPoints.insert(allPoints.end(), quad.begin(), quad.end()); // append the vectors together
		}
	}
	matches = allPoints;
}

void AntiShake::filterElements(std::vector<DMatch> &matches, int nthNumber) {
	std::nth_element(matches.begin(),	// initial position
			matches.begin() + nthNumber - 1,	// position of the sorted element
			matches.end());	// end position
	matches.erase(matches.begin() + nthNumber, matches.end()); // remove all elements after the nthNumber(th)
}

/*
 * Filters all points by the meanDistance between the pts1 and pts2
 * */
void AntiShake::meanDistancesMatches(int nthNumber, std::vector<DMatch> &matches,
		vector<KeyPoint> keypoints_1, vector<KeyPoint> keypoints_2, double absoluteRelation) {

//	//-- STEP A: Finds mean distance
//	double meanDistance = 0;
//	for (unsigned int i = 0; i < matches.size(); i++) {
//		Point2f p1 = keypoints_1[matches[i].queryIdx].pt;
//		Point2f p2 = keypoints_2[matches[i].trainIdx].pt;
//		double dist = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
//		meanDistance += dist;
//	}
//	meanDistance = meanDistance / matches.size();

// todo
//	steb B: filters the points by mean Distance
	std::vector<DMatch> new_matches;
	Point2f *p1, *p2;
	int deltaX, deltaY;
	int sumX = 5;
	int sumY = 5;
	double meanX = 5.0;
	double meanY = 5.0;
	for (unsigned int i = 0; i < matches.size(); i++) {
		*p1 = keypoints_1[matches[i].queryIdx].pt;
		*p2 = keypoints_2[matches[i].trainIdx].pt;

		deltaX = abs(p1->x - p2->x);
		deltaY = abs(p1->y - p2->y);

		cout << "deltaX = " << deltaX << endl;
		cout << "sumX = " << sumX << endl;
		cout << "meanX = " << meanX << endl;
		cout << "deltaX/meanX = " << deltaX / meanX << endl << endl;
		if ((abs(deltaX / meanX - 1) < absoluteRelation)
				&& (abs(deltaY / meanY - 1) < absoluteRelation)) {
			new_matches.push_back(matches[i]);
			sumX += deltaX;
			sumY += deltaY;
			meanX = (double) sumX / ((double) new_matches.size());
			meanY = (double) sumY / ((double) new_matches.size());
		}
	}

	if (nthNumber != 0 && nthNumber < (int) new_matches.size()) {
		//-- STEP B: gets just the first N matches with the smaller value for distance (N=nthNumber)
		filterElements(new_matches, nthNumber);
	}
	matches = new_matches;
}

struct SortOnX {
	template<typename T> bool operator()(cv::Point_<T> p1, cv::Point_<T> p2) const {
		return p1.x < p2.x;
	}
};
struct SortOnY {
	template<typename T> bool operator()(cv::Point_<T> p1, cv::Point_<T> p2) const {
		return p1.y < p2.y;
	}
};

cv::Mat cropImageBlackFrame(cv::Mat &img) { //TODO
	AntiShake *aux = AntiShake::getInstance();
	//	aux->displayWindow(img, "original");

	Mat gray, thresh, bufferImage;
	cv::copyMakeBorder(img, bufferImage, 10, 10, 10, 10, BORDER_CONSTANT);

	// STEP 1: transform to grayscale
	if (bufferImage.type() != CV_8U) {
		cvtColor(bufferImage, gray, CV_RGB2GRAY);
	} else {
		bufferImage.copyTo(gray);
	}

	// STEP 2: threshold black frame from the scene
	cv::threshold(gray, thresh, 1, 255, cv::THRESH_BINARY);
	aux->displayWindow(thresh, "thresh");
	//	aux->displayWindow(gray, "gray");

	// STEP 3: Detect image corners
	std::vector<cv::KeyPoint> kpts;
	cv::FAST(thresh, kpts, 50, false);
	cout << "==== kpts.size() " << kpts.size() << endl;

	if (kpts.size() > 0) {
		std::vector<cv::Point2f> corners, pointsX, pointsY;
		cv::KeyPoint::convert(kpts, pointsX);

		pointsX.push_back(Point2f(0, 0));
		pointsX.push_back(Point2f(0, img.rows));
		pointsX.push_back(Point2f(img.cols, 0));
		pointsX.push_back(Point2f(img.cols, img.rows));

		// STEP 4: gets only the local minimums
		for (unsigned int pVar = 0; pVar < pointsX.size(); ++pVar) {
			bool isNew = true;

			for (unsigned int cVar = 0; cVar < corners.size(); ++cVar) {
				if ((abs(pointsX[pVar].x - corners[cVar].x) < img.cols / 3)
						&& (abs(pointsX[pVar].y - corners[cVar].y) < img.rows / 3)) {
					isNew = false;
					if (corners[cVar].x < img.cols / 2) {
						corners[cVar].x = max(corners[cVar].x, pointsX[pVar].x);
					} else {
						corners[cVar].x = min(corners[cVar].x, pointsX[pVar].x);
					}

					if (corners[cVar].y < img.rows / 2) {
						corners[cVar].y = max(corners[cVar].y, pointsX[pVar].y);
					} else {
						corners[cVar].y = min(corners[cVar].y, pointsX[pVar].y);
					}
				}
			}
			if (isNew) {
				corners.push_back(pointsX[pVar]);
			}
		}

		cout << endl;
		for (unsigned int var = 0; var < corners.size(); ++var) {
			cout << "==== CornersX (x, y) = " << corners[var].x << ", " << corners[var].y << endl;
		}
		cout << endl << endl;

		if (corners.size() != 4) {
			cout << "==== corners size != 4, corners.size = " << corners.size() << endl << endl << endl;
			return img;
		} else {

			// STEP 5: Order Values according to X and Y values
			pointsX = corners;
			pointsY = pointsX;

			std::sort(pointsX.begin(), pointsX.end(), SortOnX());
			std::sort(pointsY.begin(), pointsY.end(), SortOnY());

			cv::Point2f points[2];
			points[0] = Point2f(pointsX[1].x, pointsY[1].y);
			points[1] = Point2f(pointsX[2].x, pointsY[2].y);

			cout << "points[0] (x,y) = (" << points[0].x << ", " << points[0].y << ")" << endl
					<< "points[1] (x,y) = (" << points[1].x << ", " << points[1].y << ")" << endl
					<< "(gray.cols, gray.rows) = (" << gray.cols << ", " << gray.rows << ")" << endl;
			if (points[0].x == 12 && points[0].y == 12 && (gray.cols - points[1].x == 20)
					&& (gray.rows - points[1].y == 20)) {
				cout << "==== NO NEW VERTICES" << endl << endl << endl;
				return img;
			} else {
				cv::Mat markedImage, finalImage;
				bufferImage.copyTo(markedImage);

				// STEP 6: Draw Green Lines on image
				cv::rectangle(markedImage, points[0], points[1], cv::Scalar(0, 255, 0));
				cv::Rect rect(points[0], points[1]);
				aux->displayWindow(markedImage, "markedImage");

				// STEP 7: Crop black Edges From images
				bufferImage(rect).copyTo(finalImage);
				aux->displayWindow(finalImage, "secondCrop");
				return finalImage;
			}
		}
	} else {
		return img;
	}
}

void AntiShake::compensateBrightness(Mat &src1, Mat &src2, Mat &output1, Mat &output2) {
	// STEP A: Create copies
	src1.copyTo(output1);
	src2.copyTo(output2);

	// STEP B: Calculates overall brightness
	double brightness1 = 0;
	double brightness2 = 0;
	for (int row = 0; row < output1.rows; row++) {
		for (int col = 0; col < output1.cols; col++) {
			// B.1: sums all brightness for each picture
			brightness1 = brightness1 + output1.at<Vec3b>(row, col)[0] + output1.at<Vec3b>(row, col)[1]
					+ output1.at<Vec3b>(row, col)[2];
			brightness2 = brightness2 + output2.at<Vec3b>(row, col)[0] + output2.at<Vec3b>(row, col)[1]
					+ output2.at<Vec3b>(row, col)[2];
		}
	}
	// Calculates the brightness mean
	brightness1 = brightness1 / ((output1.rows) * (output1.cols));
	brightness2 = brightness2 / ((output2.rows) * (output2.cols));

	// STEP C: Equalizes brightness:
	double brightRate, beta;
	beta = abs(brightness1 - brightness2) / 50;
	cv::Mat* darkerPic;		// The picture that will be brighten
	if (brightness1 > brightness2) {
		darkerPic = &output2;
		brightRate = brightness1 / brightness2;
	} else {
		darkerPic = &output1;
		brightRate = brightness2 / brightness1;
	}
	brightRate *= 2.0;
	for (int y = 0; y < darkerPic->rows; y++) {
		for (int x = 0; x < darkerPic->cols; x++) {
			darkerPic->at<Vec3b>(y, x)[0] = saturate_cast<uchar>(
					brightRate * (darkerPic->at<Vec3b>(y, x)[0]));
			darkerPic->at<Vec3b>(y, x)[1] = saturate_cast<uchar>(
					brightRate * (darkerPic->at<Vec3b>(y, x)[1]));
			darkerPic->at<Vec3b>(y, x)[2] = saturate_cast<uchar>(
					brightRate * (darkerPic->at<Vec3b>(y, x)[2]));
		}
	}

}

// (variable kernelSize MUST be odd) -> Blur both images so blurriness will not continue to be a difference but a common characteristic
void AntiShake::compensateBlurriness(Mat &src, Mat &output, int kernelSize) {
	blur(src, output, Size(kernelSize, kernelSize), Point(-1, -1));
}

void AntiShake::sobelOperator(Mat &src, Mat &output, int scale, int delta) {
	int ddepth = CV_16S;

	GaussianBlur(src, output, Size(3, 3), 0, 0, BORDER_DEFAULT);
//	blur( src1, output1, Size( blurSize, blurSize), Point(point, point) );

	if (output.type() == 16) {
		/// Convert it to gray
		cvtColor(output, output, CV_RGB2GRAY);
	}
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;

	int border = BORDER_ISOLATED;
	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(output, grad_x, ddepth, 1, 0, 3, scale, delta, border);
	convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(output, grad_y, ddepth, 0, 1, 3, scale, delta, border);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	grad.copyTo(output);

	grad_x.release();
	grad_y.release();
	abs_grad_x.release();
	abs_grad_y.release();
}

// Shows the image in a window, allowing it to be saved in a
void AntiShake::displayWindow(Mat image, string fileName, bool mightSave) {
	namedWindow(fileName); //Define the window
	imshow(fileName, image);
	if (mightSave) {
		fileName.append(".jpg");
		imwrite(fileName, image); // Saves the image
	}
}

Mat AntiShake::BorderDetector(Mat src, int type) {
	Mat dst;
	if (type == 0) {
		// ----- Sobel:
		Mat grad_x, grad_y, grad;
		Mat abs_grad_x, abs_grad_y;
		int ddepth = CV_16S;

		int border = BORDER_ISOLATED;
		Sobel(src, grad_x, ddepth, 1, 0, 3, 1, 1, border);
		convertScaleAbs(grad_x, abs_grad_x);

		Sobel(src, grad_y, ddepth, 0, 1, 3, 1, 1, border);
		convertScaleAbs(grad_y, abs_grad_y);

		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
		grad.copyTo(dst);
		//	threshold(dst, dst, 150, 255, cv::THRESH_TOZERO);
	} else if (type == 1) {
		// ----- Canny Corner Detector
		Canny(src, dst, 200, 6000, 5, true);
	} else if (type == 2) {
		// ----- Laplacian:
		Laplacian(src, dst, CV_8UC1, 1, 1, 1);
	} else {
		// ----- Magic Laplacian:
		cv::Laplacian(src, dst, CV_32F, 9);
		double scale = -1.0;
		if (scale < 0) {
			double lapmin, lapmax;
			cv::minMaxLoc(dst, &lapmin, &lapmax);
			scale = 255 / std::max(-lapmin, lapmax);
		}
		dst.convertTo(dst, CV_8U, scale, 256);
	}
	return dst;
}

// Shows the image in a window
void AntiShake::displayWindow(Mat image, string filename) {
	displayWindow(image, filename, false);
}

// Prints the given info String
void AntiShake::readme(string info) {
	std::cout << info << std::endl;
}

AntiShake::~AntiShake() {
}

