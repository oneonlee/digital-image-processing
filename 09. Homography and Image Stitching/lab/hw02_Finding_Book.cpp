#include <iostream>
#include <vector>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include "opencv2/objdetect.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/stitching.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

void findBook(Mat book, int threshDist = 3, int minMatches = 60);

int main()
{
    Mat book1 = imread("img/Book1.jpg", IMREAD_COLOR);
    Mat book2 = imread("img/Book2.jpg", IMREAD_COLOR);
    Mat book3 = imread("img/Book3.jpg", IMREAD_COLOR);
    findBook(book1);
    findBook(book2);
    findBook(book3);
    return 0;
}

void findBook(Mat book, int threshDist, int minMatches)
{
    // [ SIFT 특징점 (feature) 추출 ]

    Mat scene = imread("img/scene.jpg", 1);

    // < Grayscale로 변환 >
    Mat sceneGray, imgGray;
    cvtColor(scene, sceneGray, CV_BGR2GRAY);
    cvtColor(book, imgGray, CV_BGR2GRAY);

    // < SIFT를 이용한 특징점 (key point) 추출 >
    Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
    std::vector<KeyPoint> sceneKeypoints, imgKeypoints;
    detector->detect(sceneGray, sceneKeypoints); // scene에서 keypoints 감지
    detector->detect(imgGray, imgKeypoints);     // book에서 keypoints 감지

    // [ SIFT를 이용한 기술자 (descriptor) 추출 및 brute force 매칭 ]

    // < 기술자 (descriptor) 추출 >
    Ptr<SiftDescriptorExtractor> Extractor = SIFT::create(100, 4, 3, false, true);
    Mat imgDscBook, imgDscScene;
    Extractor->compute(imgGray, imgKeypoints, imgDscBook);
    Extractor->compute(sceneGray, sceneKeypoints, imgDscScene);

    // < 기술자를 이용한 특징점 매칭 >
    BFMatcher matcher(NORM_L2); // 거리 측정 시, L2 distance를 사용
    vector<DMatch> matches;
    matcher.match(imgDscBook, imgDscScene, matches);

    // < 매칭 결과 시각화 >
    Mat img_matches;
    drawMatches(imgGray, imgKeypoints, sceneGray, sceneKeypoints, matches, img_matches, Scalar::all(-1),
                Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // [ 매칭 결과 정제 ]

    // 매칭 거리가 작은 우수한 매칭 결과를 정제하는 과정
    // 최소 매칭 거리의 3배 또는 우수한 매칭 결과 60 이상까지 정제
    double maxDist = matches[0].distance; // distance는 기술자 간의 거리
    double minDist = matches[0].distance;
    double dist;

    // 가장 작은 매칭 거리 찾기
    for (int i = 0; i < imgDscBook.rows; i++)
    {
        dist = matches[i].distance;
        if (dist < minDist)
            minDist = dist;
        if (dist > maxDist)
            maxDist = dist; // max는 사실상 불필요
    }
    printf("maxDist : %f\n", maxDist);
    printf("minDist : %f\n", minDist);

    // < 우수한 매칭 결과 찾기 >
    vector<DMatch> goodMatches;
    do
    {
        vector<DMatch> goodMatches2;
        for (int i = 0; i < imgDscBook.rows; i++)
        {
            if (matches[i].distance < threshDist * minDist) // 최소매칭결과의 3배보다 작으면
                goodMatches2.push_back(matches[i]);         // 우수 매칭 결과
        }
        goodMatches = goodMatches2;
        threshDist -= 1;
    } while (threshDist != 2 && goodMatches.size() > minMatches); // 우수 매칭 결과 60이상이 될때까지

    // < 우수한 매칭 결과 시각화 >
    Mat imgMatchesGood;
    drawMatches(imgGray, imgKeypoints, sceneGray, sceneKeypoints, goodMatches, imgMatchesGood,
                Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // [ Homography 행렬 추출 및 시점 역 변환]

    // < 매칭 결과 좌표 추출 >
    vector<Point2f> b, s;
    for (int i = 0; i < goodMatches.size(); i++)
    {
        b.push_back(imgKeypoints[goodMatches[i].queryIdx].pt);   // book 기술자리스트에 저장된 인덱스의 좌표
        s.push_back(sceneKeypoints[goodMatches[i].trainIdx].pt); // scene 기술자리스트에 저장된 인덱스 좌표
    }

    // < 매칭 결과로부터 homography 행렬을 추출 >
    Mat homographyMatrix = findHomography(b, s, RANSAC);
    // 이상치 제거를 위해 RANSAC추가

    // < imgGray 이미지 꼭짓점을 corner1에 넣고 homography 행렬에 따라 투시변환 >
    vector<Point2f> imgCorners, sceneCorners;
    imgCorners.push_back(Point2f(0, 0));
    imgCorners.push_back(Point2f(imgGray.cols - 1, 0));
    imgCorners.push_back(Point2f(imgGray.cols - 1, imgGray.rows - 1));
    imgCorners.push_back(Point2f(0, imgGray.rows - 1));
    perspectiveTransform(imgCorners, sceneCorners, homographyMatrix);

    // < homography로 변환된 코너를 dstCorners에 대입 >
    vector<Point> dstCorners;
    for (Point2f pt : sceneCorners)
        dstCorners.push_back(Point(cvRound(pt.x + imgGray.cols), cvRound(pt.y)));

    // < dstCorners로부터 책의 윤곽 그리기 >
    polylines(imgMatchesGood, dstCorners, true, Scalar(0, 255, 0), 4, LINE_AA);

    // 결과 출력 및 저장
    imshow("Result", imgMatchesGood);

    waitKey(0);
    destroyAllWindows();
}
