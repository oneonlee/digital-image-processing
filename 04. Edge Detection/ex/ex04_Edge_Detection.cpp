#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

double gaussian2D(float c, float r, double sigma)
{
    return exp(-(pow(c, 2) + pow(r, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}
void myKernelConv(const Mat &src_img, Mat &dst_img, const Mat &kn)
{
    dst_img = Mat::zeros(src_img.size(), CV_8UC1);

    int wd = src_img.cols;
    int hg = src_img.rows;
    int kwd = kn.cols;
    int khg = kn.rows;
    int rad_w = kwd / 2;
    int rad_h = khg / 2;

    float *kn_data = (float *)kn.data;
    uchar *src_data = (uchar *)src_img.data;
    uchar *dst_data = (uchar *)dst_img.data;

    float wei, tmp, sum;

    // < 픽셀 인덱싱 (가장자리 제외) >
    for (int c = rad_w + 1; c < wd - rad_w; c++)
    {
        for (int r = rad_h + 1; r < hg - rad_h; r++)
        {
            tmp = 0.f;
            sum = 0.f;
            // < 커널 인덱싱 >
            for (int kc = -rad_w; kc <= rad_w; kc++)
            {
                for (int kr = -rad_h; kr <= rad_h; kr++)
                {
                    wei = (float)kn_data[(kr + rad_h) * kwd + (kc + rad_w)];
                    tmp += wei * (float)src_data[(r + kr) * wd + (c + kc)];
                    sum += wei;
                }
            }
            if (sum != 0.f)
                tmp = abs(tmp) / sum; // 정규화 및 overf1ow 방지
            else
                tmp = abs(tmp);

            if (tmp > 255.f)
                tmp = 255.f; // overflow 방지

            dst_data[r * wd + c] = (uchar)tmp;
        }
    }
}
void myGaussian(const Mat &src_img, Mat &dst_img, Size size)
{
    // < 커널 생성 >
    Mat kn = Mat::zeros(size, CV_32FC1);
    double sigma = 1.0;
    float *kn_data = (float *)kn.data;
    for (int c = 0; c < kn.cols; c++)
    {
        for (int r = 0; r < kn.rows; r++)
        {
            kn_data[r * kn.cols + c] =
                (float)gaussian2D((float)(c - kn.cols / 2), (float)(r - kn.rows / 2), sigma);
        }
    }
    // < 커널 컨볼루션 수행 >
    myKernelConv(src_img, dst_img, kn);
}

void doEdgeEx()
{
    cout << "--- doEdgeEx() --- \n"
         << endl;

    // < 입력 >
    Mat src_img = imread("rock.png", 0);
    if (!src_img.data)
        printf("No image data \n");

    // < 잡음 제거 >
    Mat blur_img;
    myGaussian(src_img, blur_img, Size(5, 5));

    // < 커널 생성 >
    float kn_data[] = {1.f, 0.f, -1.f,
                       1.f, 0.f, -1.f,
                       1.f, 0.f, -1.f};
    Mat kn(Size(3, 3), CV_32FC1, kn_data);

    cout << "Edge kernel : \n"
         << kn << endl;

    // < 커널 컨볼루션 수행 >
    Mat dst_img;
    myKernelConv(blur_img, dst_img, kn);

    // < 출력 >
    Mat result_img;
    hconcat(src_img, dst_img, result_img);
    imshow("doEdgeEx()", result_img);
    waitKey(0);
}