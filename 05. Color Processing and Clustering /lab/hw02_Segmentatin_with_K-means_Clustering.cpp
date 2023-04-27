#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

Mat segmentationByAverageWithCVKMeans(Mat src_img, int k);

int main()
{
    Mat src_img = imread("beach.jpg", 1);
    Mat dst_img;
    Mat res_img;

    // K=3
    dst_img = segmentationByAverageWithCVKMeans(src_img, 3);
    hconcat(src_img, dst_img, res_img);

    // K=5
    dst_img = segmentationByAverageWithCVKMeans(src_img, 5);
    hconcat(res_img, dst_img, res_img);

    // K=7
    dst_img = segmentationByAverageWithCVKMeans(src_img, 7);
    hconcat(res_img, dst_img, res_img);

    // K=9
    dst_img = segmentationByAverageWithCVKMeans(src_img, 9);
    hconcat(res_img, dst_img, res_img);

    // 결과 영상 출력
    imshow("[K=3, 5, 7, 9] segmentation by average with KMeans() of OpenCV", res_img);
    waitKey(0);
    destroyAllWindows();

    return 0;
}

// OpenCV kmeans() 함수를 이용한 k-means clustering
Mat segmentationByAverageWithCVKMeans(Mat src_img, int k)
{
    //< 2차원 영상 -> 1차원 벡터 >
    Mat samples(src_img.rows * src_img.cols, src_img.channels(), CV_32F);
    for (int y = 0; y < src_img.rows; y++)
    {
        for (int x = 0; x < src_img.cols; x++)
        {
            if (src_img.channels() == 3)
            {
                for (int z = 0; z < src_img.channels(); z++)
                {
                    samples.at<float>(y + x * src_img.rows, z) = (float)src_img.at<Vec3b>(y, x)[z];
                }
            }
            else
            {
                samples.at<float>(y + x * src_img.rows) = (float)src_img.at<uchar>(y, x);
            }
        }
    }

    //< OpenCV K-means 수행 >
    Mat labels;  // 군집판별 결과가 담길 1차원 벡터
    Mat centers; // 각 군집의 중앙값 (대표값)
    int attempts = 5;
    kmeans(samples, k, labels,
           TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001),
           attempts, KMEANS_PP_CENTERS,
           centers);

    // < 1차원 벡터 -> 2차원 영상 >
    Mat dst_img(src_img.size(), src_img.type());
    vector<Vec3f> mean_colors(k); // 각 군집의 평균 색상을 저장할 벡터
    vector<int> count(k, 0);      // 각 군집 내 픽셀 수를 저장할 벡터

    // 각 군집 내 픽셀 값을 추출하여 평균값을 계산
    for (int y = 0; y < src_img.rows; y++)
    {
        for (int x = 0; x < src_img.cols; x++)
        {
            int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
            if (src_img.channels() == 3)
            {
                for (int z = 0; z < src_img.channels(); z++)
                {
                    // 군집 판별 결과에 따라 각 군집의 평균값으로 결과를 생성하기 위해 총합을 먼저 구함
                    mean_colors[cluster_idx][z] += src_img.at<Vec3b>(y, x)[z];
                }
            }
            else
            {
                mean_colors[cluster_idx][0] += src_img.at<uchar>(y, x);
            }
            count[cluster_idx]++;
        }
    }

    // 각 군집 내의 픽셀 값의 평균을 계산하여 결과 생성
    for (int i = 0; i < k; i++)
    {
        if (count[i] == 0)
            continue;
        mean_colors[i] /= count[i];

        if (src_img.channels() == 3)
        {
            Vec3b color(mean_colors[i][0], mean_colors[i][1], mean_colors[i][2]);
            for (int y = 0; y < src_img.rows; y++)
            {
                for (int x = 0; x < src_img.cols; x++)
                {
                    int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
                    if (cluster_idx == i)
                    {
                        dst_img.at<Vec3b>(y, x) = color;
                    }
                }
            }
        }
        else
        {
            uchar gray = static_cast<uchar>(mean_colors[i][0]);
            for (int y = 0; y < src_img.rows; y++)
            {
                for (int x = 0; x < src_img.cols; x++)
                {
                    int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
                    if (cluster_idx == i)
                    {
                        dst_img.at<uchar>(y, x) = gray;
                    }
                }
            }
        }
    }

    return dst_img;
}