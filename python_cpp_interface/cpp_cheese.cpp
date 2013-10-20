#include "cpp_cheese.h"
#include "LibCpp.hpp"

// CheeseOpenCV::CheeseOpenCV() {
//     x0 = y0 = x1 = y1 = 0;
// }

CheeseOpenCV::CheeseOpenCV() {
// enter variables
}

CheeseOpenCV::~CheeseOpenCV() {
}

void CheeseOpenCV::openCV(char* path1, char* path2){
	antiShake(path1, path2);
}

void CheeseOpenCV::normalizeBright(char* path1, char* path2){
	// std::cout << "TEST"<< std::endl;
	normalizeBrightness(path1, path2);
}

// int Rectangle::getLength() {
//     return x1-x0;
// }
// 
// int Rectangle::getHeight() {
//     return y1-y0;
// }

// int Rectangle::getArea() {
//     return getLength()*getHeight();
// }

// void Rectangle::move(int dx, int dy) {
//     x0 += dx;
//     x1 += dx;
//     y0 += dy;
//     y1 += dy;
// }

// Rectangle Rectangle::operator+(const Rectangle& other) {
//     return Rectangle(x0 < other.x0 ? x0 : other.x0,
//                      y0 < other.y0 ? y0 : other.y0,
//                      x1 > other.x1 ? x1 : other.x1,
//                      y1 > other.y1 ? y1 : other.y1);
// }
