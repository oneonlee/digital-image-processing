#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

void CvColorModels(Mat bgr_img); // bgr_img를 다양한 Color Mode로 변환
Mat GetYCbCr(Mat src_img);       // BGR to YUV

int main()
{
    Mat src_img = imread("fruits.png", 1); // 이미지 읽기
    Mat dst_img = GetYCbCr(src_img);
    imshow("Test window", dst_img); // 이미지 출력
    waitKey(0);                     // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("Test window");   // 이미지 출력창 종료

    return 0;
}

void CvColorModels(Mat bgr_img) // bgr_img를 다양한 Color Mode로 변환
{
    Mat gray_img, rgb_img, hsv_img, yuv_img, xyz_img;
    cvtColor(bgr_img, gray_img, cv::COLOR_BGR2GRAY);
    cvtColor(bgr_img, rgb_img, cv::COLOR_BGR2RGB);
    cvtColor(bgr_img, hsv_img, cv::COLOR_BGR2HSV);
    cvtColor(bgr_img, yuv_img, cv::COLOR_BGR2YCrCb);
    cvtColor(bgr_img, xyz_img, cv::COLOR_BGR2XYZ);
    Mat print_img;
    bgr_img.copyTo(print_img);
    cvtColor(gray_img, gray_img, cv::COLOR_GRAY2BGR);
    hconcat(print_img, gray_img, print_img);
    hconcat(print_img, rgb_img, print_img);
    hconcat(print_img, hsv_img, print_img);
    hconcat(print_img, yuv_img, print_img);
    hconcat(print_img, xyz_img, print_img);
    imshow("results", print_img);
    imwrite("CvColorModels.png", print_img);
    waitKey(0);
}

Mat GetYCbCr(Mat src_img) // BGR to YUV
{
    double b, g, r, y, cb, cr;
    Mat dst_img;
    src_img.copyTo(dst_img);

    // < 화소 인덱싱 >
    for (int row = 0; row < dst_img.rows; row++)
    {
        for (int col = 0; col < dst_img.cols; col++)
        {
            // < BGR 취득 >
            // OpenCV의 Mat은 BGR의 순서를 가짐에 유의
            b = (double)dst_img.at<Vec3b>(row, col)[0];
            g = (double)dst_img.at<Vec3b>(row, col)[1];
            r = (double)dst_img.at<Vec3b>(row, col)[2];

            // < 색상 변환 계산 >
            // 정확한 계산을 위해 double 자료형 사용
            y = 0.2627 * r + 0.678 * g + 0.0593 * b;
            cb = -0.13963 * r - 0.36037 * g + 0.5 * b;
            cr = 0.5 * r - 0.45979 * g - 0.04021 * b;
            // <오버플로우 방지>
            y = y > 255.0 ? 255.0 : y < 0 ? 0
                                          : y;
            cb = cb > 255.0 ? 255.0 : cb < 0 ? 0
                                             : cb;
            cr = cr > 255.0 ? 255.0 : cr < 0 ? 0
                                             : cr;

            // < 변환된 색상 대입 >
            // double 자료형의 값을 본래 자료형으로 변환
            dst_img.at<Vec3b>(row, col)[0] = (uchar)y;
            dst_img.at<Vec3b>(row, col)[1] = (uchar)cb;
            dst_img.at<Vec3b>(row, col)[2] = (uchar)cr;
        }
    }
    return dst_img;
}