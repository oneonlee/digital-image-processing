#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

void cvAffine()
{
    Mat src = imread("Lenna.png", 1);
    Mat dst, matrix;

    Point2f srcTri[3];
    srcTri[0] = Point2f(0.f, 0.f);
    srcTri[1] = Point2f(src.cols - 1.f, 0.f);
    srcTri[2] = Point2f(0.f, src.rows - 1.f);

    Point2f dstTri[3];
    dstTri[0] = Point2f(0.f, src.rows * 0.33f);
    dstTri[1] = Point2f(src.cols * 0.85f, src.rows * 0.25f);
    dstTri[2] = Point2f(src.cols * 0.15f, src.rows * 0.7f);

    matrix = getAffineTransform(srcTri, dstTri);
    warpAffine(src, dst, matrix, src.size());

    imwrite("nonaff.jpg", src);
    imwrite("aff.jpg", dst);

    imshow("nonaff", src);
    imshow("aff", dst);
    waitKey(0);

    destroyAllWindows();
}

int main(){
    cvAffine();
    return 0;
}