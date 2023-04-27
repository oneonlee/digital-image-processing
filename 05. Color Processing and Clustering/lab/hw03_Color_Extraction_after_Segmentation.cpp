#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

Mat segmentationByAverageWithCVKMeans(Mat src_img, int k);
Mat myInRange(Mat hsv_img);
Mat myBgr2Hsv(Mat src_img);

int main()
{
    Mat src_img, hsv_img, dst_img, res_img, segmented_img;

    src_img = imread("cherry.jpeg", 1);
    cout << "(Non-segmented) Cherry : ";
    hsv_img = myBgr2Hsv(src_img);
    dst_img = myInRange(hsv_img);
    hconcat(src_img, dst_img, res_img);
    segmented_img = segmentationByAverageWithCVKMeans(src_img, 3);
    hconcat(res_img, segmented_img, res_img);
    cout << "(Segmented) Cherry : ";
    hsv_img = myBgr2Hsv(segmented_img);
    dst_img = myInRange(hsv_img);
    hconcat(res_img, dst_img, res_img);
    imshow("[K = 3] Cherry : Red", res_img);

    src_img = imread("banana.jpeg", 1);
    cout << "(Non-segmented) Banana : ";
    hsv_img = myBgr2Hsv(src_img);
    dst_img = myInRange(hsv_img);
    hconcat(src_img, dst_img, res_img);
    segmented_img = segmentationByAverageWithCVKMeans(src_img, 3);
    hconcat(res_img, segmented_img, res_img);
    cout << "(Segmented) Banana : ";
    hsv_img = myBgr2Hsv(segmented_img);
    dst_img = myInRange(hsv_img);
    hconcat(res_img, dst_img, res_img);
    imshow("[K = 3] Banana : Yellow", res_img);

    src_img = imread("blueberry.jpeg", 1);
    cout << "(Non-segmented) Blueberry : ";
    hsv_img = myBgr2Hsv(src_img);
    dst_img = myInRange(hsv_img);
    hconcat(src_img, dst_img, res_img);
    segmented_img = segmentationByAverageWithCVKMeans(src_img, 3);
    hconcat(res_img, segmented_img, res_img);
    cout << "(Segmented) Blueberry : ";
    hsv_img = myBgr2Hsv(segmented_img);
    dst_img = myInRange(hsv_img);
    hconcat(res_img, dst_img, res_img);
    imshow("[K = 3] Blueberry : Blue", res_img);

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

Mat myInRange(Mat hsv_img)
{
    Mat dst_img(hsv_img.size(), hsv_img.type());

    int img_color[5] = {0, 0, 0, 0, 0};
    for (int y = 0; y < hsv_img.rows; y++)
    {
        for (int x = 0; x < hsv_img.cols; x++)
        {
            if (0 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 20) // Red
            {
                img_color[0]++;
            }
            else if (20 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 40) // Yellow
            {
                img_color[1]++;
            }
            else if (40 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 70) // Green
            {
                img_color[2]++;
            }
            else if (70 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 120) // Blue
            {
                img_color[3]++;
            }
            else if (120 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 160) // Purple
            {
                img_color[4]++;
            }
            else if (160 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 180) // Red
            {
                img_color[0]++;
            }
        }
    }

    int maximum_color = max({img_color[0], img_color[1], img_color[2], img_color[3], img_color[4]});

    if (maximum_color == img_color[0])
    {
        cout << "Color is Red." << endl;
        for (int y = 0; y < hsv_img.rows; y++)
        {
            for (int x = 0; x < hsv_img.cols; x++)
            {
                if (0 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 20)
                {
                    dst_img.at<Vec3b>(y, x)[0] = 255;
                    dst_img.at<Vec3b>(y, x)[1] = 255;
                    dst_img.at<Vec3b>(y, x)[2] = 255;
                }
                else
                {
                    dst_img.at<Vec3b>(y, x)[0] = 0;
                    dst_img.at<Vec3b>(y, x)[1] = 0;
                    dst_img.at<Vec3b>(y, x)[2] = 0;
                }

                if (160 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 180)
                {
                    dst_img.at<Vec3b>(y, x)[0] = 255;
                    dst_img.at<Vec3b>(y, x)[1] = 255;
                    dst_img.at<Vec3b>(y, x)[2] = 255;
                }
                else
                {
                    dst_img.at<Vec3b>(y, x)[0] = 0;
                    dst_img.at<Vec3b>(y, x)[1] = 0;
                    dst_img.at<Vec3b>(y, x)[2] = 0;
                }
            }
        }
    }
    else if (maximum_color == img_color[1])
    {
        cout << "Color is Yellow." << endl;
        for (int y = 0; y < hsv_img.rows; y++)
        {
            for (int x = 0; x < hsv_img.cols; x++)
            {
                if (20 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 40)
                {
                    dst_img.at<Vec3b>(y, x)[0] = 255;
                    dst_img.at<Vec3b>(y, x)[1] = 255;
                    dst_img.at<Vec3b>(y, x)[2] = 255;
                }
                else
                {
                    dst_img.at<Vec3b>(y, x)[0] = 0;
                    dst_img.at<Vec3b>(y, x)[1] = 0;
                    dst_img.at<Vec3b>(y, x)[2] = 0;
                }
            }
        }
    }
    else if (maximum_color == img_color[2])
    {
        cout << "Color is Green." << endl;
        for (int y = 0; y < hsv_img.rows; y++)
        {
            for (int x = 0; x < hsv_img.cols; x++)
            {
                if (40 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 70)
                {
                    dst_img.at<Vec3b>(y, x)[0] = 255;
                    dst_img.at<Vec3b>(y, x)[1] = 255;
                    dst_img.at<Vec3b>(y, x)[2] = 255;
                }
                else
                {
                    dst_img.at<Vec3b>(y, x)[0] = 0;
                    dst_img.at<Vec3b>(y, x)[1] = 0;
                    dst_img.at<Vec3b>(y, x)[2] = 0;
                }
            }
        }
    }
    else if (maximum_color == img_color[3])
    {
        cout << "Color is Blue." << endl;
        for (int y = 0; y < hsv_img.rows; y++)
        {
            for (int x = 0; x < hsv_img.cols; x++)
            {
                if (70 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 120)
                {
                    dst_img.at<Vec3b>(y, x)[0] = 255;
                    dst_img.at<Vec3b>(y, x)[1] = 255;
                    dst_img.at<Vec3b>(y, x)[2] = 255;
                }
                else
                {
                    dst_img.at<Vec3b>(y, x)[0] = 0;
                    dst_img.at<Vec3b>(y, x)[1] = 0;
                    dst_img.at<Vec3b>(y, x)[2] = 0;
                }
            }
        }
    }

    else if (maximum_color == img_color[4])
    {
        cout << "Color is Purple." << endl;

        for (int y = 0; y < hsv_img.rows; y++)
        {
            for (int x = 0; x < hsv_img.cols; x++)
            {
                if (120 < hsv_img.at<Vec3b>(y, x)[0] && hsv_img.at<Vec3b>(y, x)[0] <= 160)
                {
                    dst_img.at<Vec3b>(y, x)[0] = 255;
                    dst_img.at<Vec3b>(y, x)[1] = 255;
                    dst_img.at<Vec3b>(y, x)[2] = 255;
                }
                else
                {
                    dst_img.at<Vec3b>(y, x)[0] = 0;
                    dst_img.at<Vec3b>(y, x)[1] = 0;
                    dst_img.at<Vec3b>(y, x)[2] = 0;
                }
            }
        }
    }

    return dst_img;
}

Mat myBgr2Hsv(Mat src_img)
{
    double b, g, r, h, s, v;
    Mat hsv_img(src_img.size(), src_img.type());
    for (int y = 0; y < src_img.rows; y++)
    {
        for (int x = 0; x < src_img.cols; x++)
        {
            b = (double)src_img.at<Vec3b>(y, x)[0];
            g = (double)src_img.at<Vec3b>(y, x)[1];
            r = (double)src_img.at<Vec3b>(y, x)[2];

            vector<double> vec = {r, g, b};
            double min_value = *min_element(vec.begin(), vec.end());
            double max_value = *max_element(vec.begin(), vec.end());

            v = max_value;
            if (v == 0)
                s = 0;
            else
                s = (max_value - min_value) / max_value;

            if (max_value == r)
                h = 0 + (g - b) / (max_value - min_value);
            else if (max_value == g)
                h = 2 + (b - r) / (max_value - min_value);
            else
                h = 4 + (r - g) / (max_value - min_value);
            h *= 60;

            if (h < 0)
                h += 360;

            h /= 2;
            s *= 255;

            h = std::max(0.0, std::min(255.0, h));
            s = std::max(0.0, std::min(255.0, s));
            v = std::max(0.0, std::min(255.0, v));

            hsv_img.at<Vec3b>(y, x)[0] = (uchar)h;
            hsv_img.at<Vec3b>(y, x)[1] = (uchar)s;
            hsv_img.at<Vec3b>(y, x)[2] = (uchar)v;
        }
    }
    return hsv_img;
}