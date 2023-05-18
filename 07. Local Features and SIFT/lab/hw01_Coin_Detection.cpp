#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;

void cvBlobCoinDetection(Mat &img);

int main()
{
    Mat img = imread("coin.png", 1);
    cvBlobCoinDetection(img);

    return 0;
}

void cvBlobCoinDetection(Mat &img)
{
    SimpleBlobDetector::Params params;
    params.minThreshold = 5;    // 최소 threshold값
    params.maxThreshold = 500;  // 최대 threshold값
    params.filterByArea = true; // 최소 최대 면적 제한
    params.minArea = 120;       // 최소
    params.maxArea = 9000;      // 최대

    params.filterByCircularity = true; // 원형에 얼마나 가까운지 제한
    params.minCircularity = 0.65;

    params.filterByConvexity = true; // 내부가 얼마나 볼록히 채워지는지 제한
    params.minConvexity = 0.9;

    params.filterByInertia = true; // 타원이 원에 얼마나 가까운지 제한
    params.minInertiaRatio = 0.01;

    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params); // SimpleBlobDetector 선언
    std::vector<KeyPoint> keypoints;
    detector->detect(img, keypoints); // img에서 keypoints 감지

    Mat result;
    drawKeypoints(img, keypoints, result,
                  Scalar(255, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    imshow("keypoints (The number of coins: " + to_string(keypoints.size()) + ")", result); // size()를 통해 원의 개수 구하기
    waitKey(0);
    destroyAllWindows();
}
