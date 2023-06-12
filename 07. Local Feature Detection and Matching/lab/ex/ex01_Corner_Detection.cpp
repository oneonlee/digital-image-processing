#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;

void cvHarrisCorner(); // Harris Corner Detection - OpenCV Implementation
void myHarrisCorner(); // Harris Corner Detection - Low Level Implementation

int main()
{
    cvHarrisCorner();

    return 0;
}

void cvHarrisCorner() // Harris Corner Detection - OpenCV Implementation
{
    Mat img = imread("church.jpg"); // 이미지 읽기
    if (img.empty())                // 예외 처리
    {
        cout << "Could not open or find the image" << endl;
        exit(-1);
    }

    resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // < Do Harris Corner Detection >
    Mat harr;
    cornerHarris(gray, harr, 2, 3, 0.05, BORDER_DEFAULT);
    normalize(harr, harr, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

    // < Get abs for Harris visualization >
    Mat harr_abs;
    convertScaleAbs(harr, harr_abs);

    // < Print corners >
    int thresh = 125;
    Mat result = img.clone();
    for (int y = 0; y < harr.rows; y++)
    {
        for (int x = 0; x < harr.cols; x++)
        {
            if ((int)harr.at<float>(y, x) > thresh)
            {
                circle(result, Point(x, y), 7, Scalar(255, 0, 255), 0, 4, 0);
            }
        }
    }

    imshow("Source Image", img);
    imshow("Harris Image", harr_abs);
    imshow("Target Image", result);
    waitKey(0);
    destroyAllWindows();
}

void myHarrisCorner() // Harris Corner Detection - Low Level Implementation
{
    Mat img = imread("church.jpg"); // 이미지 읽기
    if (img.empty())                // 예외 처리
    {
        cout << "Could not open or find the image" << endl;
        exit(-1);
    }

    resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    int height = gray.rows;
    int width = gray.cols;

    // < Get gradient >
    Mat blur;
    GaussianBlur(gray, blur, Size(3, 3), 1);

    Mat gx, gy;
    cv::Sobel(blur, gx, CV_64FC1, 1, 0, 3, 0.4, 128);
    cv::Sobel(blur, gy, CV_64FC1, 0, 1, 3, 0.4, 128);
    double *gx_data = (double *)gx.data;
    double *gy_data = (double *)gy.data;

    // < Get score >
    Mat harr = Mat(height, width, CV_64FC1, Scalar(0));
    double *harr_data = (double *)harr.data;
    double k = 0.02;

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int center = y * width + x;

            double dx = 0, dy = 0, dxdy = 0;
            for (int u = -1; u <= 1; u++)
            {
                for (int v = -1; v <= 1; v++)
                {
                    int cur = center + u * width + v;

                    double ix = *(gx_data + cur);
                    double iy = *(gy_data + cur);
                    dx += ix * ix;
                    dy += iy * iy;
                    dxdy += ix * iy;
                }
            }
            *(harr_data + center) = dx * dy - dxdy * dxdy - k * (dx + dy) * (dx + dy);
        }
    }

    // < Detect corner by score >
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int center = y * width + x;
            double value = *(harr_data + center);

            bool isMaximum = true, isMinimum = true;
            for (int u = -1; u <= 1; u++)
            {
                for (int v = -1; v <= 1; v++)
                {
                    if (u != 0 || v != 0)
                    {
                        int cur = center + u * width + v;

                        double neighbor = *(harr_data + cur);
                        if (value < neighbor)
                            isMaximum = false;
                        else if (value > neighbor)
                            isMinimum = false;
                    }
                }
            }
            if (isMaximum == false && isMinimum == false)
                *(harr_data + center) = 0;
            else
                *(harr_data + center) = value;
        }
    }

    // <Print corners >
    Mat result = img.clone();
    double thresh = 0.1;
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int center = y * width + x;

            if (*(harr_data + center) > thresh)
                circle(result, Point(x, y), 7, Scalar(255, 0, 255), 0, 4, 0);
        }
    }

    imshow("Source Image", img);
    imshow("Harris Image", harr);
    imshow("Target Image", result);
    waitKey(0);
    destroyAllWindows();
}