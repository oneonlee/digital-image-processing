#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

int myRGBKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height);
Mat myRGBGaussianFilter(Mat srcImg);
Mat myRGBDownSampling(Mat srcImg);
vector<Mat> myRGBLaplacianPyramid(Mat srcImg);

int main()
{
    // Step 0
    Mat src_img, dst_img;
    src_img = imread("gear.jpg", 1); // 컬러 영상

    // Step 1
    vector<Mat> pyramidImg = myRGBLaplacianPyramid(src_img); // Laplacian pyramid 확보

    // Step 2
    reverse(pyramidImg.begin(), pyramidImg.end()); // 작은 영상부터 처리하기 위해 vector의 순서를 반대로

    // Step 3
    for (int i = 0; i < pyramidImg.size(); i++)
    {
        string windowName = "RGB_Laplacian_Pyramid_" + to_string(i);
        imshow(windowName, pyramidImg[i]);
    }
    waitKey(0);
    for (int i = 0; i < pyramidImg.size(); i++)
    {
        string windowName = "RGB_Laplacian_Pyramid_" + to_string(i);
        destroyWindow(windowName);
    }

    // Step 4
    for (int i = 0; i < pyramidImg.size(); i++)
    {
        // Vector의 크기만큼 반복
        if (i == 0)
        {
            dst_img = pyramidImg[i]; // 가장 작은 영상은 차 영상이 아니기 때문에 바로 불러옴
        }
        else
        {
            resize(dst_img, dst_img, pyramidImg[i].size()); // 작은 영상을 확대

            dst_img = dst_img + pyramidImg[i] - 128;
            // 차 영상을 다시 더해 큰 영상을 복원
            // 오버플로우 방지용으로 더했던 128을 다시 빼줌
        }

        string windowName = "Constructed_RGB_Laplacian_Pyramid_" + to_string(i);
        imshow(windowName, dst_img);
    }
    waitKey(0);
    for (int i = 0; i < pyramidImg.size(); i++)
    {
        string windowName = "Constructed_RGB_Laplacian_Pyramid_" + to_string(i);
        destroyWindow(windowName);
    }

    return 0;
}

int myRGBKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height)
{
    int sum = 0;
    int sumKernel = 0;

    // 특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            if ((y + j) >= 0 && (y + j) < 3 * height && (x + i) >= 0 && (x + i) < 3 * width)
            // 영상 가장자리에서 영상 밖의 화소를 읽지 않도록 하는 조건문
            {
                sum += arr[(y + 3 * j) * width + (x + 3 * i)] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
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

Mat myRGBGaussianFilter(Mat srcImg)
{
    int width = srcImg.cols;
    int height = srcImg.rows;
    int kernel[3][3] = {1, 2, 1,
                        2, 4, 2,
                        1, 2, 1}; // 3x3 형태의 Gaussian 마스크 배열

    Mat dstImg(srcImg.size(), CV_8UC3);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < 3 * height; y += 3)
    {
        for (int x = 0; x < 3 * width; x += 3)
        {
            // 앞서 구현한 convolution에 마스크 배열을 입력해 사용
            dstData[y * width + x] = myRGBKernelConv3x3(srcData, kernel, x, y, width, height);
            dstData[y * width + x + 1] = myRGBKernelConv3x3(srcData, kernel, x + 1, y, width, height);
            dstData[y * width + x + 2] = myRGBKernelConv3x3(srcData, kernel, x + 2, y, width, height);
        }
    }

    return dstImg;
}

Mat myRGBDownSampling(Mat srcImg)
{
    int width = srcImg.cols / 2;
    int height = srcImg.rows / 2;
    Mat dstImg(height, width, CV_8UC3); // 가로 세로가 입력 영상의 절반인 영상을 먼저 생성
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < 3 * height; y += 3)
    {
        for (int x = 0; x < 3 * width; x += 3)
        {
            // 2배 간격으로 인덱싱 해 큰 영상을 작은 영상에 대입할 수 있음
            dstData[y * width + x] = srcData[(y * 2) * (width * 2) + (x * 2)];
            dstData[y * width + x + 1] = srcData[(y * 2) * (width * 2) + (x * 2) + 1];
            dstData[y * width + x + 2] = srcData[(y * 2) * (width * 2) + (x * 2) + 2];
        }
    }
    return dstImg;
}

vector<Mat> myRGBLaplacianPyramid(Mat srcImg)
{
    vector<Mat> Vec; // 여러 영상을 모아서 저장하기 위해 STL의 vector 컨테이너 사용

    for (int i = 0; i < 4; i++)
    {
        if (i != 3)
        {
            Mat highImg = srcImg; // 수행하기 이전 영상을 백업
            srcImg = myRGBGaussianFilter(srcImg);
            srcImg = myRGBDownSampling(srcImg);

            Mat lowImg = srcImg;
            resize(lowImg, lowImg, highImg.size());
            // 작아진 영상을 백업한 영상의 크기로 확대

            Vec.push_back(highImg - lowImg + 128);
            // 차 영상을 컨테이너에 삽입
            // 128을 더해준 것은 차 영상에서 오버플로우를 방지하기 위함
        }
        else
        {
            Vec.push_back(srcImg); // vector 컨테이너에 하나씩 처리결과를 삽입
        }
    }

    return Vec;
}
