#include <algorithm> // sort() 사용
#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

void myMedian(const Mat &src_img, Mat &dst_img, const Size &kn_size);
void doMedianEx();

int main()
{
    doMedianEx();
    return 0;
}

void myMedian(const Mat &src_img, Mat &dst_img, const Size &kn_size)
{
    dst_img = Mat::zeros(src_img.size(), CV_8UC1);

    int wd = src_img.cols;    // src_img의 행, 256
    int hg = src_img.rows;    // src_img의 행, 256
    int kwd = kn_size.width;  // kernel의 size
    int khg = kn_size.height; // kernel의 size
    int rad_w = kwd / 2;      // kn_size의 절반, 중간값 indexing에 사용
    int rad_h = khg / 2;      // kn_size의 절반, 중간값 indexing에 사용

    uchar *src_data = (uchar *)src_img.data;
    uchar *dst_data = (uchar *)dst_img.data;

    float *table = new float[kwd + khg](); // 커널 테이블 동적할당
    float tmp;
    float median;

    // < 픽셀 인덱싱 (가장자리 제외) >
    for (int c = rad_w + 1; c < wd - rad_w; c++)
    {
        for (int r = rad_h + 1; r < hg - rad_h; r++)
        {
            // 중간값 indexing
            tmp = 0.f;
            for (int kc = -rad_w; kc <= rad_w; kc++)
            {
                for (int kr = -rad_h; kr <= rad_h; kr++)
                {
                    tmp = (float)src_data[(r + kr) * wd + (c + kc)]; // 원본 영상의 데이터를 tmp에 저장
                    table[(kr + rad_h) * kwd + (kc + rad_w)] = tmp;  // 테이블에 tmp 저장
                }
            }
            sort(table, table + (kwd * khg));     // 커널 table 정렬
            median = table[(kwd * khg) / 2];      // 중간값 선택
            dst_data[r * wd + c] = (uchar)median; // 가장 큰 값을 중간 값으로 대체
        }
    }
    delete[] table; // 동적할당 해제
}

void doMedianEx()
{
    cout << "--- doMedianEx() ---" << endl;

    // < 입력 >
    Mat src_img = imread("salt_pepper2.png", 0);
    if (!src_img.data)
        printf("No image data\n");

    // < Median 필터링 수행 >
    Mat filtered_3x3_img;
    Mat filtered_5x5_img;

    myMedian(src_img, filtered_3x3_img, Size(3, 3));
    myMedian(src_img, filtered_5x5_img, Size(5, 5));

    // < 출력 >
    Mat temp_img;
    Mat dst_img;
    hconcat(src_img, filtered_3x3_img, temp_img);
    hconcat(temp_img, filtered_5x5_img, dst_img);

    imshow("Median Filtered Salt and Pepper Noises", dst_img);

    waitKey(0);
    destroyAllWindows();
}
