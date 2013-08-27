#include "cpp_rect.h"
#include "LibCpp.hpp"

Rectangle::Rectangle() {
    x0 = y0 = x1 = y1 = 0;
}

Rectangle::Rectangle(int a, int b, int c, int d) {
    x0 = a;
    y0 = b;
    x1 = c;
    y1 = d;  
}

Rectangle::~Rectangle() {
}

void Rectangle::openCV(char* path1, char* path2){
	std::cout << "HELLO WORLD" << std::endl;
	// std::string file_1 = "/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/Debug/aux/foot0.jpeg";
	// std::string file_2 = "/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/Debug/aux/foot1.jpeg";

	antishake(path1, path2);
	// cv::Mat img1(7,7,CV_32FC2,Scalar(1,3));
	// cout << img1.rows<<endl;
	// cout << add(3,8) <<endl;
	// AntiShake *aux = new AntiShake();
// cv::Mat img1 = cv::imread(file_1);
// 	cv::Mat img2 = cv::imread(file_2);
// 	
// AntiShake *aux = AntiShake::getInstance();
// 	cv::Mat H = aux->fixPictures(img1, img2, 1);
// 	
// 	aux->displayWindow(img1, "~/Desktop/fix_1", true);
// 	aux->displayWindow(img2, "~/Desktop/fix_2", true);
// 	
// 	//  ==== storing data ====
// 	std::stringstream ss;
// 	ss << H;
// 	std::cout << "++++++" << ss.str().c_str() << std::endl;
}

int Rectangle::getLength() {
    return x1-x0;
}

int Rectangle::getHeight() {
    return y1-y0;
}

int Rectangle::getArea() {
    return getLength()*getHeight();
}

void Rectangle::move(int dx, int dy) {
    x0 += dx;
    x1 += dx;
    y0 += dy;
    y1 += dy;
}

Rectangle Rectangle::operator+(const Rectangle& other) {
    return Rectangle(x0 < other.x0 ? x0 : other.x0,
                     y0 < other.y0 ? y0 : other.y0,
                     x1 > other.x1 ? x1 : other.x1,
                     y1 > other.y1 ? y1 : other.y1);
}
