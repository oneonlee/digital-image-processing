#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <filesystem>

using namespace cv;
using namespace std;
using std::__fs::filesystem::current_path;

string SRC_PATH = current_path() + "/src/";

Mat insertTransparentLogo(Mat background, Mat logo);

int main()
{
    Mat srcImg = imread(SRC_PATH + "img3.jpg", 1);
    Mat maskImg = imread(SRC_PATH + "img4.jpg", 1);
    resize(maskImg, maskImg, Size(srcImg.cols, srcImg.rows)); // maskImg의 크기를 srcImg의 크기와 같도록 설정

    Mat dstImg = srcImg - maskImg;

    Mat logoImg = imread(SRC_PATH + "img5.jpg", 1); // 컬러영상 읽어오기

    dstImg = insertTransparentLogo(dstImg, logoImg);

    imshow("hw03 - 12181879 이동건", dstImg);
    waitKey(0);
    destroyWindow("hw03 - 12181879 이동건");

    return 0;
}

Mat insertTransparentLogo(Mat background, Mat logo)
{
    int logoRows = background.rows / 2;
    int logoCols = background.cols / 4;

    Mat grayLogo;
    cvtColor(logo, grayLogo, CV_BGR2GRAY); // 흑백으로 만들어 grayLogo에 저장

    Mat binaryLogo;
    threshold(grayLogo, binaryLogo, 178, 255, THRESH_BINARY); // THRESH_BINARY로 영상을 이진화

    for (int i = 0; i < logo.rows; i++)
    {
        for (int j = 0; j < logo.cols; j++)
        {
            // binaryLogo에서 흰색(255)이 아닌 경우, background의 3채널 픽셀 데이터에 logo의 3채널 픽셀 데이터를 대입함
            // 참고 : https://diyver.tistory.com/99
            if (binaryLogo.at<uchar>(i, j) != 255) // grayscale 영상의 행렬 좌표로 픽셀 데이터를 접근할 때에는 <Vec3b> 대신에 <uchar>를 사용
            {
                background.at<Vec3b>(i + logoRows, j + logoCols)[0] = logo.at<Vec3b>(i, j)[0];
                background.at<Vec3b>(i + logoRows, j + logoCols)[1] = logo.at<Vec3b>(i, j)[1];
                background.at<Vec3b>(i + logoRows, j + logoCols)[2] = logo.at<Vec3b>(i, j)[2];
            }
        }
    }

    return background;
}