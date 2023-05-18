#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;

void cvFeatureSIFT();

int main()
{
    cvFeatureSIFT();
}

void cvFeatureSIFT()
{
    Mat img = imread("church.jpg", 1);
    if (img.empty())
    {
        cout << "Could not open or find the image" << endl;
        exit(-1);
    }

    Mat gray;
    cvtColor(img, gray, CV_BGR2GRAY);

    Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
    vector<KeyPoint> keypoints;
    detector->detect(gray, keypoints);

    Mat result;
    drawKeypoints(img, keypoints, result, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    imwrite("sift_result.jpg", result);
    imshow("Sift result", result);
    waitKey(0);
    cvDestroyAllWindows();
}