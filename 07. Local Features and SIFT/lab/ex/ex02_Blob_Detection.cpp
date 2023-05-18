#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;

void cvBlobDetection(); // Blob Detection - OpenCV Implementation
void myBlobDetection(); // Blob Detection - Low Level Implementation

int main()
{
    cvBlobDetection();

    return 0;
}

void myBlobDetection()
{

    Mat src, src_gray, dst;
    src = cv::imread("butt.jpg", 1);
    cv::cvtColor(src, src_gray, COLOR_BGR2GRAY);

    int gau_ksize = 11;
    int lap_ksize = 3;
    int lap_scale = 1;
    int lap_delta = 1;

    GaussianBlur(src_gray, src_gray, Size(gau_ksize, gau_ksize), 3, 3, BORDER_DEFAULT);
    Laplacian(src_gray, dst, CV_64F, lap_ksize, lap_scale, lap_delta, BORDER_DEFAULT);
    // Gaussian + Laplcaian -> LoG

    normalize(-dst, dst, 0, 255, NORM_MINMAX, CV_8U, Mat());

    imwrite("my_log_dst.png", dst);
    imshow("Original Image", src);
    imshow("Laplacian Image", dst);
    waitKey(0);
    destroyAllWindows();
}

void cvBlobDetection()
{
    Mat img = imread("circle.JPG", IMREAD_COLOR);

    // < Set params >
    SimpleBlobDetector::Params params;
    params.minThreshold = 10;
    params.maxThreshold = 300;
    params.filterByArea = true;
    params.minArea = 10;
    params.filterByCircularity = true;
    params.minCircularity = 0.895;
    params.filterByConvexity = true;
    params.minConvexity = 0.9;
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;

    // < Set blob detector >
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

    // < Detect blobs >
    vector<KeyPoint> keypoints;
    detector->detect(img, keypoints);

    // < Draw blobs >
    Mat result;
    drawKeypoints(img, keypoints, result, Scalar(255, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imshow("Keypoints", result);
    waitKey(0);
    destroyAllWindows();
}