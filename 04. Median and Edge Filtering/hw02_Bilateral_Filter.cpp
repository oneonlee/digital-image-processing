#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

double gaussian(float x, double sigma);
float distance(int x, int y, int i, int j);
void bilateral(const Mat &src_img, Mat &dst_img, int c, int r, int diameter, double sig_r, double sig_s);
void myBilateral(const Mat &src_img, Mat &dst_img, int diameter, double sig_r, double sig_s);
void doBilateralFiltering(int diameter, double sig_r, double sig_s);

int main()
{
    int diameter = 15;
    double sig_r = 25;
    double sig_s = 18;
    doBilateralFiltering(diameter, sig_r, sig_s);

    return 0;
}

double gaussian(float x, double sigma)
{
    return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}
float distance(int x, int y, int i, int j)
{
    return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}
void bilateral(const Mat &src_img, Mat &dst_img, int c, int r, int diameter, double sig_r, double sig_s)
{
    int radius = diameter / 2;

    double gr, gs, wei;
    double tmp = 0.;
    double sum = 0.;

    // 커널 인덱싱
    for (int kc = -radius; kc <= radius; kc++)
    {
        for (int kr = -radius; kr <= radius; kr++)
        {
            // range calc
            gr = gaussian((float)src_img.at<uchar>(c + kc, r + kr) - (float)src_img.at<uchar>(c, r), sig_r);
            // spatial calc
            gs = gaussian(distance(c, r, c + kc, r + kr), sig_s);

            wei = gr * gs;
            tmp += src_img.at<uchar>(c + kc, r + kr) * wei;
            sum += wei;
        }
    }
    dst_img.at<double>(c, r) = tmp / sum; // 정규화
}
void myBilateral(const Mat &src_img, Mat &dst_img, int diameter, double sig_r, double sig_s)
{
    dst_img = Mat::zeros(src_img.size(), CV_8UC1);

    Mat guide_img = Mat::zeros(src_img.size(), CV_64F);
    int wh = src_img.cols;
    int hg = src_img.rows;
    int radius = diameter / 2;

    // < 픽셀 인덱싱 (가장자리 제외) >
    for (int c = radius + 1; c < hg - radius; c++)
    {
        for (int r = radius + 1; r < wh - radius; r++)
        {
            bilateral(src_img, guide_img, c, r, diameter, sig_r, sig_s);
            // 화소별 Bilateral 계산 수행
        }
    }
    guide_img.convertTo(dst_img, CV_8UC1); // Mat type 변환
}
void doBilateralFiltering(int diameter, double sig_r, double sig_s)
{
    cout << "--- doBilateralFiltering() --- \n"
         << endl;

    // < 입력 >
    Mat src_img = imread("rock.png", 0);
    Mat dst_img;
    if (!src_img.data)
        printf("No image data \n");

    // < Bilateral 필터링 수행 >
    myBilateral(src_img, dst_img, diameter, sig_r, sig_s);

    // < 출력 >
    Mat result_img;
    hconcat(src_img, dst_img, result_img);
    imshow("doBilateralFiltering()", result_img);
    waitKey(0);
}