#include <iostream>
using namespace std;

#include "opencv2/core/core.hpp"       // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

#include <filesystem>                      // 현재 경로를 얻기 위해 사용
using std::__fs::filesystem::current_path; // 현재 경로를 얻기 위해 사용

string SRC_PATH = current_path() + "/";

int myKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height);
Mat myGaussianFilter(Mat srcImg);
Mat mySampling(Mat srcImg);
vector<Mat> myLaplacianPyramid(Mat srcImg); // Laplacian pyramid의 구현

int main()
{
    Mat src_img, dst_img;

    src_img = imread(SRC_PATH + "gear.jpg", 0);

    vector<Mat> VecLap = myLaplacianPyramid(src_img);
    // Laplacian pyramid 확보
    reverse(VecLap.begin(), VecLap.end());
    // 작은 영상부터 처리하기 위해 vector의 순서를 반대로

    for (int i = 0; i < VecLap.size(); i++)
    {
        // Vector의 크기만큼 반복
        if (i == 0)
        {
            dst_img = VecLap[i];
            // 가장 작은 영상은 차 영상이 아니기 때문에 바로 불러옴
        }
        else
        {
            resize(dst_img, dst_img, VecLap[i].size());
            // 작은 영상을 확대
            dst_img = dst_img + VecLap[i] - 128;
            // 차 영상을 다시 더해 큰 영상을 복원
            // 오버플로우 방지용으로 더했던 128을 다시 빼줌
        }

        string fname = "lap_pyr" + to_string(i) + ".png";
        imwrite(fname, dst_img);
        imshow(fname, dst_img);
        waitKey(0);
        destroyWindow(fname);
    }
    return 0;
}

// Laplacian pyramid의 구현
vector<Mat> myLaplacianPyramid(Mat srcImg)
{
    vector<Mat> Vec;

    for (int i = 0; i < 4; i++)
    {
        if (i != 3)
        {
            Mat highImg = srcImg; // 수행하기 이전 영상을 백업
#if USE_OPENCV
            pyrDown(srcImg, srcImg, size(srcImg.cols / 2, srcImg.rows / 2));
#else
            srcImg = mySampling(srcImg);
            srcImg = myGaussianFilter(srcImg);
#endif
            Mat lowImg = srcImg;
            resize(lowImg, lowImg, highImg.size());
            // 작아진 영상을 백업한 영상의 크기로 확대
            Vec.push_back(highImg - lowImg + 128);
            // 차 영상을 컨테이너에 삽입
            // 128을 더해준 것은 차 영상에서 오버플로우를 방지하기 위함
        }
        else
        {
            Vec.push_back(srcImg);
        }
    }
    return Vec;
}

Mat mySampling(Mat srcImg)
{
    int width = srcImg.cols / 2;
    int height = srcImg.rows / 2;
    Mat dstImg(height, width, CV_8UC1);
    // 가로 세로가 입력 영상의 절반인 영상을 먼저 생성
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = srcData[(y * 2) * (width * 2) + (x * 2)];
            // 2배 간격으로 인덱싱 해 큰 영상을 작은 영상에 대입할 수 있음
        }
    }
    return dstImg;
}

Mat myGaussianFilter(Mat srcImg)
{
#if USE_OPENCV // OpenCV로 구현한 것
    Mat dstImg(srcImg.size(), CV_8UC1);
    cv::GaussianBlur(srcImg, dstImg, Size(3, 3), 0);
    // 마스크의 크기를 지정하면 자체적으로 마스크 생성 후 연산

    return dstImg;

#else // 직접 구현한 것 (매크로에 의해 컴파일 시 선택됨)
    int width = srcImg.cols;
    int height = srcImg.rows;
    int kernel[3][3] = {1, 2, 1,
                        2, 4, 2,
                        1, 2, 1}; // 3x3 형태의 Gaussian 마스크 배열
    Mat dstImg(srcImg.size(), CV_8UC1);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = myKernelConv3x3(srcData, kernel, x, y, width, height);
            // 앞서 구현한 convolution에 마스크 배열을 입력해 사용
        }
    }

    return dstImg;
#endif
}

int myKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height)
{
    int sum = 0;
    int sumKernel = 0;

    // 특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width)
            // 영상 가장자리에서 영상 밖의 화소를 읽지 않도록 하는 조건문
            {
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
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