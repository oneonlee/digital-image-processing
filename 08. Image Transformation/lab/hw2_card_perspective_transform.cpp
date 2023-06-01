#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

class Corner;
void cardPerspectiveTransform();

int main()
{
    cardPerspectiveTransform();
    return 0;
}

class Corner
{
public:
    int x, y;
};

void cardPerspectiveTransform()
{
    Mat src = imread("card_per.png", 1);

    Mat gray;
    cvtColor(src, gray, CV_BGR2GRAY); // 흑백 이미지로 변환

    Mat harr;
    cornerHarris(gray, harr, 2, 3, 0.06, BORDER_DEFAULT);        // Harris Corner 검출
    normalize(harr, harr, 0, 255, NORM_MINMAX, CV_32FC1, Mat()); // 정규화
    Mat harr_abs;
    convertScaleAbs(harr, harr_abs); // 절댓값으로 변환

    Corner corner[200]; // Corner 클래스의 객체 생성
    Point2f srcQuad[4]; // 원본 이미지의 카드 꼭짓점

    int i = 0;

    // Print corners
    int threshold = 113;
    Mat result = src.clone();
    for (int y = 0; y < harr.rows; y += 1)
    {
        for (int x = 0; x < harr.cols; x += 1)
        {
            if ((int)harr.at<float>(y, x) > threshold) // threshold 값보다 작다면
            {
                if ((int)harr.at<float>(y, x) > (int)harr.at<float>(y - 1, x) && // 같은 좌표에 여러개의 코너 감지를 방지
                    (int)harr.at<float>(y, x) > (int)harr.at<float>(y + 1, x) &&
                    (int)harr.at<float>(y, x) > (int)harr.at<float>(y, x - 1) &&
                    (int)harr.at<float>(y, x) > (int)harr.at<float>(y, x + 1))
                {
                    circle(result, Point(x, y), 5, Scalar(255, 0, 255), 0, 4, 0); // 코너부분을 원으로 표시
                    corner[i].x = x;                                              // 좌표를 corner클래스 배열에 추가
                    corner[i].y = y;
                    i++;
                }
            }
        }
    }
    imshow("Detected corners", result);

    int max_y = 0;
    int min_y = 0;

    // srcQuad[0] : 좌상단, srcQuad[1] : 우상단, srcQuad[2] : 좌하단, srcQuad[3] : 우하단

    // 좌상단 : x좌표가 가로 중앙값보다 작고 y좌표가 가장 작은 값
    for (int j = 0; j < i; j++)
    {
        if (j == 0)
            min_y = corner[j].y;
        if (min_y >= corner[j].y && harr.cols / 2 > corner[j].x)
        {
            min_y = corner[j].y;
            srcQuad[0] = Point2f(corner[j].x, corner[j].y); // y가 최솟값일 때 x,y 좌표 값을 srcQuad[0]으로 설정
        }
    }

    // 우상단 : x좌표가 가로 중앙값보다 크고 y좌표가 가장 작은 값
    for (int j = 0; j < i; j++)
    {
        if (j == 0)
            max_y = corner[j].y;
        if (max_y <= corner[j].y && harr.cols / 2 > corner[j].x)
        {
            max_y = corner[j].y;
            srcQuad[1] = Point2f(corner[j].x, corner[j].y); // y가 최댓값일 때 x, y좌표 값을 srcQuad[1]로 설정
        }
    }

    // 좌하단 : x좌표가 가로 중앙값보다 작고 y좌표가 가장 큰 값
    for (int j = i - 1; j >= 0; j--)
    {
        if (j == i - 1)
            min_y = corner[j].y;
        if (min_y >= corner[j].y && harr.cols / 2 <= corner[j].x)
        {
            min_y = corner[j].y;
            srcQuad[2] = Point2f(corner[j].x, corner[j].y); // y가 최솟값일 때 x, y좌표 값을 srcQuad[2]로 설정
        }
    }

    // 우하단 : x좌표가 가로 중앙값보다 크고 y좌표가 가장 큰 값
    for (int j = 0; j < i; j++)
    {
        if (j == 0)
            max_y = corner[j].y;
        if (max_y <= corner[j].y && harr.cols / 2 <= corner[j].x)
        {
            max_y = corner[j].y;
            srcQuad[3] = Point2f(corner[j].x, corner[j].y); // y가 최대일 때 x, y좌표 값을 srcQuad[3]로 설정
        }
    }

    Mat dst, matrix;
    Point2f dstQuad[4]; // dst 이미지의 카드 꼭짓점
    dstQuad[0] = Point2f(50, 140);
    dstQuad[1] = Point2f(50, 360);
    dstQuad[2] = Point2f(450, 140);
    dstQuad[3] = Point2f(450, 360);

    matrix = getPerspectiveTransform(srcQuad, dstQuad); // src가 dst의 위치로 대응됨
    warpPerspective(src, dst, matrix, src.size());      // src를 dst의 위치로 원근 변환

    imwrite("card_front.png", dst);

    imshow("Original Card", src);
    imshow("Fronted Card", dst);

    waitKey(0);
    destroyAllWindows();
}
