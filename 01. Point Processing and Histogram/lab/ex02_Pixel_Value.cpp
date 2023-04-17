#include <iostream>
#include "opencv2/core/core.hpp"       // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void SpreadSalts(Mat img, int num)
{
    // num: 점을 찍을 개수
    for (int n = 0; n < num; n++)
    {
        int x = rand() % img.cols; // img.cols는 이미지의 폭 정보를 저장
        int y = rand() % img.rows; // img.rows는 이미지의 높이 정보를 저장
        /*
        나머지는 나누는 수를 넘을 수 없으므로
        무작위 위치가 이미지의 크기를 벗어나지 않도록
        제한하는 역할을 하여줌
        */

        if (img.channels() == 1)
        {
            // img.channels()는 이미지의 채널 수를 반환
            img.at<uchar>(y, x) = 255; // 단일 채널 접근
        }
        else
        {
            img.at<Vec3b>(y, x)[0] = 255; // Blue 채널 접근
            img.at<Vec3b>(y, x)[1] = 255; // Green 채널 접근
            img.at<Vec3b>(y, x)[2] = 255; // Red 채널 접근
        }
    }
}