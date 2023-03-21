#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"       // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

vector<Mat> myGaussainPyramid(Mat srcImg)
{
    vector<Mat> Vec; // 여러 영상을 모아서 저장하기 위해 STL의 vector 컨테이너 사용

    Vec.push_back(srcImg);
    for (int i = 0; i < 4; i++)
    {
#if USE OPENCV
        pyrDown(srcImg, srcImg, size(srcImg.cols / 2, srcImg.rows / 2));
        // Down sampling과 Gaussian filter가 포함된 OpenCV 함수
        // 영상의 크기가 가로, 세로 절반으로 줄어들도록 출력사이즈 지정

#else
        srcImg = myGaussianFilter(srcImg); // 앞서 구현한 Gaussian filtering
        srcImg = mySampling(srcImg);       // 앞서 구현한 down sampling

#endif
        Vec.push_back(srcImg); // vector 컨테이너에 하나씩 처리결과를 삽입
    }

    return Vec;
}

Mat mySampling(Mat srcImg)
{
    int width = srcImg.cols / 2;
    int height = srcImg.rows / 2;
    Mat dstImg(height, width, CV_8UC1);
    // 가로 세로가 입력 영상의 절반인 영상을 먼저 생성
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = srcData[(y * 2) * (width * 2) + (x * 2)];
            // 2배 간격으로 인덱싱 해 큰 영상을 작은 영상에 대입할 수 있음
        }
    }
    return dstImg;
}

Mat myGaussianFilter(Mat srcImg)
{
#if USE_OPENCV // OpenCV로 구현한 것
    Mat dstImg(srcImg.size(), CV_8UC1);
    cv::GaussianBlur(srcImg, dstImg, Size(3, 3), 0);
    // 마스크의 크기를 지정하면 자체적으로 마스크 생성 후 연산

    return dstImg;

#else // 직접 구현한 것 (매크로에 의해 컴파일 시 선택됨)
    int width = srcImg.cols;
    int height = srcImg.rows;
    int kernel[3][3] = {1, 2, 1,
                        2, 4, 2,
                        1, 2, 1}; // 3x3 형태의 Gaussian 마스크 배열
    Mat dstImg(srcImg.size(), CV_8UC1);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = myKernelConv3x3(srcData, kernel, x, y, width, height);
            // 앞서 구현한 convolution에 마스크 배열을 입력해 사용
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