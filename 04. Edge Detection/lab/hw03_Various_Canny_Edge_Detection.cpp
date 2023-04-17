#include <iostream>
#include <ctime>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

void nonMaximumSuppression(Mat &magnitudeImage, Mat &directionImage);
void followEdges(int x, int y, Mat &magnitude, int lowThreshold, int highThreshold, Mat &edges);
void edgeDetect(Mat &magnitude, int lowThreshold, int highThreshold, Mat &edges);
void doCannyEdgeDetection(float sigma, int lowThreshold, int highThreshold);

const float sigma = 0.5;

int main()
{
    int lowThreshold, highThreshold;

    lowThreshold = 50;
    highThreshold = 80;

    cout << "\n < highThreshold 고정, lowThreshold 변화 >";
    for (lowThreshold = 50, highThreshold = 160; lowThreshold < highThreshold; lowThreshold += 50)
        doCannyEdgeDetection(sigma, lowThreshold, highThreshold);

    cout << "\n < lowThreshold 고정, highThreshold 변화 >";
    for (lowThreshold = 50, highThreshold = 80; highThreshold < 255; highThreshold += 80)
        doCannyEdgeDetection(sigma, lowThreshold, highThreshold);

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
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            if (pos.y < magnitudeImage.rows - 1 && pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x + 1))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
        }
        else if (currentDirection > 67.5 && currentDirection <= 112.5)
        {
            if (pos.y > 0 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            if (pos.y < magnitudeImage.rows - 1 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
        }
        else if (currentDirection > 112.5 && currentDirection <= 157.5)
        {
            if (pos.y > 0 && pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x + 1))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            if (pos.y < magnitudeImage.rows - 1 && pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x - 1))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
        }
        else
        {
            if (pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y, pos.x - 1))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
            if (pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y, pos.x + 1))
                magnitudeImage.at<float>(pos.y, pos.x) = 0;
        }
    }
}
void followEdges(int x, int y, Mat &magnitude, int lowThreshold, int highThreshold, Mat &edges)
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
                if ((magnitude.at<float>(y + j, x + i) > highThreshold) &&
                    (edges.at<float>(y + j, x + i) != 255))
                {
                    followEdges(x + i, y + j, magnitude, lowThreshold, highThreshold, edges);
                    // 재귀적 방법으로 이웃 픽셀에서 불확실한 edge를 찾아 edge로 규정
                }
            }
        }
    }
}
void edgeDetect(Mat &magnitude, int lowThreshold, int highThreshold, Mat &edges)
{
    int rows = magnitude.rows;
    int cols = magnitude.cols;

    edges = Mat(magnitude.size(), CV_32F, 0.0);

    // < 픽셀 인덱싱 >
    for (int x = 0; x < cols; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            if (magnitude.at<float>(y, x) >= lowThreshold)
            {
                followEdges(x, y, magnitude, lowThreshold, highThreshold, edges);
                // edge가 확실하면 이와 연결된 불확실한 edge를 탐색
            }
        }
    }
}
void doCannyEdgeDetection(float sigma, int lowThreshold, int highThreshold)
{
    cout << "\n================== doCannyEdgeDetection() ==================\n"
         << "sigma : " << sigma << "\t"
         << "lowThreshold : " << lowThreshold << "\t"
         << "highThreshold : " << highThreshold << endl;

    // < 입력 >
    Mat src_img = imread("edge_test.jpg", 0);
    if (!src_img.data)
        printf("No image data \n");
    Mat dst_img;

    clock_t start, finish;
    double duration;
    start = clock();
#if USE_OPENCV
    // < Canny edge 탐색 수행 >
    Canny(src_img, dst_img, lowThreshold, highThreshold);
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
    finish = clock();
    duration = (double)(finish - start);
    cout << "Process Time : " << duration << "ms (" << duration / CLOCKS_PER_SEC << "sec)" << endl;

    // < 출력 >
    Mat result_img;
    hconcat(src_img, dst_img, result_img);
    imshow("doCannyEdgeDetection()", result_img);
    waitKey(0);
}