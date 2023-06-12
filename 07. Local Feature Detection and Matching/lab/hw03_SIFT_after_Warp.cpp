#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"

using namespace cv;
using namespace std;

// SIFT 특징점을 구하는 함수
Mat cvFeatureSIFT(Mat &img)
{
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
    vector<KeyPoint> keypoints;
    detector->detect(gray, keypoints);

    Mat result;
    drawKeypoints(img, keypoints, result, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    return result;
}

// 투시 변환을 수행하는 함수
Mat cvWarpPerspective(Mat &src)
{
    Mat dst;
    Point2f src_p[4], dst_p[4];

    int row = src.rows;
    int col = src.cols;

    // 변환되기 전 포인트의 지점 지정
    src_p[0] = Point2f(0, 0);
    src_p[1] = Point2f(col, 0);
    src_p[2] = Point2f(0, row);
    src_p[3] = Point2f(col, row);

    // 변환시킬 포인트의 지점 지정
    dst_p[0] = Point2f(0, 0);
    dst_p[1] = Point2f(col, 0);
    dst_p[2] = Point2f(0, row);
    dst_p[3] = Point2f(col - 200, row - 200);

    Mat perspect_mat = getPerspectiveTransform(src_p, dst_p);
    warpPerspective(src, dst, perspect_mat, Size(col, row));

    return dst;
}

// 밝기를 변화시키는 함수
Mat changeIntensity(Mat &src, int value)
{
    Mat dst = src + Scalar(value, value, value);
    return dst;
}

int main()
{
    // < 원본 영상에서 SIFT 특징점 구하기 >
    Mat origin_img = imread("church.jpg", 1);
    Mat origin_sift_result = cvFeatureSIFT(origin_img);

    // < 투시 변환 및 밝기 변환을 수행한 영상에 SIFT 특징점 구하기 >
    Mat warped_img = changeIntensity(origin_img, -50);
    warped_img = cvWarpPerspective(warped_img);
    Mat warped_sift_result = cvFeatureSIFT(warped_img);

    // < 결과 영상 출력 >
    Mat sift_results;
    hconcat(origin_sift_result, warped_sift_result, sift_results);
    imshow("Left: SIFT of the original image / Right: SIFT of the transformed image", sift_results);
    waitKey(0);
    destroyAllWindows();
    return 0;
}