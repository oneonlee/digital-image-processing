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
    Mat hsv_img, dst_img;

    cvtColor(src_img, hsv_img, CV_BGR2HSV);
    inRange(hsv_img, cv::Scalar(35, 20, 30), cv::Scalar(85, 230, 230), dst_img);

    imshow("Test window", dst_img); // 이미지 출력
    waitKey(0);                     // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("Test window");   // 이미지 출력창 종료
    
    return 0;
}