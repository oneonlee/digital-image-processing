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

void cvPanorama();
Mat makePanorama(Mat img_l, Mat img_r, int thresh_dist, int min_matches);
void myPanorama();

int main()
{
    cvPanorama();
    myPanorama();

    return 0;
}

// Stitcher를 이용한 파노라마 영상 생성
void cvPanorama()
{
    Mat img;
    vector<Mat> imgs;
    img = imread("img/img1.jpg", IMREAD_COLOR);
    imgs.push_back(img);
    img = imread("img/img2.jpg", IMREAD_COLOR);
    imgs.push_back(img);
    img = imread("img/img3.jpg", IMREAD_COLOR);
    imgs.push_back(img);

    Mat result;
    Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);
    Stitcher::Status status = stitcher->stitch(imgs, result);
    if (status != Stitcher::OK)
    {
        // 공통되는 영역이 너무 적으면 error code 1을 출력
        cout << "Can't stitch images, error code = " << int(status) << endl;
        exit(-1);
    }

    imshow("hw01_cvPanorama_result", result);
    waitKey();
    destroyAllWindows();
}

Mat makePanorama(Mat img_l, Mat img_r, int thresh_dist, int min_matches)
{
    // [ SURF 특징점 (feature) 추출 ]

    // < Grayscale로 변환 >
    Mat img_gray_l, img_gray_r;
    cvtColor(img_l, img_gray_l, CV_BGR2GRAY);
    cvtColor(img_r, img_gray_r, CV_BGR2GRAY);

    // < 특징점 (key point) 추출 >
    Ptr<SurfFeatureDetector> Detector = SURF::create(300);
    vector<KeyPoint> kpts_obj, kpts_scene;
    Detector->detect(img_gray_l, kpts_obj);
    Detector->detect(img_gray_r, kpts_scene);

    // < 특징점 시각화 >
    Mat img_kpts_l, img_kpts_r;
    drawKeypoints(img_gray_l, kpts_obj, img_kpts_l,
                  Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    drawKeypoints(img_gray_r, kpts_scene, img_kpts_r,
                  Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    // [ 기술자 (descriptor) 추출 및 brute force 매칭 ]

    // < 기술자 (descriptor) 추출 >
    Ptr<SurfDescriptorExtractor> Extractor =
        SURF::create(100, 4, 3, false, true);
    Mat img_dst_obj, img_dst_scene;
    Extractor->compute(img_gray_l, kpts_obj, img_dst_obj);
    Extractor->compute(img_gray_r, kpts_scene, img_dst_scene);

    // < 기술자를 이용한 특징점 매칭 >
    BFMatcher matcher(NORM_L2);
    vector<DMatch> matches;
    matcher.match(img_dst_obj, img_dst_scene, matches);

    // < 매칭 결과 시각화 >
    Mat img_matches;
    drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
                matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // [ 매칭 결과 정제 ]

    // 매칭 거리가 작은 우수한 매칭 결과를 정제하는 과정
    // 최소 매칭 거리의 3배 또는 우수한 매칭 결과 60 이상까지 정제
    double dist_max = matches[0].distance;
    double dist_min = matches[0].distance;
    double dist;
    for (int i = 0; i < img_dst_obj.rows; i++)
    {
        dist = matches[i].distance;
        if (dist < dist_min)
            dist_min = dist;
        if (dist > dist_max)
            dist_max = dist;
    }
    printf("max_dist : %f\n", dist_max); // max는 사실상 불필요
    printf("min_dist : %f\n", dist_min);

    vector<DMatch> matches_good;
    do
    {
        vector<DMatch> good_matches2;
        for (int i = 0; i < img_dst_obj.rows; i++)
        {
            if (matches[i].distance < thresh_dist * dist_min)
                good_matches2.push_back(matches[i]);
        }
        matches_good = good_matches2;
        thresh_dist -= 1;
    } while (thresh_dist != 2 && matches_good.size() > min_matches);

    // < 우수한 매칭 결과 시각화 >
    Mat img_matches_good;
    drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
                matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // [ Homography 행렬 추출 및 시점 역 변환]

    // < 매칭 결과 좌표 추출 >
    vector<Point2f> obj, scene;
    for (int i = 0; i < matches_good.size(); i++)
    {
        obj.push_back(kpts_obj[matches_good[i].queryIdx].pt);     // img1
        scene.push_back(kpts_scene[matches_good[i].trainIdx].pt); // img2
    }

    // < 매칭 결과로부터 homography 행렬을 추출 >
    Mat mat_homo = findHomography(scene, obj, RANSAC);
    // 이상치 제거를 위해 RANSAN 추가

    // < Homography 행렬을 이용해 시점 역변환 >
    Mat img_result;
    warpPerspective(img_r, img_result, mat_homo,
                    Size(img_l.cols * 2, img_l.rows * 1.2),
                    INTER_CUBIC);
    // 영상이 잘리는 것을 방지하기 위해 여유 공간을 부여

    // [ 기준 영상과 역변환된 시점 영상 합체 (stitching) ]
    Mat img_pano;
    img_pano = img_result.clone();
    Mat roi(img_pano, Rect(0, 0, img_l.cols, img_l.rows));
    img_l.copyTo(roi);

    // < 검은 여백 잘라내기 >
    int cut_x = 0, cut_y = 0;
    for (int y = 0; y < img_pano.rows; y++)
    {
        for (int x = 0; x < img_pano.cols; x++)
        {
            if (img_pano.at<Vec3b>(y, x)[0] == 0 &&
                img_pano.at<Vec3b>(y, x)[1] == 0 &&
                img_pano.at<Vec3b>(y, x)[2] == 0)
            {
                continue;
            }
            if (cut_x < x)
                cut_x = x;
            if (cut_y < y)
                cut_y = y;
        }
    }

    Mat img_pano_cut;
    img_pano_cut = img_pano(Range(0, cut_y), Range(0, cut_x));

    return img_pano_cut;
}

void myPanorama()
{
    Mat matImg1 = imread("img/img1.jpg", IMREAD_COLOR);
    Mat matImg2 = imread("img/img2.jpg", IMREAD_COLOR);
    Mat matImg3 = imread("img/img3.jpg", IMREAD_COLOR);
    if (matImg1.empty() || matImg2.empty() || matImg3.empty())
        exit(-1);

    Mat result;
    flip(matImg1, matImg1, 1);
    flip(matImg2, matImg2, 1);
    result = makePanorama(matImg1, matImg2, 3, 60);
    flip(result, result, 1);
    result = makePanorama(result, matImg3, 3, 60);

    imshow("hw01_myPanorama_result", result);
    // imwrite("result/hw01_myPanorama_result.png", result);
    waitKey();
    destroyAllWindows();
}
