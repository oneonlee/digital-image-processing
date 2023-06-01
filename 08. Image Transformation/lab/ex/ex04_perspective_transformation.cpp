#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

void cvPerspective()
{
    Mat src = imread("Lenna.png", 1);
    Mat dst, matrix;

    Point2f srcQuad[4];
    srcQuad[0] = Point2f(0.f, 0.f);
    srcQuad[1] = Point2f(src.cols - 1.f, 0.f);
    srcQuad[2] = Point2f(0.f, src.rows - 1.f);
    srcQuad[3] = Point2f(src.cols - 1.f, src.rows - 1.f);

    Point2f dstQuad[4];
    dstQuad[0] = Point2f(0.f, src.rows * 0.33f);
    dstQuad[1] = Point2f(src.cols * 0.85f, src.rows * 0.25f);
    dstQuad[2] = Point2f(src.cols * 0.15f, src.rows * 0.7f);
    dstQuad[3] = Point2f(src.cols * 0.85f, src.rows * 0.7f);

    matrix = getPerspectiveTransform(srcQuad, dstQuad);
    warpPerspective(src, dst, matrix, src.size());

    imwrite("nonper.jpg", src);
    imwrite("per.jpg", dst);

    imshow("nonper", src);
    imshow("per", dst);
    waitKey(0);

    destroyAllWindows();
}

int main()
{
    cvPerspective();
    return 0;
}