// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/opencv.hpp"
#include <iostream>

class Rectangle {
public:
    int x0, y0, x1, y1;
    Rectangle();
    Rectangle(int x0, int y0, int x1, int y1);
    ~Rectangle();
    int getLength();
    int getHeight();
    int getArea();
    void move(int dx, int dy);
    void openCV(char* path1, char* path2);
	void normalizeBright(char* path1, char* path2);
    Rectangle operator+(const Rectangle& other);
};
