#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

int myKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height);
Mat myDiagonalSobelFilter(Mat srcImg);

int main()
{
    // Step 0
    Mat original_gray_img = imread("gear.jpg", 0);
    imshow("original_gray_img", original_gray_img);

    // Step 1
    Mat filtered_img = myDiagonalSobelFilter(original_gray_img);
    imshow("filtered_img", filtered_img);

    // Etc
    waitKey(0);
    destroyWindow("original_gray_img");
    destroyWindow("filtered_img");

    return 0;
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

Mat myDiagonalSobelFilter(Mat srcImg)
{
    int kernel45[3][3] = {0, 1, 2,
                          1, 0, -1,
                          -2, -1, 0}; // 3x3 형태의 45도 대각선 방향 Sobel 마스크 배열
    int kernel135[3][3] = {2, 1, 0,
                           -1, 0, 1,
                           0, -1, -2}; // 3x3 형태의 135도 대각선 방향 Sobel 마스크 배열

    Mat dstImg(srcImg.size(), CV_8UC1);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;
    int width = srcImg.cols;
    int height = srcImg.rows;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernel45, x, y, width, height)) +
                                      abs(myKernelConv3x3(srcData, kernel135, x, y, width, height))) /
                                     2;
            // 두 에지 결과의 절대값 합 형태로 최종결과 도출
        }
    }

    return dstImg;
}