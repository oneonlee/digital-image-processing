#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"       // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

Mat myCopy(Mat srcImg)
{
    int width = srcImg.cols;
    int height = srcImg.rows;
    Mat dstImg(srcImg.size(), CV_8UC1); // 입력 영상과 동일한 크기의 Mat 생성
    uchar *srcData = srcImg.data;       // Mat 객체의 data를 가리키는 포인터
    uchar *dstData = dstImg.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = srcData[y * width + x];
            // 화소 값을 일일이 읽어와 다른 배열에 저장
        }
    }

    return dstImg;
}