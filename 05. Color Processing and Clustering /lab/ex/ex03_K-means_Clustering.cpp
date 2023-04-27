#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

Mat CvKMeans(Mat src_img, int k);

int main()
{
    // Mat src_img = imread("fruits.png", 1);
    Mat src_img = imread("beach.jpg", 1);
    Mat dst_img = CvKMeans(src_img, 3);

    Mat res_img;
    hconcat(src_img, dst_img, res_img);
    imshow("res_img", res_img);

    waitKey(0);
    destroyAllWindows();
    return 0;
}

// OpenCV kmeans() 함수를 이용한 k-means clustering
Mat CvKMeans(Mat src_img, int k)
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
    for (int y = 0; y < src_img.rows; y++)
    {
        for (int x = 0; x < src_img.cols; x++)
        {
            int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
            if (src_img.channels() == 3)
            {
                for (int z = 0; z < src_img.channels(); z++)
                {
                    dst_img.at<Vec3b>(y, x)[z] =
                        (uchar)centers.at<float>(cluster_idx, z);
                    // 군집 판별 결과에 따라 각 군집의 중앙값으로 결과 생성
                }
            }
            else
            {
                dst_img.at<uchar>(y, x) =
                    (uchar)centers.at<float>(cluster_idx, 0);
            }
        }
    }
    return dst_img;
}