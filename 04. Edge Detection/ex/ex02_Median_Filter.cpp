#include <algorithm> // sort() 사용
#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
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

    int wd = src_img.cols;
    int hg = src_img.rows;
    int kwd = kn_size.width;
    int khg = kn_size.height;
    int rad_w = kwd / 2;
    int rad_h = khg / 2;

    uchar *src_data = (uchar *)src_img.data;
    uchar *dst_data = (uchar *)dst_img.data;

    float *table = new float[kwd + khg](); // 커널 테이블 동적할당
    float tmp;

    for (int c = rad_w + 1; c < wd - rad_w; c++)
    {
        for (int r = rad_h + 1; r < hg - rad_h; r++)
        {
            tmp = 0.f;
            for (int kc = -rad_w; kc <= rad_w; kc++)
            {
                for (int kr = -rad_h; kr <= rad_h; kr++)
                {
                    tmp = (float)src_data[(r + kr) * wd + (c + kc)];
                    table[(kr + rad_h) * kwd + (kc + rad_w)] = tmp;
                }
            }
            sort(table, table + kwd * khg); // 커널 테이블 정렬
            dst_data[r * wd + c] = (uchar)table[(kwd * khg) / 2];
            // 중간 값 선택
        }
    }
    delete[] table; // 동적할당 해제
}

void doMedianEx()
{
    cout << "--- doMedianEx() --- \n"
         << endl;
    // < 입력 >
    Mat src_img = imread("salt_pepper.png", 0);
    if (!src_img.data)
        printf("No image data \n");
    // < Median 필터링 수행 >
    Mat dst_img;
#if USE_OPENCV
    medianBlur(src_img, dst_img, 3);
#else
    myMedian(src_img, dst_img, Size(3, 3));
#endif

    // < 출력 >
    Mat result_img;
    hconcat(src_img, dst_img, result_img);
    imshow("doMedianEx()", result_img);
    waitKey(0);
}