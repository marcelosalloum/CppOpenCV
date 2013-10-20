// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/opencv.hpp"
#include <iostream>

class CheeseOpenCV {
public:
    // int x0, y0, x1, y1;
    CheeseOpenCV();
    // Rectangle(int x0, int y0, int x1, int y1);
    ~CheeseOpenCV();
    // int getLength ();
    //     int getHeight();
    //     int getArea();
    //     void move(int dx, int dy);
    void openCV(char* path1, char* path2);
	void normalizeBright(char* path1, char* path2);
    // Rectangle operator+(const Rectangle& other);
};
