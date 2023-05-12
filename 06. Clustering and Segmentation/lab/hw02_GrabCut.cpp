// < OpenCV를 이용한 GrabCut 구현 >

#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

void exCvGrabCut(string img_path, int x1, int y1, int x2, int y2);

int main()
{
    string img_path;
    int x1, y1, x2, y2;

    img_path = "metamong.png";
    x1 = 130, y1 = 40;
    x2 = 550, y2 = 400;
    exCvGrabCut(img_path, x1, y1, x2, y2);

    img_path = "crying.png";
    x1 = 10, y1 = 10;
    x2 = 1300, y2 = 1150;
    exCvGrabCut(img_path, x1, y1, x2, y2);

    img_path = "jump.jpeg";
    x1 = 455, y1 = 150;
    x2 = 1255, y2 = 1350;
    exCvGrabCut(img_path, x1, y1, x2, y2);

    img_path = "sleepy.jpeg";
    x1 = 220, y1 = 20;
    x2 = 975, y2 = 715;
    exCvGrabCut(img_path, x1, y1, x2, y2);

    img_path = "smile.jpeg";
    x1 = 400, y1 = 15;
    x2 = 1145, y2 = 845;
    exCvGrabCut(img_path, x1, y1, x2, y2);

    return 0;
}

void exCvGrabCut(string img_path, int x1, int y1, int x2, int y2)
{
    // 이미지 읽기
    Mat img = imread(img_path);
    if (img.empty())
    {
        cerr << "Image load failed!" << endl;
        return;
    }

    // GrabCut을 위한 사각형 영역 지정
    rectangle(img, Point(x1, y1), Point(x2, y2), Scalar(255, 0, 0), 3);
    imshow("ROI", img); // bounding box 확인
    Rect rect = Rect(Point(x1, y1), Point(x2, y2));

    // GrabCut
    Mat mask, bg_model, fg_model;
    grabCut(img, mask,
            rect,
            bg_model, fg_model,
            5,
            GC_INIT_WITH_RECT);

    // 결과 출력
    Mat mask2;
    compare(mask, GC_PR_FGD, mask2, CMP_EQ);
    // GC_PR_FGD: GrabCut class foreground 픽셀
    // CMP_EQ: compare 옵션 - 두 행렬의 각 요소를 비교하여 같으면 255, 다르면 0으로 설정

    Mat result(img.size(), CV_8UC3, Scalar(255, 255, 255));
    img.copyTo(result, mask2);
    // imshow("mask", mask * 64);
    imshow("mask2", mask2 * 255);
    imshow("result", result);
    waitKey();
    destroyAllWindows();
}