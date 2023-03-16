#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <filesystem>

using namespace cv;
using namespace std;
using std::__fs::filesystem::current_path;

string SRC_PATH = current_path() + "/src/";

void upGradation(Mat img);   // 위로 갈수록 점점 어두워지는 영상을 만드는 함수
void downGradation(Mat img); // 아래로 갈수록 점점 어두워지는 영상을 만드는 함수
Mat getHistogram(Mat &src);

int main()
{
    Mat upImg = imread(SRC_PATH + "img2.jpg", 0);
    Mat downImg = imread(SRC_PATH + "img2.jpg", 0);

    upGradation(upImg);
    downGradation(downImg);

    Mat upHistogram = getHistogram(upImg);
    Mat downHistogram = getHistogram(downImg);

    imshow("upGradation", upImg);
    imshow("upHistogram", upHistogram);
    imshow("downGradation", downImg);
    imshow("downHistogram", downHistogram);

    waitKey(0);

    destroyWindow("upGradation");
    destroyWindow("upHistogram");
    destroyWindow("downGradation");
    destroyWindow("downHistogram");

    return 0;
}

// 위로 갈수록 점점 어두워지는 영상을 만드는 함수
void upGradation(Mat img)
{
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            img.at<uchar>(i, j) = img.at<uchar>(i, j) * ((double)i / img.rows);
        }
    }
}

// 아래로 갈수록 점점 어두워지는 영상을 만드는 함수
void downGradation(Mat img)
{
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            img.at<uchar>(i, j) = img.at<uchar>(i, j) * (1 - ((double)i / img.rows));
        }
    }
}

Mat getHistogram(Mat &src)
{
    Mat histogram;
    const int *channel_numbers = {0};
    float channel_range[] = {0.0, 255.0};
    const float *channel_ranges = channel_range;
    int number_bins = 255;

    // 히스토그램 계산
    calcHist(&src, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);

    // 히스토그램 plot
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / number_bins);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
    normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat()); // 정규화

    for (int i = 1; i < number_bins; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(histogram.at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0); // 값과 값을 잇는 선을 그리는 방식으로 plot
    }

    return histImage;
}