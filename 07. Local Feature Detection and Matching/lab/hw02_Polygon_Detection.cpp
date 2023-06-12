#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;

Mat cvHarrisCorner(Mat &img);
void cvBlobDetection(Mat &img);
void cvPolygonDetection(Mat polygon[4]);

int main()
{
    Mat polygon[4]; // 삼각형, 사각형, 오각형, 육각형 이미지를 읽어 polygon 배열에 넣음
    polygon[0] = imread("triangle.jpg");
    polygon[1] = imread("rectangle.jpg");
    polygon[2] = imread("pentagon.jpg");
    polygon[3] = imread("hexagon.jpg");

    cvPolygonDetection(polygon);

    return 0;
}

Mat cvHarrisCorner(Mat &img)
{
    resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // < Do Harris Corner Detection >
    Mat harr;
    cornerHarris(gray, harr, 5, 3, 0.05, BORDER_DEFAULT);
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

    return result;
}

void cvBlobDetection(Mat &img)
{
    // < Set params >
    SimpleBlobDetector::Params params;
    params.minThreshold = 10;
    params.maxThreshold = 300;
    params.filterByArea = true;
    params.minArea = 10;
    params.filterByCircularity = true;
    params.minCircularity = 0.6;
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
    imshow("keypoints (The number of angles in a polygon: " + to_string(keypoints.size()) + ")", result); // size()를 통해 원의 개수 구하기
    waitKey(0);
    destroyAllWindows();
}

void cvPolygonDetection(Mat polygon[4])
{
    for (int i = 0; i < 4; i++)
    { // 사각형부터 육각형까지 4~6
        Mat img = polygon[i];

        if (img.empty())
        {
            cout << "Empty image!\n";
            exit(1);
        }

        Mat result = cvHarrisCorner(img);
        cvBlobDetection(result); // blob detection 실행
        imshow("Source image", img);
        waitKey(0);
    }
    destroyAllWindows();
}