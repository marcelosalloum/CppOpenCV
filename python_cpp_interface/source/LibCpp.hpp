/*
 * LibCpp.hpp
 *
 *  Created on: Aug 26, 2013
 *      Author: marcelosalloum
 */

#ifndef LIBCPP_HPP_
#define LIBCPP_HPP_


int add(int a, int b);
void antiShake(std::string path1, std::string path2, int loops = 1, double final_pic_size = 0,
		double maxDetDiff = 0.15, int featurePoints = 250, int coreSize = 4, double absoluteRelation = 2.0);
void normalizeBrightness(std::string path1, std::string path2, double brightRate = 1.0, std::string outputName = "normalizeBright");

#endif /* LIBCPP_HPP_ */
