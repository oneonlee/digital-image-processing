#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"       // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

Mat mySobelFilter(Mat srcImg)
{
#if USE_OPENCV // OpenCV로 구현한 것
    Mat dstImg(srcImg.size(), CV_8UC1);
    Mat sobelX, sobelY;

    Sobel(srcImg, sobelX, CV_8UC1, 1, 0); // 가로방향 Sobel
    Sobel(srcImg, sobelY, CV_8UC1, 0, 1); // 세로방향 Sobel
    dstImg = (abs(sobelX) + abs(sobel(Y))) / 2;
    // 두 에지 결과의 절대값 합 형태로 최종결과 도출

    return dstImg;

#else // 직접 구현한 것 (매크로에 의해 컴파일 시 선택됨)
    int kernelX[3][3] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1}; // 3x3 형태의 가로방향 Sobel 마스크 배열
    int kernelY[3][3] = {-1, -2, -1,
                         0, 0, 0,
                         1, 2, 1}; // 3x3 형태의 세로방향 Sobel 마스크 배열
    // 마스크 합이 0이 되므로 1로 정규화하는 과정은 필요 없음

    Mat dstImg(srcImg.size(), CV_8UC1);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;
    int width = srcImg.cols;
    int height = srcImg.rows;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernelX, x, y, width, height)) +
                                      abs(myKernelConv3x3(srcData, kernelY, x, y, width, height))) /
                                     2;
            // 두 에지 결과의 절대값 합 형태로 최종결과 도출
        }
    }

    return dstImg;
#endif
}

int myKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height)
{
    int sum = 0;
    int sumKernel = 0;

    // 특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width)
            // 영상 가장자리에서 영상 밖의 화소를 읽지 않도록 하는 조건문
            {
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
            }
        }
    }
    if (sumKernel != 0)
    {
        // 합이 1로 정규화되도록 하여 영상의 밝기변화 방지
        return sum / sumKernel;
    }
    else
    {
        return sum;
    }
}