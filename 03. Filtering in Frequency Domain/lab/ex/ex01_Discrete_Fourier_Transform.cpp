#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

// 2D DFT
Mat doDft(Mat srcImg)
{
    Mat floatImg;
    srcImg.convertTo(floatImg, CV_32F);

    Mat complexImg;
    dft(floatImg, complexImg, DFT_COMPLEX_OUTPUT);

    return complexImg;
}

// Magnitude 영상 취득
Mat getMagnitude(Mat complexImg)
{
    Mat planes[2];
    split(complexImg, planes);
    // 실수부, 허수부 분리

    Mat magImg;
    magnitude(planes[0], planes[1], magImg);
    magImg += Scalar::all(1);
    log(magImg, magImg);
    // magnitude 취득
    // log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))

    return magImg;
}

// Phase 영상 취득
Mat getPhase(Mat complexImg)
{
    Mat planes[2];
    split(complexImg, planes);
    // 실수부, 허수부 분리

    Mat phaImg;
    phase(planes[0], planes[1], phaImg);
    // phase 취득

    return phaImg;
}

Mat myNormalize(Mat src)
{
    Mat dst;
    src.copyTo(dst);
    normalize(dst, dst, 0, 255, NORM_MINMAX);
    dst.convertTo(dst, CV_8UC1);
    return dst;
}

// 좌표계 중앙 이동 (centralize)
Mat centralize(Mat complex)
{
    Mat planes[2];
    split(complex, planes);
    int cx = planes[0].cols / 2;
    int cy = planes[1].rows / 2;

    Mat qORe(planes[0], Rect(0, 0, cx, cy));
    Mat q1Re(planes[0], Rect(cx, 0, cx, cy));
    Mat q2Re(planes[0], Rect(0, cy, cx, cy));
    Mat q3Re(planes[0], Rect(cx, cy, cx, cy));

    Mat tmp;
    qORe.copyTo(tmp);
    q3Re.copyTo(qORe);
    tmp.copyTo(q3Re);
    q1Re.copyTo(tmp);
    q2Re.copyTo(q1Re);
    tmp.copyTo(q2Re);

    Mat q0Im(planes[1], Rect(0, 0, cx, cy));
    Mat q1Im(planes[1], Rect(cx, 0, cx, cy));
    Mat q2Im(planes[1], Rect(0, cy, cx, cy));
    Mat q3Im(planes[1], Rect(cx, cy, cx, cy));

    q0Im.copyTo(tmp);
    q3Im.copyTo(q0Im);
    tmp.copyTo(q3Im);
    q1Im.copyTo(tmp);
    q2Im.copyTo(q1Im);
    tmp.copyTo(q2Im);

    Mat centerComplex;
    merge(planes, 2, centerComplex);

    return centerComplex;
}

// 2D IDFT
Mat setComplex(Mat magImg, Mat phaImg)
{
    exp(magImg, magImg);
    magImg -= Scalar::all(1);
    // magnitude 계산을 반대로 수행

    Mat planes[2];
    polarToCart(magImg, phaImg, planes[0], planes[1]);
    // 극 좌표계 -> 직교 좌표계 (각도와 크기로부터 2차원 좌표)

    Mat complexImg;
    merge(planes, 2, complexImg);
    // 실수부, 허수부 합체

    return complexImg;
}
Mat doIdft(Mat complexImg)
{
    Mat idftcvt;
    idft(complexImg, idftcvt);
    // IDFT를 이용한 원본 영상 취득

    Mat planes[2];
    split(idftcvt, planes);

    Mat dstImg;
    magnitude(planes[0], planes[1], dstImg);
    normalize(dstImg, dstImg, 255, 0, NORM_MINMAX);
    dstImg.convertTo(dstImg, CV_8UC1);
    // 일반 영상의 type과 표현범위로 변환

    return dstImg;
}

// magnitude 출력
void printMagnitude()
{
    Mat src_img = imread("img1.jpg", 0);
    Mat dst_img = doDft(src_img);
    dst_img = centralize(dst_img);
    dst_img = getMagnitude(dst_img);
    dst_img = myNormalize(dst_img);

    imshow("src_img", src_img);
    imshow("dst_img (Magnitude)", dst_img);
    waitKey(0);
    destroyAllWindows();
}

// phase 출력
void printPhase()
{
    Mat src_img = imread("img1.jpg", 0);
    Mat dst_img = doDft(src_img);
    dst_img = centralize(dst_img);
    dst_img = getPhase(dst_img);
    dst_img = myNormalize(dst_img);

    imshow("src_img", src_img);
    imshow("dst_img (Phase)", dst_img);
    waitKey(0);
    destroyAllWindows();
}

int main()
{
    printMagnitude();
    printPhase();
}