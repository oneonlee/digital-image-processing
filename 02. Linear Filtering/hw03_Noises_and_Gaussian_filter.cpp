#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include <cmath>

void spreadSaltsAndPepperNoises(Mat img, int num[]);
double myKernelConv9x9(uchar *arr, double kernel[9][9], int x, int y, int width, int height);
Mat myGaussianFilter9x9(Mat srcImg);

int main()
{
    // Step 0
    Mat original_gray_img = imread("gear.jpg", 0);
    imshow("original_gray_img", original_gray_img);

    // Step 1
    Mat noised_img = original_gray_img;
    int num[2] = {500, 500};
    spreadSaltsAndPepperNoises(noised_img, num);
    imshow("noised_img", noised_img);

    // Step 2
    Mat gaussian_filtered_img = myGaussianFilter9x9(noised_img);
    imshow("gaussian_filtered_img", gaussian_filtered_img);

    // Etc
    waitKey(0);
    destroyWindow("original_gray_img");
    destroyWindow("noised_img");
    destroyWindow("gaussian_filtered_img");

    return 0;
}

void spreadSaltsAndPepperNoises(Mat img, int num[])
{
    for (int color = 0; color < 2; color++)
    {
        for (int i = 0; i < num[color]; i++)
        {
            int x = rand() % img.cols;
            int y = rand() % img.rows;

            if (color == 0) // pepper (black noise)
            {
                if (img.channels() == 1) // grayscale
                {
                    img.at<uchar>(y, x) = 0;
                }
                else
                {
                    img.at<Vec3b>(y, x)[0] = 0;
                    img.at<Vec3b>(y, x)[1] = 0;
                    img.at<Vec3b>(y, x)[2] = 0;
                }
            }
            else if (color == 1) // salt (white noise)
            {
                if (img.channels() == 1) // grayscale
                {
                    img.at<uchar>(y, x) = 255;
                }
                else
                {
                    img.at<Vec3b>(y, x)[0] = 255;
                    img.at<Vec3b>(y, x)[1] = 255;
                    img.at<Vec3b>(y, x)[2] = 255;
                }
            }
        }
    }
}

double myKernelConv9x9(uchar *arr, double kernel[9][9], int x, int y, int width, int height)
{
    double sum = 0;
    double sumKernel = 0;

    // 특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
    for (int j = -4; j <= 4; j++)
    {
        for (int i = -4; i <= 4; i++)
        {
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width)
            // 영상 가장자리에서 영상 밖의 화소를 읽지 않도록 하는 조건문
            {
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 3][j + 3];
                sumKernel += kernel[i + 3][j + 3];
            }
        }
    }
    if (sumKernel != 0)
    {
        // 합이 1로 정규화되도록 하여 영상의 밝기변화 방지
        return sum / sumKernel;
    }
    else
    {
        return sum;
    }
}

Mat myGaussianFilter9x9(Mat srcImg)
{
    int width = srcImg.cols;
    int height = srcImg.rows;

    double kernel[9][9]; // 9x9 형태의 Gaussian 마스크 배열
    double sigma = 0.1;
    double sum = 0;

    for (int x = -4; x <= 4; x++)
    {
        for (int y = -4; y <= 4; y++)
        {
            // G(x, y) = {1/(2*pi*sigma^2)} * e^{-(x^2+y^2)/(2*sigma^2)}
            kernel[x + 4][y + 4] = 1 / (2 * 3.14 * sigma * sigma) * exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum = sum + kernel[x + 4][y + 4];
        }
    }

    Mat dstImg(srcImg.size(), CV_8UC1);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = myKernelConv9x9(srcData, kernel, x, y, width, height);
            // 앞서 구현한 convolution에 마스크 배열을 입력해 사용
        }
    }

    return dstImg;
}