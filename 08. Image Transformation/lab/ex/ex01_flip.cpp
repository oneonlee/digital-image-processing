#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

void cvFlip()
{
    Mat src = imread("Lenna.png", 1);
    Mat dst_x, dst_y, dst_xy;

    flip(src, dst_x, 0);
    flip(src, dst_y, 1);
    flip(src, dst_xy, -1);

    imwrite("nonflip.jpg", src);
    imwrite("xflip.jpg", dst_x);
    imwrite("yflip.jpg", dst_y);
    imwrite("xyflip.jpg", dst_xy);

    imshow("nonflip", src);
    imshow("xflip", dst_x);
    imshow("yflip", dst_y);
    imshow("xyflip", dst_xy);
    waitKey(0);

    destroyAllWindows();
}

int main()
{
    cvFlip();
    return 0;
}