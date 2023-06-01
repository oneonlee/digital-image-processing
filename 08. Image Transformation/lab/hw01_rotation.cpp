#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

Mat getMyRotationMatrix(Point center, double x, double y);
void myRotation();
void cvRotation();

int main()
{
    myRotation();
    cvRotation();
    return 0;
}

Mat getMyRotationMatrix(Point center, double x, double y)
{
    double angle = x * CV_PI / 180.0;

    float mcos = cos(angle);
    float a = y * mcos;
    float msin = sin(angle);
    float b = y * msin;

    Mat matrix = (Mat_<float>(2, 3) << a, b, (1 - a) * center.x - b * center.y,
                  -b, a, b * (center.x) + (1 - a) * center.y);

    return matrix;
}

void myRotation()
{
    Mat src = imread("Lenna.png", 1);
    Mat dst, matrix;

    Point center = Point(src.cols / 2, src.rows / 2);
    matrix = getMyRotationMatrix(center, 45.0, 1.0);
    warpAffine(src, dst, matrix, src.size());

    imwrite("nonrot.jpg", src);
    imwrite("rot.jpg", dst);

    imshow("mynonrot", src);
    imshow("myrot", dst);
    waitKey(0);
}

void cvRotation()
{
    Mat src = imread("Lenna.png", 1);
    Mat dst, matrix;

    Point center = Point(src.cols / 2, src.rows / 2);
    matrix = cv::getRotationMatrix2D(center, 45.0, 1.0);
    warpAffine(src, dst, matrix, src.size());

    imwrite("nonrot.jpg", src);
    imwrite("rot.jpg", dst);

    imshow("nonrot", src);
    imshow("rot", dst);
    waitKey(0);

    destroyAllWindows();
}