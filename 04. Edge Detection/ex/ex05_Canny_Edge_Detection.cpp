#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

void nonMaximumSuppression(Mat &magnitudeImage, Mat &directionImage);
void followEdges(int x, int y, Mat &magnitude, int tUpper, int tLower, Mat &edges);
void edgeDetect(Mat &magnitude, int tUpper, int tLower, Mat &edges);
void doCannyEx();

const float sigma = 1.5;
const int highThreshold = 200;
const int lowThreshold = 100;

int main()
{
    doCannyEx();

    return 0;
}

void nonMaximumSuppression(Mat &magnitudeImage, Mat &directionImage)
{
    Mat checkImage = Mat(magnitudeImage.rows, magnitudeImage.cols, CV_8U);
    MatIterator_<float> itMag = magnitudeImage.begin<float>();
    MatIterator_<float> itDirection = directionImage.begin<float>();
    MatIterator_<unsigned char> itRet = checkImage.begin<unsigned char>();
    MatIterator_<float> itEnd = magnitudeImage.end<float>();

    for (; itMag != itEnd; ++itDirection, ++itRet, ++itMag)
    {
        const Point pos = itRet.pos();

        float currentDirection = atan(*itDirection) * (180 / 3.142);

        while (currentDirection < 0)
            currentDirection += 180;

        *itDirection = currentDirection;

        if (currentDirection > 22.5 && currentDirection <= 67.5)
        {
            if (pos.y > 0 && pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x - 1))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
            if (pos.y < magnitudeImage.rows - 1 && pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x + 1))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
        }
        else if (currentDirection > 67.5 && currentDirection <= 112.5)
        {
            if (pos.y > 0 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
            if (pos.y < magnitudeImage.rows - 1 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
        }
        else if (currentDirection > 112.5 && currentDirection <= 157.5)
        {
            if (pos.y > 0 && pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x + 1))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
            if (pos.y < magnitudeImage.rows - 1 && pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x - 1))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
        }
        else
        {
            if (pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y, pos.x - 1))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
            if (pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y, pos.x + 1))
            {
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            }
        }
    }
}
void followEdges(int x, int y, Mat &magnitude, int tUpper, int tLower, Mat &edges)
{
    edges.at<float>(y, x) = 255;

    // < 이웃 픽셀 인덱싱 >
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if ((i != 0) && (j != 0) && (x + i >= 0) && (y + j >= 0) &&
                (x + i < magnitude.cols) && (y + j < magnitude.rows)) // 가장자리 예외처리
            {
                if ((magnitude.at<float>(y + j, x + i) > tLower) &&
                    (edges.at<float>(y + j, x + i) != 255))
                {
                    followEdges(x + i, y + j, magnitude, tUpper, tLower, edges);
                    // 재귀적 방법으로 이웃 픽셀에서 불확실한 edge를 찾아 edge로 규정
                }
            }
        }
    }
}
void edgeDetect(Mat &magnitude, int tUpper, int tLower, Mat &edges)
{
    int rows = magnitude.rows;
    int cols = magnitude.cols;

    edges = Mat(magnitude.size(), CV_32F, 0.0);

    // < 픽셀 인덱싱 >
    for (int x = 0; x < cols; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            if (magnitude.at<float>(y, x) >= tUpper)
            {
                followEdges(x, y, magnitude, tUpper, tLower, edges);
                // edge가 확실하면 이와 연결된 불확실한 edge를 탐색
            }
        }
    }
}
void doCannyEx()
{
    cout << " --- doCannyEx() --- \n"
         << endl;

    // < 입력 >
    Mat src_img = imread("rock.png", 0);
    if (!src_img.data)
        printf("No image data \n");
    Mat dst_img;

#if USE_OPENCV
    // < Canny edge 탐색 수행 >
    Canny(src_img, dst_img, 180, 240);
#else
    // < Gaussian filter 기반 노이즈 제거 >
    Mat blur_img;
    GaussianBlur(src_img, blur_img, Size(3, 3), sigma);

    // < Sobel edge detection >
    Mat magX = Mat(src_img.rows, src_img.cols, CV_32F);
    Mat magY = Mat(src_img.rows, src_img.cols, CV_32F);
    Sobel(blur_img, magX, CV_32F, 1, 0, 3);
    Sobel(blur_img, magY, CV_32F, 0, 1, 3);
    Mat sum = Mat(src_img.rows, src_img.cols, CV_64F);
    Mat prodx = Mat(src_img.rows, src_img.cols, CV_64F);
    Mat prodY = Mat(src_img.rows, src_img.cols, CV_64F);
    multiply(magX, magX, prodx);
    multiply(magY, magY, prodY);
    sum = prodx + prodY;
    sqrt(sum, sum);

    Mat magnitude = sum.clone();

    // <Non-maximum suppression>
    Mat slopes = Mat(src_img.rows, src_img.cols, CV_32F);
    divide(magY, magX, slopes);
    // gradient의 방향 계산
    nonMaximumSuppression(magnitude, slopes);
    // <Edge tracking by hysteresis>
    edgeDetect(magnitude, highThreshold, lowThreshold, dst_img);
    dst_img.convertTo(dst_img, CV_8UC1);
#endif
    // < 출력 >
    Mat result_img;
    hconcat(src_img, dst_img, result_img);
    imshow("doCannyEx()", result_img);
    waitKey(0);
}